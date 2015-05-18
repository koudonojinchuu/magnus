#include "crossTrainer.h"
#include "FpLibNode.h"
#include "nodeMsg.h"
#include "Ftime.h"
#include "FpLib.h"
#include "FpTree.h"
#include "FTTpres.h"
#include "FfreeList.h"
#include "FGLink.h"
#include "FpresCompareNode.h"
#include "FfreeWord.h"

FpLibNode::FpLibNode(FGLnode *aSupervisor,FpLib* apLib,
	FpTree *apTree,FTTpres *newTTpres)
	:FGLnode(aSupervisor){

	this->newTTpres=newTTpres;
	itsPresCompareNode=ZERO;
	oldTTpres=ZERO;
	hook=FGLink::NA;
	foundLinkList=FALSE;
	noFirstCoord=TRUE;
	noSecondCoord=TRUE;
	noThirdCoord=TRUE;
	relIndex=1;
	genIndex=1;
	
	entryPoint=GLstart;
	func=pLibFunc;
	v=apLib;
	pTree=apTree;
					
	// we swap out some pTree registers, to be restored in
	// the goodByeKiss
			
	bvSave=pTree->bv;
	Dsave=pTree->D;
	
	// support for timing mechanism which warns about lengthy operations
	
	totalTime=0;
			
}
FpLibNode::~FpLibNode(void){
	delete oldTTpres;oldTTpres=0;
	delete itsPresCompareNode;itsPresCompareNode=0;
}

Boolean FpLibNode::pLibFunc(FGLnode *node){	
	return( ((FpLibNode*)node)->go() );
}

Boolean FpLibNode::go(void){
	switch(entryPoint){
		case GLstart:{
			FTTpres *pres=newTTpres;
			FList *count=pres->gCount;
			FfreeList *rels=pres->f;
			long k=pres->globalSymbols->numItems;
			long n=pres->g->numItems;
			long r=pres->f->numItems;		
			v->bv=v->root;
			
			if(k==0){
				if(v->containsEmptyPres){
					send(equivalentPresentationFoundMsg);
					return FALSE;
				}
				else{
					hook=FGLink::NA; // signifies that empty presentation
										// is to be added
					return FALSE;
				}
			}
			
			if(k>0 && n==0){
				// the presentation is free.  We seek a half-edge labelled k 
				// from the freeRoot
				v->bv=v->freeRoot;
				v->e=k;
				noFirstCoord=!v->halfS();
				if(noFirstCoord){
					hook=v->bv;
					return FALSE;
				}
				else{
					// the free presentation already exists
					send(equivalentPresentationFoundMsg);
					return FALSE;
				}
			}
			
			// seek first coordinate, number of generators
			v->e=k;
			noFirstCoord=!v->rS();
			if(noFirstCoord){
				hook=v->bv;
				return FALSE;
			}
			
			// seek second coordinate, number of generators which 
			// occur in a relation at least once
			v->e=n;
			noSecondCoord=!v->rS();
			if(noSecondCoord){
				hook=v->bv;
				return FALSE;
			}
			
			// seek third coordinate, number of relators
			v->e=r;
			noThirdCoord=!v->rS();
			if(noThirdCoord){
				hook=v->bv;
				return FALSE;
			}
			
			// seek number of occurrences, up to inverse, of the i'th
			// most frequently occurring generator
			
			
			for(long i=1;i<=n;++i){
				v->e=(long)count->NthItem(i);
				if(!v->rS()){
					hook=v->bv;
					genIndex=i;
					return FALSE;
				}
			}
			genIndex=n+1;
			
			// seek number of letters in the i'th longest relator
			
			int i;
			for(i=1;i<r;++i){
				FfreeWord *w=rels->freeItem(i);	
				v->e=w->wordLength;
				if(!v->rS()){
					hook=v->bv;
					relIndex=i;
					return FALSE;
				}
			}
			
			// the last edge is a "half link" to a pTree
			// vertex.  We must seek it with a specialized routine
			
			FfreeWord *w=rels->freeItem(r);
			v->e=w->wordLength;
			if(!((FpLib*)v)->halfS()){
				hook=v->bv;
				relIndex=i;
				return FALSE;
			}
			
			relIndex=r+1;
			
			pTree->bv=v->bv;					// trick step
			foundLinkList=TRUE;
			hook=pTree->bv;	
			// control flows into next case
			
			entryPoint=GLrun;
			
		}
		case GLrun:{
			// CAUTION: can't return until after ENDTRY
			Boolean returnVal=TRUE;
#ifdef Frank_GenuineMac
			TRY{
#endif
				if(!itsPresCompareNode){
					pTree->Get();
					oldTTpres=pTree->getPres();
					itsPresCompareNode=
						new FpresCompareNode(this,oldTTpres,newTTpres);
					presentationIsNew=TRUE;
				}
				if(!itsPresCompareNode->run()){
					delete itsPresCompareNode;itsPresCompareNode=0;
					if(!presentationIsNew){
						// we are all done
						
						returnVal=FALSE;
					}
					else{
						// go to the next presentation in the linklist
						// if it exists, otherwise done
						pTree->bv=pTree->D.link.next;
						if(isNull(pTree->bv)) returnVal=FALSE;
						else hook=pTree->bv;
					}
				}
				lowFail("FpLibNode::go");

#ifdef Frank_GenuineMac
			}
			CATCH{
				if(gLastError==memFullErr){
					// we have run out of memory so we just have to add
					// this presentation to the library
					delete itsPresCompareNode;itsPresCompareNode=0;
					NO_PROPAGATE;
					returnVal=FALSE;
				}
			}
			ENDTRY;
#endif
			return returnVal;		
		}
		break;
		default: nodeErr(8);
	}
}

void FpLibNode::wakeUp(void){
	startTime=clock();
}

void FpLibNode::auRevoir(void){
	totalTime+=clock()-startTime;
	FGLnode::auRevoir();
	send(LibAuRevoirMsg);
}

void FpLibNode::goodbyeKiss(void){
	pTree->D=Dsave;
	pTree->bv=bvSave;
	send(LibGoodbyeKissMsg);
}

void FpLibNode::respond(short aMessage){
	switch(aMessage){
		case equivalentPresentationFoundMsg:
			presentationIsNew=FALSE;
			send(equivalentPresentationFoundMsg);
		break;
		case CompareAuRevoirMsg:
		break;
		case CompareGoodbyeKissMsg:
			delete oldTTpres;oldTTpres=0;
		break;	
		default: send(aMessage);
	}
}

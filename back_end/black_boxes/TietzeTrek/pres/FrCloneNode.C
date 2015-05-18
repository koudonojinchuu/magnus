#include "crossTrainer.h"

#include "FrCloneNode.h"
#include "FrTree.h"
#include "GLfunctions.h"
#include "nodeMsg.h"
#include "FGLstepper.h"
#include "FfreeWord.h"
#include "FfreeList.h"
#include "FTTpres.h"
	
FrCloneNode::FrCloneNode(FGLnode *aSupervisor, FrTree *cloneFrom,
		FrTree *cloneTo)
	:FcloneNode(aSupervisor,cloneFrom,cloneTo,cloneFrom->cloneOffset){
	
	if(cloneFrom->cloneOffset != cloneTo->cloneOffset)nodeErr(39);
	
}


Boolean FrCloneNode::walk(void){

// this code is the same as in FcloneNode.  It must be duplicated here
// so that the overriding subroutines will be called.

	switch(entryPoint){
		case firstVertex:
			if(v==ZERO)nodeErr(4);
			if(v->Qideal())nodeErr(11);
			startUp();	
			virginCloneVertex(v->bv,vClone->bv);
			entryPoint=nextVertex;
		case nextVertex:
			return(processVertex());
		case firstEdge:{
			// Process the first edge, if there is one.
			// if there is no defined edge, control flows to nextVertex
			
			GLref startRef;
			Ptr DEREF=v&v->bv;
			v->xGetPacketRef(startRef,GLr,DEREF);
			v-v->bv;
			if(isNull(startRef)){
				entryPoint=nextVertex;  // no real link
				return(TRUE);  
			}
			
			// start search for first edge at the very beginning
			GLref entryPacketRef;
			v->xsetToFirstPacket(entryPacketRef,startRef);  
	
			if(itsStepper->firstEdge(entryPacketRef)){
				// first edge detected
				GLref ref;
				v->xEdgeEntryToOwner(ref,itsStepper->refDatum);
				v->cv=ref;
				entryPoint=nextEdge; 
				return(processEdge()); 
			}
			else{	
				// real link has no edges
				
				entryPoint=nextVertex; 
				return(TRUE);  
			}
		}
		case nextEdge:
			if(itsStepper->nextEdge()){
				// next edge detected
				GLref ref;
				v->xEdgeEntryToOwner(ref,itsStepper->refDatum);
				v->cv=ref;
				entryPoint=nextEdge; 
				return(processEdge()); 
			}
			else{	
				// real link has no more edges
				
				entryPoint=nextVertex; 
				return(TRUE);  
			}
		default: nodeErr(6);
	}
	nodeErr(8);
}

Boolean FrCloneNode::processEdge(void){	
	Ptr DEREF=v&v->cv;
	long IDvertex=v->xGetID(DEREF,IDslot);
	v-v->cv;
	if(IDvertex<IDchild){
		//virgin, edge is maximal
		if(admissibleEdge()){	
			vClone->e=v->e;
			vClone->V();
			virginCloneVertex(v->cv,vClone->cv);
		}
	}
	else if(IDvertex==IDchild){
		//child, edge is a basis edge
		// this is an error because we are supposed to be walking a tree
		nodeErr(12);
	}
	else if(IDvertex==IDadult){
		if(SameAddr(v->bv,v->cv)){
			if(v->e>0){
				//loop, edge is a basis edge
				// this is an error because we are supposed to be walking a tree
				nodeErr(12);
			}
		}
	}
	else nodeErr(12);
	return(TRUE);
}

Boolean FrCloneNode::admissibleEdge(void){
	long rLocalOld=((FrTree*)v)->getLevel(v->bv)+1;  // level is zero based
	long rLocalNew=v->e;
	FfreeWord *oldWord=oldTTpres->f->freeItem(rLocalOld);	
	oldWord->leftJustify();
	FfreeWord *newWord=newTTpres->f->freeItem(rLocalNew);
	newWord->leftJustify();
	if(oldWord->wordLength!=newWord->wordLength) nodeErr(34); // this is invariant
															  // of comparability
	long noEntries=oldWord->noEntries;
	if(noEntries!=newWord->noEntries) return(FALSE);  // this is  an invariant of
													  // equivalence but not 
													  // comparability
	long iEnd=2*noEntries;
	long *oldVec=*oldWord->wordHandle;
	long *newVec=*newWord->wordHandle;
	for(long i=0;i<iEnd;i+=1){
		if(oldVec[i]==gOld){
			if(newVec[i]!=gNew) return FALSE;
			++i;
			if(oldVec[i]!=newVec[i]) return FALSE;
		}
		else ++i;
	}
	if(rLocalOld==((FrTree*)v)->numItems){
		send(validPermutationMessage);
	}
	return TRUE;
}


#include "crossTrainer.h"
#include "FgTreeNode.h"
#include "FrCloneNode.h"
#include "nodeMsg.h"
#include "FgTree.h"
#include "FrTree.h"
#include "FTTpres.h"
#include "FfreeList.h"
#include <stdio.h>

FgTreeNode::FgTreeNode(FGLnode *aSupervisor,FgTree *agTree,
	FTTpres *oldTTpres, FTTpres *newTTpres,FrTree *rTree, FrTree *rClone)
	:FGLwalkNode(aSupervisor,agTree){

// the supervisor sets up 
// oldTTpres, newTTpres, rTree, v (=gTree)
// and rClone

	this->oldTTpres=oldTTpres;
	this->newTTpres=newTTpres;
	this->rTree=rTree;
	this->rClone=rClone;
	gTree=(FgTree*)v;
	
	rCloneNode=ZERO;

}

FgTreeNode::~FgTreeNode(void){
	delete rCloneNode;rCloneNode=0;
}

Boolean FgTreeNode::walk(void){
	switch(entryPoint){
		case firstVertex:
			if(v==ZERO)nodeErr(4);
			if(v->Qideal())nodeErr(11);
			startUp();
			virginVertex(v->bv);
			entryPoint=nextVertex;
		
		case nextVertex:
			return(processVertex());
		case firstEdge:
			// Create the first admissible edge, if it exists.
			// if there is no admissible edge, control flows to nextVertex
			
			v->e=0;
			entryPoint=nextEdge;
			
			// control flows to next case
			
		case nextEdge:
			if(gTree->eNext()){
				func=maximalEdgeFunc;
				return TRUE;
			}
			else{
				entryPoint=nextVertex;
				return(TRUE);
			}
		default: nodeErr(6);
	}
	nodeErr(8);
}

Boolean FgTreeNode::processVertex(){
	
// if ZBlocal is less than the number of generators, then we
// generate admissible edges.  Otherwise we go to the next vertex on the stack

	GLref ref;
	childVertices->Pop(ref);
	v->bv=ref;
	if(isNull(v->bv)){
		return finish();
	}
	Ptr DEREF=v&v->bv;			
	v->xPutID(DEREF,IDslot,IDadult);
	long ZBlocal;
	DEREF=&DEREF[gTree->byOffset];
	BlockMove(DEREF,(Ptr)&ZBlocal,sizeof(long));
	if(ZBlocal<gTree->numItems){
		entryPoint=firstEdge;
	}
	else entryPoint=nextVertex;
	v-v->bv;
	return(TRUE);
}	

Boolean  FgTreeNode::maximalEdge(void){
	if(!rCloneNode){
		// get root for walk of rTree and construct the node
		rClone->j=0;
		rClone->jLoc();
		rClone->e=1;
		rClone->V();
		rClone->bv=rClone->cv;
		gTree->getRef(rTree->bv,v->bv);
		rCloneNode=new FrCloneNode(this,rTree,rClone);
		// other initializations
		rCloneNode->newTTpres=newTTpres;
		rCloneNode->oldTTpres=oldTTpres;
		validPermutationDetected=FALSE;
		cloneRoot=rClone->bv;

		// compute coefficient assignment gOld->gNew
		// from oldTTpres to newTTpres.
		long localOld=gTree->getLevel(v->bv)+1; // gLevel is zero based
		penultimateGvertex= localOld==gTree->numItems;
		rCloneNode->gOld=(long)oldTTpres->g->NthItem(localOld);
			
		long localNew=v->e;
		rCloneNode->gNew=(long)newTTpres->g->NthItem(localNew);
	}
	if(!rCloneNode->run()){
		delete rCloneNode;rCloneNode=0;
		if(validPermutationDetected){
			// create new vertex, reference rTree->cv, and
			// call virginVertex
			v->V();
			gTree->putRef(v->cv, rTree->cv);
			virginVertex(v->cv);
		}
		return continueWalk();
	}
	return(TRUE);
}

void FgTreeNode::respond(short aMessage){
	switch(aMessage){
		case validPermutationMessage:
			validPermutationDetected=TRUE;
			if(penultimateGvertex){
				//the old and new presentations are equivalent.
				send(equivalentPresentationFoundMsg);
				rCloneNode->dirtyAbort=TRUE;  // aborts the clone node
				dirtyAbort=TRUE;  // aborts the gTree node
			}
		break;
		case cloneAuRevoirMsg:
		break;
		case cloneGoodbyeKissMsg:
			if(validPermutationDetected){
				
				transferCounters();
				
				// detach clone root from its ideal vertex
				rClone->bv=cloneRoot;
				rClone->e=-1;
				if(!rClone->iUnlnk())xErr(54);
				
				// create new rTree ideal vertex
				++rTree->j;
				rTree->I();
				
				// attach cloneRoot
				rTree->e=1;
				rTree->cv=cloneRoot;		// trick step
				rTree->Lnk();				
			}
		break;
		default:send(aMessage);
	}
}

void FgTreeNode::transferCounters(void){
	long index;
	for(index=0;index<GLflags;++index){
		rTree->Packets[index] += rClone->Packets[index];
		rClone->Packets[index]=0;
		rTree->Seeks[index] += rClone->Seeks[index];
		rClone->Seeks[index]=0;
		rTree->Total[index] += rClone->Total[index];
		rClone->Total[index]=0;
		rTree->InUse[index] += rClone->InUse[index];
		rClone->InUse[index]=0;
	}
}

#include "crossTrainer.h"
#include "FuNode.h"
#include "nodeMsg.h"
#include "FuTree.h"
#include "FGLink.h"

FuNode::FuNode(FGLnode *aSupervisor,FuTree *uTree,long scratchOffset,
	GLref& externalRoot)
	:FGLwalkNode(aSupervisor,uTree){
	
	this->uTree=uTree;
	
//  scratchOffset is an offset into the GLrv backYard of v.
//  This is an area that FuNode will overwrite and it
//  must be at least sizeof(breadCrumb) bytes

	if(v->backYardSize[GLrv]-scratchOffset<(long)sizeof(breadCrumb)){	
		nodeErr(18);
	}
	totalOffset=v->backYardOffset[GLrv]+scratchOffset;
	
// the GLVar is OK, so proceed with the standard initialization

	v->j=0;
	v->I();
	v->e=1;
	v->V();
	v->bv=v->cv;
	root=v->bv;
	uTree->putExternalRef(root,externalRoot);
	
	currentLevel=0;
	uTree->levelSlot=v->reserveSlot();
	if(uTree->levelSlot==noIDslotAvailable){
		nodeErr(5);
	}
	Ptr DEREF=v&root;
	v->xPutID(DEREF,uTree->levelSlot,0);
	v-root;
	breadCrumb b;
	b.ref=FGLink::NA;
	b.e=0;
	uTree->putBreadCrumb(root,b);
	lettersNo=v->noEntries[GLr];
	externalRef=FGLink::NA;

// set the run registers
	bvRun=v->bv;
	cvRun=v->cv;
	eRun=v->e;
}
Boolean  FuNode::vertex(void){
// if we are at a currentLevel vertex, we set FuNode::externalRef
// to the base backyard externRef.  Then we send a message
// to the supervisor for each potential edge.  If FuNode::externalRef is
// defined after the call, create the new edge and 
// set the companion breadCrumb to support future tracing.
// We set the companion backyard externalRef to FuNode::externalRef.
// Prior to the next call, FuNode::externalRef is reset
// to the base backyard externalRef.
// After all the new edges are created,
// we set the entryModifier to nextVertex
// so that these new edges will not be walked during this pass.

	Ptr DEREF=v&v->bv;
	long level=v->xGetID(DEREF,uTree->levelSlot);
	v-v->bv;
	long newLevel=level+1;	
	if(level==currentLevel){
		breadCrumb b;
		GLref baseExternalRef;
		uTree->getExternalRef(v->bv,baseExternalRef);
		if(isNull(baseExternalRef)){
			nodeErr(43);
		}
		uTree->getBreadCrumb(v->bv,b);
		long eParent= -b.e;
		v->e=uTree->eNext(0,eParent);
		externalRef=baseExternalRef;
		send(uInitializePacketMsg);
		while(v->e<lettersNo+1){
			send(uBeforeEdgeMsg);
			if(!isNull(externalRef)){		
				v->V();
				send(uAfterEdgeMsg);
				b.ref=v->bv;
				b.e= v->e;
				uTree->putBreadCrumb(v->cv,b);
				DEREF=v&v->cv;
				v->xPutID(DEREF,uTree->levelSlot,newLevel);
				v-v->cv;	
				uTree->putExternalRef(v->cv,externalRef);
			}
			v->e=uTree->eNext(v->e,eParent);
			externalRef=baseExternalRef;
		}
		send(uTerminatePacketMsg);
		entryPoint=nextVertex;
		func=walkFunc;
		return(TRUE);
	}

// if we are not at a currentLevel vertex, then we walk the edges
	entryPoint=firstEdge;
	return continueWalk();
}

Boolean  FuNode::basisEdge(void){

// this is an error, we are supposed to be in a tree
	gout <= "Error: FuNode has detected a cycle.\n";
	return continueWalk();
}
Boolean  FuNode::maximalEdge(void){
	return continueWalk();
}
void FuNode::wakeUp(void){
	FGLwalkNode::wakeUp();
}

void FuNode::auRevoir(void){
	FGLwalkNode::auRevoir();
	send(uAuRevoirMsg);
}


Boolean FuNode::finish(void){

// we have just finished constructing currentLevel+1

	++currentLevel;	
	
// cancel the IDslot, because it will
// be set again at GLstart time

	v->cancelSlot(IDslot);
	
// restart
	v->bv=root;
	entryPoint=firstVertex;
	return continueWalk();
}


void FuNode::goodbyeKiss(void){	
	send(uGoodbyeKissMsg);
}


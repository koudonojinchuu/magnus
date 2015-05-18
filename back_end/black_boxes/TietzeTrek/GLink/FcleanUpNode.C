#include "crossTrainer.h"
#include "FGLvar.h"
#include "FcleanUpNode.h"
#include "FGLstepper.h"
#include "nodeMsg.h"

FcleanUpNode::FcleanUpNode(FGLvar *aGLvar)
	:FGLnode(aGLvar){

	v=aGLvar;
	childVertices=new FGLstack(v->ChildNoEntries);
	
	bvStart=v->bv;
	cvStart=v->cv;
	eStart=v->e;
	
	// construct the stepper
	itsStepper=new FGLstepper(v,v->dataOffset[GLr],2*v->noEntries[GLr]);
		
	// we prepare the run registers for the first wakeup.
	
	// CAUTION: it is bad style to change the v registers in a derived
	// constructor, but if you must do this, be sure to update the
	// run registers so that your alterations will take effect
	
	bvRun=v->bv;
	cvRun=v->cv;
	eRun=v->e;
	
	//  control
		
	func=FcleanUpNode::walkFunc;
	entryPoint=firstVertex;
}

FcleanUpNode::~FcleanUpNode(void){
	v->cancelSlot(IDslot);
	delete childVertices;childVertices=0;
	delete itsStepper;itsStepper=0;
}


Boolean FcleanUpNode::walk(void){
	switch(entryPoint){
		case firstVertex:
			if(v==ZERO)nodeErr(4);
			if(v->Qideal())nodeErr(11);
			startUp();	
			virginVertex(v->bv);
			entryPoint=nextVertex;
			
			// control flows to next case
		case nextVertex:
			return(processVertex());
		case firstEdge:{
			// Process the first edge, if there is one.
			// if there is no defined edge, control flows to GLnextVertex
			
			GLref startRef,entryPacketRef;
			Ptr DEREF;
			DEREF=v&v->bv;
			v->xGetPacketRef(startRef,GLr,DEREF);
			v-v->bv;
			if(isNull(startRef)){
				entryPoint=nextVertex;  // no real link
				return(TRUE);  
			}
			
			// start search for first edge at the very beginning
			
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
		case nextEdge:{
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

		}
		default: nodeErr(6);
	}
	nodeErr(8);
}

void FcleanUpNode::virginVertex(const GLref& virgin){	
	Ptr DEREF;	
	childVertices->Push(virgin);
	DEREF=v&virgin;
	v->xPutID(DEREF,IDslot,IDchild);
	v-virgin;
}

Boolean FcleanUpNode::processVertex(){	
	GLref ref;
	childVertices->Pop(ref);
	v->bv=ref;
	if(isNull(v->bv)){
		return finish();
	}
	Ptr DEREF=v&v->bv;			
	v->xPutID(DEREF,IDslot,IDadult);
	entryPoint=firstEdge;
	v-v->bv;
	cleanUpRef=v->bv;
	send(cleanUpMsg);
	return(TRUE);
}	

Boolean FcleanUpNode::processEdge(void){	

	Ptr DEREF;
	long IDvertex;
	DEREF=v&v->cv;
	IDvertex=v->xGetID(DEREF,IDslot);
	v-v->cv;
	if(IDvertex<IDchild){
		//virgin, edge is maximal
		virginVertex(v->cv);
	}
	return(TRUE);
}

void FcleanUpNode::startUp(void){		
	IDslot=v->reserveSlot();
	if(IDslot==noIDslotAvailable){
		nodeErr(5);
	}
	IDchild=v->IDcounter;
	++v->IDcounter;
	if(v->IDcounter==0){
		v->varErr(38);
	}
	IDadult=v->IDcounter;
	++v->IDcounter;
	if(v->IDcounter==0){
		v->varErr(38);
	}
}
void FcleanUpNode::auRevoir(void){	
	bvRun=v->bv;
	cvRun=v->cv;
	eRun=v->e;
	v->bv=bvStart;
	v->cv=cvStart;
	v->e=eStart;
}
void FcleanUpNode::wakeUp(void){	
	v->bv=bvRun;
	v->cv=cvRun;
	v->e=eRun;
}
Boolean FcleanUpNode::finish(void){		
	v->cancelSlot(IDslot);
	delete childVertices;childVertices=0;  // we forget these here for the
	delete itsStepper;itsStepper=0;	  // benefit of routines that reinitialize
	v->bv=bvStart;				  // without first disposing the walknode
	v->cv=cvStart;
	v->e=eStart;
	return FALSE;
}

Boolean FcleanUpNode::walkFunc(FGLnode *n){
	return ((FcleanUpNode*)n)->walk();
}

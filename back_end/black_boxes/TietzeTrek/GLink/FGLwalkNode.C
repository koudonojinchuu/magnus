#include "crossTrainer.h"
#include "FGLvar.h"
#include "FGLwalkNode.h"
#include "FGLstepper.h"


	
FGLwalkNode::FGLwalkNode(FObject *aSupervisor,FGLvar *aGLvar)
	:FGLnode(aSupervisor){
	// we break out the construction and initialization into two
	// separate routines for the benefit of derived classes that
	// must reinitialize without destructing
	
	v=aGLvar;
	xGLwalkNode();
}

void FGLwalkNode::xGLwalkNode(){

//  CAUTION: the walk begins with entry time v->bv, v->cv, and v->e
//  A derived constructor may override these entry time values by
//  resetting bvRun,cvRun, and eRun.  
//	
//  CAUTION: resetting v->bv, v->cv, and v->e in a derived constructor
//  has NO EFFECT on the walk because run time registers are initialized
//  to bvRun, cvRun, and eRun.
	
	
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
		
	func=FGLwalkNode::walkFunc;
	entryPoint=firstVertex;
}

FGLwalkNode::~FGLwalkNode(void){
	v->cancelSlot(IDslot);
	delete childVertices;childVertices=0;
	delete itsStepper;itsStepper=0;
}

Boolean FGLwalkNode::continueWalk(void){
	func=walkFunc;
	return TRUE;
}


Boolean FGLwalkNode::vertex(void){
	SubclassResponsibility();
	return TRUE;
}
Boolean FGLwalkNode::maximalEdge(void){
	SubclassResponsibility();
	return TRUE;
}
Boolean FGLwalkNode::basisEdge(void){
	SubclassResponsibility();
	return TRUE;
}



Boolean FGLwalkNode::walk(void){
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

void FGLwalkNode::virginVertex(const GLref& virgin){	
	// Overriding methods should call this routine if the standard 
	// walk mechanism is implemented.	
	//	
	// the default virginVertex behavior:
	//
	// pushes virgin onto the childVertices Stack
	//	
	// loads IDchild into the virgin, so that
	// if this vertex is encountered lated it will
	// possible to determine that it is in the childVertices stack
	//
	
	Ptr DEREF;	
	childVertices->Push(virgin);
	DEREF=v&virgin;
	v->xPutID(DEREF,IDslot,IDchild);
	v-virgin;
}

Boolean FGLwalkNode::processVertex(){
	// Overriding methods should call this routine if the standard 
	// walk mechanism is implemented.	
	//	
	// the default processVertex behavior:
	//
	// pops vertex from childVertices.  If the stack is empty, calls
	// finish and returns its value.  Otherwise loads IDadult into the vertex
	//
	// set entryPoint to GLstart in anticipaction of the call to 
	// the user defined function atVertex()
	// CAVEAT EMPTOR:  it is the responsibility of atVertex to
	// reset entryPoint to GLrun and func to castToWalk in anticipation 
	// of the subsequent call to walk()
	//
	// set entryModifer to GLfirstEdge in anticipation of the
	// subsequent call to walk()
	//
	// set func to atVertex to set up the call to atVertex
	//
	// return TRUE to indicate that the call to atVertex should
	// take place
	
	GLref ref;
	childVertices->Pop(ref);
	v->bv=ref;
	if(isNull(v->bv)){
		return finish();
	}
	Ptr DEREF=v&v->bv;			
	v->xPutID(DEREF,IDslot,IDadult);
	entryPoint=firstEdge;
	func=vertexFunc;
	v-v->bv;
	return(TRUE);
}	
Boolean FGLwalkNode::processEdge(void){	
	// Overriding methods should call this routine if the standard 
	// walk mechanism is implemented.  
	
	// the default processEdge behavior:
	//
	// This routine assumes that cv is
	// set to the other endpoint at  entry time
	//
	// if cv is a virgin,  passes cv to virginVertex and
	// sets func to atMaximalEdge
	//
	// if cv is a child, sets func to atBasisEdge
	//
	// if cv is an adult, and cv!=bv, does nothing; this edge
	// has already been processed.  If cv==bv, we have a loop.
	// In this case if e>0 sets func to atBasisEdge.  
	// Otherwise does nothing; this edge has already been processed.
	//	
	// CAVEAT EMPTOR:  it is the responsibility of atMaximalEdge and
	// atBasisEdge to reset entryPoint to GLrun and func to castToWalk 
	// in anticipation of the subsequent call to walk()
	//
	// returns TRUE to indicate that the call to the indicated
	// function should take place

	Ptr DEREF;
	long IDvertex;
	DEREF=v&v->cv;
	IDvertex=v->xGetID(DEREF,IDslot);
	v-v->cv;
	if(IDvertex<IDchild){
		//virgin, edge is maximal
		virginVertex(v->cv);
		func=maximalEdgeFunc;
	}
	else if(IDvertex==IDchild){
		//child, edge is a basis edge
		func=basisEdgeFunc;
	}
	else if(IDvertex==IDadult){
		if(SameAddr(v->bv,v->cv)){
			if(v->e>0){
				//loop, edge is a basis edge
				func=basisEdgeFunc;
			}
		}
	}
	else nodeErr(12);
	return(TRUE);
}

void FGLwalkNode::startUp(void){		
	// Overriding methods should call this routine if the standard 
	// walk mechanism is implemented.
	//	
	// the default startUp behavior:
	//
	// reserve an IDslot for the walk.  The ID system enables the walk to
	// distinguish between basis edges and maximal edges, guarenteeing that
	// the atVertex will be called once at each vertex and the appropriate
	// edge function, atBasisEdge or atMaximalEdge, will be called once at
	// each edge.  Ofcourse the underlying assumption here is that none of
	// these functions returns FALSE, thereby prematurely terminating the walk.
	//
	// sets IDchild and IDadult.  A vertex stores IDchild when it is placed
	// on the childVertices stack and stores IDadult when it is popped off
	// the stack
	//
	// get slot
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
void FGLwalkNode::auRevoir(void){	
	// Overriding methods should call this routine if the standard 
	// walk mechanism is implemented.		
	//	
	// the default auRevoir behavior:
	//
	// set the run registers
	// restore the start registers
	
	FGLnode::auRevoir();  // this call supports the abort flag
	
	bvRun=v->bv;
	cvRun=v->cv;
	eRun=v->e;
	v->bv=bvStart;
	v->cv=cvStart;
	v->e=eStart;
}
void FGLwalkNode::wakeUp(void){	
	// Overriding methods should call this routine if the standard 
	// walk mechanism is implemented.	
	//	
	// the default wakeUp behavior:
	//
	// restore the run registers.
		
	v->bv=bvRun;
	v->cv=cvRun;
	v->e=eRun;
}
Boolean FGLwalkNode::finish(void){		
	// Overriding methods should call this routine if the standard 
	// walk mechanism is implemented.
	//		
	// the default finish behavior: 
	//
	// cancel the IDslot
	// forget the childVertices stack
	// restore the start registers.
	// return FALSE
	v->cancelSlot(IDslot);
	delete childVertices;childVertices=0;  // we forget these here for the
	delete itsStepper;itsStepper=0;	  // benefit of routines that reinitialize
	v->bv=bvStart;				  // without first disposing the walknode
	v->cv=cvStart;
	v->e=eStart;
	return FALSE;
}

Boolean FGLwalkNode::walkFunc(FGLnode *n){
	return ((FGLwalkNode*)n)->walk();
}
Boolean FGLwalkNode::vertexFunc(FGLnode *n){
	return ((FGLwalkNode*)n)->vertex();
}
Boolean FGLwalkNode::maximalEdgeFunc(FGLnode *n){
	return ((FGLwalkNode*)n)->maximalEdge();
}
Boolean FGLwalkNode::basisEdgeFunc(FGLnode *n){
	return ((FGLwalkNode*)n)->basisEdge();
}




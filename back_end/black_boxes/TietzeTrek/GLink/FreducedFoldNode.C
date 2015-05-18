#include "crossTrainer.h"
#include "FreducedFoldNode.h"
#include "FreducedVertexFolder.h"
#include "nodeMsg.h"
#include "FGLstepper.h"

// This node supports the abort and dirtyAbort flags.  Set the
// dirtyAbort flag TRUE to abort the node at the next available
// opportunity, whether or not the GLvar is clean.  Set the abort flag
// to abort the node as soon as the GLvar is clean.

// CAUTION: If the dirtyAbort flag is set, the then supervisor should
// delete the GLvar immediately upon regaining control.  The dirtyAbort
// flag is intended for use in expediting disposal of a running node and
// its GLvar.  IF YOU INTEND TO USE THE GLVAR AFTER THE NODE IS DISPOSED,
// then you MUST set the abort flag, NOT the dirtyAbort flag.

// CAUTION: this node sends the GLvarIsDirty message to warn the
// supervisor that the GLvar may be in an undefined state.  Its OK
// for the super to delete such a GLvar but its NOT GOOD to perform
// any other kind of operation on the GLvar.  Each GLvarIsDirty message
// is cancelled by a GLvarIsClean message, which this node sends when
// its OK for external routines to operate on the GLvar.
//
// Upon recieving the GLvarIsDirty message, the supervisor should take
// whatever action is necessary to insure that no operations will be
// performed externally on the GLvar until the GLvarIsClean message is
// recieved.   

FreducedFoldNode::FreducedFoldNode(FGLnode *aSupervisor,FGLvar *v)
	:FGLwalkNode(aSupervisor,v){

// Folding commences at v->bv.  The basic assumption is that
// away from this vertex there are no folds at entry time
	
// recycling is deferred to the end of the algorithm
// when all the dead vertices are flushed at once

// ZERO the vertex folder

	itsVertexFolder=ZERO;

	recycleRealSave=v->recycleReal;
	v->recycleReal=FALSE;
	
	
// we are not going to use the inherited FGLwalkNode::itsStepper
// We delete it since it has already been constructed

	delete itsStepper;itsStepper=0;
	
	auRevoir(); // this call prepares the run registers for the first wakeup.
}

FreducedFoldNode::~FreducedFoldNode(void){
	delete itsVertexFolder;itsVertexFolder=0;
}


void FreducedFoldNode::foldThisVertex(const GLref& ref){
	// don't put vertex on the child stack if it is already there
	Ptr DEREF=v&ref;
	long currentID=v->xGetID(DEREF,IDslot);
	if(currentID!=IDinStack){
		v->xPutID(DEREF,IDslot,IDinStack);
		childVertices->Push(ref);
	}
	v-ref;
}

	
Boolean FreducedFoldNode::walk(void){
	switch(entryPoint){
		case firstVertex:
			if(v==ZERO)nodeErr(4);
			if(v->Qideal())nodeErr(11);
			startUp();
			foldThisVertex(v->bv);
			entryPoint=nextVertex;
		//control flows directly to nextVertex
		case nextVertex:{
			// if we find a vertex that isn't dead, we
			// fold it.  It is the responsibility of the
			// folding routine to set the vertex ID to IDnotInStack
			// when it is done.  
						
			Ptr DEREF;
			GLref ref;
			while(TRUE){
				childVertices->Pop(ref);
				if(isNull(ref)){
					return finish();
				}
				v->bv=ref;
				if(!v->isDead()) break;
			}
			entryPoint=firstVertex;  
			func=FGLwalkNode::vertexFunc;
			return TRUE;
		}
		default: nodeErr(6);
	}
	nodeErr(8);
}


Boolean  FreducedFoldNode::vertex(void){

	//we fold the real edges of this vertex, and then indicate
	//that the vertex is no longer in the child stack.

	Boolean doneFolding=FALSE;
	switch(entryPoint){
		case firstVertex:{
			itsVertexFolder=new FreducedVertexFolder(this,v,v->bv);
			doneFolding=!itsVertexFolder->initOK;
			if(!doneFolding){
				GLvarIsClean=FALSE;
				send(GLvarIsDirtyMsg);
			}
			entryPoint=nextVertex;
		}
		case nextVertex:
		if(!doneFolding){
			doneFolding=!itsVertexFolder->run();
			if(itsVertexFolder->newFolds>0){
				newFolds+=itsVertexFolder->newFolds;
			}
		}
		if(doneFolding){
			GLvarIsClean=TRUE;
			send(GLvarIsCleanMsg);
		}
	}
	
//glue

	if(doneFolding){
		delete itsVertexFolder;itsVertexFolder=0;
		Ptr DEREF=v&v->bv;			
		v->xPutID(DEREF,IDslot,IDnotInStack);
		v-v->bv;
		return continueWalk();
	}		
	return TRUE ;				
}



void FreducedFoldNode::startUp(void){		
	// reserve an IDslot, which is used to determine if a vertex
	// is on the foldStack.  A vertex has its ID set to IDinStack
	// when it is placed on the stack.  When a vertex is popped off
	// the stack, its ID is decremented.  

	IDslot=v->reserveSlot();
	if(IDslot==noIDslotAvailable){
		nodeErr(5);
	}
	IDinStack=v->IDcounter;
	IDnotInStack=IDinStack-1;
	++v->IDcounter;
	if(v->IDcounter==0){
		v->varErr(38);
	}
}

Boolean FreducedFoldNode::finish(void){
	//Flush the dead vertices
	v->Flush();
	v->recycleReal=recycleRealSave;
	
	// release allocations	
	delete itsVertexFolder;itsVertexFolder=0;
	
	// inherited activity
	return FGLwalkNode::finish();
}

// algorithm notification
		
void FreducedFoldNode::auRevoir(void){
	FGLwalkNode::auRevoir();
	send(foldAuRevoirMsg);
	newFolds=0;
}

void FreducedFoldNode::goodbyeKiss(void){
	send(foldGoodbyeKissMsg);
}






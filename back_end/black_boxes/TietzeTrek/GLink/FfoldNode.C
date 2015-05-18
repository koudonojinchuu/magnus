#include "crossTrainer.h"

#include "FfoldNode.h"
#include "FvertexFolder.h"
#include "nodeMsg.h"

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



FfoldNode::FfoldNode(FObject *aSupervisor,FGLvar* aGLvar)
	:FGLwalkNode(aSupervisor,aGLvar){
	xfoldNode();
}
	
void FfoldNode::xfoldNode(void){
	// The fold node needs to reinitialize itself without destructing,
	// so we separate these two functions.  
	// CAUTION:  when reinitializing, be sure to call the initializer
	// for FGLwalkNode first to mimic the constructor behavior
	
// start out at root vertex
	v->j=0;
	v->e=1;
	v->jLoc();
	v->iS();
	bvRun=v->bv;
	cvRun=v->cv;
	eRun=v->e;
// initialize newFoldes
	newFolds=0;
	recycleRealSave=v->recycleReal;
	v->recycleReal=FALSE;
	
// indicate that the vertex folder is not defined

	itsVertexFolder=0;

// if a fold occurs, the algorithm will be restarted.  We initialize
// restartFoldNode to FALSE

	restartFoldNode=FALSE;	
}

FfoldNode::~FfoldNode(void){
	delete itsVertexFolder;itsVertexFolder=0;
}

Boolean  FfoldNode::vertex(void){

	//we fold the real edges of this vertex if it is not dead
	//(a vertex that became unlinked as a result of a previous
	//call to fold may have just popped off the childStack.
	//We ignore these vertices and flush them at finish time.)

	Boolean doneFolding=TRUE;
	if(!v->isDead()){
		doneFolding=FALSE;
		if(!itsVertexFolder){
			itsVertexFolder=new FvertexFolder(v,v->bv);
			
			doneFolding=!itsVertexFolder->initOK;
			if(!doneFolding){
				GLvarIsClean=FALSE;
				send(GLvarIsDirtyMsg);
			}
		}
		if(!doneFolding){
			doneFolding=!itsVertexFolder->run();
			if(itsVertexFolder->newFolds>0){
				newFolds+=itsVertexFolder->newFolds;
				restartFoldNode=TRUE;
			}
		}
		if(doneFolding){
			GLvarIsClean=TRUE;
			send(GLvarIsCleanMsg);
		}
	}
//glue: if done folding, forget the vertexFolder and return control to the walk

	if(doneFolding){
		delete itsVertexFolder;itsVertexFolder=0;
		return continueWalk();
	}		
	else return(TRUE);				
}

Boolean  FfoldNode::basisEdge(void){
	return continueWalk();
}

Boolean FfoldNode::maximalEdge(void){
	return continueWalk();
}

Boolean FfoldNode::finish(void){

	//Flush the dead vertices
	v->Flush();
	v->recycleReal=recycleRealSave;
	
	//get the standard finish behavior
	FGLwalkNode::finish();
	delete itsVertexFolder;itsVertexFolder=0;
	
	//now restart if necessary
	if(restartFoldNode){
		xGLwalkNode();
		xfoldNode();
		return continueWalk();
	}
	else return FALSE;
}


void FfoldNode::auRevoir(void){

	FGLwalkNode::auRevoir();
	send(foldAuRevoirMsg);
	newFolds=0;
}

void FfoldNode::goodbyeKiss(void){
	send(foldGoodbyeKissMsg);
}


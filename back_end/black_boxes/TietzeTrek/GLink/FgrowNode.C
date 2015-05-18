#include "crossTrainer.h"
#include "FgrowNode.h"
#include "nodeMsg.h"
#include "FcloneNode.h"

// CAUTION: With the exception of an external abort, the grow node 
// always returns FALSE.  If done==FALSE, Control must pass 
// either to the fold node or the clone node.  
//
//  needToClone==TRUE		pass control to the clone node
//  needToClone==FALSE      pass control to the fold node
//
// The grow node expects the fold and clone nodes to return control to the current
// grow node when they are done.  DO NOT delete the grow node unless done==TRUE.

	
FgrowNode::FgrowNode(FGLnode *aSupervisor,FGLvar *v,FGLvar *vClones,long lettersNo)
	:FGLnode(aSupervisor){

	this->v=v;
	this->vClones=vClones;
	this->lettersNo=lettersNo;
	if(lettersNo!=0){
		if(v->noEntries[GLr]!=lettersNo)nodeErr(25);
	}
// initialize counters

	currentEdge=1;
	startEdge=0;
	startVerticesNo=v->Total[GLrv];
	done=FALSE;
			
// the clone is initially attached to the 0 ideal vertex of vClones
// and subsequently attached to v at the appropriate vertex.
	
	vClones->j=0;
	entryPoint=GLstart;
	func=growFunc;								
	
}

Boolean FgrowNode::growFunc(FGLnode *node){	
	
	return( ((FgrowNode*)node)->grow() );
}

Boolean FgrowNode::grow(void){
	switch(entryPoint){
		case GLstart:{
			FcloneNode *clone;
			Boolean callAgain;

			if(lettersNo==0){
				done=TRUE;
				return FALSE;	// all done
			}
			
		// update startEdge and startVerticesNo if they have been
		// invalidated by the last fold.  If still valid, then check
		// to see if done
			if(v->Total[GLrv]!=startVerticesNo){
				startVerticesNo=v->Total[GLrv];
				startEdge=currentEdge;
			}
			else if(startEdge==currentEdge){
				done=TRUE;
				return FALSE;
			}
			else if(startEdge==0){
				startEdge=1;
			}
		// set up v->bv and vClones->bv for the call to the clone node
			if(vClones->jLoc()){
				vClones->e=1;
				vClones->V();
				vClones->bv=vClones->cv;
			}
			else nodeErr(25);
			v->j=0;
			if(v->jLoc()){
				v->e=1;
				if(!v->iS()){
					nodeErr(24);
				}
			}
			else nodeErr(25);

			// glue
	
			// We set flag to call the clone node.
			needToClone=TRUE;
		
			func=growFunc;				// when control returns to the grow node
			entryPoint=GLgrowEdge;		// we will want to be in the growFunc	
			return(FALSE);				// identifying the clone onto the current edge		
		}	
		
		case GLgrowEdge:
		//we attach the clone to the current edge
		vClones->jLoc();
		vClones->e=1;
		vClones->iS();
		vClones->e=-1;

		vClones->iUnlnk();	 
		v->cv=vClones->bv;   // this is the trick step.
		v->j=0;
		v->jLoc();
		v->e=1;
		v->iS();
		v->e=currentEdge;
		if(!v->rS()){
			nodeErr(28);
		}
		v->Identify();
		
		// notify alg to save the entry location for folding
		send(foldEntryLocationMsg);

		// compute the next edge
		if(currentEdge>0) currentEdge=-currentEdge;
		else{
			currentEdge=1-currentEdge;
			if(currentEdge>lettersNo){
				currentEdge=1;
			}
		}

		// Set the flag for folding, i.e., negate the need to clone flag
		needToClone=FALSE;
		entryPoint=GLstart;
        
		return(FALSE);
		break;
		
		default: nodeErr(6);
	}
}

void FgrowNode::auRevoir(void){
	FGLnode::auRevoir();			// this call supports external aborts
	send(growAuRevoirMsg);
}

void FgrowNode::goodbyeKiss(void){
	send(growGoodbyeKissMsg);
}


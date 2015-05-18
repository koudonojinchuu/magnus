#include "crossTrainer.h"

#include "FcloneNode.h"
#include "GLfunctions.h"
#include "FGLstepper.h"
#include "nodeMsg.h"

	
FcloneNode::FcloneNode(FObject *aSupervisor,FGLvar *cloneFrom,
		FGLvar *cloneTo,long scratchOffset)
	:FGLwalkNode(aSupervisor,cloneFrom){

//  CAUTION: don't alter the graphs of cloneFrom or cloneTo
//  while the clone node is running.  Don't try to clone part of a GLvar
//  onto itself.  Instead, clone a GLvar to another GLvar of the same
//  class and then glue the two graphs together.
 
//	the part of v to be cloned is indicated by v->bv, which must be a
//  real vertex of v 

//	the cloned data is attached vClone at vClone->bv, which must be a
//  real vertex of vClone

//  scratchOffset is an offset into the GLrv backYard of v.
//  This is an area that FcloneNode will overwrite and it
//  must be at least sizeof(GLref) bytes

  	FGLstepper *stepper;

	vClone=cloneTo;	
	if(v==vClone){
		nodeErr(23);
	}
	if(v->backYardSize[GLrv]-scratchOffset<(long)sizeof(GLref)){	
		nodeErr(22);
	}
	totalOffset=v->backYardOffset[GLrv]+scratchOffset;	
}

Boolean FcloneNode::walk(void){
	switch(entryPoint){
		case firstVertex:
			if(v==ZERO)nodeErr(4);
			if(v->Qideal())nodeErr(11);
			startUp();	
			virginCloneVertex(v->bv,vClone->bv);
			entryPoint=nextVertex;
		//control flows directly to GLrun
		case nextVertex:
			return(processVertex());
		case firstEdge:{
			// Process the first edge, if there is one.
			// if there is no defined edge, control flows to GLnextVertex
			
			Ptr DEREF=v&v->bv;
			GLref startRef;
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

void FcloneNode::virginCloneVertex(const GLref& original,const GLref& clone){	
	childVertices->Push(original);
	Ptr DEREF=v&original;
	v->xPutID(DEREF,IDslot,IDchild);
	BlockMove((Ptr)&clone,&DEREF[totalOffset],sizeof(GLref));
	v-original;
}

Boolean FcloneNode::processVertex(){
	
	Ptr DEREF;
	GLref bvClone,ref;
	childVertices->Pop(ref);
	v->bv=ref;
	Boolean locked;
	if(isNull(v->bv)){
		return finish();
	}
	getClone(v->bv,bvClone);
	vClone->bv=bvClone;
	DEREF=v&v->bv;			
	v->xPutID(DEREF,IDslot,IDadult);
	entryPoint=firstEdge;
	v-v->bv;
	return(TRUE);
}	
Boolean FcloneNode::processEdge(void){	

	Ptr DEREF;
	long IDvertex;	
	DEREF=v&v->cv;
	IDvertex=v->xGetID(DEREF,IDslot);
	v-v->cv;
	if(IDvertex<IDchild){
		//virgin, edge is maximal
		vClone->e=v->e;
		vClone->V();
		virginCloneVertex(v->cv,vClone->cv);
	}
	else if(IDvertex==IDchild){
		//child, edge is a basis edge
		GLref clone;
		vClone->e=v->e;
		getClone(v->cv,clone);
		vClone->cv=clone;
		vClone->Lnk();
	}
	else if(IDvertex==IDadult){
		if(SameAddr(v->bv,v->cv)){
			if(v->e>0){
				//loop, edge is a basis edge
				vClone->e=v->e;
				vClone->cv=vClone->bv;
				vClone->Lnk();
			}
		}
	}
	else nodeErr(12);
	return(TRUE);
}

void FcloneNode::auRevoir(void){
	FGLwalkNode::auRevoir();
	send(cloneAuRevoirMsg);
}

void FcloneNode::goodbyeKiss(void){
	send(cloneGoodbyeKissMsg);
}
	
// backyard

void FcloneNode:: getClone(const GLref& original,GLref& clone){
	Ptr DEREF=v&original;
	BlockMove(&DEREF[totalOffset],(Ptr)&clone,sizeof(GLref));
	v-original;
}



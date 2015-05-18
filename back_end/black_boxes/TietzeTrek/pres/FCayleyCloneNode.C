#include "crossTrainer.h"
#include "FCayleyCloneNode.h"
#include "GLfunctions.h"
#include "nodeMsg.h"
#include "FGLstepper.h"
#include "FCayley.h"
#include "FuTree.h"
	
FCayleyCloneNode::FCayleyCloneNode(FGLnode *aSupervisor,FGLvar *cloneFrom,
		FGLvar *cloneTo,long scratchOffset,long cloneScratchOffset)
	:FcloneNode(aSupervisor,cloneFrom,cloneTo,scratchOffset){

// CAUTION: CayleyCloneNode expects a Boolean to follow the GLref
// at scratchOffset used by FcloneNode.
// CayleyCloneNode will read this Boolean and write it to the backYard
// of vClone at cloneScratchOffset if TRUE.  Otherwise FCayleyCloneNode 
// will not duplicate the vertex.

	if(v->backYardSize[GLrv]-scratchOffset<(long)sizeof(GLref)+(long)sizeof(Boolean)){	
		nodeErr(22);
	}
	flagOffset=totalOffset+sizeof(GLref);	
	if(vClone->backYardSize[GLrv]-cloneScratchOffset<(long)sizeof(Boolean)){	
		nodeErr(22);
	}
	cloneFlagOffset=vClone->backYardOffset[GLrv]+cloneScratchOffset;
}


Boolean FCayleyCloneNode::walk(void){

// this code is the same as in FcloneNode.  It must be duplicated here
// so that the overriding subroutines will be called.
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
	
Boolean FCayleyCloneNode::processEdge(void){	
// this routine has been modified so that
// if v->cv has TRUE doNotDuplicate flag, then
// no clone is created and virginCloneVertex is not called.
// if v->cv has FALSE doNotDuplicate flag, the
// the cloned vertex inherits this flag.
	Ptr DEREF=v&v->cv;
	long IDvertex=v->xGetID(DEREF,IDslot);
	v-v->cv;
	if(IDvertex<IDchild){
		//virgin, edge is maximal
		if(!getFlag(v->cv)){		// the flag is doNotDuplicate
			vClone->e=v->e;
			vClone->V();
			putFlag(vClone->cv,FALSE);
			virginCloneVertex(v->cv,vClone->cv);
		}
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
		Boolean locked;
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

	
// backyard

void FCayleyCloneNode::putFlag(GLref& where,Boolean aFlag){
	Ptr DEREF;
	DEREF=vClone&where;
	BlockMove((Ptr)&aFlag,&DEREF[cloneFlagOffset],sizeof(Boolean));
	vClone-where;
}

Boolean FCayleyCloneNode::getFlag(GLref& where){
	Boolean aFlag;
	Ptr DEREF=v&where;
	BlockMove(&DEREF[flagOffset],(Ptr)&aFlag,sizeof(Boolean));
	v-where;
	return(aFlag);
}

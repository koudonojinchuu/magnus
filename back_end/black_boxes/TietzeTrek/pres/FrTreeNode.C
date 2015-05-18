#include "crossTrainer.h"
#include "FrTreeNode.h"
#include  "nodeMsg.h"
#include "FrTree.h"


FrTreeNode::FrTreeNode(FGLnode *aSupervisor,FrTree* anrTree)
	:FGLwalkNode(aSupervisor,anrTree){}

Boolean FrTreeNode::walk(void){
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
			if(((FrTree*)v)->eNext()){
				v->V();
				virginVertex(v->cv);
				return(TRUE);
			}
			else{
				entryPoint=nextVertex;
				return(TRUE);
			}
		default: nodeErr(6);
	}
	nodeErr(8);
}

Boolean FrTreeNode::processVertex(){
	
// if ZBlocal is less than the number of generators, then we
// generate edges.  Otherwise we go to the next vertex on the stack

	GLref ref;
	childVertices->Pop(ref);
	v->bv=ref;
	if(isNull(v->bv)){
		return finish();
	}
	Ptr DEREF=v&v->bv;			
	v->xPutID(DEREF,IDslot,IDadult);
	DEREF=&DEREF[((FrTree*)v)->byOffset];
	long ZBlocal;
	BlockMove(DEREF,(Ptr)&ZBlocal,sizeof(long));
	if(ZBlocal<((FrTree*)v)->numItems){
		entryPoint=firstEdge;
	}
	else entryPoint=nextVertex;
	v-v->bv;
	return(TRUE);
}	


#include "crossTrainer.h"
#include "FreducedVertexFolder.h"
#include "FreducedFoldNode.h"

// We overrride the Identify routine so that it places
// a vertex v!=mipoint on the fold stack if some other
// vertex has been identified to v.  In order to do this,
// the reduced vertex folder must be aware of the 
// reduced vertex node so that it can send it the proper message.

FreducedVertexFolder::FreducedVertexFolder(
			FreducedFoldNode *n,FGLvar *v,const GLref& midpoint)
	:FvertexFolder(v,midpoint){
	itsNode=n;
}

void FreducedVertexFolder::Identify(void){
				
// indicate that bv will need to be folded again
// This is the one extra line of code that the reduced folder depends on
// to feed its stack.  Special thanks to Hamish for this.
	itsNode->foldThisVertex(v->bv);
	v->Identify();
}


#ifndef Frank_FdebugNode
#define Frank_FdebugNode
#include "FGLwalkNode.h"

class FdebugNode: public FGLwalkNode{
public:
	long verticesNo,maximalEdgesNo,basisEdgesNo;
	Boolean oldDebug,corruptEdge;
	FdebugNode(ostream &s,FGLvar *v);
	virtual Boolean vertex(void);
	virtual Boolean maximalEdge(void);
	virtual Boolean basisEdge(void);
	virtual Boolean checkEdgeIntegrity(void);
	virtual void goodbyeKiss(void);
};
#endif


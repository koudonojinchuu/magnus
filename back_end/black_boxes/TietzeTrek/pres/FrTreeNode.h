#ifndef Frank_FrTreeNode
#define Frank_FrTreeNode


#include "FGLwalkNode.h"
class FrTree;

class FrTreeNode: public FGLwalkNode{
public:
	FrTreeNode(FGLnode *aSupervisor,FrTree* anrTree);
	virtual Boolean walk(void);	
	virtual Boolean processVertex(void);
};

#endif

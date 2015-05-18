#ifndef Frank_FuNode
#define Frank_FuNode


#include "FGLwalkNode.h"
#include "FuTree.h"

class FuNode: public FGLwalkNode{
public:
	FuTree *uTree;			// owned by supervisor
	GLref root,externalRef;
	long lettersNo,totalOffset;
	long currentLevel;
	FuNode(FGLnode *aSupervisor,FuTree *uTree,long scratchOffset,
	GLref& externalRoot);
	virtual Boolean vertex(void);
	virtual Boolean basisEdge(void);
	virtual Boolean maximalEdge(void);
	virtual void wakeUp(void);	
	virtual void auRevoir(void);
	virtual Boolean finish(void);
	virtual void goodbyeKiss(void);
};

#endif

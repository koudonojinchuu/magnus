#ifndef Frank_FgTreeNode
#define Frank_FgTreeNode


#include "FGLwalkNode.h"

class FTTpres;
class FrTreeNode;
class FgTreeNode;
class FrTree;
class FgTree;
class FrCloneNode;


class FgTreeNode: public FGLwalkNode{
public:
	FgTreeNode(FGLnode *aSupervisor,FgTree *agTree,FTTpres *oldTTpres, 
		FTTpres *newTTpres,FrTree *rTree, FrTree *rClone);
	~FgTreeNode(void);
protected:
	virtual Boolean walk(void);	
	virtual Boolean processVertex(void);
	virtual Boolean maximalEdge(void);
public:
	FTTpres *newTTpres;			// owned by supervisor
	FTTpres *oldTTpres;			// owned by supervisor
	FrTree *rTree,*rClone;		// owned by supervisor
	FgTree *gTree;				// owned by supervisor

// stuff owned by the algorithm
	FrCloneNode *rCloneNode;
	Boolean penultimateGvertex,validPermutationDetected;
	GLref cloneRoot;

	virtual void respond(short aMessage);
	void transferCounters(void);

};

#endif

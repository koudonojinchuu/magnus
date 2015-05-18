#ifndef Frank_FpresCompareNode
#define Frank_FpresCompareNode


#include "FGLnode.h"

class FTTpres;
class FrTreeNode;
class FgTreeNode;
class FrTree;
class FgTree;

class FpresCompareNode:public FGLnode{
	public:
								
	FpresCompareNode(FGLnode *aSupervisor,
		FTTpres *oldTTpres,FTTpres *newTTpres);
	~FpresCompareNode(void);
	static Boolean presCompareFunc(FGLnode *node);
	virtual Boolean rTreeGo(void);
	virtual Boolean gTreeGo(void);
	virtual void auRevoir(void);
	virtual void goodbyeKiss(void);
	FTTpres *newTTpres;			// owned by supervisor
	FTTpres *oldTTpres;			// owned by supervisor

// stuff owned by the algorithm
	FGLink *itsGLink;
	FrTree *rTree;
	FrTree *rClone;
	FgTree *gTree;
	FrTreeNode *rNode;
	FgTreeNode *gNode;
	virtual void respond(short aMessage);
private:
	enum{rTreeMail,gTreeMail};
};

#endif

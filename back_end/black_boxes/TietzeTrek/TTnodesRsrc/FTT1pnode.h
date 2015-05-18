#ifndef Frank_FTT1pnode
#define Frank_FTT1pnode


#include "FTTnode.h"
#include "TietzeTypes.h"
#include "FpTree.h"
#include "Ftime.h"
class FfreeList;
class FTTpres;


class FTT1pnode:public FTTnode{



// TT1node stuff
	public:	
	clock_t timeLimit;


	FTT1pnode(FGLnode *aSupervisor,FpTree *apTree,FTTpres *currentPres);
	~FTT1pnode(void);
	static Boolean TT1pFunc(FGLnode *node);
	Boolean go(void);
	virtual void auRevoir(void);
	virtual void goodbyeKiss(void);
	virtual Boolean removeSubnode(void);
	
	FList *itsSubnodes;
	Boolean newPresDispose;
	long currentNode;
	FTTpres *currentTTpres;		// owned by supervisor
	FTTpres *newPres;			// created on the fly, ownership
								// passes to supervisor
	virtual void respond(short aMessage);

};

#endif


#ifndef Frank_FrCloneNode
#define Frank_FrCloneNode


#include "FcloneNode.h"
class FTTpres;
class FrTree;

class FrCloneNode: public FcloneNode{
public:
	FTTpres *newTTpres;			// owned by supervisor
	FTTpres *oldTTpres;			// owned by supervisor
	long gOld,gNew;				// set by supervisor

	FrCloneNode(FGLnode *aSupervisor, FrTree *cloneFrom,
		FrTree *cloneTo);
	virtual Boolean walk(void);	
	virtual Boolean processEdge(void);
	Boolean admissibleEdge(void);
};
#endif


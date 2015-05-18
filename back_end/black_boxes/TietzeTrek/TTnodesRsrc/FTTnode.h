#ifndef Frank_FTTnode
#define Frank_FTTnode


#include "FGLnode.h"
#include "Ftime.h"
#include "TietzeTypes.h"
class FpTree;
class FTTpres;


class FTTnode:public FGLnode{
	public:
	FpTree *pTree;					// owned by supervisor
	FTTpres *currentPres;
	clock_t startClock;
	long runningBalance;
	Boolean initOK,TThasFailed;
	TietzeType itsType;
	FTTnode(FGLnode *aSupervisor,FpTree *apTree,FTTpres *currentPres,
		TietzeType aTietzeType);
	virtual void wakeUp(void);	
	virtual void auRevoir(void);
	virtual void goodbyeKiss(void);
};

#endif

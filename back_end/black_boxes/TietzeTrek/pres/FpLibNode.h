#ifndef Frank_FpLibNode
#define Frank_FpLibNode


#include "FGLnode.h"
#include "TietzeTypes.h"
#include "FpTree.h"
class FTTpres;
class FpresCompareNode;
class FpTree;
class FpLib;


class FpLibNode:public FGLnode{
	public:
	pTreeRealType Dsave;		// D is swapped out, 
								// and restored in goodByeKiss
	GLref bvSave;				// ibid
	FpLibNode(FGLnode *aSupervisor,FpLib* apLib,FpTree *apTree,FTTpres *newTTpres);	
	~FpLibNode(void);
	static Boolean pLibFunc(FGLnode *node);
	virtual Boolean go(void);
	virtual void wakeUp(void);
	virtual void auRevoir(void);
	virtual void goodbyeKiss(void);
	FpTree *pTree;				// owned by supervisor
	FpLib *v;
	FTTpres *newTTpres;			// owned by supervisor
	FTTpres *oldTTpres;			// owned by pLibNode
	Boolean presentationIsNew;	
								
	// the following values are read by the supervisor at goodByeKiss
		
	Boolean foundLinkList,noFirstCoord,noSecondCoord,noThirdCoord;
	long relIndex,genIndex;
	GLref hook;

	// owned by this node	
	FpresCompareNode *itsPresCompareNode;
	
	void respond(short aMessage); 
	clock_t totalTime;			// caller can monitor totalTime from the
								// auRevoir message and take appropriate
								// action if the operation is taking too long 
private:
	enum pLibEntry{GLstart,GLrun};
	pLibEntry entryPoint;
	clock_t startTime;
};

#endif


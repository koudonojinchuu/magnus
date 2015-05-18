#ifndef Frank_FTT2node
#define Frank_FTT2node

#include "TietzeTypes.h"
#include "FTTnode.h"
#include "FpTree.h"

class FTTpres;
class FTTpresNode;
class FrenumberPresNode;
class FList;

class FTT2node:public FTTnode{
	public:	
	long TT2len,newGen;
	
// owned by node
	FList *freePart;

	FTT2node(FGLnode *aSupervisor,FpTree *apTree,FTTpres *currentPres,
		long TT2len,long newGen);	
	~FTT2node(void);
	static Boolean TT2Func(FGLnode *node);
	Boolean TT2go(void);
	Boolean presGo(void);
	Boolean renumberGo(void);
	virtual void auRevoir(void);
	virtual void goodbyeKiss(void);
	
	FTTpres *newPres;			// created on the fly, ownership
								// passes to supervisor
	Boolean newPresDispose;
	FTTpresNode *presGen;
	FrenumberPresNode *renumber;
	void respond(short aMessage);
private:
	enum{TT2mail,presMail,renumberMail};
};

#endif


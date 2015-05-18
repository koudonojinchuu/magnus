#ifndef Frank_FTT2pnode
#define Frank_FTT2pnode


#include "FTTnode.h"
#include "TietzeTypes.h"
#include "FpTree.h"
class FTTpres;
class FTTpresNode;
class FrenumberPresNode;


class FTT2pnode:public FTTnode{
	public:	
	
	long trivialLettersNo;

	FTT2pnode(FGLnode *aSupervisor,FpTree *apTree,FTTpres *currentPres);
	~FTT2pnode(void);	
	static Boolean TT2pFunc(FGLnode *node);
	Boolean FTT2pnode::TT2pgo(void);
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
	enum{TT2pmail,presMail,renumberMail};

};

#endif


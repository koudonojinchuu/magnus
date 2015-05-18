#ifndef Frank_FrenumberPresNode
#define Frank_FrenumberPresNode


#include "FGLnode.h"
class FfreeWord;
class FTTpres;

class FrenumberPresNode:public FGLnode{
	public:
	FTTpres *p; 	// owned by supervisor
	FTTpres *q;		// created by node, ownership passes to supervisor
	FList *temporarySymbols;
	long normalizedLocal;
	FrenumberPresNode(FGLnode* aSupervisor,FTTpres *aPres);
	~FrenumberPresNode(void);
	static Boolean renumberFunc(FGLnode *node);
	virtual Boolean renumber(void);	
	virtual void auRevoir(void);
	virtual void goodbyeKiss(void);
};

#endif


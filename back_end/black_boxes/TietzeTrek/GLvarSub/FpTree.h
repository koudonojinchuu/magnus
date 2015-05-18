#ifndef Frank_FpTree
#define Frank_FpTree

#include "FGLvar.h"
#include "TietzeTypes.h"
class FfreeWord;
class FpTreeInitInfo;
class FTTpres;
class FcleanUpNode;

class  pTreeRealType{ public: TTbank bank; TTinfo initInfo; 
                 compareLink link;long eParent,eMax,serialNumber;};


class FpTree:public FGLvar{
	public:
	static pTreeRealType D;
	void Put(void);
	void cPut(void);
	void Get(void);
	void cGet(void);
	
// programmer defined
	long passNumber,passSlot;
	FcleanUpNode *cleanUp;
	FpTree(FGLink *aGLink,FObject *aSupervisor, FpTreeInitInfo *info);
	~FpTree(void);
	virtual void respond(short aMessage);
	virtual void V(long serialNumber);
	virtual void putPres(FTTpres *p);
	virtual FTTpres *getPres(void);
	virtual void allocateSpace(GLref& ref,long size);
	virtual long eNext(void);
};

#endif


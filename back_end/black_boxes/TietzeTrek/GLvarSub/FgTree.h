#ifndef Frank_FgTree
#define Frank_FgTree


#include "FmapTree.h"


class FgTreeInitInfo;
class FgTree:public FmapTree{
	public:
	long refOffset;
	
// programmer defined methods for name

	FgTree(FGLink *aGLink, FList *gCount,FgTreeInitInfo *info);
	virtual void V(void);
	virtual void putRef(GLref& vRef, GLref& refDatum);
	virtual void getRef(GLref& ref,GLref& vRef);
};
#endif


#ifndef Frank_FrTree
#define Frank_FrTree


#include "FmapTree.h"

class FrTreeInitInfo;
class FfreeList;

class FrTree:public FmapTree{
	public:
	long cloneOffset;
	FrTree(FGLink *aGLink, FfreeList *rels,FrTreeInitInfo *info);
};
#endif


#ifndef Frank_FmapTree
#define Frank_FmapTree


#include "FGLvar.h"

class FGLink;
class FmapTreeInitInfo;

class FmapTree:public FGLvar{
	public:
	long byOffset,bySize,numItems;
	long **ZBrating;				// must be constructed by derived class
	long **ZBfirstIndex;			// must be constructed by derived class

	FmapTree(FGLink *aGLink,FmapTreeInitInfo *info);
	~FmapTree(void);
	virtual void V(void);
	virtual Boolean eNext(void);
	virtual long getLevel(const GLref& vRef);
};

#endif


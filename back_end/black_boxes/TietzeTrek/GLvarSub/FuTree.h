#ifndef Frank_FuTree
#define Frank_FuTree


#include "FGLvar.h"
class FuTreeInitInfo;
class FfreeWord;

class FuTree:public FGLvar{
	public:
	long externalOffset;
	long breadOffset;
	long levelSlot;		// this is a slot in the id stack that indicates
						// the level of the vertex.  It is maintained externally.
// programmer defined methods for uTree
	FuTree(FGLink *aGLink,FuTreeInitInfo *info);
	virtual long eNext(long eCurrent, long eParent);
	void putBreadCrumb(const GLref& where,breadCrumb& b);
	void getBreadCrumb(const GLref& where,breadCrumb& b);
	void getExternalRef(const GLref& where,GLref& externalRef);
	void putExternalRef(const GLref& where,GLref& externalRef);
	virtual FfreeWord *trace(const GLref& startRef);
};

#endif


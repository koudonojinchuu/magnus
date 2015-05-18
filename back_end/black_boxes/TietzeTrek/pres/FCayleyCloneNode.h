#ifndef Frank_FCayleyCloneNode
#define Frank_FCayleyCloneNode

#include "FcloneNode.h"

class FCayleyCloneNode: public FcloneNode{

public:
	long flagOffset,cloneFlagOffset;
	FCayleyCloneNode(FGLnode *aSupervisor,FGLvar *cloneFrom,
		FGLvar *cloneTo,long scratchOffset,long cloneScratchOffset);
	static Boolean cloneFunc(FGLnode *node,FGLvar *v);
	virtual Boolean walk(void);		
	virtual Boolean processEdge(void);
// backyard
	virtual void putFlag(GLref& where,Boolean aFlag);
	virtual Boolean getFlag(GLref& where);

};
#endif


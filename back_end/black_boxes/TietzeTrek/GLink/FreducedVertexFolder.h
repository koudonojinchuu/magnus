#ifndef Frank_CreducedVertexFolder
#define Frank_CreducedVertexFolder

#include "FvertexFolder.h"

class FreducedFoldNode;

class FreducedVertexFolder:public FvertexFolder{
protected:
	FreducedFoldNode *itsNode;
public:
	FreducedVertexFolder(FreducedFoldNode *n,FGLvar *v,const GLref& midpoint);
	virtual void Identify(void);
};

#endif


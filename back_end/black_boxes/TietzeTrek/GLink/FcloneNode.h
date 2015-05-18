#ifndef Frank_FcloneNode
#define Frank_FcloneNode
#include "FGLstack.h"
#include "FGLwalkNode.h"

//entry point modifiers

class FcloneNode: public FGLwalkNode{

public:
	FGLvar *vClone;
protected:
	long totalOffset;
	
public:  //the user calls Init
	FcloneNode(FObject *aSupervisor,FGLvar *cloneFrom,
		FGLvar *cloneTo,long scratchOffset);
protected:	
	virtual Boolean walk(void);		
	virtual void virginCloneVertex(const GLref& original,const GLref& clone);
	virtual Boolean processVertex(void);		
	virtual Boolean processEdge(void);
	virtual void auRevoir(void);
	virtual void goodbyeKiss(void);
// backyard
	void getClone(const GLref& original,GLref& clone);
};
#endif


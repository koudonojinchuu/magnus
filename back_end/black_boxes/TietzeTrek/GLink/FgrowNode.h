#ifndef Frank_CgrowNode
#define Frank_CgrowNode

#include "FGLnode.h"

class FcloneNode;

class FgrowNode: public FGLnode{

public: 
	FGLvar *v,*vClones;
	long lettersNo,startEdge,startVerticesNo,currentEdge;
	Boolean needToClone,done;

	FgrowNode(FGLnode *aSupervisor,FGLvar *v,FGLvar *vClones,long lettersNo);
	static Boolean growFunc(FGLnode *node);
	virtual Boolean grow(void);	
	virtual void auRevoir(void);
	virtual void goodbyeKiss(void);
private:
	enum growPoints{GLstart,GLgrowEdge};
	growPoints entryPoint;
};

#endif


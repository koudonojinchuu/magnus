#ifndef Frank_FcleanUpNode
#define Frank_FcleanUpNode

#include "FGLnode.h"
class FGLstepper;


class FcleanUpNode: public FGLnode{
	
public:
	FcleanUpNode(FGLvar *aGLvar);	
	~FcleanUpNode(void);
	GLref cleanUpRef;
protected:	
	
	virtual Boolean walk(void);		
	virtual void virginVertex(const GLref& virgin);
	virtual Boolean processVertex(void);		
	virtual Boolean processEdge(void);
	virtual void startUp(void);
	virtual void auRevoir(void);
	virtual void wakeUp(void);
	virtual Boolean finish(void);
	
// USED by derived classes

	FGLvar *v;
	FGLstack *childVertices;
	FGLstepper *itsStepper;
	long IDchild,IDadult,IDslot;
	GLref bvStart,cvStart,bvRun,cvRun;
	long eStart,eRun;
	enum walkEntry{firstVertex,nextVertex,firstEdge,nextEdge};  
	walkEntry entryPoint;

// low level control

	static Boolean walkFunc(FGLnode *n);
};

#endif


#ifndef Frank_FGLwalkNode
#define Frank_FGLwalkNode

#include "FGLnode.h"
class FGLstepper;


class FGLwalkNode: public FGLnode{
	
public:
	FGLwalkNode(FObject *aSupervisor,FGLvar *aGLvar);	
	~FGLwalkNode(void);
	void FGLwalkNode::xGLwalkNode(void);		
protected:	
	
					
// OVERRIDE these routines to provide functionality
	virtual Boolean vertex(void);
	virtual Boolean maximalEdge(void);
	virtual Boolean basisEdge(void);
	
// OVERRIDE these routines to modify key control points
// Do this carefully as these routines have very subtle
// effects on each other
		
	virtual Boolean walk(void);		
	virtual void virginVertex(const GLref& virgin);
	virtual Boolean processVertex(void);		
	virtual Boolean processEdge(void);
	virtual void startUp(void);
	virtual void auRevoir(void);
	virtual void wakeUp(void);
	virtual Boolean finish(void);
	
// USED by derived classes

	Boolean continueWalk(void);
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
	static Boolean vertexFunc(FGLnode *n);
	static Boolean maximalEdgeFunc(FGLnode *n);
	static Boolean basisEdgeFunc(FGLnode *n);
};

#endif


#ifndef Frank_CfoldNode
#define Frank_CfoldNode
#include "FGLwalkNode.h"

class FvertexFolder;

// this is a non-standard walk node with index CfoldNodeIndex

class FfoldNode: public FGLwalkNode{
public:
	Boolean restartFoldNode,recycleRealSave;
	long newFolds;
protected: 
	FvertexFolder *itsVertexFolder;
public:
	
	FfoldNode(FObject *aSupervisor,FGLvar *aGLvar);
	~FfoldNode(void);
	void xfoldNode(void);
	virtual Boolean vertex(void);
	virtual Boolean basisEdge(void);
	virtual Boolean maximalEdge(void);
		
	// we override the standard finish behavior to flush dead nodes
	// and restart the algorithm if necessary
	
	virtual Boolean finish(void);
		
	// we override auRevoir and goodbyeKiss to report the number of
	// new folds
	virtual void auRevoir(void);
	virtual void goodbyeKiss(void);
};

#endif


#ifndef Frank_CreducedFoldNode
#define Frank_CreducedFoldNode

#include "FGLwalkNode.h"

class FreducedVertexFolder;

class FreducedFoldNode: public FGLwalkNode{

protected:
	long IDinStack,IDnotInStack;
public:
	Boolean recycleRealSave;
	long newFolds;
protected: 
	FreducedVertexFolder *itsVertexFolder;
public:
	FreducedFoldNode(FGLnode *aSupervisor,FGLvar *v);
	~FreducedFoldNode(void);
	void foldThisVertex(const GLref& ref);	
protected:
	virtual Boolean walk(void);		 
	virtual Boolean vertex(void);
	virtual void startUp(void);
	virtual Boolean finish(void);
	virtual void auRevoir(void);	//reports number of newFolds
	virtual void goodbyeKiss(void); //reports number of newFolds and 
	                                //flushes dead nodes
};

#endif


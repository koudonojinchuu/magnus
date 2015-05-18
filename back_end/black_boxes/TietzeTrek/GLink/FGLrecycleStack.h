#ifndef Frank_FGLrecycleStack
#define Frank_FGLrecycleStack
#include "GLref.h"
#include "FGLstack.h"
#include "FGLvarTypes.h"

class FGLvarInterface;

class FGLrecycleStack:public FGLstack{	

	private:
	
	GLflag flag;
	public:	
	FGLrecycleStack(GLflag flag,short aBlockSize);								
	virtual Boolean recyclePush(FGLvarInterface *v,const GLref& addr);								
	void recyclePop(GLref& ref,FGLvarInterface *v);
};

#endif


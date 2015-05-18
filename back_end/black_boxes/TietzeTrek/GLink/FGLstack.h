/*******************

Written by Frank Rimlinger, August 1992

*******************/
#ifndef Frank_FGLStack
#define Frank_FGLStack

#include "GLref.h"
#include "FArray.h"				
#include "FGlobal.h"

class FGLstack :public FArray {
	public:				
	FGLstack(short aBlockSize);							
	virtual Boolean Push(const GLref& addr);								
	virtual void Pop(GLref &addr);
	virtual Boolean thisRead(int refnum);
	virtual Boolean thisWrite(int refnum);		
};
#endif


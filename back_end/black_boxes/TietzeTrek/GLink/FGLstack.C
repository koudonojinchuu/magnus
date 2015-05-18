/*******************

Written by Frank Rimlinger, August 1992

*******************/
#include "crossTrainer.h"
#include "FGlobal.h"
#include "FGLstack.h"
#include "FGLink.h"

FGLstack::FGLstack(short aBlockSize)
	:FArray(sizeof(GLref)){
	SetBlockSize(aBlockSize);
}

Boolean FGLstack::Push(const GLref& addr){ /*moves memory*/
	InsertAtIndex((void*) &addr,numItems+1);
	return TRUE;		
}	

void FGLstack::Pop(GLref& ref){
	if(numItems<=0)ref=FGLink::NA;
	else{
		GetItem(&ref,numItems);
		DeleteItem(numItems);
	}
}


Boolean FGLstack::thisRead(int refnum){ /*moves memory*/
	/*N.B. Assumes stack is initialized*/
	return TRUE;
}
Boolean FGLstack::thisWrite(int refnum){
	return TRUE;
}

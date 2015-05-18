#include "crossTrainer.h"
#include "FrandomRef.h"

FrandomRef::FrandomRef(short aBlockSize)
	:FGLstack(sizeof(GLref)){}

void FrandomRef::popRandom(GLref& ref){
	if(numItems<=0){
		ref=FGLink::NA;
		return;
	}
	long index=randBnd(1,numItems);
	GetItem(&ref,index);			
	DeleteItem(index);
}


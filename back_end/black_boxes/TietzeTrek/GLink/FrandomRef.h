#ifndef Frank_FrandomRef
#define Frank_FrandomRef

#include "FGLstack.h"
#include "FArray.h"				
#include "FGlobal.h"

class FrandomRef :public FGLstack {
	public:				
	FrandomRef(short aBlockSize);							
	virtual void popRandom(GLref &ref);
};
#endif

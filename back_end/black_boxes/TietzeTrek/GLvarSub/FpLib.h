#ifndef Frank_FpLib
#define Frank_FpLib


#include "FGLvar.h"
class FpLibInitInfo;


class FpLib:public FGLvar{
	public:
	GLref root;
	GLref freeRoot;	
	Boolean containsEmptyPres;
	Boolean containsCyclicPres;
	Boolean containsFreeFactorPres;
	Boolean groupIsFinite;

	FpLib(FGLink *aGLink, FpLibInitInfo *info);
	void halfLink(GLref refDatum);
	Boolean halfS(void);
};

#endif


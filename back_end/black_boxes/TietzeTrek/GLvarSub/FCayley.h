#ifndef Frank_FCayley
#define Frank_FCayley

#include "FGLvar.h"

class FCayleyInitInfo;
class FGLink;

class FCayley:public FGLvar{
	public:
	GLref itsRoot;
	FGLnode *itsSupervisor;
	long flagOffset;
	
	FCayley(FGLnode *aSupervisor,FGLink *aGLink,FCayleyInitInfo *info);
	virtual void Identify(void);
	virtual void Append(GLref& lastPacket,const GLref& midpoint, const GLref& vRef);
	virtual void ReportToSuper(const GLref& vRef);
	virtual void putFlag(const GLref& where,Boolean aFlag);
};

#endif

#ifndef Frank_CfoldStepper
#define Frank_CfoldStepper

#include "GLref.h"
#include "FObject.h"

class FGLvar;

class FfoldStepper:public FObject{
public:
	FGLvar *v;
	GLref edgeRef,packetRef,refDatum,firstPacketOwnerRef;
	long edgeRefOffset,orientedEdgesNo,edgeIndex,edgeSign,count;
	FfoldStepper(FGLvar *v,long edgeRefOffset,long orientedEdgesNo,
	              const GLref& startPacket);
	virtual Boolean firstEdge(void);
	virtual Boolean nextEdge(void);
};
#endif


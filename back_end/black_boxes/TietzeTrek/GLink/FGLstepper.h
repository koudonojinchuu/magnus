#ifndef Frank_CGLstepper
#define Frank_CGLstepper

#include "GLref.h"
#include "FObject.h"

class FGLvar;

class FGLstepper:public FObject{
public:
	FGLvar *v;
	GLref edgeRef,packetRef,refDatum;
	long edgeRefOffset,orientedEdgesNo,edgeIndex,edgeSign,count;
	FGLstepper(FGLvar *v,long edgeRefOffset,long orientedEdgesNo);
	virtual Boolean firstEdge(const GLref& entryPacketRef);
	virtual Boolean nextEdge(void);
};
#endif


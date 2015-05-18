#ifndef Frank_CvertexFolder
#define Frank_CvertexFolder

#include "GLref.h"
#include "FGLrecycleStack.h"
// data structures for reentrant folding
enum{
	foldStart,
	foldFirstEdge,
	foldNextEdge
};

class FoldInfo {public: Boolean isOccupied,isMidpoint;
                GLref edgeDatum;};

class FfoldStepper;
class FGLvar;
class FList;

class FvertexFolder:public FObject{
public:
	FGLrecycleStack *itsRecycleStack;
	FfoldStepper *itsFoldStepper;
	FList *itsFoldInfo;
	FGLvar *v;
	GLref firstPacket,currentPacket,lastPacket,firstPacketOwnerLocation;
	Boolean initOK;
	Boolean status;
	short entryPoint;
	GLref midpoint,bvSave;
	long newFolds,ownerOffset;
	FvertexFolder(FGLvar *v,const GLref& midpoint);
	~FvertexFolder(void);
	virtual Boolean run(void);
	virtual Boolean handleStepper(Boolean entryDefined);
	virtual void Identify(void);
};

#endif


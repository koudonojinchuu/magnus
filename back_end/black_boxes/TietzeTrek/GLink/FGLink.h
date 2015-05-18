/*******************

Written by Frank Rimlinger, August 1992

*******************/
#ifndef Frank_FGLink
#define Frank_FGLink
#include "GLref.h"
#include "FCluster.h"
#include "Ftime.h"

class FBlockManager;
class FGLvarInterface;
class FGLvar;

class FGLink : public FCluster{ 
public:
	FBlockManager *itsBlockManager;
	static clock_t timeout;
	static GLref NA,DEADVERTEX;
	static long triggerWarning;
	static long MaxBlocks;
	long MaxPacketSize;
	Boolean DisposalSequenceInProgress;

public:  
	FGLink(void);
	~FGLink(void);
	void AddGLvar(FGLvarInterface *v);
	void RemoveGLvar(FGLvarInterface *v);
	static void Exit(void);	
private:
	Boolean DoForEach2(BooleanFunc theFunc,void *param);	
};
#endif

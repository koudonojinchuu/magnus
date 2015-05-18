/*******************

Written by Frank Rimlinger, August 1992

*******************/
#ifndef Frank_FBlockManager
#define Frank_FBlockManager

#include "GLref.h"
#include "FList.h"
#ifdef Frank_GenuineMac
#include "operators.h"
#else
#include <iostream.h>
#endif

class FGLink;
class FBlockManager: public FList{
	private:
	FGLink *itsGLink;
	FList *lock;
	GLref NextFreeAddress;
	Boolean ToppedOff;
	short ListSize;
	long BlockSize;
public:
	long TotalAllocation;
	long  TopSize;
	~FBlockManager(void);
	void LockBlock(const GLref& ref);
	void RestoreBlock(const GLref& ref);
	FBlockManager(FGLink *aGLink);
	void ReserveSpace(GLref& ref,long PacketSize);	
	void LargerPacketSize(long PacketSize);
	ostream&  repBlockSize(ostream& s);
private:	
	void SetListSize(void);
	void NewBlock(GLref& ref);
	Boolean DoForEach2(BooleanFunc theFunc,void *param);
};

#endif


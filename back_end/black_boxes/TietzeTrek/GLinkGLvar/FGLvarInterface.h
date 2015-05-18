/*******************

Written by Frank Rimlinger, August 1992
Revised by Frank Rimlinger, Jan 1993
*******************/

#ifndef Frank_FGLvarInterface
#define Frank_FGLvarInterface

#include "FObject.h"
#include "FGLvarTypes.h"

class FGLink;
class FGLvarInitInfo;
class FGLrecycleStack;
class FList;
// an alteration
class FGLvarInterface: public FObject{

public:
	FGLink *itsGLink;	
	Str255 itsName;		// this is a Pascal string
	FGLvarInterface(FGLink *aGLink, FGLvarInitInfo* info);
	GLref bv;					
	GLref cv;			
	long e;						
	long j;								
	//NameRealType D;			real data type of subclass
	//NameIdealType iD			ideal data type of subclass
	long recycleBlockSize;
	Boolean recycleReal;		// recycling support
	Boolean recycleIdeal;
	FGLrecycleStack *recycle[GLflags];		
	FGLrecycleStack *rDead;
	FGLrecycleStack *iDead;			
	long IDactiveMax;			// CGLwalkNode support		
	long IDnoActive;			
	unsigned long IDcounter;		
	long Packets[GLflags];		// Memory fine-tuning support	
	long Seeks[GLflags];		
	long Total[GLflags];		
	long InUse[GLflags];

	Boolean useD;
	Boolean useiD;	
	long DtypeSize;			
	long iDtypeSize; 
	long backYardOffset[GLflags];	
	long backYardSize[GLflags];		

	GLref ivLutRef;				// location of current ideal vertex Lut packet
	long noEntries[GLflags];
	long packetSize[GLflags];
	long dataOffset[GLflags];
	long ownerOffset[GLflags];

	long ChildNoEntries;
	Boolean **IDinUse;			
	long **currentID;		
	FList *activeNodes;

// INITIALIZATION
	~FGLvarInterface(void);
	virtual void ComputePacketSizes(void);
	virtual void xFlush(short vertexType);
	virtual Boolean isDead(void);
			
//ERROR DETECTION
	virtual void varErr(int ID);
	virtual	Ptr xCheckPtr(const GLref& addr);	
	
// DISK I/O
	virtual Boolean thisRead(int refnum);	
	virtual Boolean thisWrite(int refnum);
	
};

#endif


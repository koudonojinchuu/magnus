/*******************

Written by Frank Rimlinger, August 1992

*******************/

#ifndef Frank_FGLvarLow
#define Frank_FGLvarLow

#include "FGLvarInterface.h"
class FGLrecycleStack;

class FGLvarLow: public FGLvarInterface{


// LOW LEVEL SUPPORT FOR USER METHODS
public:
	FGLvarLow(FGLink *aGLink, FGLvarInitInfo *info);
	virtual GLflag xPacketTypeToFlag(short packetType,const GLref& vRef);
	virtual void xSetFlags(Ptr DEREFb,Ptr DEREFc,GLflag& bflag, 
						  GLflag& cflag);
	virtual GLflag xLnkFlag(GLflag flag,Ptr DEREF);	
	virtual void xPacketInit(GLflag flag,Ptr DEREF,long base,
							GLref prev,const GLref& next,const GLref& owner);    					                 
	virtual void newPacket(GLref& ref,GLflag flag);	
	virtual Boolean packetWrite(writePacketInfo *info);
	virtual Boolean seekWrite(writePacketInfo *info);  
	virtual Boolean packetRead(readPacketInfo *info);
	Boolean xGetPacketisGarbage(Ptr DEREF);
	void xPutPacketisGarbage(Ptr DEREF,Boolean value);
	Boolean xGetPacketisIdeal(Ptr DEREF);
	void xPutPacketisIdeal(Ptr DEREF,Boolean value);
	void xGetPacketNext(GLref& nextRef,Ptr DEREF);
	void xPutPacketNext(Ptr DEREF,const GLref& nextRef);
	void xGetPacketPrev(GLref& prevRef,Ptr DEREF);
	void xPutPacketPrev(Ptr DEREF,const GLref &prevRef);
	void xGetPacketOwner(GLref& ownerRef,GLflag flag,Ptr DEREF);
	void xPutPacketOwner(GLflag flag,Ptr DEREF,const GLref& ownerRef);
	void xEdgeEntryToOwner(GLref& ownerRef,const GLref& refDatum);
	Ptr	xEdgeEntryToPacketPtr(GLflag flag, const GLref& refDatum);
	Ptr xEdgeEntryToOppEdgePtr(GLflag flag, const GLref& refDatum, long modIndex);
	void xPacketRefToOwnerRef(GLref& ownerRef,GLflag flag,const GLref& packetRef);
	long xGetPacketBase(Ptr DEREF);
	void xPutPacketBase(Ptr DEREF,long base);
	void xGetPacketRef(GLref& ref,GLflag flag,Ptr DEREF);
	void xPutPacketRef(GLflag flag, Ptr DEREF,const GLref& ref);
	long xGetPacketIdealindex(Ptr DEREF);
	void xPutPacketIdealindex(Ptr DEREF,long idealindex);
	
	virtual void xsetToFirstPacket(GLref& ref,const GLref& startRef);
	virtual void xsetToLastPacket(GLref& ref,const GLref& startRef);

			 
			virtual void xIndexToGLploc(GLploc& p,GLflag flag,long index); 
			virtual long xmodIndexToOffset(GLflag flag,long modIndex);
			virtual void xfindBaseOrNextPacket(GLref& ref,Boolean overShoot,
										   long base);
		
			virtual void xPutPacket(GLflag flag,Ptr DEREF,long modIndex,  
					    			const GLref& Ref);  
	
// SUPPORT FOR THE WALKS WITH ID
 
			public: //these are invoked within subclasses of CGLnode
			
/*CHECK*/	virtual long xGetID(Ptr DEREF,long IDindex);
/*CHECK*/	virtual void xPutID(Ptr DEREF,long IDindex,long IDcounter);
/*CHECK*/	virtual long reserveSlot(void);
/*CHECK*/	virtual void cancelSlot(long slot);

		

// SCANNING ROUTINES, FOR RAPID, PAINLESS PACKET SEARCHING
			public:
			virtual void xStartSingleEntryScan( scanSingleInfo *info);
			virtual	void xScanSingleEntries( scanSingleInfo *info);
			virtual	void xSkipSingleEntry( scanSingleInfo *info);
			
			virtual void xStartDoubleEntryScan( scanDoubleInfo *info);
			virtual	void xScanDoubleEntries( scanDoubleInfo *info);
			virtual	void xSkipDoubleEntry( scanDoubleInfo *info);
			virtual Boolean xScanForEmpty( scanDoubleInfo *info);
			
			virtual long singleEntryToIndex( scanSingleInfo *info);
			virtual long doubleEntryToIndex( scanDoubleInfo *info);
// THE SCANNERS
			virtual Boolean scanSinglePacket( scanSingleInfo *packetInfo,
										 scanSingleFunc func);
			virtual Boolean scanDoublePacket( scanDoubleInfo *packetInfo,
										 scanDoubleFunc func);	

};

#endif


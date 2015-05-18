/*******************

Written by Frank Rimlinger, August 1992

*******************/

#ifndef Frank_FGLvarHigh
#define Frank_FGLvarHigh

#include "FGLvarLow.h"
class FGLrecycleStack;

class FGLvarHigh: public FGLvarLow{


//INTERFACE WITH THE USER GLvar SUBCLASSES
protected: //these are accessed through the GLvar subclasses    
			FGLvarHigh(FGLink *aGLink, FGLvarInitInfo *info);
			virtual void xPutI(void *iD);      
			virtual void xcPutI(void *iD);        
			virtual void xjPutI(void *iD);     
			virtual void xGetI(void *D);      
			virtual void xcGetI(void *D);       
			virtual void xjGetI(void *D);     
			virtual void xPut(void *iD);      
			virtual void xcPut(void *iD); 
			virtual void xGet(void *D); 
			virtual void xcGet(void *D);
// HIGH-LEVEL SUPPORT FOR USER METHODS	
protected: 	virtual Boolean xI(GLref& vertexRef,Boolean createLink);
 			virtual Boolean xjDatum(GLref& refDatum);
			virtual long xNum(short packetType,const GLref& vRef);
			virtual Boolean xQunlinked(const GLref& vRef);
 			virtual Boolean xS(short packetType, GLref& refDatum);
 			virtual Boolean xForLe(short packetType,const GLref& vRef,
 			                       Boolean goForward);
 			virtual Boolean xForLeFlag(GLflag flag,Ptr vDEREF,
 			                           Boolean goForward);
 			virtual Boolean xNorPe(short packetType,const GLref& vRef,
 			                       Boolean goForward);
 			virtual Boolean xNorPeFlag(GLflag flag,Ptr vDEREF,
 			                           Boolean goForward);
			virtual Boolean xidealLutScan(Boolean goForward);
			static Boolean xidealLutFunc( scanSingleInfo* info);
			virtual Boolean xedgeScan(short packetType,Boolean goForward,
                              	   const GLref& vertexRef);
			static Boolean xedgeFunc( scanDoubleInfo* info);
			virtual Boolean xDstng(short packetType,const GLref& vRef,const GLref& refDatum);
			virtual void xLnk(void);
			virtual void xIdentify(GLflag flag);
			virtual void xMergePackets(GLflag flag);
			static Boolean xRedirectFunc(scanDoubleInfo* info);
			virtual	Boolean xUnlnk(short packetType,const GLref& vRef,GLref& othervRef);
			virtual void xFlush(short vertexType);
			virtual void xPack(short packetType,const GLref& vRef);
			virtual Boolean xPackFunc( scanDoubleInfo *info);
			virtual void recyclePackets(GLref& ref, scanDoubleInfo *info);
			virtual Boolean packScan( scanDoubleInfo *info);		
};
#endif

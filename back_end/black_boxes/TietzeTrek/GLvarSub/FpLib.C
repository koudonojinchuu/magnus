#include "crossTrainer.h"

#include "FpLib.h"
#include "FpLibInitInfo.h"

FpLib::FpLib(FGLink *aGLink, FpLibInitInfo *info)
	:FGLvar(aGLink,info){
	containsEmptyPres=FALSE;
	containsCyclicPres=FALSE;
	containsFreeFactorPres=FALSE;
	groupIsFinite=FALSE;
}

void FpLib::halfLink(GLref refDatum){
	// creates reference in base along edge e to refDatum
	// This routine must be used with caution because it
	// creates a "half edge" that potentially may be 
	// trapped as an error in various GLvar routines.
	
	// CAUTION: use halfS() to step along a "half edge"
	// created by halfLink
	
	writePacketInfo info;
	info.flag=GLr;
	info.request=GLunusedOrNew;
	info.index=e;
	info.refDatum=refDatum;
	info.ownerVertexRef=bv;
	Ptr DEREF=this&bv;
	xGetPacketRef(info.entryPacketRef,GLr,DEREF);
	packetWrite(&info);
	xPutPacketRef(GLr,DEREF,info.exitPacketRef);
	this-bv;
}

Boolean FpLib::halfS(void){
	// Steps base along a "half edge" created by halfLink().
	// Note that rS() expects a reference to an edge packet,
	// NOT a vertex.  A word to the wise is sufficient.
	 
	GLref oldRef=bv;
	Ptr DEREF=this&bv;
	Boolean isIdeal=xGetPacketisIdeal(DEREF);
	if(isIdeal)varErr(61);
// set up readPacketInfo
	readPacketInfo info;
	info.flag=GLr;
	info.request=GLdefinedOrBest;
	info.index=e;
	xGetPacketRef(info.entryPacketRef,GLr,DEREF);
	Boolean entryIsDefined=packetRead(&info);
	if(entryIsDefined){	
		bv=info.refDatum;
	}
	xPutPacketRef(GLr,DEREF,info.exitPacketRef);
	this-oldRef;
	return(entryIsDefined);
}

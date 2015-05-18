/*******************

Written by Frank Rimlinger, August 1992

*******************/
#include "crossTrainer.h"
#include "FGLvar.h"
#include "FGLink.h"
#include "GLfunctions.h"
#include "FGLrecycleStack.h"

FGLvar::FGLvar(FGLink *aGLink, FGLvarInitInfo *info)
	:FGLvarHigh(aGLink,info){
	// we just need to pass along the constructor arguments
	// to the base class 
}

// MOVING AROUND IN THE GRAPH
Boolean FGLvar::jN(void){
	// sets j to next defined ideal vertex index 
	// if j<0, sets j to first defined ideal vertex index
	// returns FALSE if no such index
	
	Boolean goForward=TRUE;

	return(xidealLutScan(goForward));
}

Boolean FGLvar::jP(void){
	// sets j to previously defined ideal vertex index 
	// returns FALSE if no such index, or if j²0.
	
	Boolean goForward=FALSE;
	return(xidealLutScan(goForward));
}

Boolean FGLvar::rS(void){
	// Steps base vertex along real edge e 
	// returns FALSE if no such edge	
	GLref ref=bv;
	Boolean edgeIsDefined=xS(GLreal,ref);
	if(edgeIsDefined) bv=ref;
	return(edgeIsDefined);
}

Boolean FGLvar::rcS(void){
	// Steps companion vertex along real edge e 
	// returns FALSE if no such edge
	GLref ref=cv;
	Boolean edgeIsDefined=xS(GLreal,ref);
	if(edgeIsDefined) cv=ref;
	return(edgeIsDefined);
}

Boolean FGLvar::iS(void){
	// Steps base vertex along ideal edge e 
	// returns FALSE if no such edge	
	GLref ref=bv;
	Boolean edgeIsDefined=xS(GLideal,ref);
	if(edgeIsDefined) bv=ref;
	return(edgeIsDefined);
}

Boolean FGLvar::icS(void){
	// Steps companion vertex along distinguished entry of ideal edge e 
	// returns FALSE if no such edge	
	GLref ref=cv;
	Boolean edgeIsDefined=xS(GLideal,ref);
	if(edgeIsDefined) cv=ref;
	return(edgeIsDefined);
}

Boolean FGLvar::rN(void){
	// Distinguishes the next real edge entry of the base
	// and sets e to the corresponding edge index
	// if e=0, sets e to first real edge entry
	// returns FALSE if no such edge
	Boolean returnVal=xedgeScan(GLreal,TRUE,bv);
	return(returnVal);
}
Boolean FGLvar::rcN(void){
	// Distinguishes the next real edge entry of the companion
	// and sets e to the corresponding edge index
	// if e=0, sets e to first real edge entry
	// returns FALSE if no such edge
	Boolean returnVal=xedgeScan(GLreal,TRUE,cv);
	return returnVal;
}
Boolean FGLvar::rP(void){
	// Distinguishes the previous real edge entry of the base and
	// and sets e to the corresponding edge index
	// returns FALSE if no such edge, or if e=0
	Boolean returnVal=xedgeScan(GLreal,FALSE,bv);
	return(returnVal);
}
Boolean FGLvar::rcP(void){
	// Distinguishes the previous real edge entry of the companion
	// and sets e to the corresponding edge index
 	// returns FALSE if no such edge, or if e=0
	Boolean returnVal=xedgeScan(GLreal,FALSE,cv);
	return(returnVal);
}
Boolean FGLvar::iN(void){
	// Distinguishes the next ideal edge entry of the base 
	// and sets e to the corresponding edge index
	// if e=0, sets e to first ideal edge entry
	// returns FALSE if no such edge
	Boolean returnVal=xedgeScan(GLideal,TRUE,bv);
	return(returnVal);
}
Boolean FGLvar::icN(void){
	// Distinguishes the next ideal edge entry of the companion 
	// and sets e to the corresponding edge index
	// if e=0, sets e to first ideal edge entry
	// returns FALSE if no such edge
	Boolean returnVal=xedgeScan(GLideal,TRUE,cv);
	return(returnVal);
}

Boolean FGLvar::iP(void){
	// Distinguishes the previous ideal edge entry of the base 
	// and sets e to the corresponding edge index
	// returns FALSE if no such edge, or if e=0
	Boolean returnVal=xedgeScan(GLideal,FALSE,bv);
	return(returnVal);
}

Boolean FGLvar::icP(void){
	// Distinguishes the previous ideal edge entry of the 
	// companion and sets e to the corresponding edge index
	// returns FALSE if no such edge, or if e=0	
	Boolean returnVal=xedgeScan(GLideal,FALSE,cv);
	return(returnVal);
}

Boolean FGLvar::rFe(void){
	// Distinguishes the first real edge entry 
	// of the base with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xForLe(GLreal,bv,FALSE);
	return(returnVal);
}
Boolean FGLvar::rNe(void){
	// Distinguishes the next real edge entry 
	// of the base with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xNorPe(GLreal,bv,TRUE);
	return(returnVal);
}
Boolean FGLvar::rPe(void){
	// Distinguishes the previous real edge entry 
	// of the base with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xNorPe(GLreal,bv,FALSE);
	return(returnVal);
}
Boolean FGLvar::rLe(void){
	// Distinguishes the last real edge entry 
	// of the base with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xForLe(GLreal,bv,TRUE);
	return(returnVal);
}
Boolean FGLvar::rcFe(void){
	// Distinguishes the first real edge entry 
	// of the companion with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xForLe(GLreal,cv,FALSE);
	return(returnVal);
}
Boolean FGLvar::rcNe(void){
	// Distinguishes the next real edge entry 
	// of the companion with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xNorPe(GLreal,cv,TRUE);
	return(returnVal);
}
Boolean  FGLvar::rcPe(void){
	// Distinguishes the previous real edge entry 
	// of the companion with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xNorPe(GLreal,cv,FALSE);
	return(returnVal);
}
Boolean  FGLvar::rcLe(void){
	// Distinguishes the last real edge entry 
	// of the companion with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xForLe(GLreal,cv,TRUE);
	return(returnVal);
}
Boolean  FGLvar::iFe(void){
	// Distinguishes the first ideal edge entry 
	// of the base with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xForLe(GLideal,bv,FALSE);
	return(returnVal);
}
Boolean  FGLvar::iNe(void){
	// Distinguishes the next ideal edge entry 
	// of the base with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xNorPe(GLideal,bv,TRUE);
	return(returnVal);
}
Boolean  FGLvar::iPe(void){
	// Distinguishes the previous ideal edge entry 
	// of the base with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xNorPe(GLideal,bv,FALSE);
	return(returnVal);
}
Boolean  FGLvar::iLe(void){
	// Distinguishes the last ideal edge entry 
	// of the base with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xForLe(GLideal,bv,TRUE);
	return(returnVal);
}
Boolean  FGLvar::icFe(void){
	// Distinguishes the first ideal edge entry 
	// of the companion with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xForLe(GLideal,cv,FALSE);
	return(returnVal);
}
Boolean  FGLvar::icNe(void){
	// Distinguishes the next ideal edge entry 
	// of the companion with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xNorPe(GLideal,cv,TRUE);
	return(returnVal);
}
Boolean  FGLvar::icPe(void){
	// Distinguishes the previous ideal edge entry 
	// of the companion with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xNorPe(GLideal,cv,FALSE);
	return(returnVal);
}
Boolean  FGLvar::icLe(void){
	// Distinguishes the last ideal edge entry 
	// of the companion with edge index e.
	// returns FALSE if no such edge
	Boolean returnVal=xForLe(GLideal,cv,TRUE);
	return(returnVal);
}

// QUERIES

Boolean FGLvar::Qempty(void){
	// returns TRUE if no data has been put in the base vertex
	Ptr DEREF=this&bv;
	Boolean returnVal=xGetPacketisGarbage(DEREF);
	this-bv;
	return returnVal;
}

Boolean FGLvar::cQempty(void){
	// returns TRUE if no data has been put in the companion vertex
	Ptr DEREF=this&cv;
	Boolean returnVal=xGetPacketisGarbage(DEREF);
	this-cv;
	return returnVal;
}

Boolean FGLvar::Qideal(void){
	// returns TRUE if base vertex is ideal 
	Ptr DEREF=this&bv;
	Boolean returnVal=xGetPacketisIdeal(DEREF);
	this-bv;
	return returnVal;
}
Boolean FGLvar::cQideal(void){
	// returns TRUE if companion vertex is ideal 
	Ptr DEREF=this&cv;
	Boolean returnVal=xGetPacketisIdeal(DEREF);
	this-cv;
	return returnVal;
}
Boolean FGLvar::jQideal(void){
	// returns TRUE if j indicates an ideal vertex 
	GLref notUsed;
	return(xjDatum(notUsed));
}

Boolean FGLvar::QrealTypeEdge(void){
	// returns TRUE if the base and companion both real 
	Ptr DEREFb=this&bv;
	Ptr DEREFc=this&cv;
	Boolean returnVal=(!xGetPacketisIdeal(DEREFb)) && (!xGetPacketisIdeal(DEREFc));
	this-bv;
	this-cv;
	return returnVal;
}
long FGLvar::rQnum(void){
	// returns number of edges with index e in the real link of base vertex 
	Boolean returnVal=xNum(GLreal,bv);
	return returnVal;
}

long FGLvar::rcQnum(void){
	// returns number of edges with index e in the real link of companion vertex
	Boolean returnVal=xNum(GLreal,cv);
	return returnVal;
}
long FGLvar::iQnum(void){
	// returns of edges with index e in the ideal the link of base vertex 

	Boolean returnVal=xNum(GLideal,bv);
	return returnVal;
} 

long FGLvar::icQnum(void){
	// returns of edges with index e in the ideal link of companion vertex 
	Boolean returnVal=xNum(GLideal,cv);
	return returnVal;
} 

Boolean FGLvar::Qunlinked(void){
	// returns TRUE if base is unlinked
	long eSave;
	eSave=e;
	e=0;
	if(!rN()){
		if(!iN()){
			e=eSave;
			return(TRUE);
		}
	}
	e=eSave;
	return(FALSE);
}

Boolean FGLvar::cQunlinked(void){
	// returns TRUE if companion is unlinked
	long eSave;
	eSave=e;
	e=0;
	if(!rcN()){
		if(!icN()){
			e=eSave;
			return(TRUE);
		}
	}
	e=eSave;
	return(FALSE);
}

#include "FGLink.h"
#include "GLfunctions.h"
#include "debugGLink.h"

// USER METHODS.  

// miscellaneous routines for vertices

void FGLvar::Clear(void){
	// makes the base vertex empty
	Ptr DEREF=this&bv;
	xPutPacketisGarbage(DEREF,TRUE);
	this-bv;
}

void FGLvar::cClear(void){
	// makes the companion vertex empty
	Ptr DEREF=this&cv;
	xPutPacketisGarbage(DEREF,TRUE);
	this-cv;
}

// IDEAL VERTICES

Boolean FGLvar::I(void){
	// creates jÕth ideal vertex, sets bv to its location
	// returns FALSE if vertex already exists
	GLref ref=bv;
	Boolean vertexOK=xI(ref,TRUE);
	if(vertexOK) bv=ref;
	return(vertexOK);
}

Boolean FGLvar::Iunlinked(void){
	// creates unlinked ideal vertex, sets bv to its location
	// returns TRUE
	Boolean vertexOK;
	GLref ref=bv;
	vertexOK=xI(ref,FALSE);
	if(vertexOK) bv=ref;
	return(vertexOK);
}


Boolean FGLvar::cI(void){
	// creates jÕth ideal vertex, sets cv to its location
	// returns FALSE if vertex already exists
	Boolean vertexOK;
	GLref ref=cv;
	vertexOK=xI(ref,TRUE);
	if(vertexOK) cv=ref;
	return(vertexOK);
}

Boolean FGLvar::cIunlinked(void){
	// creates unlinked ideal vertex, sets cv to its location
	// returns TRUE
	Boolean vertexOK;
	GLref ref=cv;
	vertexOK=xI(ref,FALSE);
	if(vertexOK) cv=ref;
	return(vertexOK);
}


void FGLvar::PutI(void){
	// Puts data from iD into ideal vertex at bv 
	// raises error if bv is real
	// OVERRIDE using template in GLsubclasses
	SubclassResponsibility();
}

void FGLvar::cPutI(void){
	// Puts data from iD into ideal vertex at cv 
	// raises error if cv is real
	// OVERRIDE using template in GLsubclasses
	SubclassResponsibility();
}

void FGLvar::jPutI(void){
	// Puts data from iD into j'th ideal vertex
	// raises error if no such vertex 
	// OVERRIDE using template in GLsubclasses
	SubclassResponsibility();
}

void FGLvar::GetI(void){
	// Gets data from ideal vertex at bv, stores in iD 
	// raises error if bv is real or empty
	// OVERRIDE using template in GLsubclasses
	SubclassResponsibility();
}
void FGLvar::cGetI(void){
	// Gets data from ideal vertex at cv, stores in iD 
	// raises error if cv is real or empty
	// OVERRIDE using template in GLsubclasses
	SubclassResponsibility();
}
void FGLvar::jGetI(void){
	// Gets data from j'th ideal vertex, stores in iD 
	// raises error if vertex empty or no such vertex
	// OVERRIDE using template in GLsubclasses
	SubclassResponsibility();
}

long FGLvar::jVal(void){
	// returns index number of ideal vertex at bv
	// raises error if bv not ideal
	Ptr DEREF=this&bv;
	if(!xGetPacketisIdeal(DEREF)){
		varErr(8);
	}
	long index=xGetPacketIdealindex(DEREF);
	this-bv;
	return(index);
}

long FGLvar::jcVal(void){
	// returns index number of ideal vertex at cv
	// raises error if cv not ideal
	Ptr DEREF=this&cv;
	if(!xGetPacketisIdeal(DEREF)){
		varErr(8);
	}
	long index=xGetPacketIdealindex(DEREF);
	this-cv;
	return(index);
}


Boolean FGLvar::jLoc(void){
	// sets bv to the location of the j'th ideal vertex,
	// if it exists.  Returns FALSE if no such vertex.
	GLref ref;
	Boolean vertexOK=xjDatum(ref);
	if(vertexOK) bv=ref;
	return(vertexOK);
}

Boolean FGLvar::jcLoc(void){
	// sets cv to the location of the j'th ideal vertex,
	// if it exists.  Returns FALSE if no such vertex.
	
	GLref ref;
	Boolean vertexOK=xjDatum(ref);
	if(vertexOK) cv=ref;
	return(vertexOK);
}

// REAL VERTICES

void FGLvar::V(void){
	// creates real vertex, stores location in cv 
	// links bv to cv with edge index e
	// distinguishes the new edge entry in the 
	// bv and cv edge look-up tables
	
// make sure base is OK
	if(isNull(bv)){
		varErr(32);
	}
// get space
	GLref ref;
	newPacket(ref,GLrv);
// set cv
	cv=ref;
// initialize the real vertex
	Ptr DEREF=this&cv;
	xPutPacketisGarbage(DEREF,TRUE);
	xPutPacketisIdeal(DEREF,FALSE);
	GLref NA=FGLink::NA;
	xPutPacketRef(GLir,DEREF,NA);
	xPutPacketRef(GLr,DEREF,NA);
	DEREF=&DEREF[2*sizeof(Boolean)+2*sizeof(GLref)];
	xUnsignedLongZeroInit(DEREF, IDactiveMax);
	this-cv;
// set up base as initial vertex
	DEREF=this&bv;
	Boolean isbvIdeal=xGetPacketisIdeal(DEREF);
	GLflag bflag;
	if(isbvIdeal){
		bflag=GLi;		//an ideal vertex always uses the ideal link
	}
	else {
		bflag=GLr;		//two real vertices use the real link
	}
// set up info for packetWrite.  The refDatum is garbage for this write
// because we don't know what to put here yet
	writePacketInfo info;
	info.flag=bflag;
	info.request=GLunusedOrNew;
	info.index=e;
	info.refDatum=bv;		// this is a "fake" value.  Any valid non null
							// address would do
	info.ownerVertexRef=bv;
	xGetPacketRef(info.entryPacketRef,bflag,DEREF);

	packetWrite(&info);
	GLref bpacketRef=info.exitPacketRef;
	long boffset=info.offset;
	xPutPacketRef(bflag,DEREF,bpacketRef);
	this-bv;
/*set up companion as terminal vertex*/
	GLflag cflag=bflag;
	if(bflag==GLi) cflag=GLir;  //a real vertex connected to an ideal vertex
						     //uses the ideal link of a real vertex
// set up info for packetWrite
	DEREF=this&cv;
	info.flag=cflag;
	info.request=GLunusedOrNew;
	info.index= -e;
	xPacketRefToOwnerRef(info.refDatum,bflag,bpacketRef);
	info.ownerVertexRef=cv;
	xGetPacketRef(info.entryPacketRef,cflag,DEREF);
	packetWrite(&info);
	GLref cpacketRef=info.exitPacketRef;
	xPutPacketRef(cflag,DEREF,cpacketRef);
// now we can set the datum in the base vertex packet
	GLref bDatum;
	xPacketRefToOwnerRef(bDatum,cflag,cpacketRef);
	this-cv;
	DEREF=this&bpacketRef;
	BlockMove((Ptr)&bDatum,&DEREF[boffset],sizeof(GLref));
	this-bpacketRef;
}

void FGLvar::Put(void){
	// Puts data from D into real vertex at bv 
	// raises error if bv is ideal
	// OVERRIDE using template in GLsubclasses
	SubclassResponsibility();
}
void FGLvar::cPut(void){
	// Puts data from D into real vertex at cv 
	// raises error if cv is ideal
	// OVERRIDE using template in GLsubclasses
	SubclassResponsibility();
}
void FGLvar::Get(void){
	// Gets data from real vertex at bv, stores in D 
	// raises error if bv is ideal or empty
	// OVERRIDE using template in GLsubclasses
	SubclassResponsibility();
}
void FGLvar::cGet(void){
	// Gets data from real vertex at cv, stores in D 
	// raises error if cv is ideal or empty
	// OVERRIDE using template in GLsubclasses
	SubclassResponsibility();
}




// MODIFYING THE STRUCTURE OF THE GRAPH

void FGLvar::Lnk(void){
	// creates edge with base e and companion -e link indices
	// distinguishes the new edge entry in the 
	// bv and cv edge look-up tables
	xLnk();
}

Boolean FGLvar::rUnlnk(void){
	// removes edge with distinguished real edge index e in base vertex
	// sets cv to the location of the other endpoint of this edge.
	// returns TRUE if edge found

	GLref ref;
	Boolean returnVal=xUnlnk(GLreal,bv,ref);
	if(returnVal){
		cv=ref;
	}
	return(returnVal);
}

Boolean FGLvar::rcUnlnk(void){
	// removes edge with distinguished real edge index e in companion vertex
	// sets bv to the location of the other endpoint of this edge
	// returns TRUE if edge found

	GLref ref;
	Boolean returnVal=xUnlnk(GLreal,cv,ref);
	if(returnVal){
		bv=ref;
	}	
	return(returnVal);
}

Boolean FGLvar::iUnlnk(void){
	// removes edge with distinguished ideal edge index e in base vertex
	// sets cv to the location of the other endpoint of this edge.
	// returns TRUE if edge found

	GLref ref;
	Boolean returnVal=xUnlnk(GLideal,bv,ref);
	if(returnVal){
		cv=ref;
	}	
	return(returnVal);
}

Boolean FGLvar::icUnlnk(void){
	// removes edge with distinguished ideal edge index e in companion vertex
	// sets bv to the location of the other endpoint of this edge
	// returns TRUE if edge found
	
	GLref ref;
	Boolean returnVal=xUnlnk(GLideal,cv,ref);
	if(returnVal){
		bv=ref;
	}	
	return(returnVal);
}
void FGLvar::Identify(void){
	// redirects all edges meeting companion to base 
	// if companion is real and recycleReal is TRUE, 
	// recycles companion and sets cv to FGLink::NA
	// Otherwise places companion on the dead stack
	// raises error if base and companion are not both real vertices
	// does nothing if bv=cv at entry-time

	GLref bvSave;
	if(Qideal() || cQideal()) varErr(43);
	if(SameAddr(bv,cv)){
		return;
	}
	xIdentify(GLr);
	xIdentify(GLir);
	bvSave=bv;
	bv=cv;
	Pack(); 
	cv=bv; 
	bv=bvSave;
}



Boolean FGLvar::jRemove(void){
	// makes j'th slot in the ideal vertex lut undefined
	// if recycleIdeal is TRUE,
	// 	 	recycles jÕth ideal vertex.
	// 		sets bv to FGLink::NA if it is the entry-time location of the jÕth vertex
	// 		sets cv to FGLink::NA if it is the entry-time location of the jÕth vertex
	// Otherwise places jÕth ideal vertex on the iDead stack.
	// If entry time jÕth ideal vertex not unlinked, raises error  
	// returns FALSE if no such vertex at entry time

	GLref bvSave,jv,NA;
	writePacketInfo info;
	NA=FGLink::NA;
// save bv
	bvSave=bv;
// set bv to j'th ideal vertex
	if(!jLoc())return(FALSE);
// make sure j'th vertex is unlinked
	if(!Qunlinked())varErr(45);
// restore bv
	jv=bv;
	bv=bvSave;
// set up writePacketInfo
	info.flag=GLlut;
	info.request=GLdefined;
	info.index=j;
	info.refDatum=NA;
	info.entryPacketRef=ivLutRef;
	if(!packetWrite(&info))varErr(46);
// recycle
	--Total[GLiv];
	if(recycleIdeal){
		(recycle[GLiv])->recyclePush((FGLvarInterface*)this,jv);
		if(SameAddr(bv,jv))bv=NA;
		if(SameAddr(cv,jv))cv=NA;
	}
	else iDead->recyclePush((FGLvarInterface*)this,jv);

	return(TRUE);
}



void FGLvar::Flush(void){
	// recycles all vertices on the rDead and iDead stacks
	xFlush(GLreal);
	xFlush(GLideal);
}

void FGLvar::rFlush(void){	
	// recycles all vertices on the rDead stack
	xFlush(GLreal);
}

void FGLvar::iFlush(void){	
	// recycles all vertices on the iDead stack
	xFlush(GLideal);
}

Boolean FGLvar::Pack(void){	
	// packs edge-packets of base.  Returns TRUE if base is unlinked.
	//In this event, if base is real
	//	if recycleReal is TRUE, recycles base and sets bv to FGLink::NA
	//	else places base on rDead stack
	
	GLflag flag;
	Boolean isIdeal;
	GLref ref;	
	xPack(GLreal,bv);
	xPack(GLideal,bv);
	flag=xPacketTypeToFlag(GLideal,bv);
	Ptr DEREF=this&bv;
	xGetPacketRef(ref,flag,DEREF);
	if(!isNull(ref)){
		this-bv;
		return(FALSE);
	}
	if(Qideal()){
		this-bv;
		return(TRUE);
	}
	xGetPacketRef(ref,GLr,DEREF);
	this-bv;
	if(!isNull(ref)){
		return(FALSE);
	}
	--Total[GLrv];
	if(recycleReal){
		(recycle[GLrv])->recyclePush((FGLvarInterface*)this,bv);
		bv=FGLink::NA;
	}
	else{
	 	rDead->recyclePush((FGLvarInterface*)this,bv);
	 }
	return(TRUE);
}

void FGLvar::Append(GLref& lastPacket,const GLref& midpoint,const GLref& vRef){
// updates lastPacket
	Ptr DEREF;
	GLref ref;	
	// redirect the ideal edges of vRef to midpoint
	DEREF=this&vRef;
	xGetPacketRef(ref,GLir,DEREF);
	if(!isNull(ref)){
		cv=vRef;
		bv=midpoint;
		xIdentify(GLir);
	}
		
	// get real packet of vRef, cut link, and pack
	
	xGetPacketRef(ref,GLr,DEREF);
	xPutPacketRef(GLr,DEREF,FGLink::NA);
	this-vRef;
	bv=vRef;
	Pack();
	
	// rewind to first packet and link to last packet of midpoint
	xsetToFirstPacket(ref,ref);
	DEREF=this&ref;
	xPutPacketPrev(DEREF,lastPacket);
	this-ref;
	DEREF=this&lastPacket;
	xPutPacketNext(DEREF,ref);
	this-lastPacket;
	
	// redirect all owner packet locations to midpoint, update lastPacket
	while(!isNull(ref)){
		DEREF=this&ref;
		BlockMove((Ptr)&midpoint,&DEREF[ownerOffset[GLr]],sizeof(GLref));
		lastPacket=ref;
		xGetPacketNext(ref,DEREF);
		this-lastPacket;
	}
}


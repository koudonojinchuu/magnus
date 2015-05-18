/*******************

Written by Frank Rimlinger, August 1992

*******************/

#include "crossTrainer.h"

#include "FGLvarHigh.h"
#include "FGLink.h"
#include "GLfunctions.h"
#include "debugGLink.h"
#include "FGLrecycleStack.h"


//INTERFACE WITH THE USER GLvar SUBCLASSES
FGLvarHigh::FGLvarHigh(FGLink *aGLink, FGLvarInitInfo *info)
	:FGLvarLow(aGLink,info){
	// we just need to pass along the constructor arguments
	// to the base class 
}


void FGLvarHigh::xPutI(void *iD){
	Ptr DEREF;
	if(!useiD){
		varErr(23);
	}
	DEREF=this&bv;
	if(!xGetPacketisIdeal(DEREF)){
		/*vertex not ideal*/
		varErr(8);
	}
	BlockMove((Ptr)iD,&DEREF[dataOffset[GLiv]],iDtypeSize);
	xPutPacketisGarbage(DEREF,FALSE);
	this-bv;
}
void FGLvarHigh::xcPutI(void *iD){
	Ptr DEREF;
	if(!useiD){
		varErr(23);
	}
	DEREF=this&cv;
	if(!xGetPacketisIdeal(DEREF)){
		/*vertex not ideal*/
		varErr(8);
	}
	BlockMove((Ptr)iD,&DEREF[dataOffset[GLiv]],iDtypeSize);
	xPutPacketisGarbage(DEREF,FALSE);
	this-cv;
}
void FGLvarHigh::xjPutI(void *iD){
	Ptr DEREF;
	readPacketInfo info;
	if(!useiD){
		varErr(23);
	}
	// check to see that j'th vertex exists
	// set up info for packetRead
	info.flag=GLlut;
	info.request=GLdefinedOrBest;
	info.index=j;
	info.entryPacketRef=ivLutRef;
	packetRead(&info);
	ivLutRef=info.exitPacketRef;
	DEREF=this&info.refDatum;
	if(!xGetPacketisIdeal(DEREF)){
		/*vertex not ideal*/
		varErr(36);
	}
	BlockMove((Ptr)iD,&DEREF[dataOffset[GLiv]],iDtypeSize);
	xPutPacketisGarbage(DEREF,FALSE);
	this-info.refDatum;
}

void FGLvarHigh::xGetI(void *iD){
	Ptr DEREF;
	if(!useiD){
		varErr(22);
	}
	DEREF=this&bv;
	if(!xGetPacketisIdeal(DEREF)){
		/*vertex not ideal*/
		varErr(8);
	}
	if(xGetPacketisGarbage(DEREF)){
		/*vertex contains garbage*/
		varErr(6);
	}
	BlockMove(&DEREF[dataOffset[GLiv]],(Ptr)iD,iDtypeSize);
	this-bv;
}
void FGLvarHigh::xcGetI(void *iD){
	Ptr DEREF;
	if(!useiD){
		varErr(22);
	}
	DEREF=this&cv;
	if(!xGetPacketisIdeal(DEREF)){
		/*vertex not ideal*/
		varErr(8);
	}
	if(xGetPacketisGarbage(DEREF)){
		/*vertex contains garbage*/
		varErr(6);
	}
	BlockMove(&DEREF[dataOffset[GLiv]],(Ptr)iD,iDtypeSize);
	this-cv;
}
void FGLvarHigh::xjGetI(void *iD){
	Ptr DEREF;
	readPacketInfo info;
	if(!useiD){
		varErr(22);
	}
	// make sure j'th ideal vertex exists
	// set up info for packetRead
	info.flag=GLlut;
	info.request=GLdefinedOrBest;
	info.index=j;
	info.entryPacketRef=ivLutRef;
	if(!packetRead(&info)) varErr(35);
	ivLutRef=info.exitPacketRef;
	DEREF=this&info.refDatum;
	if(!xGetPacketisIdeal(DEREF)){
		/*vertex not ideal*/
		varErr(36);
	}
	if(xGetPacketisGarbage(DEREF)){
		/*vertex contains garbage*/
		varErr(6);
	}
	BlockMove(&DEREF[dataOffset[GLiv]],(Ptr)iD,iDtypeSize);
	this-info.refDatum;
}

void FGLvarHigh::xPut(void *D){
	Ptr DEREF;
	if(!useD){
		varErr(23);
	}
	DEREF=this&bv;
	if(xGetPacketisIdeal(DEREF)){
		/*vertex not real*/
		varErr(7);
	}
	BlockMove((Ptr)D,&DEREF[dataOffset[GLrv]],DtypeSize);
	xPutPacketisGarbage(DEREF,FALSE);
	this-bv;
}
void FGLvarHigh::xcPut(void *D){
	Ptr DEREF;
	if(!useD){
		varErr(23);
	}
	DEREF=this&cv;
	if(xGetPacketisIdeal(DEREF)){
		/*vertex not real*/
		varErr(7);
	}
	BlockMove((Ptr)D,&DEREF[dataOffset[GLrv]],DtypeSize);
	xPutPacketisGarbage(DEREF,FALSE);
	this-cv;
}

void FGLvarHigh::xGet(void *D){
	Ptr DEREF;
	if(!useD){
		varErr(22);
	}
	DEREF=this&bv;
	if(xGetPacketisIdeal(DEREF)){
		/*vertex not real*/
		varErr(7);
	}
	if(xGetPacketisGarbage(DEREF)){
		/*vertex contains garbage*/
		varErr(6);
	}
	BlockMove(&DEREF[dataOffset[GLrv]],(Ptr)D,DtypeSize);
	this-bv;
}
void FGLvarHigh::xcGet(void *D){
	Ptr DEREF;
	if(!useD){
		varErr(22);
	}
	DEREF=this&cv;
	if(xGetPacketisIdeal(DEREF)){
		/*vertex not real*/
		varErr(7);
	}
	if(xGetPacketisGarbage(DEREF)){
		/*vertex contains garbage*/
		varErr(6);
	}
	BlockMove(&DEREF[dataOffset[GLrv]],(Ptr)D,DtypeSize);
	this-cv;
}


// HIGH-LEVEL SUPPORT FOR USER METHODS	
Boolean FGLvarHigh::xI(GLref& vertexRef,Boolean createLink){
	Ptr DEREF;
	GLref NA;
	writePacketInfo info;
	readPacketInfo readInfo;
	NA=FGLink::NA;
	
// if createLInk
// check to see if vertex already exists*/
// set up readInfo for packetRead

	if(createLink){
		readInfo.flag=GLlut;
		readInfo.request=GLdefinedOrBest;
		readInfo.index=j;
		readInfo.entryPacketRef=ivLutRef;
		if(packetRead(&readInfo)){
			ivLutRef=readInfo.exitPacketRef;
			return(FALSE);
		}
		ivLutRef=readInfo.exitPacketRef;  //this is the location of the closest packet
	}
	
// get space
	newPacket(vertexRef,GLiv);
	
// if createLink
// make vertexRef the j'th entry in the ideal vertex lut

	if(createLink){
		// set up info for packetWrite
		info.flag=GLlut;
		info.request=GLunusedOrNew;
		info.index=j;
		info.refDatum=vertexRef;
		info.entryPacketRef=ivLutRef;
		packetWrite(&info);  
		ivLutRef=info.exitPacketRef;
	}
	
// initialize the ideal vertex
	DEREF=this&vertexRef;
	xPutPacketisGarbage(DEREF,TRUE);
	xPutPacketisIdeal(DEREF,TRUE);
	xPutPacketRef(GLi,DEREF,NA);
	if(createLink) xPutPacketIdealindex(DEREF,j);
	else xPutPacketIdealindex(DEREF,-1);  // this will trigger an error if
										  // an attempt is made to take the index
	this-vertexRef;
	return(TRUE);
}

Boolean FGLvarHigh::xjDatum(GLref &refDatum){
	readPacketInfo info;
	Boolean entryIsDefined;
	info.flag=GLlut;
	info.request=GLdefinedOrBest;
	info.index=j;
	info.entryPacketRef=ivLutRef;
	entryIsDefined=packetRead(&info);
	if(entryIsDefined){
		refDatum=info.refDatum;
	}
	ivLutRef=info.exitPacketRef;
	return(entryIsDefined);
}

long FGLvarHigh::xNum(short packetType,const GLref& vRef){
	long count;
	Boolean isIdeal;
	GLflag flag;
	Ptr vDEREF;
	// get flag
	vDEREF=this&vRef;
	isIdeal=xGetPacketisIdeal(vDEREF);
	if(isIdeal&& (packetType==GLreal)){
		this-vRef;
		return(FALSE);
	}
	flag=xPacketTypeToFlag(packetType,vRef);
// go to first entry
	if(!xForLeFlag(flag,vDEREF,FALSE)){
		this-vRef;
		return(0);  // no entries
	}
	count=1;
	while(xNorPeFlag(flag,vDEREF,TRUE)){
		++count;
	}
	this-vRef;
	return(count);
}

Boolean FGLvarHigh::xQunlinked(const GLref& vRef){
	Boolean isIdeal;
	Ptr DEREF;
	GLref ref;
	
//perform a pack to recycle the empty packets
	DEREF=this&vRef;
	isIdeal=xGetPacketisIdeal(DEREF);
	if(!isIdeal){
		xPack(GLreal,vRef);
	}
	xPack(GLideal,vRef);
	if(isIdeal){
		xGetPacketRef(ref,GLi,DEREF);
		this-vRef;
		if(isNull(ref))return(TRUE);
		else return(FALSE);
	}
	xGetPacketRef(ref,GLr,DEREF);
	if(isNull(ref)){
		xGetPacketRef(ref,GLir,DEREF);
		this-vRef;
		if(isNull(ref))return(TRUE);
	}
	this-vRef;
	return(FALSE);
}

Boolean FGLvarHigh::xS(short packetType,GLref& refDatum){
	Ptr DEREF;
	readPacketInfo info;
	Boolean entryIsDefined;
	Boolean isIdeal;
	GLflag flag;
// compute the GLflag;
	DEREF=this&refDatum;
	isIdeal=xGetPacketisIdeal(DEREF);
	if(isIdeal){
		if(packetType==GLreal){
			this-refDatum;
			return(FALSE);  // no real link
		}
		else flag=GLi;
	}
	else{
		if(packetType==GLreal) flag=GLr;
		else flag=GLir;
	}
// set up readPacketInfo
	info.flag=flag;
	info.request=GLdefined;
	info.index=e;
	xGetPacketRef(info.entryPacketRef,flag,DEREF);
	entryIsDefined=packetRead(&info);
	xPutPacketRef(flag,DEREF,info.exitPacketRef);
	this-refDatum;
	if(entryIsDefined){	
		xEdgeEntryToOwner(refDatum,info.refDatum);
	}
	return(entryIsDefined);
}

Boolean FGLvarHigh::xForLe(short packetType,const GLref& vRef,Boolean goForward){
	Ptr vDEREF;
	Boolean isIdeal;
	GLflag flag;
// get flag
	vDEREF=this&vRef;
	isIdeal=xGetPacketisIdeal(vDEREF);
	if(isIdeal&& (packetType==GLreal)){
		this-vRef;
		return(FALSE);
	}
	flag=xPacketTypeToFlag(packetType,vRef);
// now call
	Boolean returnVal=xForLeFlag(flag,vDEREF,goForward);
	this-vRef;
	return returnVal;
}

Boolean FGLvarHigh::xForLeFlag(GLflag flag,Ptr vDEREF,Boolean goForward){
	readPacketInfo info;
	Ptr DEREF;
	Boolean edgeFound;
	long offset,base,newBase;
	GLploc p;
	GLref ref,refDatum;
	
// set up readPacketInfo
	info.flag=flag;
	info.request=GLdefinedOrBest;
	info.index=e;
	xGetPacketRef(info.entryPacketRef,flag,vDEREF);
	edgeFound=packetRead(&info);
	
// distinguish whatever it was that the read came up with
	xPutPacketRef(flag,vDEREF,info.exitPacketRef);
	if(!edgeFound){
		return(FALSE);  //no defined occurrences
	}
//search to beginning or end for defined occurrences
	xIndexToGLploc(p,flag,e);
	base=p.base;
	offset=xmodIndexToOffset(flag,p.modIndex);
	ref=info.exitPacketRef;
	DEREF=this&ref;
	while(TRUE){
		GLref oldRef=ref;
		if(goForward) xGetPacketNext(ref,DEREF);
		else xGetPacketPrev(ref,DEREF);
		this-oldRef;
		if(isNull(ref))return(TRUE);  //search is done
		DEREF=this&ref;
		newBase=xGetPacketBase(DEREF);
		if(newBase!=base){
			this-ref;
			return(TRUE);  //search is done
		}
		BlockMove(&DEREF[offset],(Ptr)&refDatum,sizeof(GLref));
		if(!isNull(refDatum)){
			//distinguish this defined entry
			xPutPacketRef(flag,vDEREF,ref);
		}
	}	
}
Boolean FGLvarHigh::xNorPe(short packetType,const GLref& vRef,Boolean goForward){
// get flag
	Ptr vDEREF=this&vRef;
	Boolean isIdeal=xGetPacketisIdeal(vDEREF);
	if(isIdeal&& (packetType==GLreal)){
		this-vRef;
		return(FALSE);
	}
	GLflag flag=xPacketTypeToFlag(packetType,vRef);
// now call
	Boolean returnVal=xNorPeFlag(flag,vDEREF,goForward);
	this-vRef;
	return returnVal;
}

Boolean FGLvarHigh::xNorPeFlag(GLflag flag,Ptr vDEREF,Boolean goForward){
//check for well defined entry packet
	GLploc p;
	xIndexToGLploc(p,flag,e);
	long base=p.base;
	GLref ref;
	xGetPacketRef(ref,flag,vDEREF);
	if(isNull(ref))return(FALSE);
	Ptr DEREF=this&ref;
	long newBase=xGetPacketBase(DEREF);
	if(newBase!=base)varErr(34);
	long offset=xmodIndexToOffset(flag,p.modIndex);
	while(TRUE){
		GLref oldRef=ref;
		if(goForward) xGetPacketNext(ref,DEREF);
		else xGetPacketPrev(ref,DEREF);
		this-oldRef;
		if(isNull(ref))return(FALSE);  //no entry found
		DEREF=this&ref;
		newBase=xGetPacketBase(DEREF);
		if(newBase!=base){
			this-ref;
			return(FALSE);  //no entry found
		}
		GLref refDatum;
		BlockMove(&DEREF[offset],(Ptr)&refDatum,sizeof(GLref));
		if(!isNull(refDatum)){
			//distinguish this defined entry
			xPutPacketRef(flag,vDEREF,ref);
			this-ref;
			return(TRUE);
		}
	}	
}


Boolean FGLvarHigh::xidealLutScan(Boolean goForward){
    // if j<0 and goForward scans from zero ideal Lut index
    // if j<=0 and !goForward, returns FALSE
    // if next Lut index found, sets j and returns TRUE
    // otherwise returns FALSE
	
// save entry value of j	
	long jSave=j;	

// make sure there is a link to search
	GLref ref=ivLutRef;
	if(isNull(ref)) return(FALSE); //no link
	
// locate the packet for search start	
// and set info.skipFirstEntry and info.modIndexStart

	scanSingleInfo info;
	if(j<0){
		if(goForward){
			xsetToFirstPacket(ref,ref);
			j=0;
			info.skipFirstEntry=FALSE;
		}
		else return(FALSE); //as documented, the case j=0 will be trapped below
	}
	else{
		GLploc p;
		xIndexToGLploc(p,GLlut,j);
		xfindBaseOrNextPacket(ref,goForward,p.base);
		if(isNull(ref)) return(FALSE); // no suitable packet for searching
		Ptr DEREF=this&ref;
		long base=xGetPacketBase(DEREF);
		this-ref;
		if(p.base==base){
			info.skipFirstEntry=TRUE;  // skip old j entry
			info.modIndexStart=j;
		}
		else {
			info.skipFirstEntry=FALSE;  // no existing j entry, so start
										// search at nearest location
			if(goForward){
				info.modIndexStart=0;	// start at beginning of packet
			}
			else info.modIndexStart=noEntries[GLlut]-1; //start at end of packet
		}	
	}
//set scanning info
	info.flag=GLlut;
	info.firstPacket=ref;
	info.goForward=goForward;
//scan for new j
	Boolean edgeFound=scanSinglePacket(&info,(scanSingleFunc)xidealLutFunc);
	if(!edgeFound)j=jSave; //unsuccessful search does not alter j
	return(edgeFound);
}


Boolean FGLvarHigh::xidealLutFunc( register scanSingleInfo* info){
	FGLvarLow *var=(FGLvarLow*)info->var;
	var->ivLutRef=info->newPacket;
	var->j=var->singleEntryToIndex(info);
	return(FALSE); //search for ideal vertex index has concluded
}


Boolean FGLvarHigh::xedgeScan(short packetType,Boolean goForward,
                              const GLref& vertexRef){
                                                    
    // if e=0 and goForward scans from first real edge entry
    // if e=0 and !goForward, returns FALSE
    // if e!=0
	// 		if the packet of vertexRef contains a defined entry for 
	// 		edge index e, then we skip this entry for the scan
	//      If the base of this packet doesn't match that of the e
	// 		We raise the "ambiguous edge" error.  You can't
	//      find the next thing until you found the first or last, and if you
	//		did that you should still be pointing to a packet with the correct
	//      base.  
	// If new entry is found, sets e, distinguishes the entry, returns TRUE
	// If no suitable entry is found we return FALSE.
	
// save entry value of e	
	long eSave=e;	
	
// make sure there is a link to search
	Ptr vDEREF=this&vertexRef;
	if(xGetPacketisIdeal(vDEREF)&&packetType==GLreal){
		this-vertexRef;
		return(FALSE);  //no link
	}
	long flag=xPacketTypeToFlag(packetType,vertexRef);
	GLref ref;
	xGetPacketRef(ref,flag,vDEREF);
	this-vertexRef;
	if(isNull(ref)) return(FALSE); //no link
	
// locate the packet for search start	
// and set info.skipFirstEntry

	GLploc p;
	scanDoubleInfo info;
	if(e==0){
		if(goForward){
			xsetToFirstPacket(ref,ref);
			e=1;
			xIndexToGLploc(p,flag,e);
			info.skipFirstEntry=FALSE;
		}
		else return(FALSE); //as documented
	}
	else{	
		Ptr DEREF=this&ref;
		long base=xGetPacketBase(DEREF);
		this-ref;
		xIndexToGLploc(p,flag,e);
		if(p.base==base){
			info.skipFirstEntry=TRUE;
		}
		else varErr(34);  // can't find the next edge because we are in
						  // a packet where the current e doesn't exist
	}
//set scanning info
	info.flag=flag;
	info.userPtr=(void*)&vertexRef; // responsibility of caller
									// to tie down this reference
	info.firstPacket=ref;
	info.modIndexStart=p.modIndex;
	info.goForward=goForward;
//scan for new e
	Boolean edgeFound=scanDoublePacket(&info,(scanDoubleFunc)xedgeFunc);
	if(!edgeFound) e=eSave; //unsuccessful search does not alter e
	return(edgeFound);
}

Boolean FGLvarHigh::xedgeFunc( register scanDoubleInfo* info){
	GLref ref=*((GLref*)(info->userPtr));
	FGLvarLow *var=(FGLvarLow*)info->var;
	Ptr vDEREF=var&ref;
	var->xPutPacketRef(info->flag,vDEREF,info->newPacket);
	var-ref;	
	var->e=var->doubleEntryToIndex(info);
	return(FALSE); //search for edge index has concluded
}

Boolean FGLvarHigh::xDstng(short packetType,const GLref& vRef,
							const GLref& refDatum){
	if(!xForLe(packetType,vRef,FALSE))return(FALSE);
	GLflag flag=xPacketTypeToFlag(packetType,vRef);
	GLploc p;
	xIndexToGLploc(p,flag,e);
	long offset=xmodIndexToOffset(flag,p.modIndex);
	Ptr vDEREF=this&vRef;
	GLref ref;
	xGetPacketRef(ref,flag,vDEREF);
	Ptr DEREF=this&ref;
	GLref newDatum;
	BlockMove(&DEREF[offset],(Ptr)&newDatum,sizeof(GLref));
	this-ref;
	if(SameAddr(refDatum,newDatum)){
		this-vRef;
		return(TRUE);
	}
	while(TRUE){
		if(!xNorPe(packetType,vRef,TRUE)){
			this-vRef;
			return(FALSE);
		}
		xGetPacketRef(ref,flag,vDEREF);
		DEREF=this&ref;
		BlockMove(&DEREF[offset],(Ptr)&newDatum,sizeof(GLref));	
		this-ref;
		if(SameAddr(refDatum,newDatum)){
			this-vRef;
			return(TRUE);		
		}
	}
}

void FGLvarHigh::xLnk(void){

/*get appropriate flags for the link*/
	Ptr DEREFb=this&bv;
	Ptr DEREFc=this&cv;
	GLflag bflag,cflag;
	xSetFlags(DEREFb,DEREFc,bflag,cflag);

// link base, notice we write garbage to the edge entry because
// we don't know what to put here yet
	writePacketInfo info;
	info.flag=bflag;
	info.request=GLunusedOrNew;
	info.index=e;
	info.refDatum=bv;  	// this is a "fake".  Just needs to be a valid non-null
						// address
	info.ownerVertexRef=bv;
	xGetPacketRef(info.entryPacketRef,bflag,DEREFb);
	packetWrite(&info); 
	GLref bpacketRef=info.exitPacketRef;
	long bOffset=info.offset;
	xPutPacketRef(bflag,DEREFb,bpacketRef);

// link companion, since we know where the base edge entry is we can
// write a valid refDatum in the companion edge entry
	info.flag=cflag;
	info.index=-e;
	info.ownerVertexRef=cv;
	xGetPacketRef(info.entryPacketRef,cflag,DEREFc);
	xPacketRefToOwnerRef(info.refDatum,bflag,bpacketRef);
	packetWrite(&info); 
	xPutPacketRef(cflag,DEREFc,info.exitPacketRef);
	GLref cpacketRef=info.exitPacketRef;

	this-cv;
	this-bv;

// now we can put a valid datum in the base edge entry
	DEREFb=this&bpacketRef;
	GLref bDatum;
	xPacketRefToOwnerRef(bDatum,cflag,cpacketRef);
	BlockMove((Ptr)&bDatum,&DEREFb[bOffset],sizeof(GLref));
	this-bpacketRef;
}


void FGLvarHigh::xIdentify(GLflag flag){

// scans packets, changing owner vertex to bv
// merges the packets of cv with those of bv

	long offsetToOwner=ownerOffset[flag];
	GLref refDatum=bv;
	
//forward scan
	Ptr DEREF=this&cv;
	GLref startRef;
	xGetPacketRef(startRef,flag,DEREF);
	this-cv;
	GLref ref=startRef;
	while(!isNull(ref)){
		DEREF=this&ref;
		BlockMove((Ptr)&refDatum,&DEREF[offsetToOwner],sizeof(GLref));
		GLref oldRef=ref;
		xGetPacketNext(ref,DEREF);
		this-oldRef;
	}
	
// backward scan
	ref=startRef;
	if(!isNull(ref)){
		DEREF=this&ref;
		GLref oldRef=ref;
		xGetPacketPrev(ref,DEREF);
		this-oldRef;
	}
	while(!isNull(ref)){
		DEREF=this&ref;
		BlockMove((Ptr)&refDatum,&DEREF[offsetToOwner],sizeof(GLref));
		GLref oldRef=ref;
		xGetPacketPrev(ref,DEREF);
		this-oldRef;
	}
	
// merge the packets of cv with those of bv
	xMergePackets(flag);
}


void FGLvarHigh::xMergePackets(GLflag flag){

// CAUTION: in order to support xFold1, this routine guarentees
// that the 0-based packets of cv are merged at the end of the
// list of 0-based packets of bv

	GLref bRef,cRef,oldbRef,oldcRef,NA;
	Ptr bDEREF,oldbDEREF,oldcDEREF;
	long bBase,cBase;
	Boolean linkingBase;
	NA=FGLink::NA;
// initialize companion counters
	Ptr cvDEREF=this&cv;
	xGetPacketRef(cRef,flag,cvDEREF);
	if(isNull(cRef)){
		this-cv;
		return; //no link to merge
	}
	xsetToFirstPacket(cRef,cRef);
	Ptr cDEREF=this&cRef;
	cBase=xGetPacketBase(cDEREF);
	
// cut link to companion
	xPutPacketRef(flag,cvDEREF,NA);
	this-cv;

// initialize base counters
	Ptr bvDEREF=this&bv;
	xGetPacketRef(bRef,flag,bvDEREF);
	if(isNull(bRef)){
		xPutPacketRef(flag,bvDEREF,cRef);  //just link the companion
		this-bv;
		this-cRef;
		return;
	}
	xsetToFirstPacket(bRef,bRef);
	bDEREF=this&bRef;
	bBase=xGetPacketBase(bDEREF);
	
	
// establish link to base and set the linkingBase flag
	if(bBase<=cBase){
		linkingBase=TRUE;
	}
	else{
		xPutPacketRef(flag,bvDEREF,cRef);
		linkingBase=FALSE;
	}
	this-bv;
// merge 
	while(TRUE){
		if(linkingBase){
			oldbRef=bRef;
			oldbDEREF=bDEREF;
			xGetPacketNext(bRef,bDEREF);
			if(isNull(bRef)){
				xPutPacketNext(bDEREF,cRef);
				xPutPacketPrev(cDEREF,oldbRef);
				this-oldbRef;
				this-cRef;
				return;
			}
			bDEREF=this&bRef;
			bBase=xGetPacketBase(bDEREF);
			if(bBase>cBase){
				xPutPacketNext(oldbDEREF,cRef);
				xPutPacketPrev(cDEREF,oldbRef);
				linkingBase=FALSE;
			}
			this-oldbRef;
		}
		else{
			oldcRef=cRef;
			oldcDEREF=cDEREF;
			xGetPacketNext(cRef,cDEREF);
			if(isNull(cRef)){
				xPutPacketNext(cDEREF,bRef);
				xPutPacketPrev(bDEREF,oldcRef);
				this-oldcRef;
				this-bRef;
				return;
			}
			cDEREF=this&cRef;
			cBase=xGetPacketBase(cDEREF);
			if(cBase>bBase){
				xPutPacketNext(oldcDEREF,bRef);
				xPutPacketPrev(bDEREF,oldcRef);
				linkingBase=TRUE;
			}
			this-oldcRef;
		}
	}
}



Boolean FGLvarHigh::xUnlnk(short packetType,const GLref& vRef,GLref& othervRef){

// check for valid request
	Ptr DEREF=this&vRef;
	Boolean isIdeal=xGetPacketisIdeal(DEREF);
	if(isIdeal && packetType==GLreal){
		this-vRef;
		return(FALSE);  // no real link
	}

// get vFlag	
	GLflag vFlag=xPacketTypeToFlag(packetType,vRef);
	
// set up readInfo
	readPacketInfo readInfo;
	readInfo.flag=vFlag;
	readInfo.request=GLdefinedOrBest;
	readInfo.index=e;
	xGetPacketRef(readInfo.entryPacketRef,vFlag,DEREF);
	Boolean edgeDefined=packetRead(&readInfo);
	GLref vPacketRef=readInfo.exitPacketRef;
	xPutPacketRef(vFlag,DEREF,vPacketRef);
	this-vRef;
	if(!edgeDefined) return(FALSE);  //no such edge
	
// load refDatum at othervRef
	GLref vEdgeDatum=readInfo.refDatum;
	GLref oRef;
	xEdgeEntryToOwner(oRef,vEdgeDatum);
	othervRef=oRef;

// get flag for other vertex 
	DEREF=this&oRef;
	GLflag oFlag=xLnkFlag(vFlag,DEREF);
	this-oRef;

	
// erase link from vRef
	
	DEREF=this&vPacketRef;
	GLref NA=FGLink::NA;
	BlockMove((Ptr)&NA,&DEREF[readInfo.offset],sizeof(GLref));
	this-vPacketRef;
	--InUse[vFlag];

// erase link from oRef
	GLploc p;	
	xIndexToGLploc(p,oFlag,-e);

	DEREF=xEdgeEntryToOppEdgePtr(oFlag,vEdgeDatum,p.modIndex);
	BlockMove((Ptr)&NA,DEREF,sizeof(GLref));
	this-vEdgeDatum;
	--InUse[oFlag];

	return(TRUE);
}


void FGLvarHigh::xFlush(short vertexType){
	GLref ref;
	switch(vertexType){
		case GLreal:{
			while(rDead->numItems>0){
				rDead->recyclePop(ref,(FGLvarInterface*)this);
				recycle[GLrv]->recyclePush((FGLvarInterface*)this,ref);
			}
		}
		break;
		case GLideal:{
			while(iDead->numItems>0){
				iDead->recyclePop(ref,(FGLvarInterface*)this);
				recycle[GLiv]->recyclePush((FGLvarInterface*)this,ref);
			}

		}
		break;
		default: varErr(33);
	}
}

void FGLvarHigh::xPack(short packetType,const GLref& vRef){
	Ptr DEREF=this&vRef;
	Boolean isIdeal=xGetPacketisIdeal(DEREF);
	if(isIdeal &&packetType==GLreal){
		this-vRef;
		return;  // no real link
	}
	GLflag flag=xPacketTypeToFlag(packetType,vRef);
	GLref ref;
	xGetPacketRef(ref,flag,DEREF);
	if(isNull(ref)){
		this-vRef;
		return; //no packet to search
	}
	xsetToFirstPacket(ref,ref);
	scanDoubleInfo info;
	info.flag=flag;
	info.firstPacket=ref;
	info.modIndexStart=1;
	info.goForward=TRUE;
	info.skipFirstEntry=FALSE;
	packScan(&info); 
	xPutPacketRef(flag,DEREF,info.firstPacket);
	this-vRef;
}

Boolean FGLvarHigh::xPackFunc( register scanDoubleInfo *info){

// CAUTION:  assumes info->DEREF points into a locked block

// searches forward packets with same base for defined entry at modIndex.
// if entry found, it is set to FGLink::NA and its value is stored at DEREFcurrent,
// and the opposite edge datum is set to the owner vertex location in this packet
// In this event, returns TRUE

// CAUTION: this routine assumes that noEntries[GLi] and noEntries[GLir]
// are equal.  This implies that the corresponding owner offsets are equal
// so that it is possible to locate and update the entry of the opposite edge

	long base=info->base;
	long modIndex;
	if(info->sign>0)modIndex=info->plusModIndex;
	else modIndex=info->minusModIndex;
	long offset=xmodIndexToOffset(info->flag,modIndex);
	Ptr DEREF=info->DEREF;
	GLref ref=FGLink::NA;
	while(TRUE){
		GLref refDatum;
		GLref oldRef=ref;
		char hState;
		xGetPacketNext(ref,DEREF);
		if(!isNull(oldRef))this-oldRef;
		if(isNull(ref)){
			return(FALSE);
		}
		DEREF=this&ref;
		if(xGetPacketBase(DEREF)!=base){
			this-ref;
			return(FALSE);
		}
		BlockMove(&DEREF[offset],(Ptr)&refDatum,sizeof(GLref));
		if(!isNull(refDatum)){
			GLref ownerVertexLoc;
			BlockMove((Ptr)&FGLink::NA,&DEREF[offset],sizeof(GLref));
			this-ref;
			BlockMove((Ptr)&refDatum,&((info->DEREF)[offset]),sizeof(GLref));
// here is where the assumption noEntries[GLi]==noEntries[GLir] is used 
			DEREF=xEdgeEntryToOppEdgePtr(info->flag,refDatum,-modIndex);
			xPacketRefToOwnerRef(ownerVertexLoc,info->flag,info->newPacket);
			BlockMove((Ptr)&ownerVertexLoc,DEREF,sizeof(GLref));	
			this-refDatum;
			return(TRUE);
		}
	}
}


void FGLvarHigh::recyclePackets(GLref& ref,register scanDoubleInfo *info){
// recycles all forward packets with same base.  If preceding packet
// and following packet both exist, they are linked.
// If preceding packet exists, but not following packet, then the
// preceding packet next reference is set to FGLink::NA.
// returns location of following packet, or FGLink::NA if there is no following packet 
// if preceding packet does not exist, sets info->firstPacket to return value

	GLflag flag=info->flag;
	long base=info->base;
	Ptr DEREF=info->DEREF;
	GLref precedingRef;
	xGetPacketPrev(precedingRef,DEREF);
	ref=info->newPacket;
	this+ref;		// need to lock ref because it gets unlocked in the loop
	GLref NA=FGLink::NA;
	while(TRUE){
		(recycle[flag])->recyclePush((FGLvarInterface*)this,ref);
		Total[flag]=Total[flag]-noEntries[flag]*2;
		GLref oldref=ref;
		xGetPacketNext(ref,DEREF);
		this-oldref;
		if(isNull(ref)){
			if(isNull(precedingRef)){
				info->firstPacket=NA;
				return;
			}
			else{
				DEREF=this&precedingRef;
				xPutPacketNext(DEREF,NA);
				this-precedingRef;
				return;
			}
		}
		DEREF=this&ref;
		if(xGetPacketBase(DEREF)!=base){
			if(isNull(precedingRef)){
				this-ref;
				info->firstPacket=ref;
				return;
			}
//link preceding packet to ref
			xPutPacketPrev(DEREF,precedingRef);
			this-ref;
			DEREF=this&precedingRef;
			xPutPacketNext(DEREF,ref);
			this-precedingRef;
			return;
		}
	}
}


// CUSTOM MADE SCAN FOR PACKING

Boolean FGLvarHigh::packScan( register scanDoubleInfo *info){		
	// returns TRUE if func called at least once

	Boolean returnVal=FALSE;
	info->var=this;
	info->newPacket=info->firstPacket;
	while(!isNull(info->newPacket)){
		Boolean hit=FALSE;
		Ptr DEREF=this&info->newPacket;
		GLref nextPacket;
		xGetPacketNext(nextPacket,DEREF);
		info->base=xGetPacketBase(DEREF);
		info->DEREF=DEREF;
		info->modIndexStart=1;
		xStartDoubleEntryScan(info);
		while(info->DEREFcurrent!=NULL){
			if(xScanForEmpty(info))hit=TRUE;
			if(info->DEREFcurrent!=NULL){
// call the passed function
				if(xPackFunc(info))hit=TRUE;
				returnVal=TRUE;
// skip to next entry
				xSkipDoubleEntry(info);
			}
		}
		GLref oldRef=info->newPacket;
		if(!hit){
			recyclePackets(info->newPacket,info);
		}
		else info->newPacket=nextPacket;
		this-oldRef;
	}
	return returnVal;
}




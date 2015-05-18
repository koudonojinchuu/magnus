/*******************

Written by Frank Rimlinger, August 1992

*******************/
#include "crossTrainer.h"
#include "FGLvarLow.h"
#include "FGLink.h"
#include "FBlockManager.h"
#include "GLfunctions.h"
#include "FGLrecycleStack.h"
#include <math.h>
#include <stdlib.h>


// LOW LEVEL SUPPORT FOR USER METHODS
FGLvarLow::FGLvarLow(FGLink *aGLink, FGLvarInitInfo *info)
	:FGLvarInterface(aGLink,info){
	// we just need to pass along the constructor arguments
	// to the base class 
}

GLflag FGLvarLow::xPacketTypeToFlag(short packetType,const GLref& vRef){

//get flag
	Ptr DEREF=this&vRef;
	Boolean isIdeal=xGetPacketisIdeal(DEREF);
	this-vRef;
	if(isIdeal){ 
		if(packetType==GLideal)return(GLi);
		else varErr(41);
	}
	if(packetType==GLreal)return(GLr);
	else if(packetType==GLideal) return(GLir);
	varErr(41);
}

void FGLvarLow::xSetFlags(Ptr DEREFb, Ptr DEREFc, GLflag& bflag, GLflag& cflag){
	bflag=GLr;
	cflag=GLr;
	Boolean bIdeal=xGetPacketisIdeal(DEREFb);
	Boolean cIdeal=xGetPacketisIdeal(DEREFc);
	Boolean edgeIsIdeal= bIdeal || cIdeal;
	if(edgeIsIdeal){
		bflag=cflag=GLi;
		if(!bIdeal)bflag=GLir;
		if(!cIdeal)cflag=GLir;
	}
}

GLflag FGLvarLow::xLnkFlag(GLflag flag,Ptr DEREF){
	//This is a thinking person's routine. 
	//DEREF must be the dereferenced address of a vertex packet.
	//flag must be a packet type for a vertex which could be 
	//joined to DEREF by an edge.  Returns the type of packet
	//that DEREF would maintain to support such an edge.
	
	Boolean isDEREFideal=xGetPacketisIdeal(DEREF);
	if(isDEREFideal)return(GLi); //ideal vertex only has ideal packet
	if(flag==GLi)return(GLir); //real joined to ideal vertex uses ideal packet
	if(flag==GLr)return(GLr);  //real joined to real uses real packet;
	varErr(25); //there are no other valid possibilities
}

void FGLvarLow::xPacketInit(GLflag flag,Ptr DEREF,long base,GLref prev,
							const GLref& next,const GLref& owner){
	xPutPacketBase(DEREF,base);
	xPutPacketPrev(DEREF,prev);
	xPutPacketNext(DEREF,next);
	xPutPacketOwner(flag,DEREF,owner);
	DEREF=&(DEREF[dataOffset[flag]]);
	if(flag==GLlut){
		xNAinit(DEREF,noEntries[GLlut]);
	}
	else{
		xNAinit(DEREF,2*noEntries[flag]);
	}
	
}

void FGLvarLow::newPacket(GLref& ref,GLflag flag){ 

	switch(flag){
		case GLr: 
		case GLir:
		case GLi:Total[flag]=Total[flag]+2*noEntries[flag];
		break;
		case GLlut:Total[flag]=Total[flag]+noEntries[flag];
		break;
		case GLrv:
		case GLiv: ++Total[flag];
		break;
		default: varErr(25);
	}

// attempt to get memory from recycle stack 
// before going to the BlockManager

	(recycle[flag])->recyclePop(ref,(FGLvarInterface*)this);
	if(!isNull(ref)){
		return;
	}
	
// nothing on the recycle stack, so let the BlockManager handle
// the request for space

	FBlockManager* BlockManager=itsGLink->itsBlockManager;
	BlockManager->ReserveSpace(ref,packetSize[flag]);
	if(isNull(ref)) Failure(memFullErr,0);

}

Boolean FGLvarLow::packetWrite(register writePacketInfo *info){
	// 	if request is GLdefined
	//		if there is a defined entry
	//			puts refDatum in this entry, returns TRUE
	//		else returns FALSE
	//	else if request is GLunusedOrNew
	//		if there is an undefined entry
	//			puts refDatum in this entry, returns TRUE
	//		else puts refdatum in new entry, returns FALSE

	GLref NA=FGLink::NA;
	GLflag flag=info->flag;
	GLploc p;
	xIndexToGLploc(p,flag,info->index);
	info->base=p.base;
	info->offset=xmodIndexToOffset(flag,p.modIndex);
	
// get the rest of the writePacketInfo from seekWrite

	Boolean returnVal=seekWrite(info);

// now follow the instructions determined in seekWrite for
// writing the data, and set exitPacketRef if creating new packet

	if(info->request==GLunusedOrNew){
		Ptr DEREF;
		GLref ref;
		switch(info->status){
			case GLcreateInitialPacket:{
				newPacket(ref,flag);
				DEREF=this&ref;
				xPacketInit(flag,DEREF,p.base,NA,NA,info->ownerVertexRef);
			}
			break;
			case GLuseExistingPacket:{
				GLref oldDatum;
				ref=info->existingRef;
				DEREF=this&ref;
			}
			break;
			case GLprependPacket:{
				newPacket(ref,flag);
				DEREF=this&info->smallRef;
				xPutPacketPrev(DEREF,ref);
				this-info->smallRef;
				DEREF=this&ref;
				xPacketInit(flag,DEREF,p.base,NA,info->smallRef,info->ownerVertexRef);
			}
			break;
			case GLinsertPacket:{
				newPacket(ref,flag);
				DEREF=this&info->smallRef;
				xPutPacketNext(DEREF,ref);
				this-info->smallRef;
				DEREF=this&info->bigRef;
				xPutPacketPrev(DEREF,ref);
				this-info->bigRef;
				DEREF=this&ref;
				xPacketInit(flag,DEREF,p.base,info->smallRef,info->bigRef,
				            info->ownerVertexRef);
			}
			break;
			case GLappendPacket:{
				newPacket(ref,flag);
				DEREF=this&info->bigRef;
				xPutPacketNext(DEREF,ref);
				this-info->bigRef;
				DEREF=this&ref;
				xPacketInit(flag,DEREF,p.base,info->bigRef,NA,info->ownerVertexRef);
			}
			break;
			default: varErr(27);
		}
	// put refDatum in packet
		BlockMove((Ptr)&info->refDatum,&DEREF[info->offset],sizeof(GLref));	
		this-ref;
		if(!isNull(info->refDatum))++InUse[flag];
	// set exit packet
		info->exitPacketRef=ref;
	}
	else if(info->request==GLdefined && returnVal){
		// put refDatum in packet
		Ptr DEREF=this&info->exitPacketRef;
		BlockMove((Ptr)&info->refDatum,&DEREF[info->offset],sizeof(GLref));	
		this-info->exitPacketRef;
		if(isNull(info->refDatum))--InUse[flag];
	}
	else{
		varErr(42);
	}
	return(returnVal);
}

Boolean FGLvarLow::seekWrite(register writePacketInfo *info){

	//	SeekWrite determines the values of the following info members
	//
	//	status
	//  smallRef
	//  existingRef
	//  bigRef
	//
	//  With the exception of cases in which packetWrite will create
	//  a new packet, seekWrite sets the value of exitPacketRef
	//
	//  seekWrite returns TRUE or FALSE according to the following scheme
	//
	// 	if request is GLdefined
	//		if there is a defined entry, returns TRUE
	//		else returns FALSE
	//	else if request is GLunusedOrNew
	//		if there is an undefined entry, returns TRUE
	//		else returns FALSE

	if(info->base<0){
		varErr(28);
	}
	
// we begin by setting exitPacketRef to entryPacketRef and checking to
// see if there is a link in the first place

	GLref ref=info->exitPacketRef=info->entryPacketRef;
	if(isNull(ref)){
		info->status=GLcreateInitialPacket;
		return(FALSE);
	}
	++Seeks[info->flag];
	++Packets[info->flag];
	Ptr DEREF=this&ref;
	long base=xGetPacketBase(DEREF);
	long offset=info->offset;
	
// if the entry packet has a different base than info->base,
// then we set exitPacketRef to the location of a closest packet to 
// info->base.
// this is just a first approximation which may be refined according
// to the request

 	if(base!=info->base){
		Boolean goForward= base<info->base;
		Boolean keepSearching=TRUE;
		while(keepSearching){
			GLref oldRef=ref;
			if(goForward) xGetPacketNext(ref,DEREF);
			else xGetPacketPrev(ref,DEREF);
			this-oldRef;
			if(isNull(ref))keepSearching=FALSE;
			else{
				info->exitPacketRef=ref;
				++Packets[info->flag];
				DEREF=this&ref;
				base=xGetPacketBase(DEREF);
				if(base==info->base)keepSearching=FALSE;
				else if(goForward && base>info->base)keepSearching=FALSE;
				else if(!goForward && base<info->base)keepSearching=FALSE;
				if(!keepSearching)this-ref;
			}
		}
	}
	else{
		// ref is still locked if we don't pass through the above logic,
		// so we just unlock it here.
		
		this-ref;
	}

// if the base now equals info->base, then we search for a defined entry
// if the request is GLdefined, and an undefined entry if the request
// is GLundefinedOrNew.  Since we don't know where we are in the link
// of packets with the same base, we search forwards and then backwards
	
	
	if(base==info->base){
		ref=info->exitPacketRef;
		DEREF=this&ref;
		GLref newDatum;
	
	// forward search
		Boolean keepSearching=TRUE;
		while(keepSearching){
			BlockMove(&DEREF[offset],(Ptr)&newDatum,sizeof(GLref));
			switch(info->request){
				case GLdefined:
					if(!isNull(newDatum)){
						info->status=GLuseExistingPacket;
						info->existingRef=ref;
						info->exitPacketRef=ref;
						this-ref;
						return(TRUE);
					}
				break;
				case GLunusedOrNew:
					if(isNull(newDatum)){
						info->status=GLuseExistingPacket;
						info->existingRef=ref;
						info->exitPacketRef=ref;
						this-ref;
						return(TRUE);
					}
				break;
				default: varErr(42);
			}
			GLref oldRef=ref;
			xGetPacketNext(ref,DEREF);
			this-oldRef;
			if(isNull(ref))keepSearching=FALSE;
			else{
				++Packets[info->flag];
				DEREF=this&ref;
				base=xGetPacketBase(DEREF);
				if(base!=info->base){
					this-ref;
					keepSearching=FALSE;
				}
			}
		}
	
	//backward search	
		GLref betterRef=info->exitPacketRef;
		ref=betterRef;
		DEREF=this&ref;
		keepSearching=TRUE;
		while(keepSearching){
			GLref oldRef=ref;
			xGetPacketPrev(ref,DEREF);
			this-oldRef;
			if(isNull(ref))break;
			++Packets[info->flag];
			DEREF=this&ref;
			base=xGetPacketBase(DEREF);
			if(base!=info->base){
				keepSearching=FALSE;
				this-ref;
			}
			else{
				BlockMove(&DEREF[offset],(Ptr)&newDatum,sizeof(GLref));
				switch(info->request){
					case GLdefined:
						if(!isNull(newDatum)){
							info->status=GLuseExistingPacket;
							info->existingRef=ref;
							info->exitPacketRef=ref;
							this-ref;
							return(TRUE);
						}
						break;
					case GLunusedOrNew:
						if(isNull(newDatum)){
							info->status=GLuseExistingPacket;
							info->existingRef=ref;
							info->exitPacketRef=ref;
							this-ref;
							return(TRUE);
						}
						break;
					default: varErr(42);
				}
				betterRef=ref;
			}
		}
					
// at this point, the forward and backward searches have failed to find
// an existing packet that satisfies the request.  But we have found a
// better approximation of exitPacketRef, namely, the last packet searched
// which is in betterRef.  So we set exitPacketRef to betterRef
		
		info->exitPacketRef=betterRef;
	}
	
// There is no existing packet which satisfies the request.  We now
// process the request with a return value of FALSE.  

	switch(info->request){
		case GLdefined: return(FALSE);
		case GLunusedOrNew:{
			ref=info->exitPacketRef;
			DEREF=this&ref;
			base=xGetPacketBase(DEREF);
			if(base>=info->base){
		//here we use the fact that the backward search was
		//performed after the forward search
				GLref prevRef;
				xGetPacketPrev(prevRef,DEREF);
				if(isNull(prevRef)){
					info->status=GLprependPacket;
					info->smallRef=ref;
				}
				else{
					info->status=GLinsertPacket;
					info->smallRef=prevRef;
					info->bigRef=ref;
				}
			}
			else{
				GLref nextRef;
				xGetPacketNext(nextRef,DEREF);
				if(isNull(nextRef)){
					//append
					info->status=GLappendPacket;
					info->bigRef=ref;
				}
				else{
					info->status=GLinsertPacket;
					info->smallRef=ref;
					info->bigRef=nextRef;
				}
			}
			this-ref;
			return(FALSE);
		}
		default: varErr(42);
	}
}

Boolean FGLvarLow::packetRead(register readPacketInfo *info){
	//  puts ownerVertexRef in info
	// 	if request is GLdefinedOrBest
	//		if there is a defined entry
	//			puts refDatum equal to this entry, returns TRUE
	//		else returns FALSE
	//	else if request is GLundefinedOrBest
	//		if there is an undefined entry
	//			returns TRUE
	//		else returns FALSE

// compute base and offset

	GLflag flag=info->flag;
	GLploc p;
	xIndexToGLploc(p,flag,info->index);
	info->base=p.base;
	if(info->base<0){
		varErr(28);
	}
	info->offset=xmodIndexToOffset(flag,p.modIndex);
	
// set exitPacketRef to entryPacketRef and check to
// see if there is a link in the first place

	GLref ref=info->exitPacketRef=info->entryPacketRef;
	if(isNull(ref)) return(FALSE);
	++Seeks[info->flag];
	++Packets[info->flag];
	Ptr DEREF=this&ref;
	long base=xGetPacketBase(DEREF);
	long offset=info->offset;

// if the entry packet has a different base than info->base,
// then we set exitPacketRef to the location of a closest packet to 
// info->base.
// this is just a first approximation which may be refined according
// to the request

 	if(base!=info->base){
		Boolean goForward= base<info->base;
		Boolean keepSearching=TRUE;
		while(keepSearching){
			GLref oldRef=ref;
			if(goForward) xGetPacketNext(ref,DEREF);
			else xGetPacketPrev(ref,DEREF);
			this-oldRef;
			if(isNull(ref))keepSearching=FALSE;
			else{
				info->exitPacketRef=ref;
				++Packets[info->flag];
				DEREF=this&ref;
				base=xGetPacketBase(DEREF);
				if(base==info->base)keepSearching=FALSE;
				else if(goForward && base>info->base)keepSearching=FALSE;
				else if(!goForward && base<info->base)keepSearching=FALSE;
				if(!keepSearching)this-ref;
			}
		}
	}
	else{
		// ref is still locked if we don't pass through the above logic,
		// so we just unlock it here.
		
		this-ref;
	}

// if the base still does not equal info->base, we return FALSE

	if(base!=info->base)return(FALSE);
	
// if the base now equals info->base, then we search for a defined entry
// if the request is GLdefinedOrBest, and an undefined entry if the request
// is GLundefinedOrBest.  Since we don't know where we are in the link
// of packets with the same base, we search forwards and then backwards

	ref=info->exitPacketRef;
	DEREF=this&ref;
	
// forward search
	Boolean keepSearching=TRUE;
	while(keepSearching){
		BlockMove(&DEREF[offset],(Ptr)&info->refDatum,sizeof(GLref));
		switch(info->request){
			case GLdefinedOrBest:
				if(!isNull(info->refDatum)){
					info->exitPacketRef=ref;
					this-ref;
					return(TRUE);
				}
				break;
			case GLundefinedOrBest:
				if(isNull(info->refDatum)){
					info->exitPacketRef=ref;
					this-ref;
					return(TRUE);
				}
				break;
			default: varErr(42);
		}
		GLref oldRef=ref;
		xGetPacketNext(ref,DEREF);
		this-oldRef;
		if(isNull(ref))keepSearching=FALSE;
		else{
			++Packets[info->flag];
			DEREF=this&ref;
			base=xGetPacketBase(DEREF);
			if(base!=info->base){
				keepSearching=FALSE;
				this-ref;
			}
		}
	}

//backward search
	ref=info->exitPacketRef;
	DEREF=this&ref;
	keepSearching=TRUE;
	while(keepSearching){
		GLref oldRef=ref;
		xGetPacketPrev(ref,DEREF);
		this-oldRef;
		if(isNull(ref))return(FALSE);
		++Packets[info->flag];
		DEREF=this&ref;
		base=xGetPacketBase(DEREF);
		if(base!=info->base){
			this-ref;
			return(FALSE);
		}
		BlockMove(&DEREF[offset],(Ptr)&info->refDatum,sizeof(GLref));
		switch(info->request){
			case GLdefinedOrBest:
				if(!isNull(info->refDatum)){
					info->exitPacketRef=ref;
					this-ref;
					return(TRUE);
				}
				break;
			case GLundefinedOrBest:
				if(isNull(info->refDatum)){
					info->exitPacketRef=ref;
					this-ref;
					return(TRUE);
				}
				break;
			default: varErr(42);
		}
	}
}

Boolean FGLvarLow::xGetPacketisGarbage(Ptr DEREF){
	Boolean value;
	BlockMove(DEREF,(Ptr)&value,sizeof(Boolean));
	return(value);
}

void FGLvarLow::xPutPacketisGarbage(Ptr DEREF,Boolean value){
	BlockMove((Ptr)&value,DEREF,sizeof(Boolean));
}
Boolean FGLvarLow::xGetPacketisIdeal(Ptr DEREF){
	Boolean value;
	++DEREF;
	BlockMove(DEREF,(Ptr)&value,sizeof(Boolean));
	return(value);
}

void FGLvarLow::xPutPacketisIdeal(Ptr DEREF,Boolean value){
	++DEREF;
	BlockMove((Ptr)&value,DEREF,sizeof(Boolean));
}
void FGLvarLow::xGetPacketNext(GLref& nextRef,Ptr DEREF){
	DEREF=&DEREF[sizeof(long)+sizeof(GLref)];
	BlockMove(DEREF,(Ptr)&nextRef,sizeof(GLref));
}

void FGLvarLow::xPutPacketNext(Ptr DEREF,const GLref& nextRef){
	DEREF=&DEREF[sizeof(long)+sizeof(GLref)];
	BlockMove((Ptr)&nextRef,DEREF,sizeof(GLref));
}


void FGLvarLow::xGetPacketPrev(GLref& prevRef,Ptr DEREF){
	DEREF=&DEREF[sizeof(long)];
	BlockMove(DEREF,(Ptr)&prevRef,sizeof(GLref));
}

void FGLvarLow::xPutPacketPrev(Ptr DEREF,const GLref& prevRef){
	DEREF=&DEREF[sizeof(long)];
	BlockMove((Ptr)&prevRef,DEREF,sizeof(GLref));
}

void FGLvarLow::xGetPacketOwner(GLref& ownerRef,GLflag flag,Ptr DEREF){
  //CAUTION: ownerRef is garbage if flag is not one of 
  //	     GLr,GLir, or GLi	
	DEREF=&DEREF[ownerOffset[flag]];
	BlockMove(DEREF,(Ptr)&ownerRef,sizeof(GLref));
}

void FGLvarLow::xPutPacketOwner(GLflag flag,Ptr DEREF,const GLref& ownerRef){
// we don't allow this routine to write unless flag is appropriate
	switch(flag){
		case GLr:
		case GLir:
		case GLi:	
			DEREF=&DEREF[ownerOffset[flag]];
			BlockMove((Ptr)&ownerRef,DEREF,sizeof(GLref));
		break;
	}
}

void FGLvarLow::xEdgeEntryToOwner(GLref& ownerRef,const GLref& refDatum){
							// Tricky:
	GLref oldRef=refDatum;  // we must save the input value of refDatum
							// because if ownerRef and refDatum are the same
							// at entry time, the value of refDatum may
							// change after the call to BlockMove.
	Ptr DEREF=this&refDatum;
	BlockMove(DEREF,(Ptr)&ownerRef,sizeof(GLref));
	this-oldRef;
}

Ptr	FGLvarLow::xEdgeEntryToPacketPtr(GLflag flag, const GLref &refDatum){
// CAUTION: this routine locks the block containing refDatum
// Its up to the caller to unlock this block
	Ptr DEREF=this&refDatum;
	DEREF= &DEREF[-ownerOffset[flag]];
	return DEREF;
}

Ptr FGLvarLow::xEdgeEntryToOppEdgePtr(GLflag flag, const GLref& refDatum, 
										long modIndex){
// CAUTION: this routine locks the block containing refDatum
// Its up to the caller to unlock this block
// CAUTION: this routine expects modIndex to refer to the packet in which the
// opposite edge lives.  Its up to the caller to set this correctly

	Ptr DEREF=xEdgeEntryToPacketPtr(flag,refDatum);
	return &DEREF[xmodIndexToOffset(flag,modIndex)];
}

void FGLvarLow::xPacketRefToOwnerRef(GLref& ownerRef,GLflag flag,
									const GLref& packetRef){
	ownerRef.index=packetRef.index;
	ownerRef.offset=packetRef.offset+ownerOffset[flag];
}

long FGLvarLow::xGetPacketBase(Ptr DEREF){
	long base;
	BlockMove(DEREF,(Ptr)&base,sizeof(long));
	return(base);
}

void FGLvarLow::xPutPacketBase(Ptr DEREF,long base){
	BlockMove((Ptr)&base,DEREF,sizeof(long));
}

void FGLvarLow::xGetPacketRef(GLref &ref,GLflag flag,Ptr DEREF){
	switch(flag){
		case GLr: DEREF=&DEREF[2*sizeof(Boolean)+sizeof(GLref)];
		break;
		case GLir:
		case GLi: DEREF=&DEREF[2*sizeof(Boolean)];
		break;
		default: varErr(25);
	}
	BlockMove(DEREF,(Ptr)&ref,sizeof(GLref));
}

void FGLvarLow::xPutPacketRef(GLflag flag, Ptr DEREF,const GLref& ref){
	switch(flag){
		case GLr: DEREF=&DEREF[2*sizeof(Boolean)+sizeof(GLref)];
		break;
		case GLir:
		case GLi: DEREF=&DEREF[2*sizeof(Boolean)];
		break;
		default: varErr(25);
	}
	BlockMove((Ptr)&ref,DEREF,sizeof(GLref));
}



long FGLvarLow::xGetPacketIdealindex(Ptr DEREF){
	DEREF=&DEREF[2*sizeof(Boolean)+sizeof(GLref)];
	long idealindex;
	BlockMove(DEREF,(Ptr)&idealindex,sizeof(long));
	return(idealindex);
}

void FGLvarLow::xPutPacketIdealindex(Ptr DEREF,long idealindex){
	DEREF=&DEREF[2*sizeof(Boolean)+sizeof(GLref)];
	BlockMove((Ptr)&idealindex,DEREF,sizeof(long));
}



void FGLvarLow::xPutPacket(GLflag flag,Ptr DEREF, long modIndex, const GLref &ref){
	long offset=xmodIndexToOffset(flag,modIndex);
	DEREF=&DEREF[offset];
	GLref currentRef;
	BlockMove(DEREF,(Ptr)&currentRef,sizeof(GLref));
	Boolean alreadyInUse=TRUE;
	if((currentRef.index==-1)) alreadyInUse=FALSE;
	BlockMove((Ptr)&ref,DEREF,sizeof(GLref));
	Boolean refNotNA=TRUE;
	if(ref.index==-1)refNotNA=FALSE;
	if((!alreadyInUse)&&refNotNA){ 
		++InUse[flag];
	}
	else if(alreadyInUse && (!refNotNA)){
		--InUse[flag];
	}
}


void FGLvarLow::xIndexToGLploc(GLploc& p,GLflag flag,long index){
	switch(flag){
		case GLr:
		case GLir:
		case GLi:{
			if(index==0){
				varErr(30);
			}
			long bias;
			if(index<0) {
				++index;
				bias= -1;
			}
			else {
				--index;
				bias=1;
			}
			p.modIndex=fmod(index,noEntries[flag]);
			p.base=index-p.modIndex;
			p.modIndex=p.modIndex+bias;
			if(p.base<0){
				p.base= -p.base;
			}
		}
		break;
		case GLlut:{
			/*index must be non-negative*/
			if(index<0){
				varErr(30);
			}
			p.modIndex=fmod(index,noEntries[GLlut]);
			p.base=index-p.modIndex;
			if(p.base<0){
				varErr(28);
			}
		}
		break;
		default: varErr(25);
	}
}

long FGLvarLow::xmodIndexToOffset(GLflag flag,long modIndex){
	long offset,noEntries;
	noEntries=this->noEntries[flag];
	switch(flag){
		case GLr:
		case GLir:
		case GLi:{
			/*modIndex must be in the range 1 to*/
			/*noEntries or -1 to -noEntries*/
			if(modIndex<-noEntries || modIndex==0 || modIndex>noEntries){
				varErr(29);
			}
			long signBias=0;
			if(modIndex<0){
				modIndex= -modIndex;
				signBias=1;
			}
			offset=2*(modIndex-1)+signBias;
			offset=dataOffset[flag]+offset*sizeof(GLref);
		}
		break;
		case GLlut:{
			/*modIndex must be in the range 0 to noEntries-1*/
			if(modIndex<0 || modIndex>noEntries-1){
				varErr(29);
			}
			offset=dataOffset[flag]+modIndex*sizeof(GLref);
		}
		break;
		default: varErr(25);
	}
	return(offset);
		
}

void FGLvarLow::xsetToFirstPacket(GLref& lastRef,const GLref& startRef){
	lastRef=startRef;
	while(TRUE){
		Ptr DEREF=this&lastRef;
		GLref ref;
		xGetPacketPrev(ref,DEREF);
		this-lastRef;
		if(isNull(ref))return;
		lastRef=ref;
	}
}

void FGLvarLow::xsetToLastPacket(GLref& lastRef,const GLref& startRef){
	lastRef=startRef;
	while(TRUE){
		Ptr DEREF=this&lastRef;
		GLref ref;
		xGetPacketNext(ref,DEREF);
		this-lastRef;
		if(isNull(ref))return;
		lastRef=ref;
	}
}


void FGLvarLow::xfindBaseOrNextPacket(GLref& ref,Boolean overShoot,
										   long base){
	// returns first base packet detected 
	// if base not detected returns next packet if overShoot is TRUE
	// and previous packet if overShoot is FALSE
	// returns FGLink::NA if no suitable packet found
	
	Ptr DEREF=this&ref;
	long currentBase=xGetPacketBase(DEREF);
	Boolean goForward= (currentBase<=base);
	while(TRUE){
		if(currentBase==base){
			return;
		}
		GLref oldRef=ref;
		long oldBase=currentBase;
		if(goForward)xGetPacketNext(ref,DEREF);
		else xGetPacketPrev(ref,DEREF);
		this-oldRef;
		if(isNull(ref)){
			if(overShoot){
				if(goForward){
					return;
				}
				else{
					ref=oldRef;
					return;
				}
			}
			else{
				if(goForward){
					ref=oldRef;
					return;
				}
				else{
					return;
				}
			}
		}
		DEREF=this&ref;
		currentBase=xGetPacketBase(DEREF);
		if(overShoot){
			if(goForward){
				if(currentBase>base){
					this-ref;
					return;
				} 
			}
			else{
				if(currentBase<base){
					this-ref;
					ref=oldRef;
					return;
				}
			}
		}
		else{
			if(goForward){
				if(currentBase>base){
					this-ref;
					ref=oldRef;
					return;
				}
			}
			else{
				if(currentBase<base){
					this-ref;
					return;
				}
			}
		}
	} 
}

// SUPPORT FOR THE WALKS WITH ID

long FGLvarLow::xGetID(Ptr DEREF,long IDindex){
	DEREF=&DEREF[2*sizeof(Boolean)+2*sizeof(GLref)];
	DEREF=&DEREF[IDindex*sizeof(long)];
	long IDcounter;
	BlockMove(DEREF,(Ptr)&IDcounter,sizeof(long));
	return(IDcounter);
}

void FGLvarLow::xPutID(Ptr DEREF,long IDindex,long IDcounter){
	DEREF=&DEREF[2*sizeof(Boolean)+2*sizeof(GLref)];
	DEREF=&DEREF[IDindex*sizeof(long)];
	BlockMove((Ptr)&IDcounter,DEREF,sizeof(long));
}

long FGLvarLow::reserveSlot(void){
	//returns the slot
	++IDcounter;
	if(IDcounter==0){
		varErr(38);
	}
	for(long i=0;i<IDactiveMax;++i){
		if(!(*IDinUse)[i]){
			(*IDinUse)[i]=TRUE;
			(*currentID)[i]=IDcounter;
			return(i);
		}
	}
	varErr(39);
	return(noIDslotAvailable);
}

void FGLvarLow::cancelSlot(long slotID){
	(*IDinUse)[slotID]=FALSE;
}


// SCANNING ROUTINES, FOR RAPID, PAINLESS PACKET SEARCHING

// single entry scan routines
void FGLvarLow::xStartSingleEntryScan(register  scanSingleInfo *info){
	long offset=xmodIndexToOffset(GLlut,info->modIndexStart);
	info->modIndex=info->modIndexStart;
	info->DEREFcurrent=&(info->DEREF)[offset];
	if(info->goForward){
		info->DEREFlast=&(info->DEREF)[ownerOffset[GLlut]-(long)sizeof(GLref)];
	}
	else{
		info->DEREFlast=&(info->DEREF)[dataOffset[GLlut]];		
	}
}
void FGLvarLow::xScanSingleEntries( register scanSingleInfo *info){	
	//sets info->DEREFcurrent to NULL if no non-NULL address found
	//searches from info->DEREFcurrent to info->DEREFlast, inclusive
	//info must be initialized with call to xStartStartEntryScan.
	//CAUTION:  Prior to subsequent calls, be sure to update parameters by passing
	//them to xSkipSingleEntry.  Otherwise the last entry read will be read
	//again, which may throw the caller into an infinite loop.
	long GLsize=sizeof(GLref);
	while(TRUE){
		GLref ref;
		BlockMove(info->DEREFcurrent,(Ptr)&ref,GLsize);
		if(!isNull(ref)){
			info->refDatum=ref;
			return;
		}
		xSkipSingleEntry(info);
		if(info->DEREFcurrent==NULL)return;
	}
}

void FGLvarLow::xSkipSingleEntry( register scanSingleInfo *info){
	//sets info->DEREFcurrent to NULL if at last address in packet
	
	long GLsize=sizeof(GLref);
	if(info->DEREFcurrent==info->DEREFlast){
		info->DEREFcurrent=NULL;
		return;
	}
	if(info->goForward){
		info->DEREFcurrent=&(info->DEREFcurrent)[GLsize];
		++info->modIndex;
	}
	else{
		info->DEREFcurrent=&(info->DEREFcurrent)[-GLsize];
		--info->modIndex;
	}
}

//double entry scan routines
void FGLvarLow::xStartDoubleEntryScan( register scanDoubleInfo *info){
	
	long offset=xmodIndexToOffset(info->flag,info->modIndexStart);
	info->DEREFcurrent=&(info->DEREF)[offset];
	if(info->goForward){
		info->DEREFlast=&(info->DEREF)[ownerOffset[info->flag]-(long)sizeof(GLref)];
		if(info->modIndexStart>0){
			info->plusModIndex=info->modIndexStart;
			info->minusModIndex=-info->modIndexStart;
			info->sign=1;
		}
		else{
			info->minusModIndex=info->modIndexStart;
			info->plusModIndex=-info->modIndexStart+1;
			info->sign=-1;
		}
	}
	else{
		info->DEREFlast=&(info->DEREF)[dataOffset[info->flag]];	
		if(info->modIndexStart>0){
			info->plusModIndex=info->modIndexStart;
			info->minusModIndex=-info->modIndexStart+1;
			info->sign=1;
		}
		else{
			info->minusModIndex=info->modIndexStart;
			info->plusModIndex=-info->modIndexStart;
			info->sign=-1;
		}	
	}
}


void FGLvarLow::xSkipDoubleEntry( register scanDoubleInfo *info){
	//sets info->DEREFcurrent to NULL if at last address in packet
	long GLsize=sizeof(GLref);
	if(info->DEREFcurrent==info->DEREFlast){
		info->DEREFcurrent=NULL;
		return;
	}
	if(info->goForward){
		info->DEREFcurrent=&(info->DEREFcurrent)[GLsize];
		if(info->sign>0) ++ info->plusModIndex;
		else -- info->minusModIndex;
	}
	else{
		info->DEREFcurrent=&(info->DEREFcurrent)[-GLsize];
		if(info->sign>0) -- info->plusModIndex;
		else ++ info->minusModIndex;		
	}
	info->sign= -info->sign;
}

long FGLvarLow::singleEntryToIndex( register scanSingleInfo *info){
	return(info->base+info->modIndex);
}

long FGLvarLow::doubleEntryToIndex( register scanDoubleInfo *info){
	long modIndex;
	if(info->sign>0)modIndex=info->plusModIndex;
	else modIndex=info->minusModIndex;
	return(info->sign*info->base  + modIndex);
}



// THE SCANNERS


Boolean FGLvarLow::scanSinglePacket( register scanSingleInfo *info,
										scanSingleFunc func){	
	// returns TRUE if func called at least once
	// CAUTION: See CGLvarTypes.h for input requirements	

	info->var=this;
	
	if(info->skipFirstEntry){
		info->DEREF=this&info->firstPacket;
		xStartSingleEntryScan(info);
		xSkipSingleEntry(info);
		if(info->DEREFcurrent){
			info->newPacket=info->firstPacket;
			info->modIndexStart=info->modIndex;
		}
		else{
			if(info->goForward){
				xGetPacketNext(info->newPacket,info->DEREF);
				info->modIndexStart=0;
			}
			else{
				xGetPacketPrev(info->newPacket,info->DEREF);
				info->modIndexStart= noEntries[info->flag]-1;
			}
		}
		this-info->firstPacket;
	}
	else info->newPacket=info->firstPacket;
	
	Boolean madeCall=FALSE;
	while(!isNull(info->newPacket)){
		info->DEREF=this&info->newPacket;
		info->base=xGetPacketBase(info->DEREF);
		xStartSingleEntryScan(info);
		while(info->DEREFcurrent!=NULL){
			xScanSingleEntries(info);
			if(info->DEREFcurrent!=NULL){
				madeCall=TRUE;
				if(!func(info)){
					return(madeCall);
				}
				xSkipSingleEntry(info);
			}
		}
		GLref nextPacket;
		if(info->goForward){
			xGetPacketNext(nextPacket,info->DEREF);
			info->modIndexStart=0;
		}
		else{
			xGetPacketPrev(nextPacket,info->DEREF);
			info->modIndexStart= noEntries[info->flag]-1;
		}
		this-info->newPacket;
		info->newPacket=nextPacket;
	}
	return(madeCall);
}

Boolean FGLvarLow::scanDoublePacket( register scanDoubleInfo *info,
										scanDoubleFunc func){	
	// returns TRUE if func called at least once
	// CAUTION: See CGLvarTypes.h for input requirements	

	info->var=this;
	
	if(info->skipFirstEntry){
		info->DEREF=this&info->firstPacket;
		xStartDoubleEntryScan(info);
		xSkipDoubleEntry(info);
		if(info->DEREFcurrent){
			info->newPacket=info->firstPacket;
			if(info->sign>0)info->modIndexStart=info->plusModIndex;
			else info->modIndexStart=info->minusModIndex;
		}
		else{
			if(info->goForward){
				xGetPacketNext(info->newPacket,info->DEREF);
				info->modIndexStart=1;
			}
			else{
				xGetPacketPrev(info->newPacket,info->DEREF);
				info->modIndexStart= -noEntries[info->flag];
			}
		}
		this-info->firstPacket;
	}
	else info->newPacket=info->firstPacket;
	
	Boolean madeCall=FALSE;
	while(!isNull(info->newPacket)){
		info->DEREF=this&info->newPacket;
		info->base=xGetPacketBase(info->DEREF);
		xStartDoubleEntryScan(info);
		while(info->DEREFcurrent!=NULL){
			xScanDoubleEntries(info);
			if(info->DEREFcurrent!=NULL){
				madeCall=TRUE;
				if(!func(info)){
					return(madeCall);
				}
				xSkipDoubleEntry(info);
			}
		}
		GLref nextPacket;
		if(info->goForward){
			xGetPacketNext(nextPacket,info->DEREF);
			info->modIndexStart=1;
		}
		else{
			xGetPacketPrev(nextPacket,info->DEREF);
			info->modIndexStart= -noEntries[info->flag];
		}
		this-info->newPacket;
		info->newPacket=nextPacket;
	}
	return(madeCall);
}


// A single packet scanner which does not call a function
// Use this scanner when possible because it is much more efficient

void FGLvarLow::xScanDoubleEntries( register scanDoubleInfo *info){	
	//sets info->DEREFcurrent to NULL if no non-NULL address found
	//searches from info->DEREFcurrent to info->DEREFlast, inclusive
	//
	//
	//CAUTION: info must be initialized with call to xStartDoubleEntryScan.
	// Before calling xStartDoubleEntryScan be sure that info->DEREF is
	//the address of the packet to be searched.  Be sure you have set info->base
	//if you are going to use doubleEntryToIndex.
	//
	//CAUTION:  Prior to subsequent calls, be sure to update info by 
	//calling xSkipDoubleEntry.  Otherwise the last entry read will be read
	//again, which may throw the caller into an infinite loop.
	
	while(TRUE){
		GLref ref;
		BlockMove(info->DEREFcurrent,(Ptr)&ref,sizeof(GLref));
		if(!isNull(ref)){
			info->refDatum=ref;
			return;
		}
		xSkipDoubleEntry(info);
		if(info->DEREFcurrent==NULL)return;
	}
}


//Special scanning routine for finding empty double entires

Boolean FGLvarLow::xScanForEmpty( register scanDoubleInfo *info){	
	//sets info->DEREFcurrent to NULL if no empty address found
	//searches from info->DEREFcurrent to info->DEREFlast, inclusive
	
	//CAUTION: info must be initialized with call to xStartDoubleEntryScan.
	//before calling, be sure info->DEREF points to the packet to be searched
	//Also, if you are going to pass info to doubleEntryToIndex, be sure that
	//info->base is set
	
	//CAUTION:  Prior to subsequent calls, be sure to update info by 
	//calling xSkipDoubleEntry.  Otherwise the last entry read will be read
	//again, which may throw the caller into an infinite loop.
	//returns TRUE if a non-empty address is detected
	
	long GLsize=sizeof(GLref);
	Boolean hit=FALSE;
	while(TRUE){
		GLref ref;
		BlockMove(info->DEREFcurrent,(Ptr)&ref,GLsize);
		if(isNull(ref)){
			return(hit);
		}
		hit=TRUE;
		xSkipDoubleEntry(info);
		if(info->DEREFcurrent==NULL)return(hit);
	}
}


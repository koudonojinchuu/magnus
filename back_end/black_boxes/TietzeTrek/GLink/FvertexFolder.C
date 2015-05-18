#include "crossTrainer.h"
#include "FvertexFolder.h"
#include "FfoldStepper.h"
#include "GLfunctions.h"
#include "FGLink.h"

FvertexFolder::FvertexFolder(FGLvar *v,const GLref& midpoint){
// CAUTION: if this routine sets initOK FALSE then it is done.
// DO NOT run it.

// zero the objects for proper disposal handling
	itsFoldStepper=0;
	itsFoldInfo=0;
	
// check to see if any folding is possible

	Ptr DEREF=v&midpoint;
	GLref ref;
	v->xGetPacketRef(ref,GLr,DEREF);
	if(isNull(ref)){
		// this is an error, shouldn't be passed unlinked vertex
		v-midpoint;
		v->varErr(50);
	}
	v->xsetToFirstPacket(firstPacket,ref);
	v->xsetToLastPacket(lastPacket,ref);
	if(SameAddr(firstPacket,lastPacket)){
		newFolds=0;
		v-midpoint;
		initOK=FALSE;  // only one packet, no folding possible
		return;  
	}
// initializations
	ownerOffset=(v->ownerOffset)[GLr];
	firstPacketOwnerLocation.index=firstPacket.index;
	firstPacketOwnerLocation.offset=firstPacket.offset+ownerOffset;
	itsRecycleStack=(v->recycle)[GLr];
	this->v=v;
	this->midpoint=midpoint;
	entryPoint=foldStart;
	bvSave=v->bv;  // restored when run returns FALSE
	
// set the packetRef of midpoint to the first packet
	v->xPutPacketRef(GLr,DEREF,firstPacket);
	v-midpoint;
	initOK=TRUE;
	return;
}

FvertexFolder::~FvertexFolder(void){
	delete itsFoldStepper;itsFoldStepper=0;
	if(itsFoldInfo){
		for (long i=1;i<=itsFoldInfo->numItems;++i){
			FoldInfo* info=(FoldInfo*)itsFoldInfo->NthItem(i);
			delete info;
		}
		delete itsFoldInfo;
		itsFoldInfo=0;
	}
}

Boolean FvertexFolder::run(void){
	newFolds=0;
	switch(entryPoint){
		case foldStart:{
			// set up the fold info
			long orientedEdgesNo=2*v->noEntries[GLr];
			long edgeRefOffset=v->dataOffset[GLr];
			FList *info= new FList(orientedEdgesNo);
			itsFoldInfo=info;
			for(long i=1;i<=orientedEdgesNo;++i){
				FoldInfo *f=new FoldInfo;
				info->Append((FObject*)f);
			}
			Ptr DEREF=v&firstPacket;
			DEREF=&DEREF[edgeRefOffset];
			for(long i=1;i<=orientedEdgesNo;++i){
				FoldInfo *f=(FoldInfo*)info->NthItem(i);
				GLref refDatum;
				BlockMove(DEREF,(Ptr)&refDatum,sizeof(GLref));
				if(!isNull(refDatum)){
					f->isOccupied=TRUE;
					f->edgeDatum=refDatum;
					v->xEdgeEntryToOwner(refDatum,refDatum);
					
				// isMidpoint must be recomputed on the fly if it is
				// FALSE, because it can go TRUE without warning.
				// Once it is TRUE it stays TRUE.
				
					f->isMidpoint=SameAddr(refDatum,midpoint);					
				}
				else f->isOccupied=FALSE;
				DEREF=&DEREF[sizeof(GLref)];
			}
			// set current packet to the location of the next packet, 
			// which is known to exist
			
			DEREF=v->xCheckPtr(firstPacket); // block already locked, but DEREF
											 // has been altered
			v->xGetPacketNext(currentPacket,DEREF);
			
			// cut the link from the first packet
			v->xPutPacketNext(DEREF,FGLink::NA);
			v-firstPacket;
		
			// set up the stepper
			edgeRefOffset=v->dataOffset[GLr];
			itsFoldStepper
				=new FfoldStepper(v,edgeRefOffset,orientedEdgesNo,currentPacket);
				
			// set entry point
			entryPoint=foldFirstEdge;
		}
		// runs into next case
		case foldFirstEdge:{
			if(handleStepper(itsFoldStepper->firstEdge())){
				v->bv=bvSave;
				return TRUE;
			}
			else{
				v->bv=bvSave;
				return FALSE;
			}
		}
		case foldNextEdge:
			if(handleStepper(itsFoldStepper->nextEdge())){
				v->bv=bvSave;
				return TRUE;
			}
			else{
				v->bv=bvSave;
				return FALSE;
			}
		break;
		default: v->varErr(52);
	}
}

Boolean FvertexFolder::handleStepper(Boolean entryDefined){
	if(entryDefined){
		// set up parameters needed for analysis
		long count=itsFoldStepper->count;
		GLref currentDatum=itsFoldStepper->refDatum;
		GLref opp2;
		v->xEdgeEntryToOwner(opp2,currentDatum);
		Boolean opp2isMidpoint=SameAddr(midpoint,opp2);
		GLref opp2packet;
		opp2packet.index=itsFoldStepper->refDatum.index;
		opp2packet.offset=(itsFoldStepper->refDatum).offset-ownerOffset;
		if(((FoldInfo*)itsFoldInfo->NthItem(count+1))->isOccupied){
			// set up additional parameters for case by case analysis
			
			if(!((FoldInfo*)itsFoldInfo->NthItem(count+1))->isMidpoint){
				// we must check to see if opp1 became the midpoint because
				// of some "third party" fold
				GLref ref;
				v->xEdgeEntryToOwner(ref,
					((FoldInfo*)itsFoldInfo->NthItem(count+1))->edgeDatum);
				((FoldInfo*)itsFoldInfo->NthItem(count+1))->isMidpoint
					=SameAddr(ref,midpoint);
			}
			Boolean opp1isMidpoint
				=((FoldInfo*)itsFoldInfo->NthItem(count+1))->isMidpoint;
			
			// set the opp2packet -e reference to NA

			long offset=v->xmodIndexToOffset(GLr,-v->e);
			Ptr DEREF=v&opp2packet;
			BlockMove((Ptr)&FGLink::NA,&DEREF[offset],sizeof(GLref));
			v-opp2packet;
			
			// if opp2isMidpoint and opp2packet is the first packet
			// set the -e foldInfo isOccupied FALSE
			
			if(opp2isMidpoint){
				if(SameAddr(firstPacket,opp2packet)){
					long minusEindex=count+itsFoldStepper->edgeSign;  // tricky tricky
					((FoldInfo*)itsFoldInfo->NthItem(minusEindex+1))->isOccupied=FALSE;
				}
			}

			// case by case activity

			if(!opp1isMidpoint && !opp2isMidpoint){
				// opp1 is not midpoint, opp2 is not midpoint
				// identify opp2 to opp1
				v->bv=((FoldInfo*)itsFoldInfo->NthItem(count+1))->edgeDatum;
				GLref ref;
				v->xEdgeEntryToOwner(ref,v->bv);
				v->bv=ref;
				v->cv=opp2;
				Identify();
			}
			else if(!opp1isMidpoint && opp2isMidpoint){
				// opp1 is not the midpoint, opp2 is the midpoint
				// Append opp1, updating lastPacket
				// set the e foldInfo isMidpoint TRUE
				
				GLref opp1=((FoldInfo*)itsFoldInfo->NthItem(count+1))->edgeDatum;
				v->xEdgeEntryToOwner(opp1,opp1);
				v->Append(lastPacket,midpoint,opp1);
				((FoldInfo*)itsFoldInfo->NthItem(count+1))->isMidpoint=TRUE;
			}
			else if(opp1isMidpoint && !opp2isMidpoint){			
				// opp1 is midpoint, opp2 is  not midpoint
				// Append opp2 to midpoint, updating lastPacket
				
				v->Append(lastPacket,midpoint,opp2);
			}
			// the case opp1 and opp2 are both the midpoint requires no
			// additional action
			
			++newFolds;
			v->InUse[GLr]=v->InUse[GLr]-2;
		}
		else{
			// set the e edgeEntry of the first packet
			// to the e edgeEntry of the current packet 
			// (which is contained in itsFoldStepper->refDatum).
			// set the -e edgeEntry of opp2packet to 
			// the first packet owner refererence location
			// set the e foldInfo isOccupied TRUE
			// if opp2 is the midpoint, set e foldInfo isMidpoint TRUE
			// else set e foldInfo isMidpoint FALSE, and
			//      set e foldInfo opp1 to opp2
			//      set e foldInfo edgeDatum to currentDatum

			Ptr DEREF=v&firstPacket;
			long offset=v->xmodIndexToOffset(GLr,v->e);
			BlockMove((Ptr)&itsFoldStepper->refDatum,&DEREF[offset],sizeof(GLref));
			v-firstPacket;
			DEREF=v&opp2packet;
			offset=v->xmodIndexToOffset(GLr,-v->e);
			BlockMove((Ptr)&firstPacketOwnerLocation,&DEREF[offset],sizeof(GLref));
			v-opp2packet;
			((FoldInfo*)itsFoldInfo->NthItem(count+1))->isOccupied=TRUE;
			if(!(((FoldInfo*)itsFoldInfo->NthItem(count+1))->isMidpoint=opp2isMidpoint)){  // want assignment here
				((FoldInfo*)itsFoldInfo->NthItem(count+1))->edgeDatum=currentDatum;
			}
		}
		entryPoint=foldNextEdge;
		return(TRUE);
	}
	else{
		// recycle current packet
		itsRecycleStack->recyclePush(v,currentPacket);
		(v->Total)[GLr]=(v->Total)[GLr]-(v->noEntries[GLr])*2;
		
		// figure out what to do

		currentPacket=itsFoldStepper->packetRef;
		if(isNull(currentPacket)){
			return FALSE ;  // all done
		}
		else{
			entryPoint=foldFirstEdge;   // search the next packet
			return TRUE;
		}
	}
}

void FvertexFolder::Identify(){
	// this routine is broken out here so that it can be
	// overridden by the reduced vertex folder
	
	v->Identify();
}



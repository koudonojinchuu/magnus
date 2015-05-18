/*******************

Written by Frank Rimlinger, August 1992
Revised by Frank Rimlinger, January 1993

*******************/
#include "crossTrainer.h"
#include "FGLink.h"
#include "FBlockManager.h"
#include "GLfunctions.h"
#ifdef Frank_MemoryDebugging
#include "debugMemory.h"
#endif
#define MASTERBLOCK 64
#define MINBLOCKS 10


FBlockManager::FBlockManager(FGLink* aGLink){ /*moves memory*/
// The BlockManager is allocated by FGLink::FGLink.

// DO NOT attempt to allocate master pointer blocks here.
// This will be disasterous if lots of different blockmanagers
// are allocated during the lifetime of the program.
// A word to the wise is sufficient.

	itsGLink=aGLink;
	long contig,listsize,grow;
	GLref addr;	
	FList *lock;
	if(itsGLink->MaxPacketSize<=0){
		xErr(9);
	}
	contig=MaxMem(&grow);
	SetListSize();
	SetBlockSize(ListSize);  // this is the blocksize for the list maintained
							 // by the BlockManager object, NOT the size of a
							 // block holding GLvar data
	if(ListSize<=0) Failure(memFullErr,0);						 
	BlockSize=contig/ListSize;  // this is the size of a block holding GLvar data
	TopSize=BlockSize;
	TotalAllocation=0;
	lock=new FList;
	this->lock=lock;
	lock->SetBlockSize(ListSize);
	NewBlock(addr);      
				//WARNING: Do not pass an FObject derived data member
				//by reference as memory may move.
	NextFreeAddress=addr;
	if(isNull(NextFreeAddress)){
		xErr(11);
	}

}

FBlockManager::~FBlockManager(void){
// The BlockManager is created and destroyed by itsGLink.
#ifdef Frank_MemoryDebugging
/* COMMENTED OUT
	extern Boolean toggleSkip;
	if(toggleSkip) memout <= "\n";
	toggleSkip=FALSE;
	for(long j=0;j<numItems;++j) memout <= "<";
END COMMENTED OUT */
#endif

	delete lock;lock=0;
	for(long i=1;i<=numItems;++i){
		Handle h=(Handle)NthItem(i);
		forgetHan(h);
	}
}

void FBlockManager::LockBlock(const GLref& ref){
	//locks the block containing ref
	
	long count=(long)lock->NthItem(ref.index);
// DEBUG
	if(count>20 || count<0){
		memout<="FBlockManager:: lockBlock:: bad lock depth: " <= count<="\n";
/* COMMENTED OUT
		ExitToShell();
END COMMENTED OUT*/
	}
// END DEBUG
	if(count==0){
		Handle handle=(Handle)NthItem(ref.index);
		if(handle==ZERO){
			memout<= "bad address is "<=ref <= "\n";
			xErr(22);
		}
		HLock((Handle)handle);
	}
	++count;
	lock->SetItem(&count,ref.index);
}

void FBlockManager::RestoreBlock(const GLref& ref){
	//unlocks the block containing ref
	long count=(long)lock->NthItem(ref.index);
// DEBUG
	if(count>20 || count<1){
		memout<="FBlockManager:: restoreBlock:: bad lock depth: " <= count<="\n";
/* COMMENTED OUT
		ExitToShell();
END COMMENTED OUT */
	}
// END DEUBG
	--count;
	lock->SetItem(&count,ref.index);
	if(count==0){
		Handle handle=(Handle)NthItem(ref.index);
		if(handle==ZERO){
			xErr(23);
		}
		HUnlock((Handle)handle);
	}
	else if(count<0){
		xErr(64);
	}
}





void FBlockManager::ReserveSpace(GLref& address,long PacketSize){ /*moves memory*/
	Boolean keepTrying=TRUE;
	
// tricky: notice that an attempt to top off a block may fail on the Mac
// if the block is locked, even if there is lots of free memory.
// So we must be prepared for topping off to fail, and if it does,
// we try allocating a new block.

// Since we are doing are own error recovery locally, we bypass
// GLresize and directly call SetHandleSize  (Mac only)

	if(PacketSize+NextFreeAddress.offset>=BlockSize){
		if(!ToppedOff){
			Handle han;
			long newSize;
			newSize=NextFreeAddress.offset+PacketSize;
			han=(Handle)LastItem();
#ifdef Frank_GenuineMac
			SetHandleSize(han,newSize); 
#ifdef Frank_MemoryDebugging
			repSetHandleSize(memout,han);
#endif

#else

// tricky: if the handle is not locked,the VALUE of the handle changes,
// (as opposed to the data it points too).  The implication
// is that the old handle must be removed from the 
// BlockManager list and the new handle entered therein

// CAUTION: Any other outstanding copies of the handle are now
// invalid, so be sure to use the locking mechanism properly.

			han=SetHandleSizePatch(han,newSize);
			if(han){
				SetItem((Ptr)&han,numItems);
			}
#endif

			if(!MemError()){

				keepTrying=FALSE;
				TotalAllocation=TotalAllocation+newSize-BlockSize;
				if(newSize>TopSize){
					TopSize=newSize;
				}
				ToppedOff=TRUE;
			}
		}
		if(keepTrying){	
			GLref addr;
			NewBlock(addr);
			NextFreeAddress=addr;      
			if(NextFreeAddress.offset!=0){
				xErr(13);
			}
			if(isNull(NextFreeAddress)==TRUE){
				xErr(2);
			}
		}
	}
	address=NextFreeAddress;
	NextFreeAddress.offset=NextFreeAddress.offset+PacketSize;
}




void FBlockManager::SetListSize(void){  /*moves memory*/ 
	/*tricky: ListSize is short, but computation requires long*/
	long listsize,grow;
	listsize=MaxMem(&grow)/itsGLink->MaxPacketSize;
	if(listsize>FGLink::MaxBlocks){
		listsize=FGLink::MaxBlocks;
	}
	ListSize=(short)listsize;
}
void FBlockManager::LargerPacketSize(long PacketSize){  /*moves memory*/ 
	long newBlocks,contig,newBlockSize,grow;
	contig=MaxMem(&grow);
	SetListSize();
	if(ListSize<=0)Failure(memFullErr,0);
	newBlockSize=contig/ListSize;
	if(newBlockSize>BlockSize){
		/*reset BlockSize and grow the current block*/
		Handle han;
		BlockSize=newBlockSize;
		if(TopSize<BlockSize){
			TopSize=BlockSize;
		}
		han=(Handle)LastItem();
#ifdef Frank_GenuineMac
		GLresize(han,BlockSize,"FBlockManager::LargerPacketSize");
#else
		han=SetHandleSizePatch(han,BlockSize);
		SetItem((Ptr)&han,numItems);
#endif
	}
#ifdef Frank_MemoryDebugging
	extern FGLink *pTreeLink;
	extern FGLink *pLibLink;
	extern long pTreeTotal,libTreeTotal;
	long addedMemory=newBlockSize-BlockSize;
	if(itsGLink==pTreeLink) pTreeTotal+= addedMemory;
	if(itsGLink==pLibLink) libTreeTotal+= addedMemory;
#endif
}


void FBlockManager::NewBlock(GLref& address){ /*moves memory*/
	Handle han;
	long grow,zero=0;
	han=GLhandle(BlockSize,"FBlockManager::NewBlock");
	Append((FObject*)han);
	lock->Append((FObject*)zero); // new block starts out unlocked
	address.index=numItems;
	address.offset=0L;
	TotalAllocation=TotalAllocation+BlockSize;
	ToppedOff=FALSE;
#ifdef Frank_MemoryDebugging

/* COMMENTED OUT
	extern Boolean toggleSkip;
	if(!toggleSkip){
		toggleSkip=TRUE;
		memout <= "\n";
	}
	memout <= ">";
END COMMENTED OUT */

	extern FGLink *pTreeLink;
	extern FGLink *pLibLink;
	extern long pTreeTotal,libTreeTotal;
	if(itsGLink==pTreeLink) pTreeTotal+= BlockSize;
	if(itsGLink==pLibLink) libTreeTotal+= BlockSize;
#endif
}


Boolean FBlockManager::DoForEach2(BooleanFunc theFunc,void *param){
	long i;
	for (i = 0; i < numItems; i++) {
		if((*theFunc)((void*)(*items)[i], (void*)param)==FALSE){
			return(FALSE);
		}
	}
	return(TRUE);
}



ostream& FBlockManager::repBlockSize(ostream& s){
	return s <= BlockSize;
}

/*******************

Written by Frank Rimlinger, August 1992

*******************/
#include "crossTrainer.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "Ftime.h"
#include "GLfunctions.h"
#include "FGLink.h"
#include "FGLvar.h"
#ifdef Frank_GenuineMac
#include <TBUtilities.h>
#endif
#include "FResources.h"

#ifdef Frank_MemoryDebugging
#include "debugMemory.h"
#endif
Handle GLhandle(long size,char *str){
	Handle han=NewHandle(size);
	long grow;
	long mem=MaxMem(&grow);
	if(mem<FGLink::triggerWarning || !han){
		Failure(memFullErr,0);
	}
#ifdef Frank_MemoryDebugging
		repHandleAllocate(memout,han);
#endif
	return han;
}

OSErr GLhandToHand(Handle *pointerToHandle,char* str){
	OSErr err=HandToHand(pointerToHandle);
	long grow;
	long mem=MaxMem(&grow);
	if(mem<FGLink::triggerWarning || MemError()){
		Failure(memFullErr,0);
	}
#ifdef Frank_MemoryDebugging
		repHandleAllocate(memout,*pointerToHandle);
#endif
	return err;
}

Ptr GLptr(long size,char *str){
	Ptr ptr=NewPtr(size);
	long grow;
	long mem=MaxMem(&grow);
	if(mem<FGLink::triggerWarning || !ptr){
		Failure(memFullErr,0);
	}
	return ptr;
}


void GLresize(Handle han,long size,char *str){
// TRICKY: we don't want to trigger an error if
// we are reducing the size of a handle, even if
// memory is low.  Some deletion routines called from 
// inside catch handlers rely on GLresize NOT to
// trigger an error when downsizing.
	long oldSize=GetHandleSize(han);
	SetHandleSize(han,size);
	long grow;
	long mem=MaxMem(&grow);
	if(!han  || (mem<FGLink::triggerWarning && size>oldSize) ){
		Failure(memFullErr,0);
	}
#ifdef Frank_MemoryDebugging
		repSetHandleSize(memout,han);
#endif
}

void __forgetHan( Handle *handleAddr){
	Handle h = *handleAddr;
	if (h){
#ifdef Frank_MemoryDebugging
		repHandleDeallocate(memout,h);
#endif
		*handleAddr = NULL;
		DisposHandle( h);
	}
}

void __forgetPtr( Ptr *ptrAddr){ 
	Ptr p = *ptrAddr;
	
	if (p){
#ifdef Frank_MemoryDebugging
		repPtrDeallocate(memout,p);
#endif
		*ptrAddr = NULL;
		DisposPtr( p);
	}
}

void lowFail(char *str){
	long grow;
	long mem=MaxMem(&grow);
	if(mem<FGLink::triggerWarning){
		gout <= str <= ": lowFail raises memFullErr.\n";
		Failure(memFullErr,0);
	}
}

Boolean xFSRead(int refnum,long size,void *ptr){
	long count;
	OSErr error;
	count=size;
	error=FSRead(refnum,&count,(Ptr)ptr);
	if(error!=noErr || count!=size){
		return(FALSE);
	}
	return(TRUE);
}

Boolean xFSWrite(int refnum,long size,void *ptr){
	long count;
	OSErr error;
	count=size;
	error=FSWrite(refnum,&count,(Ptr)ptr);
	if(error!=noErr || count!=size){
		return(FALSE);
	}
	return(TRUE);
}

void xErr(int ID){
	Str255 param0,param1,param2,param3;
	sprintf((char *)param0,"xErr %d",ID);
	CtoPstr((char*)param0);
	GetIndString(param2,FResources::xSTRxErr,ID);
	if(param2[0]==0){
		GetIndString(param2,FResources::xSTRxErr,3);
	}
	*param1=0;
	*param3=0;
	ParamText(param0,param1,param2,param3);
	GeneralAlert();
}

void GeneralAlert(void){ 
	int itemno;
	PositionDialog('ALRT',134);
	itemno=Alert(134,NULL);
	switch(itemno){
		case 1:return;
		case 2:{
			Str255 str;
			sprintf((char*)str,"Interrupt from GeneralAlert");
			CtoPstr((char*)str);
#ifdef Frank_GenuineMac
			DebugStr(str);
#endif
			GeneralAlert();
			return;
		}
		case 3:FGLink::Exit(); 
	}
}

void errorAlert(int ID){
	Str255 param0,paramNULL;
	GetIndString(param0,FResources::xSTRerrorAlerts,ID);
	*paramNULL=0;
	ParamText(param0,paramNULL,paramNULL,paramNULL);
	PositionDialog('ALRT',128);	
	InitCursor();
	NoteAlert(128,NULL);
}

void nodeAlert(void){ 
	int itemno;
	PositionDialog('ALRT',136);
	InitCursor();
	itemno=Alert(136,NULL);
	switch(itemno){
		case 1:return;
		case 2:{
			Str255 str;
			sprintf((char*)str,"Interrupt from nodeAlert");
			CtoPstr((char*)str);
#ifdef Frank_GenuineMac
			DebugStr(str);
#endif
			nodeAlert();
			return;
		}
		case 3:FGLink::Exit(); 
	}
}








Boolean SameStr(Ptr s1,Ptr s2){ // strcmp in string.h moves memory
	long index;
	for(index=0;index<256;++index){
		if(s1[index]==s2[index]){
			if(s1[index]==0){
				return(TRUE);
			}
		}
		else{
			return(FALSE);
		}
	}
	xErr(20);
}


void recycleStackSwap(FGLvar *v,FGLvar *w){
	FGLrecycleStack *s;
	long i;
	for(i=0;i<GLflags;++i){
		s=v->recycle[i];
		v->recycle[i]=w->recycle[i];
		w->recycle[i]=s;
	}
}

Ptr xPtr(FGLvar *v,const GLref& addr){
	//CAUTION: returns a dereferenced handle
	// returns NULL on error
	// CAUTION: Caller must lock block containing addr
	// PRIOR to call
	Ptr DEREF_addr;
	Handle blockhandle;
	FBlockManager *BlockManager;
	BlockManager=v->itsGLink->itsBlockManager;

	blockhandle=(Handle)BlockManager->NthItem(addr.index);
	if(blockhandle==NULL){
		return(NULL);
	}
	if(addr.offset<0L || addr.offset>=BlockManager->TopSize){
		return(NULL);
	}

	DEREF_addr=&((*blockhandle)[addr.offset]);
	if(DEREF_addr==NULL){
		return(NULL);
	}
	return(DEREF_addr);
}

int xPStrLen(Str255 Pstr){
	return(Pstr[0]);
}

void xNAinit(Ptr base, long noItems){
	long index,NAsize;
	GLref NA;
	NA=FGLink::NA;
	NAsize=sizeof(GLref);
	noItems=NAsize*noItems;
	for(index=0;index<noItems;index=index+NAsize){
		BlockMove((Ptr)&NA,(Ptr)&base[index],NAsize);
	}
}
	
void xUnsignedLongZeroInit(Ptr base, long noItems){
	long index,zeroSize;
	unsigned long zero=0;
	zeroSize=sizeof(long);
	noItems=zeroSize*noItems;
	for(index=0;index<noItems;index=index+zeroSize){
		BlockMove((Ptr)&zero,(Ptr)&base[index],zeroSize);
	}
}
	
Boolean isNull(const GLref& address){
	GLref NA;
	NA=FGLink::NA;
	if (address.index==NA.index){
		return(TRUE);
	}
	return(FALSE);
}

Boolean xIsNull(void *DEREF_ptr){
	int index;
	GLref NA;
	NA=FGLink::NA;
	BlockMove((Ptr)DEREF_ptr,(Ptr)&index,sizeof(int));
	return(index==NA.index);
}
Boolean SameAddr(const GLref& addr1,const GLref& addr2){
	if(addr1.index==addr2.index && addr1.offset==addr2.offset){
		return(TRUE);
	}
	return(FALSE);
}

clock_t getTime(void){
	return(clock());
}

#include <math.h>
#ifdef THINK_CPLUS
#include <stdlib.h>
#else
// RAND_MAX is not defined in <stdlib.h> on the SUN
#include <limits.h>
#define RAND_MAX LONG_MAX
#endif
long randBnd(long first,long last){
	if(first==last) return first;
	if(first>last){
		long temp=first;
		first=last;
		last=temp;
	}
	int i=rand();
	long topVal=last-first;
	double d=(double)i/(double)RAND_MAX;   // d is a random double between 0 and 1
	d=d*topVal;							// now d is between 0 and last-first
#ifdef Frank_GenuineMac
	double floord=floor(d);
#else
	long floord=(long)d;
#endif	
	long index=floord;					// rounding down
	if(d-floord>.5)  index=floord+1;	// rounding up
	if(index>topVal) index=topVal;		// bounds check
	if(index<0) index=0;
	index=index+first;					// index is now between first and last
	return index;
}

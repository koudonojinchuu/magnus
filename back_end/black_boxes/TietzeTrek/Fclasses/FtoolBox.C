#include "FtoolBox.h"
#include <stdio.h>
#include <stdlib.h>

#include "GLfunctions.h"
#include "FList.h"
#include "FResources.h"

#include "operators.h"
extern ostream& memout;

#ifndef Frank_GenuineMac
extern Str255 macParam0,macParam1,macParam2,macParam3;
extern OSErr MemErr,memFullErr;
#endif

// Most of the following are Mac OS A-traps.  They
// have been rewritten to invoke ANSI storage routines.
// Handles are provided in the SUN implementation to maintain 
// compatibility with the Mac.  It is important to understand that calls
// to realloc "move memory", so locking and unlocking of handles must be
// supported on the SUN.  

// In our crude SUN implementation, there is no master block for 
// the Handles.  Each handle h allocated with NewHandle reserves a Ptr,a long, 
// and a char up front for private use.  The Ptr contains the logical 
// address of the data, i.e., 
//          &h[sizeof(Ptr)+sizeof(long]+sizeof(Boolean)+3*sizeof(Boolean)
// The long contains the size of the block, for the exclusive use of 
// GetHandleSize.  The char is set if the handle is "locked," i.e., when
// realloc should fail rather than potentially change the value of the handle.
// The three bytes of padding guarentee that the data is word aligned.

// CAUTION: don't use this padding.  It doesn't exist in the Macintosh
// implementation.  Just LEAVE IT ALONE!!!

// A pointer p has one extra long up front which contains the size of the 
// pointer. NewPtr returns &p[sizeof(long)], so that we get the
// address of the data as expected.  The long is for the exclusive use of
// GetPtrSize. 

// Notice that with this implementation you can pass a dereferenced handle
// to GetPtrSize and the call will succeed.  BE CAREFUL NOT TO DO THIS, as
// such a call will cause the MAC to get very sick indeed. Notice that mixing
// up HandleDispos and PtrDispos should cause the SUN to gag since the offset
// to the SUN address differs for handles and pointers.

// PRIME DIRECTIVE: No funny business.  Just write genuine Macintosh code and
// all will be well.  Obviously any code that relies on this hoky-poky SUN
// implementation is GUARANTEED to be incompatible with the MAC.

// CAUTION: int is FOUR BYTES on the SUN, so don't ever use int for anything
// unless it is necessary to maintain compatibility.  Thus only use ints 
// when required by THINKC or ANSI C stuff.
// Use short for two bytes and long for four bytes.  This will help maintain
// compatibility when something naughty has been hardcoded.

// CAUTION: as defined on page 506 of Inside Macintosh I,
// IUequalString is case insensitive.  In this implementation
// IUequalString is case sensitive, as is the SUN file manager.

// CAUTION: GetIndString will fail if sizeof(short) is not 2.

// tricky: when running the SUN version on the Mac, the debugger will 
// turn blue and puke if you try to walk through a routine
// that has the same name as a genuine toolbox routine.  For this reason
// all the fake toolbox routines are redirected to routines with names
// prefixed by "x".


void unimplemented(void){
	// unimplemented trap that really should do something if called
	// but for one reason or another never got coded.
	memout <="unimplemented trap, goodbye.\n";
	ExitToShell();
}

#ifndef Frank_GenuineMac

	Handle xNewHandle(long size){
		// see explanation above
		Ptr data;

		Ptr block= (char*)malloc(size+sizeof(Ptr)+sizeof(long)
							+sizeof(Boolean)+3*sizeof(Boolean));
		if(!block){
			memout<="malloc has failed\n";
			MemErr=TRUE;
			return 0;
		}
		Boolean zero=0;		// handles start out unlocked
		data=&block[sizeof(Ptr)+sizeof(long)+sizeof(Boolean)+3*sizeof(Boolean)]; 
		BlockMove((Ptr)&data,block,sizeof(Ptr));
		BlockMove((Ptr)&size,&block[sizeof(Ptr)],sizeof(long));
		BlockMove((Ptr)&zero,&block[sizeof(Ptr)+sizeof(long)],sizeof(Boolean));
		MemErr=FALSE;
		return (Handle)block;
	}
	Handle NewHandle(long size){
		return xNewHandle( size);
	}
	
	Ptr xNewPtr(long size){
/* COMMENTED OUT
		Ptr block= (char*)malloc(size+sizeof(long));
END COMMENTED OUT*/
// DEBUG
		Ptr block=(char*)malloc(size+2*sizeof(long));
// END DEBUG
		if(!block){
			memout<="malloc has failed\n";
			MemErr=TRUE;
			return 0;
		}
		BlockMove((Ptr)&size,(Ptr)block,sizeof(long));
		MemErr=FALSE;
/* COMMENTED OUT
		return &block[sizeof(long)];
END COMMENTED OUT*/
// DEBUG
		Ptr data=&block[2*sizeof(long)];
		BlockMove((Ptr)&data,&block[sizeof(long)],sizeof(Ptr));
		return data;
// END DEBUG
	}
	Ptr NewPtr(long size){
		return xNewPtr( size);
	}
	
	Handle SetHandleSizePatch( Handle oldHandle, long newSize){
		// We have to patch the Mac OS SetHandleSize because
		// the corresponding ANSI realloc() routine returns
		// a new pointer, as opposed to updating the value
		// of a handle.

		// CAUTION CAUTION CAUTION
		// A lot of precious blood has been spilled over this
		// patch.  The BIG trouble is that the VALUE of the
		// handle changes, not just the data it points to.
		// This means that all extant copies of the handle,
		// stored in lists or whatever, are now INVALID
		// and must be replaced. 
		
		// If the handle is "locked", then we return 0, 
		// setting MemErr TRUE.  Its up
		// to the caller to recover from this situation. 
		
		// After you pass a handle to SetHandleSizePatch
		//   ** LEAVE IT ALONE **
		// DO NOT attempt to delete a handle after it has been
		// passed to SetHandleSizePatch.  The realloc routine 
		// called below takes care of this.
		
		char locked=HGetState(oldHandle);
		if( locked){
/* DEBUG
			memout<="realloc has failed because handle is locked\n"
				<= "and the handle must be relocated in order to grow\n";
END DEBUG */
			MemErr=TRUE;
			return 0;
		}
	
		long physicalSize=newSize+sizeof(Ptr)+sizeof(long)+sizeof(Boolean)
				+3*sizeof(Boolean);
		Ptr data;
		Ptr block=(char*)realloc(oldHandle,physicalSize);
		if(!block){
			memout<="realloc has failed because memory not available\n";
			MemErr=TRUE; 
			return 0; 
		}
		data=&block[sizeof(Ptr)+sizeof(long)+sizeof(Boolean)+3*sizeof(Boolean)]; 
		BlockMove((Ptr)&data,block,sizeof(Ptr));
		BlockMove((Ptr)&newSize,&block[sizeof(Ptr)], 
			  sizeof(long));
		MemErr=FALSE;
		return (Handle)block;
	}

	void xSetHandleSize(Handle h, long newSize){
		// can't do this on the SUN.  See SetHandleSizePatch
		// above.
		memout<=" can't call SetHandleSize, use\n";
		memout<="SetHandleSizePatch and be sure to update\n";
		memout<="ALL handle copies stored in lists, etc...\n";
		ExitToShell();
	}
	void SetHandleSize(Handle h,long newSize){
		xSetHandleSize(h,newSize);
	}
	
	void xDisposHandle(Handle h){
		// don't worry, ANSI knows how big h is, even
		// though it won't tell
// DEBUG
		if((long)*h!=(long)h+12){
			memout<="cannot free corrupt handle, goodbye\n";
			ExitToShell();
		}
// END DEBUG
		free(h);	
	}
	void DisposHandle(Handle h){
		xDisposHandle(h);
	}
	
	
	void xDisposPtr( Ptr p){
		// tricky: must account for the "physical" beginning
		// of the pointer
/* COMMENTED OUT
		free( &p[-sizeof(long)]);
END COMMENTED OUT*/
// DEBUG
		if((long)p != *(long*)&p[-sizeof(Ptr)]){
			memout<="cannot free corrupt pointer, goodbye\n";
			ExitToShell();
		}
		free( &p[-2*sizeof(long)]);
// END DEBUG
	}
	void DisposPtr(Ptr p){
		xDisposPtr(p);
	}
	
	void xBlockMove(Ptr a,Ptr b,long size){
	// notice BlockMove succeeds even if the blocks overlap
		Ptr done;
		if(b==a)return;
		if(b<a){
			done=&a[size];
			while(a<done){
				*b=*a;
				++a;
				++b;
			}
		}
		else{
			done=a;
			--done;
			a=&a[size-1];
			b=&b[size-1];
			while(a>done){
				*b=*a;
				--a;
				--b;
			}
		}
	}
	void BlockMove(Ptr a,Ptr b,long size){		
		xBlockMove(a,b,size);
	}
	
	SignedByte xHGetState(Handle h){
		// this returns 1 if locked, 0 if not locked
		Ptr hPtr=(Ptr)h;
		return (SignedByte)hPtr[sizeof(Ptr)+sizeof(long)];
	}
	SignedByte HGetState(Handle h){
		return xHGetState(h);
	}
	
	void xHLock(Handle h){
		Ptr hPtr=(Ptr)h;
		hPtr[sizeof(Ptr)+sizeof(long)]=1;
	}
	void HLock(Handle h){
		xHLock(h);
	}
	
	void xHSetState(Handle h, SignedByte state){
		Ptr hPtr=(Ptr)h;
		hPtr[sizeof(Ptr)+sizeof(long)]=state;		
	}
	void HSetState(Handle h, SignedByte state){
		xHSetState(h,state);
	}
	
	OSErr xHandToHand(Handle *h){ 
		// be sure to pass the ADDRESS of the handle
		// NOT the handle itself
		Handle j;
		Handle hand=*h;
		long size=GetHandleSize(hand);
		j=NewHandle(size);
		if(MemErr)return memFullErr;
		BlockMove((Ptr)*hand,(Ptr)*j,size);
		*(Handle *)h=j;
		return noErr;
	}
	OSErr HandToHand(Handle *h){
		return xHandToHand(h);
	}
	
	void xHUnlock(Handle h){
		Ptr hPtr=(Ptr)h;
		hPtr[sizeof(Ptr)+sizeof(long)]=0;
	}
	void HUnlock(Handle h){
		xHUnlock(h);
	}
	
	void xSysBeep(int duration){
//@rn f'chrissakes		memout<="*** BEEP ***\n";
	}
	void SysBeep(int duration){
		xSysBeep(duration);
	}
	
	Handle xNewHandleClear( long size){
		Ptr hPtr,donePtr;
		Handle h=NewHandle(size);
		if(h){
			hPtr=*h;
			donePtr=&hPtr[size];
			while(hPtr<donePtr){
				*hPtr=NULL;
				++hPtr;
			}
		}
		return h;
	}
	Handle NewHandleClear( long size){
		return(xNewHandleClear(size));
	}
	
	void xExitToShell(void){
		exit(0);
	}
	void ExitToShell(void){
		xExitToShell();
	}
	
	OSErr xMemError(void){
		return MemErr;
	}
	OSErr MemError(void){
		return xMemError();
	}
	
	OSErr xResError(){
		// this has no functionality on the SUN
	
		return 0;
	}
	OSErr ResError(){
		return xResError();
	}
	
	int xIUEqualString(Str255 a, Str255 b){
		unsigned char len;
		unsigned char *done;
		len=a[0];
		
		if(len!=*b){
			return 1;
		}
		done=&a[len];
		while(a<=done){
			if(*a!=*b) return 1;
			++a;
			++b;
		}
		return 0;
	}
	int IUEqualString(Str255 aStr, Str255 bStr){
		return xIUEqualString(aStr,bStr);
	}
	
	void xGetIndString(Str255 str, int id, int index){
		unsigned char *theData=0;
		short noItems,count=0;
		unsigned char len=1;
		str[0]=0;  	// empty string returned if resource stuff
					// is commented out
// DEBUGGING RESOURCES
		// offset id for indexing into itsResources
		id=id-FResources::xSTRoffset+1; //pascal indexing starts at 1
		extern FResources *debuggingResources;
		theData=(unsigned char*)(debuggingResources->STR->NthItem(id));

		if(!theData) return;
		BlockMove((Ptr)theData,(Ptr)&noItems,2);
		if(index<1 || index >noItems)return;
		while(count<index){
			// we scan through the Pascal strings
			theData=&theData[len+1];
			len=*theData;
			++count;
		}
		// load the string
		str[0]=len;
		BlockMove((Ptr)&theData[1],(Ptr)&str[1],len);
// END DEBUGGING RESOURCES

	}
	void GetIndString(Str255 str, int id, int index){
		xGetIndString(str,id,index);
	}
	
	void xParamText(Str255 cite0,Str255 cite1,Str255 cite2,Str255 cite3){
		BlockMove((Ptr)cite0,(Ptr)macParam0,256);
		BlockMove((Ptr)cite1,(Ptr)macParam1,256);
		BlockMove((Ptr)cite2,(Ptr)macParam2,256);
		BlockMove((Ptr)cite3,(Ptr)macParam3,256);
	}
	void ParamText(Str255 cite0,Str255 cite1,Str255 cite2,Str255 cite3){
		xParamText(cite0,cite1,cite2,cite3);
	}
	
	int xStringWidth(Str255 str){
		// this has no functionality on the SUN
		return 0;
	}
	int StringWidth(Str255 str){
		return xStringWidth(str);
	}
	
	void xNumToString(long n, Str255 str){
		sprintf((char*)str,"%ld",n);
		CtoPstr((char*)str);  // callers expect a Pascal string
	}
	void NumToString(long n, Str255 str){
		xNumToString(n,str);
	}
	
	long xMaxMem(long *grow){
	// this does not attempt to do what its supposed to do.
	// Instead, it always returns 2meg.
		return 2000000;
	}
	long MaxMem(long *grow){
		return xMaxMem(grow);
	}
	
	long xFreeMem(void){
	// this does not attempt to do what its supposed to do.
	// Instead, it always returns 2meg.

		return 2000000;
	}

	long FreeMem(void){
		return xFreeMem();
	}
	
	long xGetHandleSize(Handle h){
		long size;
		Ptr p=(Ptr)h;
		BlockMove((Ptr)&p[sizeof(Ptr)],(Ptr)&size,sizeof(long));
		return size;
	}
	long GetHandleSize(Handle h){
		return xGetHandleSize(h);
	}
	
	long xGetPtrSize(Ptr p){
		long size;
		BlockMove((Ptr)&p[-sizeof(long)],(Ptr)&size,sizeof(long));
		return size;
	}
	long GetPtrSize(Ptr p){
		return xGetPtrSize(p);
	}
	
	void xMoreMasters(void){
		// this has no functionality on the SUN
	}
	void MoreMasters(void){
		xMoreMasters();
	}
	
	int xFSRead(int refnum,long *count,Ptr ptr){
		memout <= "can't use Mac File Manager when writing SUN code\n"
			<= "use streamio instead\n";
		return 0;
	}
	int FSRead(int refnum,long *count,Ptr ptr){
		return xFSRead(refnum,count,ptr);
	}
	
	int xFSWrite(int refnum,long *count,Ptr ptr){
		memout <= "can't use Mac File Manager when writing SUN code\n"
			<= "use streamio instead\n";
		return 0;
	}
	int FSWrite(int refnum,long *count,Ptr ptr){
		return xFSWrite(refnum,count,ptr);
	}
	
	int xAlert(int id,void* filter){
		Str255 inputStr,str;
		int count;
		char aKluge;
		
		Str255 cparam0,cparam1,cparam2,cparam3;
		int itemno;
		BlockMove((Ptr)macParam0,(Ptr)cparam0,sizeof(Str255));
		PtoCstr(cparam0);
		BlockMove((Ptr)macParam1,(Ptr)cparam1,sizeof(Str255));
		PtoCstr(cparam1);
		BlockMove((Ptr)macParam2,(Ptr)cparam2,sizeof(Str255));
		PtoCstr(cparam2);
		BlockMove((Ptr)macParam3,(Ptr)cparam3,sizeof(Str255));
		PtoCstr(cparam3);
		memout <= "\n" <= "Alert " <= id <= "\n";
		memout <= "param0: " <= (char*)cparam0 <= "\n";
		memout <= "param1: " <= (char*)cparam1 <= "\n";
		memout <= "param2: " <= (char*)cparam2 <= "\n";
		memout <= "param3: " <= (char*)cparam3 <= "\n\n";
		memout.flush();
		ExitToShell();
		return 0;
	}
	int Alert(int id,void* filter){
		return xAlert(id,filter);
	}
	
	void xInitCursor(void){
		// this has no functionality on the SUN
	}
	void InitCursor(void){
		xInitCursor();
	}
	
	int xNoteAlert(int id,void* filter){
		return Alert(id,filter);
	}
	int NoteAlert(int id,void* filter){
		return xNoteAlert(id,filter);
	}
	
	unsigned char* CtoPstr(char* str){
		unsigned char len=0;
		while(len<255 && str[len]!= 0){
			++len;
		}
		BlockMove((Ptr)str,(Ptr)&str[1],len);
		str[0]=len;
		return (unsigned char *)str;
	}
	
	char * PtoCstr(Str255 str){
		unsigned char len=str[0];
		BlockMove((Ptr)&str[1],(Ptr)str,len);
		str[len]=0;
		return (char *)str;
	}
	
	void PositionDialog(short theType, int theID){
		// this has no functionality on the SUN
	}	
	
	
	int xIUCompString(Str255 aStr,Str255 bstr){
		unimplemented();
		return 0;
	}
	int IUCompString(Str255 aStr,Str255 bstr){
		return xIUCompString(aStr,bstr);
	}
	
	// here we define some miscellaneous stuff declared
	// in types.h
	
	void DebugCStr(char *aStr){
		Str255 str;
		char aKluge;
		sprintf((char*)str,"DebugCStr: %s",aStr);
		memout<=(char*)str<="\n";
		memout.flush();
		ExitToShell();
	}

	void DebugStr(ConstStr255Param aStr){
		Str255 str,outStr;
		BlockMove((Ptr)(char*)aStr,(Ptr)str,256);
		PtoCstr(str);
		DebugCStr((char*)str);
	}
	
// This is something used by TCL error handler
// If this is called from the SUN we just exit

	void Failure( OSErr err, long message){
		memout<= "Failure: err=" <= err <= "message=" <= message <= "\n";
		ExitToShell();
	}
	

#endif

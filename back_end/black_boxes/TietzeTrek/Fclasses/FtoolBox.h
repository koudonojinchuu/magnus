/*****************

We provide some functionality and some dummies for 
tool box patches, mostly concerned with memory

********************/


#ifndef Frank_FtoolBox
#define Frank_FtoolBox
#include "Ftypes.h" // a lot of code uses the definitions 
					// in this file


void unimplemented(void);
#ifndef Frank_GenuineMac
	Handle NewHandle(long size);
	Ptr NewPtr(long size);
	Handle SetHandleSizePatch( Handle oldHandle, long newSize);
	void SetHandleSize( Handle h, long newSize);
	void DisposHandle(Handle h);
	void DisposPtr( Ptr p);
	void BlockMove(Ptr a,Ptr b,long size);
	SignedByte HGetState(Handle h);
	void HLock(Handle h);
	void HSetState( Handle handle, SignedByte state);
	OSErr HandToHand(Handle *h);
	void HUnlock(Handle  h);
	Handle NewHandleClear( long size);

	OSErr MemError(void);
	OSErr ResError(void);

	void ExitToShell(void);
	int IUEqualString(Str255 aStr, Str255 bstr);
	int IUCompString(Str255 aStr,Str255  bstr);
	void GetIndString(Str255 str, int id, int index);
	void ParamText(Str255 cite0,Str255 cite1,Str255 cite2,Str255 cite3);
	void SysBeep(int duration);
	int StringWidth(Str255 str);
	void NumToString(long n, Str255 str);
	long MaxMem(long *grow);
	long FreeMem(void);
	long GetHandleSize(Handle h);
	long GetPtrSize(Ptr pData);
	void MoreMasters(void);
	int FSRead(int refnum,long *count,Ptr ptr);	
	int FSWrite(int refnum,long *count,Ptr ptr);
	int Alert(int id,void* filter);	
	void InitCursor(void);
	int NoteAlert(int id,void* filter);
	void PositionDialog(short theType, int theID);
	
	/* this is not tool box stuff, just some think C stuff for strings
	we put it here because it doesn't have any other home*/
	
	unsigned char *CtoPstr(char *);
	char *PtoCstr(unsigned char *);
	void Failure( OSErr err, long message);

#endif
#endif


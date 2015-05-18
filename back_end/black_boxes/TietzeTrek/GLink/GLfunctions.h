/*******************

Written by Frank Rimlinger, August 1992

*******************/
#ifndef Frank_GLfunctions
#define Frank_GLfunctions
#include "GLref.h"
#include "Ftime.h"

class FGLvar;
class FStringArray;
class FGLnode;

Handle GLhandle(long size,char *str);
OSErr GLhandToHand(Handle *pointerToHandle,char* str);
Ptr GLptr(long size,char *str);
void GLresize(Handle han,long size,char *str);

// here is some gri gri to transparantly
// zero handles and pointers when disposed

#define forgetHan(h)  __forgetHan( (Handle*) &h)
#define forgetPtr(p)  __forgetPtr( (Ptr*) &p)

// Call these without the underscore.  Trust the gri gri.

void __forgetHan( Handle *handleAddr);

void __forgetPtr( Ptr *ptrAddr);
	
void lowFail(char *str);
Boolean SameStr(Ptr s1,Ptr s2); 
long myStrLen(Ptr str);   
void myStrCpy(Ptr s1,Ptr s2); 
					
//used by CGLstack, which is a subclass of CCollection				
Boolean xFSRead(int refnum,long size,void *ptr);
Boolean xFSWrite(int refnum,long size,void *ptr);
                               
void xErr(int errID);                                 
void GeneralAlert(void);
void errorAlert(int ID);
void nodeAlert(void);
void recycleStackSwap(FGLvar *v,FGLvar *w);
Ptr xPtr(FGLvar *v,const GLref& addr);
	
int xPStrLen(Str255 Pstr);
void xNAinit(Ptr base, long noItems);	
void xUnsignedLongZeroInit(Ptr base, long noItems);	
Boolean isNull(const GLref& address);
Boolean xIsNull(void *DEREF_ptr);
Boolean SameAddr(const GLref& addr1,const GLref& addr2);
clock_t getTime(void);
long randBnd(long first,long last);
#endif


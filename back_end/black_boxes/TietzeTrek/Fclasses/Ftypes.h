#ifndef Frank_types
#define Frank_types
#define ZERO 0
							
// Special stream classes

#ifndef Frank_GenuineMac
#include <stdio.h>
typedef unsigned char Boolean;
#endif	
typedef Boolean (*BooleanFunc)(void*,void*);
	
typedef char Cstr[256];
#ifdef Frank_GenuineMac
#include <types.h>
#else

#define ASSERT( condition)	\
	condition ? 0 : printf("assertion failed\n")


#define nil				ZERO
#define noErr 0         /*All is well*/

typedef unsigned char Byte;
typedef signed char SignedByte;
typedef char *Ptr;
typedef Ptr *Handle;    /*  pointer to a master pointer */
typedef long Fixed;     /* fixed point arithmatic type */
typedef Fixed *FixedPtr;
typedef long Fract;
typedef Fract *FractPtr;

//@rn enum { false, true };
#define TRUE			1
#define FALSE			0



typedef unsigned char Str255[256],Str63[64],Str32[33],Str31[32],Str27[28],Str15[16],*StringPtr,**StringHandle;


typedef const unsigned char *ConstStr255Param;
typedef ConstStr255Param ConstStr63Param,ConstStr32Param,ConstStr31Param,
    ConstStr27Param,ConstStr15Param;

typedef short OSErr;    /* error code */
typedef unsigned long OSType;
typedef OSType *OSTypePtr;
struct Point {
    short v;
    short h;
};

typedef struct Point Point;
typedef Point *PointPtr;

struct Rect {
    short top;
    short left;
    short bottom;
    short right;
};

typedef struct Rect Rect;
typedef Rect *RectPtr;


void Debugger(void); 


void DebugStr(ConstStr255Param aStr);
void DebugCStr(char* aStr);
void debugstr(char *aStr); 
void SysBreak(void); 
void SysBreakStr(ConstStr255Param debugStr); 
void SysBreakFunc(ConstStr255Param debugFunc); 

#endif
#endif

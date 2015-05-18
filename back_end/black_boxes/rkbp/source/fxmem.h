// fxmem.h

/*
   This header file declares the type bucket, the macros, and the
   public functions of the fixed memory management packaged based
   on malloc.

   A bucket is a union type which is guaranteed to be alligned
   for any scalar data type.
*/


/////////////////////////////////////////////////////////////////
//
//                          bucket
//
/////////////////////////////////////////////////////////////////

typedef union bucket {
  void          huge *pointer;
  int                 integer;
  long                longint;
  float               flt;
  double              dbl;
//  long double         ldbl;  // To be fully compatible with
                               // C standard, long doubles must
                               // be allowed.
  ulong               size;
} bucket;



// Macro to find the usable size in bytes of the block pointed to
// by ptr.

#define  FXSIZE(ptr)  ( ((bucket huge *)(ptr)-1)->size )


// The functions

extern void         fxask(void);
extern void         fxfree(void huge *);
extern void         fxexit(char *,long);
extern ulong        fxinit(void);
extern void   huge *fxmllc(ulong);
extern void   huge *fxrllc(void huge *, ulong);
extern long         fxsetm(ulong);
extern void         fxstat(void);


// end of fxmem.h

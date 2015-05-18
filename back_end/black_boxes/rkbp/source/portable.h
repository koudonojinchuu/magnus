/* portable.h -- type and macro definitions for portable C */

/* Based on Appendix A of Portable C and UNIX System
   Programming by J. E. Lapin, Prentice-Hall */


/* Scope control pseudo-keywords.  At the top level, static
   means private. */

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE 1
#endif

enum boulean_enum_t {BOOL_FALSE = FALSE, BOOL_TRUE = TRUE};
typedef int _boolean_t;


/* type abbreviations */

typedef signed char     schar;
typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned long   ulong;



/* some basic macros, which assume that their arguments do not
   have side effects. */

#ifndef min
  #define min(x,y)   ((x) < (y) ? (x) : (y))
  #define max(x,y)   ((x) > (y) ? (x) : (y))
#endif

#define even(x)    (0 == ((x)&1))


/* Standard constants. */

#define   UNDEFINED           (-1)


/* The macro CHAR_BIT in limits.h gives the number of bits in a
   char.  The following macros do the same for shorts, ints, and
   longs. */

#define   SHORT_BIT             (CHAR_BIT*sizeof(short))
#define   INT_BIT               (CHAR_BIT*sizeof(int))
#define   _LONG_BIT              (CHAR_BIT*sizeof(long))


#ifdef MSVC
  #define  huge    huge
#else
  #define  huge
#endif


/* memory management functions */

extern  void  *malloc();
extern  void  *realloc();
extern  void   free();


/* portable.h ends here */

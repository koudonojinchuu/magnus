#ifndef __GROUP__
#define __GROUP__
#define TRUE    1
#define FALSE   0
#define DEFAULT 0
#define Public
#define Private static

/* #define basic data-types in case we need to change them later
   e.g. to use long rather than int */
#define Void    void
#define Char    char
#define Short   short
#define Int     int
#define Logical int
#define Float   float
#define Double  double

/* some macros  */
#define min0(x,y)	(((x) < (y)) ? (x) : (y))
#define iabs(x)		(((x) > 0) ? (x) : -(x))
#define max0(x,y)	(((x) > (y)) ? (x) : (y))
#endif





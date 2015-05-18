#ifndef Frank_Global
#define Frank_Global

#ifdef Frank_GenuineMac
#include <Global.h>
#else


#define		MAXINT		32767			/* Maximum int value				*/
#define		MININT		-32768			/* Mininum int value				*/
#define		MAXLONG		0x7FFFFFFF		/* Maximum long int value			*/
#define		MINLONG		0x80000000		/* Minimum long int value			*/
#define		NOTHING		0				/* Useful flag						*/
	
											/* For convenience, define types	*/
											/*   for handles to:				*/
	typedef		short		**ShortHandle;	/*     a short integer				*/
	typedef		long		**LongHandle;	/*     a long integer				*/
		
											/* Efficient C versions of HiWord	*/
											/*   and LoWord Traps				*/
#define		HiShort(longNum)	(short)(((longNum) >> 16) & 0xFFFF)
#define		LoShort(longNum)	(short)((longNum) & 0xFFFF)
	
											/* Macros for extracting the first	*/
											/*   and last 8 bits of a short		*/
#define		HiByte(shortNum)	(((shortNum) >> 8) & 0x00FF)
#define		LoByte(shortNum)	((shortNum) & 0x00FF)
	
											/* Macro for absolute value			*/
											/*   x may be any numeric type		*/
#define		Abs(x)			((x) < 0 ? -(x) : (x))
	
											/* Macros for maximum and minimum	*/
											/*   of two numbers					*/
#define		Max(x, y)		((x) > (y) ? (x) : (y))
#define		Min(x, y)		((x) < (y) ? (x) : (y))
	
#endif
#endif

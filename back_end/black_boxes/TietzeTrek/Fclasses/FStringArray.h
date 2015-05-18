#ifndef Frank_FStringArray
#define Frank_FStringArray

// This file only applies to the SUN version.  The analgous file on the
// Mac is called CStringArray

#include "FArray.h"
#include "Fsize_t.h"

class FStringArray : public FArray
{
public:
	FStringArray( short maxStringLength);
	FStringArray(const FStringArray* copyMe);
	virtual long FindString( StringPtr targetString);
	virtual void InsertAtIndex( void *itemPtr, long index);
protected:

	static Boolean			cCompAscending;
	static FStringArray		*cCurrArray;

	static void swap( size_t i, size_t j);
	static int CompareStrings( size_t index1, size_t index2);

};
#endif

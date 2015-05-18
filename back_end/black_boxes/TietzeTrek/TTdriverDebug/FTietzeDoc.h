#ifndef Frank_FTietzeDoc
#define Frank_FTietzeDoc

// This file only applies to SUN.  The genuine Mac version is called
// CTietzeDoc

#ifndef Frank_GenuineMac
#include "Ftime.h"
class FTietzeNode;
class FStringArray;
class FpresText;
class FTTpres;

class FTietzeDoc : public FObject{

public:
	FTietzeNode *itsTietzeNode;
	FStringArray* itsStrings;
	FpresText* itsEditItem;

	clock_t TT1minRequired;		
	clock_t TT1pminRequired;
	clock_t TT2minRequired;
	clock_t TT2pminRequired;

	clock_t TT1PayCheck;
	clock_t TT1pPayCheck;
	clock_t TT2PayCheck;
	clock_t TT2pPayCheck;

	long TT1RepeatMax;
	long TT1pRepeatMax;
	long TT2RepeatMax;
	long TT2pRepeatMax;
		
	long TT2len;   // max reduced length of word chosen by TT2

	clock_t LibTimeLimit;
	
	FTietzeDoc(void);
	~FTietzeDoc(void);
	virtual void OpenStdin(void);
	virtual void respond(short aMessage);
	virtual void setDisplay(short itemNo,long value);
	virtual void setDisplayFromID(short itemNo, short strID);
};

#endif
#endif


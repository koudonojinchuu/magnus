#ifndef Frank_FpresText
#define Frank_FpresText

// this file only applies to the SUN version.  The analgous file on
// the sun is CpresText

#ifndef Frank_GenuineMac
class FTietzeDoc;

class FpresText:public FObject{
public:
	FTietzeDoc* itsBoss;

	FpresText(FTietzeDoc *theBoss);
	virtual Boolean ValidateString(StringPtr str);
};
#endif
#endif

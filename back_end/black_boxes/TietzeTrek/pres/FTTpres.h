#ifndef Frank_FTTpres
#define Frank_FTTpres


#include "FObject.h"
class FfreeWord;
class FfreeList;

// CAUTION: To operate efficiently, many nodes require that input 
// FTTpres objects have consecutively numbered generators.  
// Accordingly,  each presentation maintains its own consecutive
// numbering scheme of "local generators."  Be sure to translate
// from local generators to globalSymbols to compare different sets 
// of relators as elements of the same ambient free group.  
// In particlar, convert to global symbols prior to display so
// that the user can compare different presentations meaningfully.
//
// RULE: all local generators less than or equal to the
// number of elements of the g List occur at least once in a presentation.
// Higher numbered local generators do not occur in any of the
// relators.  This rule MUST BE ENFORCED when building new 
// presentations.  

// IMPLICATION: This rule implies that max numbered generator
// occuring in a relator is g->numItems, and indeed most nodes
// initialize off of g->numItems.
// 
// Check all guns at the door.  We ain't lookin for no trouble.

class FTTpres: public FObject{
public:
	FList *g;			// i'th most frequently occuring local
						// generator, up to inverse.  Generators which
						// do not occur at all are not included in this
						// list.
	FList *gCount;		// number of occurrences of the i'th most
						// frequently occurring local generator
	FList *globalSymbols; // local to global symbol translation
	FfreeList *f;		// FfreeWord pointer to i'th longest relator
						// relators use local generators
	Boolean ownsFreeWords;
	FTTpres(Boolean anOwnerShipValue);
	FTTpres(FTTpres* copyMe);
	FTTpres(Ptr& DEREF);
	~FTTpres(void);
	virtual void addOccurrence(long e,long exp);
	virtual long computePacketSize(void);
	virtual void storeData(Ptr& DEREF);
};
ostream& operator<=(ostream& s,FTTpres *p);
#endif



#ifndef Frank_FfreeList
#define Frank_FfreeList


#include "FList.h"
class FfreeWord;

									
class FfreeList: public FList{
public:
	FfreeList(short aBlockSize);
	FfreeList(FfreeList* copyMe);
	virtual Boolean isDuplicate(FfreeWord *w);
	virtual FfreeWord *freeItem(long i);
};
ostream& operator<=(ostream& s,FfreeList *f);
#endif

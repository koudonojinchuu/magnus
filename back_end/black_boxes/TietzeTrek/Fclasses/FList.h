#ifndef Frank_FList
#define Frank_FList

#include "FCluster.h"

class FList : public FCluster {				

public:
	FList(void);
	FList(const FList *copyMe);
	FList(short aBlockSize);
	virtual void		Append(FObject *theObject);
	virtual void		Prepend(FObject *theObject);
	virtual void		InsertAfter(FObject *theObject, FObject *afterObject);
	virtual void		InsertAt(FObject *theObject, long index);
	
	virtual void		BringFront(FObject *theObject);
	virtual void		SendBack(FObject *theObject);
	virtual void		MoveUp(FObject *theObject);
	virtual void		MoveDown(FObject *theObject);
	virtual void		MoveToIndex(FObject *theObject, long index);
	
	virtual FObject*	FirstItem(void);
	virtual FObject*	LastItem(void);
	virtual FObject*	NthItem(long n);
	virtual long		FindIndex(FObject *theObject);
};



#endif



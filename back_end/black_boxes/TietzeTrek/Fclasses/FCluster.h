
#ifndef Frank_FCluster
#define Frank_FCluster

#include "FArray.h"	
#include "FGlobal.h"


class FCluster : public FArray {
public:
	LongHandle	items;	
	FCluster(void);	
	FCluster(const FCluster *copyMe);
	FCluster(short aBlockSize);
	virtual void		DisposeAll(void);		
	virtual void		DisposeItems(void);		
	virtual void		Add(FObject *theObject);
	virtual void 		Resize( long numSlots);
	virtual void		Remove(FObject *theObject);
	virtual Boolean		Includes(FObject *theObject);
	virtual long		Offset(FObject *theObject);
	
};

#define		fSLOT_SIZE		4
#endif


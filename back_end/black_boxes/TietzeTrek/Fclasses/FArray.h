#ifndef Frank_FArray
#define Frank_FArray
#include "FCollection.h"
#include "FGlobal.h"

typedef	int (*CompareFunc)( void*, void*);	 

class FArray : public FCollection
{
public:
	
	short		blockSize;				/* Number of slots to allocate when */
										/*  more space is needed			*/
	long		slots;					/* Total number of slots allocated	*/
	Handle		hItems;					/* Items in the array				*/
	
	long 		elementSize;			/* size of each element in bytes	*/
	Boolean		lockChanges;			/* can't insert or delete if locked */
	Boolean		usingTemporary;			/* TRUE if temporary element storage*/
										/* buffer is in use					*/
	
	FArray(void);	
	FArray(long elementSize);
	FArray(const FArray* copyMe);
	~FArray(void);
	virtual void	SetBlockSize( short aBlockSize);	
	virtual void	InsertAtIndex( void *itemPtr, long index);
	virtual void	DeleteItem( long index);
	virtual void	MoveItemToIndex( long currentIndex, long newIndex);
	virtual void	SetItem( void *itemPtr, long index);
	virtual void	GetItem( void *itemPtr, long index);
	virtual void	Swap( long index1, long index2);
	virtual long	Search( void *itemPtr, CompareFunc compare);
	virtual Boolean SetLockChanges( Boolean fLockChanges);

	virtual void	Resize( long numSlots);
	virtual void	MoreSlots( void);
	virtual void	Store( void *itemPtr, long index);
	virtual void	Retrieve( void *itemPtr, long index);
	virtual void	CopyToTemporary( long index);
	virtual void	CopyFromTemporary( long index);
	virtual long	ItemOffset( long itemIndex);

};

class FMovedElementInfo
{
	public:
	long	originalIndex;	
	long	newIndex;		
	
};



#define		fBAD_INDEX		-1L			

#endif


#include "FArray.h"
#include "GLfunctions.h"
#include "operators.h"
#ifndef Frank_GenuineMac
#include "FtoolBox.h"
#include <iostream.h>
#endif
extern ostream &memout;


#define ASSERT_INDEX( index)	ASSERT( (index > 0)&&(index <= numItems))
#define ITEM_OFFSET( index) ((index-1) * elementSize)
#define ITEM_PTR( index) (&(*hItems)[ ITEM_OFFSET(index) ])
#define TMP_ITEM_PTR	(&(*hItems)[ ITEM_OFFSET(numItems+1) ])

FArray::FArray(long anElementSize)
	:FCollection(){	
	blockSize = 3;
	elementSize = anElementSize;						
	slots = 0;
	lockChanges = FALSE;
	usingTemporary = FALSE;
	hItems =GLhandle( elementSize,"FArray::FArray");
}

// THIS SPECIALIZED CONSTRUCTOR IS ONLY USED BY FStringArray
FArray::FArray(void)
	:FCollection(){}

FArray::FArray(const FArray* copyMe)
	:FCollection(){
	// this copies items but NOT objects pointed to by elements
	// of items.  If you are storing pointers you should use
	// a derived class of FCluster or FList.  The deep copy 
	// constructor must be in a drived class which knows the
	// type of the elements pointed to.
	
	numItems=copyMe->numItems;   // this has to be done here since FCollection
								 // has no copy constructor
	blockSize=copyMe->blockSize;										
	slots=copyMe->slots;
	hItems=copyMe->hItems;				
	GLhandToHand(&hItems,"FArray::FArray(FArray* copyMe)");
	elementSize=copyMe->elementSize;			
	lockChanges=FALSE;			// these are sensible initialization 		
	usingTemporary=FALSE;		// values			
}

FArray:: ~FArray(void){
	forgetHan( hItems);				
}

void	FArray::SetBlockSize(short aBlockSize){
	blockSize = aBlockSize;
}
void FArray::SetItem( void *itemPtr, long index){
	ASSERT_INDEX( index);	
	Store( itemPtr, index);
}
void FArray::Store( void *itemPtr, long index){	
	BlockMove( (Ptr)itemPtr, (Ptr)ITEM_PTR( index), elementSize);
	
}
void FArray::GetItem( void *itemPtr, long index){
	ASSERT_INDEX( index);
	Retrieve( itemPtr, index);
	
}
void FArray::Retrieve( void *itemPtr, long index){	
	BlockMove( (Ptr)ITEM_PTR( index), (Ptr)itemPtr, elementSize);
}
void FArray::InsertAtIndex( void *itemPtr, long index){	
	ASSERT(index > 0);
	ASSERT( lockChanges == FALSE);
	if (lockChanges) return;
	if (numItems >= slots)				
		MoreSlots();

	if (index <= numItems){
		BlockMove( (Ptr)ITEM_PTR( index), (Ptr)ITEM_PTR( index + 1),
					(numItems - index + 1) * elementSize);
	}
	else
		index = numItems + 1;
		
	numItems++;							

										
	Store( itemPtr, index);
}
void FArray::DeleteItem( long index)
{
	ASSERT_INDEX( index);
	ASSERT( lockChanges == FALSE);

	if (lockChanges) return;

	numItems--;							
	
	if (index <= numItems) {
		BlockMove( (Ptr)ITEM_PTR( index+1), (Ptr)ITEM_PTR( index),
						(numItems - index + 1) * elementSize);
	}
		
	if (slots > numItems + blockSize) {
		Resize( slots-blockSize);
	}
	

}
void FArray::Resize( long numSlots)
{
	slots = numSlots;
#ifdef Frank_GenuineMac
	GLresize( hItems,  (slots+1L) * elementSize,"FArray::Resize");
#else
	hItems= SetHandleSizePatch(hItems, (slots+1L) * elementSize);
#endif
}
void	FArray::MoreSlots( void){
	Resize( slots + blockSize);
	
}
void FArray::MoveItemToIndex( long currentIndex, long newIndex){
	FMovedElementInfo moveInfo;
	
	ASSERT_INDEX( currentIndex);
	ASSERT_INDEX( newIndex);
	
	if (currentIndex == newIndex) return;
	CopyToTemporary( currentIndex);
	
	if (currentIndex < newIndex) 
	{
			BlockMove( (Ptr)ITEM_PTR( currentIndex+1), (Ptr)ITEM_PTR( currentIndex),
						(newIndex - currentIndex) * elementSize);
									
	}
	else if (currentIndex > newIndex)
	{
			BlockMove( (Ptr)ITEM_PTR( newIndex), (Ptr)ITEM_PTR( newIndex+1),
						(currentIndex - newIndex) * elementSize);
							
	}
	CopyFromTemporary( newIndex);
	
	moveInfo.originalIndex = currentIndex;
	moveInfo.newIndex = newIndex;

}
Boolean FArray::SetLockChanges( Boolean fLockChanges)
{
	Boolean wasLocked = lockChanges;
	
	lockChanges = fLockChanges;
	
	return wasLocked;

}
long FArray::Search( void *itemPtr, CompareFunc compare)
{
	char	state;
	register Ptr  items;
	register long i;
	long	foundIndex = fBAD_INDEX;
	
	state = HGetState((Handle) hItems);
	HLock((Handle) hItems);
	
	items = *hItems;
	for (i = 0; i < numItems; i++)
	{
		if (compare( itemPtr, items) == 0)
		{
			foundIndex = i+1;
			break;
		}
		items += elementSize;
	}
	HSetState((Handle) hItems, state);
	
	return foundIndex;

}
void FArray::Swap( long index1, long index2)
{
	ASSERT_INDEX( index1);
	ASSERT_INDEX( index2);
	
	CopyToTemporary( index1);
	BlockMove( (Ptr)ITEM_PTR( index2), (Ptr)ITEM_PTR( index1), elementSize);
	CopyFromTemporary( index2);
	
}
void FArray::CopyToTemporary( long index)
{
	ASSERT( usingTemporary == FALSE);
	
	usingTemporary = TRUE;
	
	BlockMove( (Ptr)ITEM_PTR( index), (Ptr)TMP_ITEM_PTR, elementSize);

}
void FArray::CopyFromTemporary( long index)
{
	ASSERT( usingTemporary == TRUE);

	usingTemporary = FALSE;
	
	BlockMove( (Ptr)TMP_ITEM_PTR, (Ptr)ITEM_PTR( index), elementSize);

}
long FArray::ItemOffset( long itemIndex)
{
	ASSERT_INDEX( itemIndex);
	
	return ITEM_OFFSET( itemIndex);

}

#include "FGlobal.h"
#include "FList.h"

FList::FList(void){}
FList::FList(const FList *copyMe)
	:FCluster(copyMe){
}
FList::FList(short aBlockSize)
	:FCluster(aBlockSize){}
	
void	FList::Append(
	FObject 	*theObject)
{
	FCluster::Add(theObject);
}
void	FList::Prepend(
	FObject 	*theObject)
{
	InsertAtIndex( &theObject, 1);
}
void	FList::InsertAfter(
	FObject		*theObject,				/* Object to insert in the list		*/
	FObject		*afterObject)			/* Object to insert it after		*/
{
	long	afterOffset;
	
	afterOffset = Offset(afterObject);	/* Find location of afterObject		*/

	if (afterOffset == fBAD_INDEX)
		afterOffset = 0;
		
	InsertAtIndex( &theObject, afterOffset + 2);
}
void	FList::InsertAt(
	FObject				*theObject,
	register long		index)
{
	InsertAtIndex( &theObject, index);
}
void	FList::BringFront(
	FObject 	*theObject)				/* Object to move to the front		*/
{
	register long		i;				/* Index into list					*/
	
	i = Offset(theObject);				/* Find location of the object		*/
										
	if (i > 0) {						/* If it's not already in front ...	*/
	
		MoveItemToIndex( i+1, 1);
	}
}
void	FList::SendBack(
	FObject 	*theObject)				/* Object to move to the back		*/
{
	register long	i;					/* Index into list					*/
	
	i = Offset(theObject);				/* Find location of the object		*/
										
	if ((i >= 0) && (i < numItems - 1)) {				
										/* If it's not already in back ...	*/

		MoveItemToIndex( i + 1, numItems);
	}
}
void	FList::MoveUp(
	FObject		*theObject)				/* Object to move up in list		*/
{
	long	i;					/* Index into list					*/
	
	i = Offset(theObject);				/* Find location of object			*/
										
	if (i > 0) {						/* If it's not at the front ...		*/
	
		MoveItemToIndex( i+1, i);
	}
}
void	FList::MoveDown(
	FObject		*theObject)				/* Object to move down				*/
{
	long	i;					/* Index into list					*/
	
	i = Offset(theObject);				/* Find locaton of object			*/
                                                            
    if (i >= 0  &&  i < numItems-1) {   /* If it's not at the back ...      */
    
    	MoveItemToIndex( i+1, i + 2);
	}
}
void	FList::MoveToIndex(
	FObject				*theObject,		/* Object to move within the list	*/
	register long		index)			/* Index at which to move object	*/
{
	long	i;							/* Index into the list				*/
	
	i = Offset(theObject);				/* Find location of the object		*/
	
	if (i != fBAD_INDEX)
	{
		MoveItemToIndex( i + 1, index);
	}
}
FObject*	FList::FirstItem()
{
	return(NthItem(1));
}
FObject*	FList::LastItem()
{
	return(NthItem(numItems));
}
FObject*	FList::NthItem(
	long		n)
{
	if (n < 1  ||  n > numItems)		/* If index is out of range			*/
		return(ZERO);					/*   pass back ZERO					*/
	else
										/* Do direct indexing into list		*/
		return( (FObject*) (*items)[n-1]);
}
long	FList::FindIndex(
	FObject		*theObject)
{
	return(Offset(theObject) + 1);		/* Index is one greater than the	*/
										/*   offset, which is zero-based	*/
}

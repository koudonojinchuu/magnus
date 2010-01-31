#include "FGlobal.h"
#include "FCluster.h"
#include "FtoolBox.h"
#include "operators.h"
#ifndef Frank_GenuineMac
#include <iostream>
#endif

FCluster::FCluster()
	:FArray(sizeof(FObject*)){
	items = (LongHandle) hItems;
}
FCluster::FCluster(const FCluster *copyMe)
	:FArray(copyMe){
	items = (LongHandle) hItems;
}

FCluster::FCluster(short aBlockSize)
	:FArray(sizeof(FObject*)){
	items = (LongHandle) hItems;
	SetBlockSize(aBlockSize);
}
void	FCluster::DisposeAll()
{
	DisposeItems();
			
	delete this;		// this is automatic, so doesn't mean anything
						// to set it equal to zero.		
}
void	FCluster::DisposeItems()
{
	if(numItems>0){
		extern ostream& gout;
		gout <= "I'm sorry, you can\'t delete this List until you\n"
			<= "have deleted all of its FObjects.  This is a pisser\n"
			<= "but I can\'t get the compiler to recognize the run\n"
			<= "time destructor for an FObject, so FCluster can\'t\n"
			<= "just run through its list deleting the FObjects\n"
			<= "What am I doing wrong??\n";
		ExitToShell();
	}

/* THIS IS HOW IT IS SUPPOSED TO WORK
	register long	i;				
	FObject			*itemToPurge;
	long	index;
	
	for (i = 0; i < numItems; i++){
		itemToPurge = (FObject*) (*items)[i];
		(*items)[i] = 0L;			
		delete itemToPurge;
		
		index = i+1;				
	}
	
	numItems = 0;					
	if (slots >  blockSize) {		
		Resize( blockSize);			
	}
END THIS IS HOW IT IS SUPPOSED TO WORK*/
}
void	FCluster::Add(
	FObject		*theObject)				
{	
	InsertAtIndex( &theObject, numItems+1);	
}	
void FCluster::Resize( long numSlots)
{
	FArray::Resize(numSlots);
	items=(LongHandle)hItems;

}	
void	FCluster::Remove(
	FObject		*theObject)			
{
	register long		i;			
	
	i = Offset(theObject);				
	if (i != fBAD_INDEX)
	{
		DeleteItem( i+1);
	}
}
Boolean	FCluster::Includes(
	FObject		*theObject)			
{
	return(Offset(theObject) != fBAD_INDEX);
}
long	FCluster::Offset(
	FObject		*theObject)				
{
	register long	i;					
	
	for (i = 0; i < numItems; i++) {	
										
		if ( (long)theObject == (*items)[i] )
			return(i);
	}
										
	return(fBAD_INDEX);					
}


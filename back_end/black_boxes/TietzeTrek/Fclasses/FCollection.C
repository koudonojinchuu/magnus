#include "FGlobal.h"
#include "FCollection.h"

FCollection::FCollection(void)
	:FObject(){	
	numItems = 0L;						
}

long	FCollection::GetNumItems()
{
	return(numItems);
}

Boolean	FCollection::IsEmpty()
{	
	return(numItems == 0L);
}

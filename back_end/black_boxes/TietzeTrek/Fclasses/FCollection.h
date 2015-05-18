
#ifndef Frank_FCollection
#define Frank_FCollection
#include "FObject.h"
				
class FCollection : public FObject {
public:			
				
								
	long numItems;
	FCollection(void);
	virtual long GetNumItems(void);
	virtual Boolean IsEmpty(void);
};

#endif

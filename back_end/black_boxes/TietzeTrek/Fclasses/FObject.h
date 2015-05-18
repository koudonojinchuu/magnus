#ifndef Frank_FObject
#define Frank_FObject

#include "Ftypes.h" // a lot of code uses the definitions 
					// in this file
							
class FObject {

public:			
	FObject(void);	
	virtual void respond(short aMessage);
	virtual void send(short aMessage);
	virtual void		SubclassResponsibility( void);
	FObject *super;				// this object's supervisor

};

#endif

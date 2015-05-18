#ifndef Frank_FGLnode
#define Frank_FGLnode

#include "FObject.h"
class FGLnode;


typedef Boolean (*nodeFunc)(FGLnode *node);

class FGLnode:public FObject{

public:
	nodeFunc func;
	Boolean dirtyAbort;				// initialized to FALSE.  Set TRUE to abort the
									// node whether or not the GLvar is dirty.
	Boolean abort;					// initialized to FALSE, set TRUE to abort as
									// soon as the GLvar is clean.

	FGLnode(FObject* aSupervisor);
	virtual Boolean run(void);		// when you "run" the node it calls its func
									// repeatedly until timeout or func is FALSE.  
									// "run" the node again if run is TRUE
									// forget the node if run is FALSE
	virtual void nodeErr(int ID);
protected:
	Boolean GLvarIsClean;			// initialized to TRUE.  Subnodes must support
									// this flag in order to for the abort mechanism
									// to function properly.  
private:	
	static Boolean  goodbyeFunc(FGLnode *node);  

protected:
	short mailPerson;				// for internal communication
	
// OVERRIDE to customize your node's behavior
// at key control points

	virtual void wakeUp(void);			// called prior to each call to func
	virtual void auRevoir(void);		// called if func returns TRUE.  Default
										// auRevior supports the abort mechanism.
	virtual void goodbyeKiss(void);		// called if func returns FALSE
};
#endif

#include "crossTrainer.h"
#include "FGLnode.h"
#include "FResources.h"
#include <stdio.h>
FGLnode::FGLnode(FObject *aSupervisor){
	super=aSupervisor;
	mailPerson= -1;		// not a legal value
	func=NULL;
	abort=FALSE;
	dirtyAbort=FALSE;
	GLvarIsClean=TRUE;
}

Boolean FGLnode::run(void){
	if(func==ZERO)nodeErr(2);
	
	wakeUp();
	
//Once we wake up we guarentee
//at least one call whether or not
//time has run out. 

	Boolean callAgain=TRUE, mustCall=TRUE;
	
	while((getTime()<FGLink::timeout && callAgain) || mustCall){
		mustCall=FALSE;
		callAgain=func(this);
	}
	if(callAgain)auRevoir();
	else goodbyeKiss();
	return(callAgain);
}
void FGLnode::wakeUp(void){
	// default method does nothing
}
void FGLnode::auRevoir(void){
	// default method sets func to goodbyeFunc in the
	// following situations:
	// 		dirtyAbort is TRUE, or
	//		abort is TRUE and GLvarIsClean is TRUE.  
	// If you override auRevoir, be sure
	// to call this method if your node supports the abort
	// mechanism.
	
	if(dirtyAbort || (abort && GLvarIsClean))func=goodbyeFunc;
}
void FGLnode::goodbyeKiss(void){
	// default method does nothing
}


void FGLnode::nodeErr(int ID){ /*moves memory*/
	// displays nodeErr resource message with index ID.
	Str255 param0,param1,param2,param3;
	int itemno;
	long offset;
	sprintf((char *)param0,"nodeErr %d",ID);  
	CtoPstr((char*)param0);
	sprintf((char*)param1,"");
	CtoPstr((char*)param1);
	sprintf((char*)param2,"");
// convert param2 to Pascal string
		CtoPstr((char*)param2);
//get the message
	
	GetIndString(param3,FResources::xSTRnodeErr,ID);  
	if(param3[0]==0){
		GetIndString(param3,FResources::xSTRnodeErr,1);
	}
//display
	ParamText(param0,param1,param2,param3);
	nodeAlert();
} 


Boolean  FGLnode::goodbyeFunc(FGLnode *node){
	// this is just a trap to terminate processing.
	// this trap is invoked indirectly by setting the
	// abort flag to TRUE.  If your node overrides auRevoir,
	// be sure to call GLnode::auRevoir or the abort flag
	// will have no effect and the goodbye func will not
	// be invoked.
	// In the event the goodbyeFunc is invoked, control passes
	// directly to the goodbyeKiss of the node.

	return FALSE;
}

/*******************

Written by Frank Rimlinger, August 1992
Revised by Frank Rimlinger, January 1993
*******************/
#include "crossTrainer.h"
#include <stdio.h>
#include "FGLink.h"

FGLink::FGLink(void){ 
	MaxPacketSize=0;	
	FGLink::NA.index=-1;
	FGLink::DEADVERTEX.index=-3;
	itsBlockManager=0;  // created and deleted as needed
						// on the fly
	DisposalSequenceInProgress=FALSE;
}

FGLink::~FGLink(void){

	DisposalSequenceInProgress=TRUE;
	if(numItems>0){
		gout <= "I'm sorry, you can\'t delete this GLink until you\n"
			<= "have deleted all of its GLvars.  This is a pisser\n"
			<= "but I can\'t get the compiler to recognize the run\n"
			<= "time destructor for an FObject, so FGLink can\'t\n"
			<= "just run through its list deleting the extant\n"
			<= "GLvars.  What am I doing wrong??\n";
		ExitToShell();
	}
/* COMMENTED OUT
	DisposeItems();  // this is how is should work.
END COMMENTED OUT*/
	delete itsBlockManager;itsBlockManager=0;
}
void FGLink::AddGLvar(FGLvarInterface *v){	
// This routine is called by FGLvarInterface::FGLvarInterface().

	long maxPacketSizeOfThisGLvar=0,index;
	Add((FObject*)v);
	for(index=0;index<GLflags;++index){	
		if(v->packetSize[index]>maxPacketSizeOfThisGLvar){
			maxPacketSizeOfThisGLvar=v->packetSize[index];
		}
	}	
// adjust the maximum packet size, used to fine tune the
// BlockManager

	if(maxPacketSizeOfThisGLvar>MaxPacketSize){	
		MaxPacketSize=maxPacketSizeOfThisGLvar;

// make sure the block manager exists 

		if(!itsBlockManager){
			itsBlockManager=new FBlockManager(this);
		}

		itsBlockManager->LargerPacketSize(MaxPacketSize);
	}
}

void FGLink::RemoveGLvar(FGLvarInterface *v){	
// This routine is called by ~FGLvarInterface.

	Remove((FObject*)v);
	if(numItems==0){
	
		// we delete all the data stored in the block manager 
		// because there are no remaining valid references to 
		// any of this data.  All such references are via
		// the GLvars in the list, and these have all been deleted
		
		MaxPacketSize=0;
		delete itsBlockManager; itsBlockManager=0;
	}
}

void FGLink::Exit(void){

#ifdef Frank_MemoryDebugging
	extern Boolean rosterOK;
	rosterOK=FALSE;
#endif
	ExitToShell();
}



Boolean FGLink::DoForEach2(BooleanFunc theFunc,void *param){
/*moves memory if and only if theFunc moves memory*/
	long i;
	for (i = 0; i < numItems; i++) {
		if((*theFunc)((void*)(*items)[i], (void*)param)==FALSE){
			return(FALSE);
		}
	}
	return(TRUE);
}


/*******************

Written by Frank Rimlinger, May 1993
*******************/

#include "crossTrainer.h"
#include "FrTreeInitInfo.h"
#include <stdio.h>

FrTreeInitInfo::FrTreeInitInfo(long numRelators)
	:FmapTreeInitInfo(numRelators){

// NAME
	sprintf((char *)errorStr,"rTree"); // designation of GLvar for error handler
	CtoPstr((char*)errorStr);				  // use Pascal version
	

// BACKYARD
// this extra space is used by the clone node
	rvBackYardSize=rvBackYardSize+sizeof(GLref);
}

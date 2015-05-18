/*******************

Written by Frank Rimlinger, May 1993
*******************/

#include "crossTrainer.h"
#include "FgTreeInitInfo.h"
#include <stdio.h>
#include "FGLvar.h"

FgTreeInitInfo::FgTreeInitInfo(long numGenerators)
	:FmapTreeInitInfo(numGenerators){

// NAME
	sprintf((char *)errorStr,"gTree"); // designation of GLvar for error handler
	CtoPstr((char *)errorStr);				  // use Pascal version
// BACKYARD
// this extra space stores the reference to the pruned rTree
	rvBackYardSize=rvBackYardSize+sizeof(GLref);
}

/*******************

Written by Frank Rimlinger, Jan 1993
*******************/
#include "crossTrainer.h"

#include "FGLvarInitInfo.h"
#include <stdio.h>
#include "FGLvar.h"


void FGLvarInitInfo::InitInfoFromGLvar(FGLvar *v){
	BlockMove((Ptr)v->itsName,(Ptr)errorStr,256);
	IDactiveMax=v->IDactiveMax;	
	rNoEntries=v->noEntries[GLr];				
	iNoEntries=v->noEntries[GLi];				 			
	lutNoEntries=v->noEntries[GLlut];							
	ChildNoEntries=v->ChildNoEntries;			
	useD=v->useD;
	DtypeSize=v->DtypeSize;
	useiD=v->useiD;
	iDtypeSize=v->iDtypeSize;
	recycleReal=v->recycleReal;
	recycleIdeal=v->recycleIdeal;
	rBackYardSize=v->backYardSize[GLr];
	irBackYardSize=v->backYardSize[GLir];
	iBackYardSize=v->backYardSize[GLi];
	lutBackYardSize=v->backYardSize[GLlut];
	rvBackYardSize=v->backYardSize[GLrv];
	ivBackYardSize=v->backYardSize[GLiv];
}

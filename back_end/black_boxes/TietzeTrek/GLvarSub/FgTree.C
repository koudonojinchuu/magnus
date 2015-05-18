#include "crossTrainer.h"

#include "FGLvar.h"
#include "GLfunctions.h"
#include "FgTree.h"
#include "FGLink.h"
#include "FgTreeInitInfo.h"
#include "FList.h"
#include "FBlockManager.h"


// definitions for programmer defined methods

FgTree::FgTree(FGLink *aGLink, FList *gCount,FgTreeInitInfo *info)
	:FmapTree(aGLink,info){

// HINT: ZB means Zero Based.  Anything that addresses off a C pointer
// is zero based.  Anything that addresses off a List structure is one
// based.  "local" refers to an index for the g and gCount lists.

	// check for consistency with info
	numItems=gCount->numItems;
	if(numItems!=info->numItems)varErr(60);
	
	// set up ZBrating
	
	ZBrating=(long**)GLhandle(numItems*sizeof(long),"FgTree::FgTree");
	**ZBrating=0;
	long count=(long)gCount->NthItem(1);
	long ratingNum=0;
	for(long ZBe=1;ZBe<numItems;++ZBe){
		long newCount;
		newCount=(long)gCount->NthItem(ZBe+1);
		if(newCount!=count){
			count=newCount;
			++ratingNum;
		}
		(*ZBrating)[ZBe]=ratingNum;
	}
	
	// construct ZBfirstIndex
	
	ZBfirstIndex=(long**)GLhandle((numItems+1)*sizeof(long),"FgTree::FgTree");  
												// this is max amount of
												// of space required
	(*ZBfirstIndex)[0]=0;
	long rate=0;
	for(long i=1;i<numItems;++i){
		long newRate;
		newRate=(*ZBrating)[i];
		if(rate!=newRate){
			++rate;
			if(rate!=newRate) varErr(59);
			(*ZBfirstIndex)[rate]=i;
		}
	}
	++rate;
	(*ZBfirstIndex)[rate]=numItems;  // this is used as a delimiter
		
	// compute refOffset
	
	refOffset=byOffset+bySize-(long)sizeof(GLref);
}

void FgTree::V(void){
	FmapTree::V();		// superclass
	
	// set GLref to NA
	putRef(cv,FGLink::NA);
}

void FgTree::putRef(GLref& vRef, GLref& refDatum){
	Ptr DEREF=this&vRef;
	BlockMove((Ptr)&refDatum,&DEREF[refOffset],sizeof(GLref));
	this-vRef;
}

void FgTree::getRef(GLref& refDatum,GLref& vRef){
	Ptr DEREF=this&vRef;
	BlockMove(&DEREF[refOffset],(Ptr)&refDatum,sizeof(GLref));
	this-vRef;
}


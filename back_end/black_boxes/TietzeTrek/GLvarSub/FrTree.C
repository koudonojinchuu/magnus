#include "crossTrainer.h"

#include "FGLvar.h"
#include "GLfunctions.h"
#include "FrTree.h"
#include "FGLink.h"
#include "FrTreeInitInfo.h"
#include "FfreeWord.h"
#include "FfreeList.h"


// definitions for programmer defined methods

FrTree::FrTree(FGLink *aGLink, FfreeList *rels,FrTreeInitInfo *info)
	:FmapTree(aGLink,info){

	// check for consistency with info
	numItems=rels->numItems;
	if(numItems!=info->numItems)varErr(60);
	
	// set up ZBrating
	long **aZBrating=(long**)GLhandle(numItems*sizeof(long),"FrTree::IrTree");
	ZBrating=aZBrating;
	**ZBrating=0;
	FfreeWord *f=rels->freeItem(1);
	long length=f->wordLength;
	long ratingNum=0;
	for(long ZBr=1;ZBr<numItems;++ZBr){
		f=rels->freeItem(ZBr+1);
		long newLength=f->wordLength;
		if(newLength!=length){
			length=newLength;
			++ratingNum;
		}
		(*ZBrating)[ZBr]=ratingNum;
	}
	
	ZBfirstIndex=(long**)GLhandle((numItems+1)*sizeof(long),"FrTree::IrTree");  
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
	
	// initialize clone offset
	cloneOffset=bySize-(long)sizeof(GLref);
}

#include "crossTrainer.h"

#include "FGLvar.h"
#include "GLfunctions.h"
#include "FmapTree.h"
#include "FGLink.h"
#include "FmapTreeInitInfo.h"


// definitions for programmer defined methods

FmapTree::FmapTree(FGLink *aGLink,FmapTreeInitInfo *info)
	:FGLvar(aGLink,info){


	// initialize FmapTree members
	ZBrating=ZERO;		// CAUTION: this must be constructed by a derived class
	ZBfirstIndex=ZERO;	// CAUTION: this must be constructed by a derived class
	numItems=info->numItems;
	byOffset=backYardOffset[GLrv];
	bySize=backYardSize[GLrv];
}
	
FmapTree::~FmapTree(void){
#ifdef Frank_GenuineMac
	forgetHan((Handle)ZBrating);
	forgetHan((Handle)ZBfirstIndex);
#else
	Handle han=(Handle)ZBrating;
	forgetHan(han);
	han=(Handle)ZBfirstIndex;
	forgetHan(han);
#endif
}

void FmapTree::V(void){
// CAUTION: error if base ZBlocal is greater than
// or equal to numGenerators

	FGLvar::V(); 	// superclass
	Ptr bvDEREF=this&bv;
	Ptr cvDEREF=this&cv;
	bvDEREF=&bvDEREF[byOffset];
	cvDEREF=&cvDEREF[byOffset];
	
	
	if(Qideal()){
		// if bv is ideal, we are at the root, which has
		// ZBlocal zero
		
		long zero=0;
		BlockMove((Ptr)&zero,cvDEREF,sizeof(long));
		
		// set all the Booleans TRUE
		cvDEREF=&cvDEREF[sizeof(long)];
		for(Ptr endDEREF=&cvDEREF[numItems];cvDEREF<endDEREF;++cvDEREF){
			*cvDEREF=TRUE;
		}
	}
	else{
		// if bv is not ideal, copy the backyard of bv 
		// to the backyard of cv
	
		BlockMove(bvDEREF,cvDEREF,bySize);
		
		// increase ZBlocal by 1
		long ZBlocal;
		BlockMove(cvDEREF,(Ptr)&ZBlocal,sizeof(long));
		if(ZBlocal>=numItems) varErr(57);
		++ZBlocal;
		BlockMove((Ptr)&ZBlocal,cvDEREF,sizeof(long));
		
		// set ZBe Boolean FALSE
		cvDEREF=&cvDEREF[sizeof(long)];
		long ZBe=e-1;
		cvDEREF[ZBe]=FALSE; 				
	}	
	this-bv;
	this-cv;
}

	
Boolean FmapTree::eNext(){
	// if e is zero, sets e to first admissible value for the
	// base ZBlocal.
	// Otherwise sets e to next admissible value
	// 
	// Returns TRUE if admissible value exists, otherwise FALSE
	// CAUTION: If returns FALSE, then e is undefined.
	//
	// Error if base ZBlocal is greater than or equal to the number
	// of generators
	
	Ptr DEREF=this&bv;
	DEREF=&DEREF[byOffset];
	long ZBlocal;
	BlockMove(DEREF,(Ptr)&ZBlocal,sizeof(long));	// base ZBlocal
	if(ZBlocal>=numItems)varErr(58);
	long rateNum=(*ZBrating)[ZBlocal];
	long ZBe=(*ZBfirstIndex)[rateNum];
	long ZBendIndex=(*ZBfirstIndex)[rateNum+1];
	if(e==0) e=ZBe+1;							// first admissible
	else{
		ZBe=e++;								// next admissible
	}
	Ptr endDEREF=DEREF=&DEREF[sizeof(long)];
	endDEREF=&DEREF[ZBendIndex];
	for(DEREF=&DEREF[ZBe];DEREF<endDEREF;++DEREF){
		if(*DEREF){
			this-bv;
			return TRUE;
		}
		++e;
	}
	this-bv;
	return FALSE;
}


long FmapTree::getLevel(const GLref& vRef){
	Ptr DEREF=this&vRef;
	long level;
	BlockMove(&DEREF[byOffset],(Ptr)&level,sizeof(long));
	this-vRef;
	return level;
}


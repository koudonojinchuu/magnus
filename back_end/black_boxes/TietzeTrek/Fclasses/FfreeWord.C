#include "crossTrainer.h"
#include "FtoolBox.h"
#include "FfreeWord.h"
#include "GLfunctions.h"
#include <stdlib.h>
#include "FGlobal.h"



//  An "entry" is a pair e,n, where e is a signed edge index, and n is 
//  an exponent.
//	N.B., the output identity element is always 0,0.
//	At ouput time, a pair e,n always is normalized to e>=0
//  Any input pair e,0 or 0,n is interpreted as the identity element

//  The following discussion is about terminology involved in multiplying
//  x,xexp by y,yexp.  
//  By definition, "consolidation" occurs when either x,xexp or y,yexp is 
//	the identity element, or else labs(x)=labs(y).  
//  If x,xexp and y,yexp can be consolodated, then "cancellation" occurs
//  if either of x,xexp and y,yexp are trivial or else xexp and yexp are
//  of different sign.

//  TECHNICAL NOTE
//	Everything is measured in longs, and we use long pointer aritmetic
//	We only convert to bytes for memory allocation

FfreeWord::FfreeWord(long blockSize){
	blockSize=blockSize/(long)sizeof(long);		// we use long pointer aritmetic,
											// except when allocating	
	if(blockSize<6)blockSize=6;				// min valid block size
	this->blockSize=blockSize;
	handleSize=blockSize;	
	wordHandle=(long**) GLhandle(handleSize*sizeof(long),"FfreeWord::IfreeWord");
	leftJustified=FALSE;
	nextRight=handleSize/2+1;
	nextLeft=nextRight-3;
	(*wordHandle)[nextLeft+1]=0;	// initialize to identity element
	(*wordHandle)[nextLeft+2]=0;	// in the middle of the block

	wordSize=2;					// this is the amount of memory in
								// logical use, measured in longs, NOT bytes
	noEntries=1;				// the identity element counts as an entry
	wordLength=0;				// but it does not contribute to the 
								// mathematical word length
}

FfreeWord::~FfreeWord(void){
#ifdef Frank_GenuineMac
	forgetHan((Handle)wordHandle);
#else
	Handle han=(Handle)wordHandle;
	forgetHan(han);
#endif
}

FfreeWord::FfreeWord(Ptr& DEREF){
	// initializes from data stored at DEREF 
	// updates DEREF

	// CAUTION:  if DEREF points into a relocatable block,
	// this block MUST BE LOCKED before calling this routine.

	BlockMove(DEREF,(Ptr)&wordLength,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	BlockMove(DEREF,(Ptr)&noEntries,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	BlockMove(DEREF,(Ptr)&wordSize,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	BlockMove(DEREF,(Ptr)&leftJustified,sizeof(Boolean));
	DEREF=&DEREF[sizeof(Boolean)];
	BlockMove(DEREF,(Ptr)&blockSize,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	BlockMove(DEREF,(Ptr)&handleSize,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	BlockMove(DEREF,(Ptr)&nextLeft,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	BlockMove(DEREF,(Ptr)&nextRight,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	wordHandle=(long**)GLhandle(handleSize*sizeof(long),
					"FfreeWord::FfreeWord(Ptr& DEREF)");
	SignedByte hState=HGetState((Handle)wordHandle);
	HLock((Handle)wordHandle);
	BlockMove(DEREF,(Ptr)*(wordHandle),wordSize*sizeof(long));
	HSetState((Handle)wordHandle,hState);
	DEREF=&DEREF[wordSize*sizeof(long)];
}

FfreeWord::FfreeWord(const FfreeWord* copyMe){
	wordHandle=copyMe->wordHandle;
	GLhandToHand((Handle*)&wordHandle,"FfreeWord::copyFreeWord");
	wordLength=copyMe->wordLength;
	noEntries=copyMe->noEntries;
	wordSize=copyMe->wordSize;
	leftJustified=copyMe->leftJustified;
	blockSize=copyMe->blockSize;
	handleSize=copyMe->handleSize;
	nextLeft=copyMe->nextLeft;
	nextRight=copyMe->nextRight;
}



void FfreeWord::leftJustify(void){
	if(!leftJustified){
		shift(0);
	}
}

Boolean FfreeWord::leftMultiply(long x,long xexp){
	// updates noEntries, nextLeft, wordLength
	
	freeMultiplyInfo info;
	Boolean cancellation,consolidation;
	
	if(xexp==0)x=0;		//canonical identity
	if(x==0)xexp=0;
	info.x=x;
	info.xexp=xexp;
	info.y=(*wordHandle)[nextLeft+1];
	info.yexp=(*wordHandle)[nextLeft+2];
	cancellation=multiply(info);
	consolidation=info.consolidation;
	
	// if no consolidation, put x on left
	
	if(!consolidation){
		putLeft(info.x,info.xexp);  // we use info here because mulitiply()
									// forces the canonical x>0
		return(FALSE);
	}
	
	else{
	
	// if consolidation, we update the wordlength
	
		wordLength=wordLength-Abs(info.yexp)+Abs(info.zexp);
	
	// if consolidation, and z not the identity, update
	// the left end with z
	
		if(info.z!=0){
			(*wordHandle)[nextLeft+1]=info.z;
			(*wordHandle)[nextLeft+2]=info.zexp;
		}
	 
	// if consolidation, and z is the identity, and noEntries>1, decrement 
	// noEntries and add 2 to nextLeft and subtract 2 from wordSize
	// in order to cancel y
	
		else if(noEntries>1){
			--noEntries;
			nextLeft=nextLeft+2;
			wordSize=wordSize-2;
		}
	
	// if consolidation, and z is the identity, and noEntries==1, overwrite
	// the left end with the identity.  The word is trivial.
	
		else{
			(*wordHandle)[nextLeft+1]=0;
			(*wordHandle)[nextLeft+2]=0;
		}
	}
	
	return(cancellation);
}

Boolean FfreeWord::rightMultiply(long y,long yexp){
	// updates noEntries, nextRight, and wordSize
	
	
	if(yexp==0)y=0;	// canonical identity
	if(y==0)yexp=0;
	
	freeMultiplyInfo info;	
	info.x=(*wordHandle)[nextRight-2];
	info.xexp=(*wordHandle)[nextRight-1];
	info.y=y;
	info.yexp=yexp;
	Boolean cancellation=multiply(info);
	Boolean consolidation=info.consolidation;
	
	// if no consolidation, put y on right
	
	if(!consolidation){
		putRight(info.y,info.yexp);  	// we use info here because mulitiply()
										// forces the canonical y>0
	}
	
	else{
	
	// if consolidation, we update the wordlength

		wordLength=wordLength-Abs(info.xexp)+Abs(info.zexp);
	
	
	// if consolidation, and z not the identity, update
	// the right end exponent with z 
	
		if(info.z!=0){
			(*wordHandle)[nextRight-2]=info.z;
			(*wordHandle)[nextRight-1]=info.zexp;
		} 

	// if consolidation, and z is the identity, and noEntries>1, decrement
	// noEntries and subtract 2 from nextRight and wordSize 
	// in order to cancel x
	
		else if(noEntries>1){
			--noEntries;
			nextRight=nextRight-2;
			wordSize=wordSize-2;
		}
	
	// if consolidation, and z is the identity, and noEntries==1, overwrite
	// the right end with the identity.  The word is trivial.
	
		else{
			(*wordHandle)[nextRight-2]=0;
			(*wordHandle)[nextRight-1]=0;
		}
	}
	return(cancellation);
}

Boolean FfreeWord::multiply(freeMultiplyInfo& info){

	// reformats x,xexp and y,yexp if necessary so that 
	// x and y are positive.
	// returns FALSE if no cancellation occurs. (By definition,
	// cancellation occurs if one of x or y is the identity element) 
	
	// returns TRUE if cancellation occurs.  In this case
	// the result, z,zexp, is computed, with positive z.
	
	// CAUTION: this routine assumes that an identity element
	// pass in the info block is the canonical identity 0,0
	
	if(info.x<0){
		info.x=-info.x;
		info.xexp=-info.xexp;
	}
	if(info.y<0){		
		info.y=-info.y;
		info.yexp=-info.yexp;
	}
	long x=info.x;
	long xexp=info.xexp;
	long y=info.y;
	long yexp=info.yexp;
	if(x!=y && x!=0 && y!=0){
		info.consolidation=FALSE;		// no consolidation, hence no cancellation
		return(FALSE);
	}
	
// consolidation has occured

	info.consolidation=TRUE;
	info.zexp=xexp+yexp;
	
	if((xexp<0 && yexp<0) || (xexp>0 && yexp>0)){
		info.z=x;
		return FALSE;		// no cancellation
	}

// cancellation has occured
	if(x==0){					
		info.z=y;
	}
	else if(y==0){
		info.z=x;
	}
	else if(xexp==-yexp){
		info.z=0;
	}
	else{
		info.z=x;
	}
	return(TRUE);
}


void FfreeWord::putLeft(long x,long xexp){
	// updates wordLength,wordSize, nextLeft, noEntries
	wordLength=wordLength+Abs(xexp);
	makeSpaceAvailable();
	++noEntries;
	wordSize=wordSize+2;
	(*wordHandle)[nextLeft-1]=x;
	(*wordHandle)[nextLeft]=xexp;
	nextLeft=nextLeft-2;
}

void FfreeWord::putRight(long y,long yexp){
	// updates wordSize, nextRight, noEntries
	wordLength=wordLength+Abs(yexp);
	makeSpaceAvailable();
	++noEntries;
	wordSize=wordSize+2;
	(*wordHandle)[nextRight]=y;
	(*wordHandle)[nextRight+1]=yexp;
	nextRight=nextRight+2;
}

void FfreeWord::makeSpaceAvailable(void){
// checks to see if there is enough room for a new entry
// on both the right and left.  If not, grows the block
// and centers the word

// updates handleSize

	Boolean grow=FALSE;
	if(nextRight+2>handleSize) grow=TRUE;
	if(nextLeft-2<0) grow=TRUE;
	if(grow){
		long startAt;
		handleSize=handleSize+blockSize;
#ifdef Frank_GenuineMac
		GLresize((Handle)wordHandle,handleSize*sizeof(long),
					"FfreeWord::makeSpaceAvailable");
#else
		wordHandle=(long**)SetHandleSizePatch((Handle)wordHandle,handleSize*sizeof(long));
#endif
		if(MemError()!=noErr){
			xErr(38);
		}
		startAt=(handleSize-wordSize)/2;
		shift(startAt);
	}
}

void FfreeWord::shift(long startAt){
	// this is a low level routine that performs no bounds check
	// updates nextLeft, nextRight, and leftJustified
	SignedByte hState=HGetState((Handle)wordHandle);
	HLock((Handle)wordHandle);
	BlockMove((Ptr)&(*wordHandle)[nextLeft+1],
				(Ptr)&(*wordHandle)[startAt],
				wordSize*sizeof(long));
	HSetState((Handle)wordHandle,hState);
	nextLeft=startAt-1;
	nextRight=startAt+wordSize;
	if(startAt==0)leftJustified=TRUE;
	else leftJustified=FALSE;
}


long FfreeWord::datumSize(void){
	// returns the size of a free word, not counting the
	// data handle
	long freeWordSize=0;
	freeWordSize+=sizeof(long);		// wordlength
	freeWordSize+=sizeof(long);		// noEntries
	freeWordSize+=sizeof(long);		// wordSize
	freeWordSize+=sizeof(Boolean);	// lefJustified
	freeWordSize+=sizeof(long);		// blockSize
	freeWordSize+=sizeof(long);		// handleSize
	freeWordSize+=sizeof(long);		// nextLeft
	freeWordSize+=sizeof(long);		// nextRight
	return freeWordSize;
}

long FfreeWord::totalSize(void){
	return freeWordSize+wordSize*sizeof(long);
}


void FfreeWord::storeData(Ptr& DEREF){
	leftJustify();
	BlockMove((Ptr)&wordLength,DEREF,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	BlockMove((Ptr)&noEntries,DEREF,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	BlockMove((Ptr)&wordSize,DEREF,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	BlockMove((Ptr)&leftJustified,DEREF,sizeof(Boolean));
	DEREF=&DEREF[sizeof(Boolean)];
	BlockMove((Ptr)&blockSize,DEREF,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	BlockMove((Ptr)&handleSize,DEREF,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	BlockMove((Ptr)&nextLeft,DEREF,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	BlockMove((Ptr)&nextRight,DEREF,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	SignedByte hState =HGetState((Handle)wordHandle);
	HLock((Handle)wordHandle);
	BlockMove((Ptr)*wordHandle,DEREF,wordSize*sizeof(long));
	HSetState((Handle)wordHandle,hState);
	DEREF=&DEREF[wordSize*sizeof(long)];
}



ostream& operator<=(ostream& s,FfreeWord *f){

// CAUTION: this is for low level debugging
// Use repGlobalFreeWord if you want to see the global generator symbols

// CAUTION: THIS ROUTINE LEFT JUSTIFIES
	f->leftJustify();

// COMMENTED OUT
	s <= "wordlength= " <= f->wordLength <= ": ";
// END COMMENTED OUT
	long count=0;
	for(long index=0;index<f->noEntries;++index){
		long e=(*f->wordHandle)[2*index];
		long exp=(*f->wordHandle)[2*index+1];
		s <= e <= "^" <= exp;
		if(index!=f->noEntries-1) s <= " * ";
		++count;
	}
	s <= "\n";
	return s;
}

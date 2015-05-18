#include "crossTrainer.h"
#include "FTTpres.h"
#include "FtoolBox.h"
#include "FfreeWord.h"
#include "TietzeTypes.h"
#include "FfreeList.h"
#include "GLfunctions.h"
#include "debugTT.h"
#include <limits.h>


FTTpres::FTTpres(Boolean anOwnerShipValue){
	ownsFreeWords=anOwnerShipValue;
	g=ZERO;
	gCount=ZERO;
	f=ZERO;
	globalSymbols=ZERO;
}

FTTpres::FTTpres(FTTpres* copyMe){
	globalSymbols=new FList(copyMe->globalSymbols);
	g=new FList(copyMe->g);
	gCount=new FList(copyMe->gCount);
	f=new FfreeList(copyMe->f);
// the new TTpres owns its free words since it has
// made a fresh copy
	ownsFreeWords=TRUE;
}

FTTpres::FTTpres(Ptr& DEREF){

// CAUTION:  if DEREF points into a relocatable block,
// this block MUST BE LOCKED before calling this routine.

	long num;
	BlockMove(DEREF,(Ptr)&num,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	
// allocate g and gCount

	short blockSize=Min(num,SHRT_MAX);
	g=new FList(blockSize);
	gCount=new FList(blockSize);
	
// load g
	long datum;
	for(long i=1;i<=num;++i){
		BlockMove(DEREF,(Ptr)&datum,sizeof(long));	
		DEREF=&DEREF[sizeof(long)];
		g->Append((FObject*)datum);
	}
// load gCount
	for(int i=1;i<=num;++i){
		BlockMove(DEREF,(Ptr)&datum,sizeof(long));	
		DEREF=&DEREF[sizeof(long)];
		gCount->Append((FObject*)datum);
	}

// allocate and initialize f, construct the free words and
// load their references in f
	BlockMove(DEREF,(Ptr)&num,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	blockSize=Min(num,SHRT_MAX);
	f=new FfreeList(blockSize);
	for(int i=1;i<=num;++i){
		FfreeWord *word=new FfreeWord(DEREF);
		f->Append((FObject*)word);
	}
	ownsFreeWords=TRUE;  // because we just made fresh handles
		
// allocate globalSymbols
	BlockMove(DEREF,(Ptr)&num,sizeof(long));
	DEREF+=sizeof(long);
	blockSize=Min(num,SHRT_MAX);
	globalSymbols=new FList(blockSize);
	
	// load globalSymbols
	for(int i=1;i<=num;++i){
		BlockMove(DEREF,(Ptr)&datum,sizeof(long));	
		DEREF+=sizeof(long);
		globalSymbols->Append((FObject*)datum);
	}
}

FTTpres::~FTTpres(void){
	delete g;g=0;
	delete gCount;gCount=0;
	if(ownsFreeWords && f){
		long num=f->numItems;
		for(long i=1;i<=num;++i){
			FfreeWord *x=f->freeItem(i);
			delete x;
		}
		delete f; f=0;
	}
	delete globalSymbols;globalSymbols=0;
}

void FTTpres::addOccurrence(long e,long exp){
	e= Abs(e);
	exp= Abs(exp);
	long index=g->FindIndex((FObject*)e);
	if(index){
		long count=(long)gCount->NthItem(index)+exp;
		gCount->SetItem(&count,index);
	}
	else{
		g->Append((FObject*)e);
		gCount->Append((FObject*)exp);
	}
}

long FTTpres::computePacketSize(void){
	if(g->numItems!=gCount->numItems){
		xErr(50);
	}
	long packetSize=(1+2*(g->numItems))*sizeof(long);    // space for g and gCount,
													// including common numItems
	
	packetSize=packetSize+sizeof(long);			    // space for f->numItems
	
	for(long i=1;i<=f->numItems;++i){
		FfreeWord *theWord=f->freeItem(i);
		packetSize+=theWord->totalSize();
	}
	
	
	long num=globalSymbols->numItems;
	packetSize+=(1+num)*sizeof(long);    // space for globalSymbols, 
										 // and its numItems
	return packetSize;
}

void FTTpres::storeData(Ptr& DEREF){
// store presentation at DEREF
// caller is responsible for making sure
// DEREF points to locked allocated memory of the proper
// size.

	long num=g->numItems;
	BlockMove((Ptr)&num,DEREF,sizeof(long));
	DEREF=&DEREF[sizeof(long)];
	long datum;
	for(long i=1;i<=num;++i){
		datum=(long)g->NthItem(i);
		BlockMove((Ptr)&datum,DEREF,sizeof(long));	
		DEREF=&DEREF[sizeof(long)];
	}
	for(int i=1;i<=num;++i){
		datum=(long)gCount->NthItem(i);
		BlockMove((Ptr)&datum,DEREF,sizeof(long));	
		DEREF=&DEREF[sizeof(long)];
	}
	BlockMove((Ptr)&f->numItems,DEREF,sizeof(long));
	DEREF=&DEREF[sizeof(long)]; 
	for(int i=1;i<=f->numItems;++i){
		FfreeWord *theWord=f->freeItem(i);
		theWord->storeData(DEREF);
	}
	num=globalSymbols->numItems;
	BlockMove((Ptr)&num,DEREF,sizeof(long));
	DEREF+=sizeof(long);
	for(int i=1;i<=num;++i){
		datum=(long)globalSymbols->NthItem(i);
		BlockMove((Ptr)&datum,DEREF,sizeof(long));	
		DEREF+=sizeof(long);
	}
}




ostream& operator<=(ostream& s,FTTpres *p){

// CAUTION: this is for low level debugging
// Use repGlobalTTpres if you want to see the global generator symbols
	long numf=p->f->numItems;
	long numg=p->g->numItems;
	xrepGlobalSymbols(s,p) <= "\n";
	s <= p->f <= "The (generator,number of occurrences) pairs are\n";
	long count=0;
	for(long index=1;index<=numg;++index){
		long e=(long)p->g->NthItem(index);
		long eTotal=(long)p->gCount->NthItem(index);
		s <= "(" <= e <= "," <= eTotal <= ") ";
		++count;
	}
	return s <= "\n";
}

#include "crossTrainer.h"
#include "sortingFunctions.h"
#include "FfreeList.h"
#include "FfreeWord.h"

FfreeList::FfreeList(short aBlockSize)
	:FList(aBlockSize){}
	
FfreeList::FfreeList(FfreeList* copyMe)
	:FList(copyMe){

	// we must call the copy constructors for the
	// FfreeWords pointed to in the old list, and then
	// put pointers to the new words in the new list.
	// Got that??
	
	for(long i=1;i<=numItems;++i){
		FfreeWord *oldWord=copyMe->freeItem(i);
		FfreeWord *newWord=new FfreeWord(oldWord);
		SetItem(&newWord,i);
	}
}

Boolean FfreeList::isDuplicate(FfreeWord *w){
	// returns TRUE if w is one of the words of f.
	// Assumes that the words of f are decreasing in length.

	long length=w->wordLength;
	w->leftJustify();
	for(long i=1;i<=numItems;++i){
		FfreeWord *x=freeItem(i);
		long xLength=x->wordLength;
		if(xLength<length){
		 	return FALSE;  // words listed in decreasing mathematical length
		 }
		if(xLength==length){	// mathematical lengths equal
			if(x->wordSize==w->wordSize){	// storage size equal
				x->leftJustify();
				if(sameLongHandle(w->wordHandle,x->wordHandle,x->wordSize)){
					 return TRUE;
				}	
			}
		}
	}
	return FALSE;
}

FfreeWord* FfreeList::freeItem(long i){
	return (FfreeWord *)NthItem(i);
}

ostream& operator<=(ostream& s,FfreeList *f){
// CAUTION: this is for low level debugging
// Use repGlobalFreeList if you want to see the global generator symbols
	long num=f->numItems;
	for(long i=1;i<=num;++i){
		FfreeWord *w=f->freeItem(i);
		s<= w;
	}
	return s;
}

#include "crossTrainer.h"
#include "FList.h"
#include "FfreeWord.h"
#include "FtoolBox.h"
#include "sortingFunctions.h"
#include "FfreeList.h"


Boolean sortList1(FList *list){
// sorts from highest to lowest
// returns TRUE if must call again
	Boolean callAgain=FALSE;
	long num=list->numItems;
	for(long i=1;i<num;++i){
		long count1,count2;
		count1=(long)list->NthItem(i);
		count2=(long)list->NthItem(i+1);
		if(count1<count2){
			list->Swap(i,i+1);
			callAgain=TRUE;
		}
	}
	return callAgain;
}

Boolean sortList2(FList *list,FList *otherlist){
// sorts list from highest to lowest
// returns TRUE if must call again
// also sorts otherlist in tandem
	Boolean callAgain=FALSE;
	long num=list->numItems;
	for(long i=1;i<num;++i){
		long count1,count2;
		count1=(long)list->NthItem(i);
		count2=(long)list->NthItem(i+1);
		if(count1<count2){
			list->Swap(i,i+1);
			otherlist->Swap(i,i+1);
			callAgain=TRUE;
		}
	}
	return callAgain;
}

Boolean sortFreeList(FfreeList *f){
// f must be a list of free words.  Sorts
// from longest to shortest.  Returns TRUE
// if must call again
	Boolean callAgain=FALSE;
	long num=f->numItems;
	for(long i=1;i<num;++i){
		FfreeWord *f1=f->freeItem(i);
		FfreeWord *f2=f->freeItem(i+1);
		if(f1->wordLength<f2->wordLength){
			f->Swap(i,i+1);
			callAgain=TRUE;
		}
	}
	return callAgain;
}

Boolean sameLongHandle(long **a,long **b,long size){
	// returns TRUE if longs of a and b 
	// are pairwise equal.  Only checks
	// first size pairs
	// returns TRUE is size is 0 or negative.
	long *aPtr=*a;
	long *bPtr=*b;
	long *endPtr=&aPtr[size-1];
	for(;aPtr<=endPtr;++aPtr){
		if(*aPtr!=*bPtr)return FALSE;
		++bPtr;
	}
	return TRUE;
}

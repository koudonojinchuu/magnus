// this file applies only to SUN version.  The analgous file on
// Mac is CStringArray.c

#ifndef Frank_GenuineMac
#include "crossTrainer.h"
#include "FStringArray.h"
#include <stdlib.h>
#include <limits.h>
#include "GLfunctions.h"


	
Boolean			FStringArray::cCompAscending;	// compare for ascending sort if true
FStringArray	*FStringArray::cCurrArray;		// array being sorted

FStringArray::FStringArray( short maxStringLength):
	FArray(Min(maxStringLength,sizeof(Str255))){}

FStringArray::FStringArray(const FStringArray* copyMe)
	:FArray(){
	numItems=copyMe->numItems;
	blockSize=copyMe->blockSize;
	slots=copyMe->slots;
	elementSize=copyMe->elementSize;
	Handle itemsCopy = copyMe->hItems;
	OSErr err = GLhandToHand( (Handle*)&itemsCopy,"FStringArray::CopyData");
	if(err){
		// if copying item handle failed, then we give up
		memout<="FStringArray: can't copy handle, goodbye\n";
		ExitToShell();
	}
	hItems = itemsCopy;	
}

int EqualStrings( void* str1, void* str2){
		return IUEqualString( (unsigned char*)str1, (unsigned char*)str2);
}

long FStringArray::FindString( StringPtr targetString){
	cCompAscending = TRUE;
	return Search( targetString, EqualStrings);
}

void FStringArray::swap( size_t i, size_t j){
	FStringArray::cCurrArray->Swap( i+1, j+1);
}
	
int FStringArray::CompareStrings( size_t index1, size_t index2){
	unsigned char *items = (unsigned char*) *FStringArray::cCurrArray->hItems;
	
	if (!cCompAscending)
	{
		long tmp = index1;
		index1 = index2;
		index2 = tmp;
	}
	return IUCompString( items + FStringArray::cCurrArray->ItemOffset( index1+1), 
					items + FStringArray::cCurrArray->ItemOffset( index2+1));
}


void FStringArray::InsertAtIndex( void *itemPtr, long index){
	if(index>=INT_MAX){
		errorAlert(3);
		ExitToShell();
	}
	FArray::InsertAtIndex( itemPtr,index);
}

#endif

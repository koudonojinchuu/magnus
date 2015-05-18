#include "crossTrainer.h"

#include "text_functions.h"

#ifdef Frank_GenuineMac
#include "CStringArray.h"
#endif

#include "FStringArray.h"
#include "FfreeWord.h"
#include "GLfunctions.h"

short getCharType(unsigned char item){
	if(GLbeginLow<item && item<GLendLow)return GLletter;
	if(GLbeginCaps<item && item<GLendCaps)return GLletter;
	if(GLbeginNum<item && item<GLendNum)return GLnumber;
	if(item==GLMINUS)return GLminus;
	if(item==GLSPACE)return GLspace;
	if(item==GLslash)return GLcomment;
	if(item==GLTILDE)return GLtilde;
	return GLother;
}

void removeBlanks(
#ifdef Frank_GenuineMac
		CStringArray *strs,
#else 
		FStringArray *strs,
#endif
		long index){
	Str255 str;
	strs->GetItem(str,index);
	long width=strs->elementSize;
	for(long i=1;i<=str[0];++i){
		short charType=getCharType(str[i]);
		if(charType==GLspace){
			//we get rid of the space
			long noRemaining;
			--str[0]; 
			noRemaining=width-(i+1);
			BlockMove((Ptr)&str[i+1],(Ptr)&str[i],noRemaining);
			--i;
		}
		else if(charType==GLcomment){
			break;		// we don't want to remove blanks from a comment
		}
	}
	strs->SetItem(str,index);
}

void wordToStringArray(FfreeWord *w,FwordToStringArrayInfo *info){
#ifdef Frank_GenuineMac
	int txFont=thePort->txFont;
	int txSize=thePort->txSize;
	extern short textFont;
	extern short textFontSize;
	thePort->txFont=textFont;		// for proper computation of string width
	thePort->txSize=textFontSize;
#endif
	
	char hState=HGetState((Handle)w->wordHandle);
	HLock((Handle)w->wordHandle);
	long *wordPtr=*w->wordHandle; 
	unsigned char *item=(unsigned char *)GLptr(info->strs->elementSize,
			"text_functions:wordToStringArray");
	item[0]=0;						// initialize the new item
	for(long i=0;i<w->wordSize;i=i+2){
		long e=wordPtr[i];
		long exp=wordPtr[i+1];
		if(e<1 || e>info->lettersNo){
			xErr(37);
		}
		Str255 str;						// build the string
		if(exp!=1){
			NumToString(exp, str);
			BlockMove((Ptr)&str[1],(Ptr)&str[2],str[0]);  // pascal string arithmatic
			str[1]=(*info->numberToLetter)[e];
			++str[0];
		}
		else{
			str[0]=1;
			str[1]=(*info->numberToLetter)[e];
		}
		int width=info->displayWidth;
		buildItem(info,item,str,width);		// add the string to the item
											// add item to basis and make
											// continuation line if necessary
	}
	
// now add the last item built to info->strs

	info->strs->InsertAtIndex( item, info->strs->numItems+1);

// clean up
	HSetState((Handle)w->wordHandle,hState);
#ifdef Frank_GenuineMac
	thePort->txFont=txFont;
	thePort->txSize=txSize;
#endif
	forgetPtr(item);			// all done
}

void buildItem(FwordToStringArrayInfo *info,unsigned char *item,
			   Str255 str,int width){
	unsigned char itemLen=item[0];
	unsigned char strLen=str[0];
	Boolean makeNewEntry=FALSE;
	if(itemLen+strLen>info->lettersPerLine){   
		makeNewEntry=TRUE;
	}
	else{
		int itemWidth=StringWidth(item);
		int strWidth=StringWidth(str);
		if(itemWidth+strWidth>=width-info->slack){
			makeNewEntry=TRUE;
		}
	}
	if(makeNewEntry==TRUE){
		// this item is full, so we add it to 
		// info->strs and reinitialize with a ~
		info->strs->InsertAtIndex( item, info->strs->numItems+1);  
		item[0]=1;
		item[1]=GLTILDE;
		itemLen=1;
	}
	// finally, add the new string to the item
	item[0]=itemLen+strLen;
	BlockMove((Ptr)&str[1],(Ptr)&item[itemLen+1],strLen);
}

#ifdef Frank_GenuineMac
void stringParse(CStringArray *aStringArray,Handle theData){
	char hState=HGetState(theData);
	HLock(theData);
	unsigned char  *d=(unsigned char *)*theData;
	long i=0,len=0;
	long size=GetHandleSize(theData);
	unsigned char *dEnd=&d[size];
	Str255 str;
	while(d<dEnd){
		if(*d != GLnewLine && *d != GLcarriageReturn){
			++len;
			if(len>255) Failure(1000,0);
			str[len]= *d;
		}
		else{
			str[0]=len;       //we are building a Pascal string
			++i;
			aStringArray->InsertAtIndex( str, i);
			len=0;
		}
		++d;
	}
	if(len>0){
		// we  have built a string that did not terminate with a
		// new line or carriage return, so add it here
		str[0]=len;
		++i;
		aStringArray->InsertAtIndex( str,i);
	}
	HSetState(theData,hState);
}

#else

void stringParse(FStringArray *aStringArray,Handle theData){
	unsigned char  *d,*dEnd,len;
	long i,size;
	Str255 str;
	char hState=HGetState(theData);
	HLock((Handle)theData);
	d=(unsigned char *)*theData;
	i=len=0;
	size=GetHandleSize((Handle)theData);
	dEnd=&d[size-1];
	while(d<=dEnd && *d!='!'){

	  //@rn quick-n-dirty hack to skip '^' in input:
	  if ( *d == '^' ) ++d;

		if(*d != ',' && *d != GLcarriageReturn){
			if(len==255) Failure(1000,0);
			++len;
			str[len]= *d;
		}
		else{
			str[0]=len;       //we are building a Pascal string
			++i;
			aStringArray->InsertAtIndex( str, i);
			len=0;
		}
		++d;
	}
	if(len>0){
		// we  have built a string that did not terminate with a
		// new line or carriage return, so add it here
		str[0]=len;
		++i;
		aStringArray->InsertAtIndex( str,i);
	}
	HSetState((Handle)theData,hState);
}

#endif




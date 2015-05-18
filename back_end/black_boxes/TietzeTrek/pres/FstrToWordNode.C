#include "crossTrainer.h"

#include "FstrToWordNode.h"
#include "FStringArray.h"

#ifdef Frank_GenuineMac
#include "CStringArray.h"
#endif

#include "nodeMsg.h"
#include "text_functions.h"
#include <stdio.h>
#include <stdlib.h>

FstrToWordNode::FstrToWordNode(FGLnode *aSupervisor,
#ifdef Frank_GenuineMac
		CStringArray *strs,
#else 
		FStringArray *strs,
#endif 
			long lettersNo,long **letterToNumber,long beginIndex,
			long endIndex)
	:FGLnode(aSupervisor){
	f=ZERO;
// the supervisor must set up

// C (or F) StringArray* strs  		the string object to be parsed
// lettersNo  						the number of letters in the words
// long **letterToNumber			a handle to the letter to number look
//									up table
// long beginIndex					index of the first string to process
// long endIndex					index of the last string to process, or zero if
// 									list is empty

// CAUTION: be sure to set up letter to number lut BEFORE initializing this node!
	
	this->strs=strs;
	this->lettersNo=lettersNo;
	this->letterToNumber=letterToNumber;
	this->beginIndex=beginIndex;
	this->endIndex=endIndex;
	extern long freeWordBlockSize;
	f=new FfreeWord(freeWordBlockSize); // ownership of nonempty words passes
										// to supervisor.  Empty words are
										// forgotten on the fly.
	

	elementSize=strs->elementSize;
	entryPoint=GLstart;
	func=castToStrToWord;
}

Boolean FstrToWordNode::castToStrToWord(FGLnode *node){
	FstrToWordNode *n;
	n=(FstrToWordNode*)node;
	return n->strToWord();
}

Boolean FstrToWordNode::strToWord(void){

	switch(entryPoint){
		case GLstart:
		
// it is permissible to have either stringlist empty
// we just notify the algorithm and quit

			if(!endIndex){
				send(emptyWordStrMsg);
				delete f;f=0;
				return FALSE;
			}				
			strIndex=beginIndex;
			addPreviousWord=FALSE;	
			entryPoint=GLrun;
			//runs into next case
		case GLrun:{
			Str255 s;
			lineAttributesInfo info;
		// check for unexpected end
			if(strIndex>endIndex)nodeErr(14);
			
		// ask the algorithm to remove the blanks, if any
		// we don't do this here because if they were already removed
		// earlier, its a waste of time, and only the alg knows about this
		
			send(removeBlanksMsg);
				
		// get the next string and its attributes
			strs->GetItem(s,strIndex);

			BlockMove((Ptr)s,(Ptr)info.s,elementSize);   
			getLineAttributes(&info);

		//	if this is a continuation line, and this line is not
		//  skipped we set addPreviousWord FALSE
		
			if(info.continuationLine && !info.skipLine){
				addPreviousWord=FALSE;
			}
						
		// 	if this is not a continuation, and it is not a line
		// 	that is skipped, and addPreviousWord is TRUE, then we send 
		//  the append word message to the algorithm.  
		//
		//  Finally, we set addPreviousWord FALSE
		
			if(!info.continuationLine && !info.skipLine && addPreviousWord){
				send(appendWordMsg);	
				extern long freeWordBlockSize;
				f=new FfreeWord(freeWordBlockSize);
	
				addPreviousWord=FALSE;
			}

		//  if this line is not skipped, we link its letters and set
		//  addPreviousWord TRUE

			if(!info.skipLine){
				processLine(s,info.firstLetter);
				addPreviousWord=TRUE;
			}
			
		//  if this is the last line, and addPreviousWord is TRUE,
		//  send the append last word message to the algorithm.
		
			if(strIndex==endIndex && addPreviousWord){
				send(appendLastWordMsg);
			}
			
		//  if this is not the last line, process next line,
		//  otherwise quit
					
			++strIndex;
			if(strIndex<=endIndex) return TRUE;
			else return FALSE;
		}
		break;
		default: nodeErr(6);
	}
}

void FstrToWordNode::processLine(Str255 s,long  firstLetter){
	Boolean scanning=TRUE;
	long len=(long)s[0];
	for(long i=firstLetter;i<=len && scanning;++i){
		unsigned char c=s[i];
		short charType=getCharType(c);
		if(charType==GLcomment)scanning=FALSE;
		else if(charType!=GLletter)nodeErr(13);
		if(scanning){
			// check for exponent, and
			// create the edge(s) which represent the letters
			// in the word
			
			long lastScannedi=i;
			long exp=collectExp(s,len,lastScannedi);
			f->rightMultiply((*letterToNumber)[c],exp);
			
			//reset i to lastScannedi
			i=lastScannedi;
		}
	}
}

void FstrToWordNode::getLineAttributes(lineAttributesInfo *info){
	
// initialize to output info to nominal values
	info->firstLetter=1;
	info->continuationLine=FALSE;
	info->skipLine=FALSE;

	long len=(long)info->s[0];
	if(len==0)info->skipLine=TRUE;  		//skip this empty line
	short charType;
	if(!info->skipLine){
		unsigned char c=info->s[1];
		charType=getCharType(c);
		if(charType==GLcomment)info->skipLine=TRUE;	//skip this comment line
	}
	
//	check to see if this line is a continuation of the previous line
//  and skip over the tilde if there is one.
	if(!info->skipLine){
		if(charType==GLtilde){
			++info->firstLetter;
			if(addPreviousWord){
				info->continuationLine=TRUE;
			}
		}
	}
	
//	if this is an empty continuation line, we skip it
	
	if(info->continuationLine && len==1)info->skipLine=TRUE;
}

long FstrToWordNode::collectExp(StringPtr s,long len,long& lastScannedi){

	// at entry time, lastScannedi indicates the letter to be processed
	// it is not known whether or not this letter is followed by an exponent
	Str255 numStr;
	
	long i=lastScannedi;
	++i;
	if(i>len)return(1);  //last letter has no explicit exponent
	unsigned char c=s[i];
	long sign=1;
	if(getCharType(c)==GLminus){
		sign= -1;
		++i;
		if(i>len)nodeErr(13);   //ending with a minus should have
								//already been screened out
		c=s[i];
	}
	if(getCharType(c)==GLletter || getCharType(c)==GLcomment){
		// no explicit exponent
		
		return(1);
	}
	long numi=0;
	lastScannedi=i;
	while(TRUE){	
		numStr[numi]=c;			//numStr is set up as a C string
		++numi;
		++i;
		c=s[i];
		if(i>len || getCharType(c)!=GLnumber){
			// at this point, we have collect the number in numStr,
			// and lastScannedi is correctly set.  Before converting
			// the ascii number to long, we must add the terminating
			// NULL.  A stitch in time saves nine.
			numStr[numi]=0;
			long exp=1;
			sscanf((char *)numStr,"%ld",&exp);
			return(exp*sign);
		}
		lastScannedi=i;
	}
}

void FstrToWordNode::goodbyeKiss(){
	send(strToWordGoodbyeKissMsg);
}

void FstrToWordNode::auRevoir(){
	FGLnode::auRevoir();
	send(strToWordAuRevoirMsg);
}


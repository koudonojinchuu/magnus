#include "crossTrainer.h"
#include "FwordsToGLvarNode.h"
#include "nodeMsg.h"
#include "FfreeWord.h"
#include "FfreeList.h"
#include <stdio.h>
#include <stdlib.h>


FwordsToGLvarNode::FwordsToGLvarNode(FGLnode* aSupervisor, FGLvar *aGLvar,
		long lettersNo,FfreeList *words,GLref& root,long scratchOffset)
		:FGLnode(aSupervisor){

	v=aGLvar;
	this->lettersNo=lettersNo;
	this->words=words;
	itsRoot=root;

//  scratchOffset is an offset into the GLrv backYard of v.
//  This is an area that FwordsToGLvarNode will overwrite and it
//  must be at least sizeof(Boolean) bytes.  Logically, we are
//  setting the doNotDuplicate flag FALSE

	if(v->backYardSize[GLrv]-scratchOffset<(long)sizeof(Boolean)){	
		nodeErr(22);
	}
	flagOffset=v->backYardOffset[GLrv]+scratchOffset;
	
// the supervisor responds to the initWordsToGLvarMsg by setting up

// FfreeList* words  		the FfreeWords to be converted
// lettersNo  				the number of letters in the words
// GLref itsRoot			the first word is attached at itsRoot

	func=wordsToGLvarFunc;
	entryPoint=GLstart;
}

Boolean FwordsToGLvarNode::wordsToGLvarFunc(FGLnode *node){
	return ((FwordsToGLvarNode*)node)->wordsToGLvar();
}

Boolean FwordsToGLvarNode::wordsToGLvar(void){

	switch(entryPoint){
		case GLstart:
		
			index=1;	
		
			entryPoint=GLrun;
			//runs into next case
		case GLrun:{

		// check for end
			if(index>words->numItems)return FALSE;
			
		// get the word
			FfreeWord *f=(FfreeWord*)words->NthItem(index);
			long **wordHandle=f->wordHandle;
			f->leftJustify();

		//  link the letters
			v->cv=itsRoot;		// the next edge is always linked at
								// at the companion
			long len=f->wordSize;
			for(long i=0;i<len;i=i+2){
				v->e=(*wordHandle)[i];	
				if(v->e>lettersNo)v->varErr(56);			
				long exp=(*wordHandle)[i+1];
				Boolean closeTheCircle= i==len-2;
				if(exp<0)v->e= -v->e;
				
				//create all but the last edge
				long noAllButLast=Abs(exp)-1;
				for(long notLasti=0;notLasti<noAllButLast;++notLasti){
					v->bv=v->cv;
					v->V();
					putFlag(v->cv,FALSE);
				}
				
				//set the last edge
				if(closeTheCircle){	
					send(terminateWordMsg);	
				}
				else{
					v->bv=v->cv;
					v->V();
				}				
			}
			
			// get ready for the next word
			++index;
			return TRUE;
		}
		break;
		default: nodeErr(6);
	}
}

void FwordsToGLvarNode::putFlag(GLref& where,Boolean aFlag){
	Ptr DEREF=v&where;
	BlockMove((Ptr)&aFlag,&DEREF[flagOffset],sizeof(Boolean));
	v-where;
}

void FwordsToGLvarNode::goodbyeKiss(){
	send(wordsToGLvarGoodbyeKissMsg);
}

void FwordsToGLvarNode::auRevoir(){
	FGLnode::auRevoir();
	send(wordsToGLvarAuRevoirMsg);
}


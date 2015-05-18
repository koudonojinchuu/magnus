#include "crossTrainer.h"
#include "FTTpresNode.h"
#include "FTTpres.h"
#include "FfreeWord.h"
#include "sortingFunctions.h"
#include "nodeMsg.h"
#include "FfreeList.h"

#define gBlockSize 20


// CAUTION: This node takes p->f as input and updates
// p->g, p->gCount, and p->f.  If the input presentation
// satisifies the RULE stated in FTTpres.h, then so will
// the output presentation.  If the input presentation
// does not satisfy the RULE, then be sure to pass the
// output of FTTpresNode to FrenumberPresNode to renumber
// the local generators.

	
FTTpresNode::FTTpresNode(FGLnode *aSupervisor,FTTpres *aTTpres)
	:FGLnode(aSupervisor){
// the node expects aTTpres->f to already be initialized to
// the input list of FfreeWords that will generate the rest of 
// the TTpres data

	p=aTTpres;
	func=TTpresFunc;
	
	delete p->g;p->g=0;
	p->g= new FList(gBlockSize);	
	delete p->gCount;p->gCount=0;
	p->gCount= new FList(gBlockSize);
	entryPoint=GLstart;
}

Boolean FTTpresNode::TTpresFunc(FGLnode *node){	
	
	return( ((FTTpresNode*)node)->TTpresentation() );
}


Boolean FTTpresNode::TTpresentation(void){
	switch(entryPoint){
		case GLstart:{
			fIndex=1;
			if(!p->f->numItems){				
				return FALSE;  // all done
			}
			entryIndex= 0;
			FfreeWord *fcurrent=(FfreeWord*)p->f->NthItem(fIndex);
			fcurrent->leftJustify();
			noEntries=fcurrent->noEntries;
			fHandle=fcurrent->wordHandle;
			entryModifier= addOccurrenceEntry;
			entryPoint=GLrun;
		}
		case GLrun:
			switch(entryModifier){
				case addOccurrenceEntry:{
					FfreeWord *fcurrent;
					long e=(*fHandle)[2*entryIndex];
					long exp=(*fHandle)[2*entryIndex+1];
					p->addOccurrence(e,exp);
					++entryIndex;
					if(entryIndex==noEntries){
						++fIndex;
						if(fIndex>p->f->numItems){
							entryModifier=gsortEntry;
							return TRUE;
						}
						entryIndex=0;
						fcurrent=(FfreeWord*)p->f->NthItem(fIndex);
						fcurrent->leftJustify();
						noEntries=fcurrent->noEntries;
						fHandle=fcurrent->wordHandle;
					}
					return TRUE;
				}
				break;
				case gsortEntry:
					if(!sortList2(p->gCount,p->g)) entryModifier=fsortEntry;
					return TRUE;
				break;
				case fsortEntry:
					return sortFreeList(p->f);
				break;
				default: nodeErr(6);
			}
		
		default: nodeErr(6);
	}
}

void FTTpresNode::auRevoir(void){
	FGLnode::auRevoir();
	send(presAuRevoirMsg);
}

void FTTpresNode::goodbyeKiss(void){
	send(presGoodbyeKissMsg);
}


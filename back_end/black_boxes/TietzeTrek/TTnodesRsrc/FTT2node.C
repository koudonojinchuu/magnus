#include "crossTrainer.h"
#include "FTT2node.h"
#include "nodeMsg.h"
#include "FTTpres.h"
#include "FfreeList.h"
#include "FTTpresNode.h"
#include "FrenumberPresNode.h"
#include "FfreeWord.h"
#include "FGLink.h"
#include <stdio.h>
#include "text_functions.h"

// NOTE:  TT2 will not initialize if the presentation has an element
//        that only occurs once with plus or minus one exponent, i.e.,
//        if the last item of currentPres->gCount is 1.

FTT2node::FTT2node(FGLnode *aSupervisor,FpTree *apTree,FTTpres *currentPres,
		long TT2len, long newGen)
	:FTTnode(aSupervisor,apTree,currentPres,t2){
	this->TT2len=TT2len;
	this->newGen=newGen;
	this->currentPres=currentPres;
	
// zero the member objects for proper destruction in case 
// of initialization failure.
	newPresDispose=FALSE;
	newPres=ZERO;
	presGen=ZERO;
	renumber=ZERO;
	freePart=0;

	if(!initOK)return;
	long leastOccurring=(long)currentPres->gCount->LastItem();
	if(leastOccurring==1){
		initOK=FALSE;
		return;
	}
	
// set freePart, which is initialized to 0.  When a free index is
// included in the new word, its freePart entry is set to its
// new consecutively numbered local index, and the global symbol
// table is updated accordingly.
	
	long numItems=currentPres->globalSymbols->numItems;
	freePart=new FList(numItems);
	for(long i=1;i<=numItems;++i){
		freePart->Append((FObject*)0);
	}
	

// startup function
	func=TT2Func;	
	mailPerson=TT2mail;
}

FTT2node::~FTT2node(void){
	if(newPresDispose){delete newPres;newPres=0;}
	delete presGen;presGen=0;
	delete renumber;renumber=0;
	delete freePart; freePart=0;
}

Boolean FTT2node::TT2Func(FGLnode *node){
	FTT2node *n=(FTT2node*)node;	
	switch(n->mailPerson){
		case TT2mail: return n->TT2go();
		case presMail: return n->presGo();
		case renumberMail: return n->renumberGo();
		default: n->nodeErr(6);
	}
}

Boolean FTT2node::TT2go(void){
	// create the new word
	extern long freeWordBlockSize;
	FfreeWord *w=new FfreeWord(freeWordBlockSize);		
											// note w is initialized to the
											// trivial word
	long localGen=1;	// initialized for empty presentation
	long numSymbols=currentPres->globalSymbols->numItems;
	if(numSymbols){
		// presentation is not empty, so we choose random word in the
		// generators
		long lettersNo=randBnd(1,TT2len);		// number of letters
		long freeDelim=currentPres->g->numItems;
		localGen=freeDelim+1;
		for(long i=1;i<=lettersNo;++i){
			long index=randBnd(1,numSymbols);
			if(index>freeDelim){
				// if index indicates a 0 freePart:
				// Set index entry of freePart to localGen,
				// retrieve the global symbol for index, 
				// delete the index entry of globalSymbols,
				// and insert the global symbol for index at the localGen
				// entry.  Now set index to localGen and increment localGen.
				
				// Otherwise, set index equal to freePart, which is now
				// the redirected local index for the intended global symbol
				
				long freeIndex=(long)freePart->NthItem(index);
				if(!freeIndex){
					freePart->SetItem((Ptr)&localGen,index);
					long global=(long)currentPres->globalSymbols->NthItem(index);
					currentPres->globalSymbols->DeleteItem(index);
					currentPres->globalSymbols->InsertAt((FObject*)global,localGen);
					index=localGen;
					++localGen;
				}
				else{
					index=freeIndex;
				}
			}
			// now you are ready to create the new entry
			long exp=randBnd(0,1);
			if(!exp)exp=-1;
			w->rightMultiply(index,exp);
		}
	}
	
	// create the new relation
	
	w->leftMultiply(localGen,-1);

	// create newPres
	newPres=new FTTpres(currentPres);
	newPres->f->Append(w);
	newPres->globalSymbols->InsertAt((FObject*)(++newGen),localGen);
	send(incrementNewGenMsg);

	// normalize newPres
	mailPerson=presMail;
	return TRUE;
}

Boolean FTT2node::presGo(void){
	if(!presGen){
		presGen=new FTTpresNode(this,newPres);
	}
	if(!presGen->run()){
		delete presGen;presGen=0;
		mailPerson=renumberMail;		
	}
	return(TRUE);
}

Boolean FTT2node::renumberGo(void){
	if(!renumber){
		renumber=new FrenumberPresNode(this,newPres);
	}
	if(!renumber->run()){
		delete renumber;renumber=0;
		return FALSE;		// all done, successful outcome
	}
	return(TRUE);
}



void FTT2node::auRevoir(void){
	FTTnode::auRevoir();
	send(TT2AuRevoirMsg);
}

void FTT2node::goodbyeKiss(void){
	FTTnode::goodbyeKiss();
	if(TThasFailed){
		delete newPres;newPres=0;		// this may be created but not fully
								// initialized at timeout.  We get rid
								// of it here so the driver doesn't get
								// confused.
	}
	send(TT2GoodbyeKissMsg);
}

void FTT2node::respond(short aMessage){
	switch(aMessage){
		case presAuRevoirMsg:
		break;
		case presGoodbyeKissMsg:
		break;
		case renumberPresAuRevoirMsg:
		break;
		case renumberPresGoodbyeKissMsg:
			// the renumberPresNode makes a copy of
			// newPres and renumbers it.  We don't need
			// the original anymore so we dispose it and
			// set newPres to the renumbered version
			
			delete newPres;newPres=0;
			newPres=renumber->q;
			
			// the fat lady is stepping out onto the stage.
			// the construction of newPres is now complete.
			// ownership passes up the chain of command
			
			newPresDispose=FALSE;
		break;

		default: send(aMessage);
	}
}



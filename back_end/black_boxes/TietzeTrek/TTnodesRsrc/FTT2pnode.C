#include "crossTrainer.h"
#include "FTT2pnode.h"
#include "nodeMsg.h"
#include "FTTpres.h"
#include "FfreeList.h"
#include "FTTpresNode.h"
#include "FrenumberPresNode.h"
#include "FfreeWord.h"
#include "FGLink.h"

FTT2pnode::FTT2pnode(FGLnode *aSupervisor,FpTree *apTree,FTTpres *currentPres)
	:FTTnode(aSupervisor,apTree,currentPres,t2p){
	
	this->currentPres=currentPres;

// zero the member objects for proper destruction in case 
// of initialization failure.

	newPresDispose=FALSE;
	newPres=ZERO;
	presGen=ZERO;
	renumber=ZERO;
	
// check base class initialization
	if(!initOK)return;
	
	
// compute number of trivial letters
	FList *gCount=currentPres->gCount;
	long num=gCount->numItems;
	trivialLettersNo=0;
	for(long i=1;i<=num;++i){
		if((long)gCount->NthItem(i)==1)++trivialLettersNo;
	}

// check for presentation with no removable letters
	if(!trivialLettersNo){
		pTree->D.initInfo.TT2p.done=TRUE;
		initOK=FALSE;
		return;
	}
	
// startup function
	func=TT2pFunc;	
	mailPerson=TT2pmail;
}

FTT2pnode::~FTT2pnode(void){
	if(newPresDispose){delete newPres;newPres=0;}
	delete presGen;presGen=0;
	delete renumber;renumber=0;
}

Boolean FTT2pnode::TT2pFunc(FGLnode *node){
	FTT2pnode *n=(FTT2pnode*)node;	
	switch(n->mailPerson){
		case TT2pmail: return n->TT2pgo();
		case presMail: return n->presGo();
		case renumberMail: return n->renumberGo();
		default: n->nodeErr(6);
	}
}


Boolean FTT2pnode::TT2pgo(void){	
	// create newPres
	newPres=new FTTpres(currentPres);
	// target number is a 1-based offset from the end
	// of gCount

	long lettersNo=newPres->gCount->numItems;
	long letterIndex=lettersNo+1-pTree->D.initInfo.TT2p.targetNumber; 
	if((long)newPres->gCount->NthItem(letterIndex)!=1)nodeErr(39);
	long localLetter=(long)newPres->g->NthItem(letterIndex);
	
	// delete the trivial symbol from the global symbol table
	newPres->globalSymbols->DeleteItem(localLetter);
	
	// tricky, local symbols greater than localLetter must now
	// be decremented to stay in sync with the global symbol
	// table.  We do this while simultaneously
	// searching for the the relator to remove
	
	long relatorsNo=newPres->f->numItems;
	long relatorIndex=0;
	Boolean relatorFound=FALSE;
	for(long i=1;i<=relatorsNo;++i){
		long *DEREF,*endDEREF;
		FfreeWord *w=newPres->f->freeItem(i);
		w->leftJustify();
		DEREF=*w->wordHandle;
		endDEREF=&DEREF[2*w->noEntries];
		for(;DEREF<endDEREF;DEREF+=2){
			if(Abs(*DEREF)==localLetter){
				relatorIndex=i;
				relatorFound=TRUE;
			}
			else if(Abs(*DEREF)>localLetter)--*DEREF;
		}
	}
	if(!relatorFound)nodeErr(39);
	
	
	// delete the trivial relation
	// careful, DeleteItem only deletes
	// the copy of the handle in the list
		
	FfreeWord *w=newPres->f->freeItem(relatorIndex);	
	delete w;w=0;				
	newPres->f->DeleteItem(relatorIndex);
	newPresDispose=TRUE;	// TT2palg owns newPres
								// until this flag goes FALSE.  
								// It ain't over till
								// the fat lady sings.
	mailPerson=presMail;
	return TRUE;
}

Boolean FTT2pnode::presGo(void){
	if(!presGen){
		presGen=new FTTpresNode(this,newPres);
	}
	if(!presGen->run()){
		delete presGen;presGen=0;
		mailPerson=renumberMail;		
	}
	return(TRUE);
}

Boolean FTT2pnode::renumberGo(void){
	if(!renumber){
		renumber=new FrenumberPresNode(this,newPres);
	}
	if(!renumber->run()){
		delete renumber;renumber=0;
		return FALSE;		// all done, successful outcome
	}
	return(TRUE);

}


void FTT2pnode::auRevoir(void){
	FTTnode::auRevoir();
	send(TT2pAuRevoirMsg);
}

void FTT2pnode::goodbyeKiss(void){
	FTTnode::goodbyeKiss();
	if(TThasFailed){
		delete newPres;newPres=0;	// this may be created but not fully
								// initialized at timeout.  We get rid
								// of it here so the driver doesn't get
								// confused.
	}
	else{
		long target;
		target=pTree->D.initInfo.TT2p.targetNumber;
		++target;
		if(target>trivialLettersNo){
			pTree->D.initInfo.TT2p.done=TRUE;
		}
		else{
			pTree->D.initInfo.TT2p.targetNumber=target;
		}
	}
	send(TT2pGoodbyeKissMsg);
}

void FTT2pnode::respond(short aMessage){
	switch(aMessage){
		break;
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




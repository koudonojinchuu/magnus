#include "crossTrainer.h"
#include "FTT1pSubnode.h"
#include "FTT1pnode.h"
#include "Ftime.h"
#include "FpTree.h"
#include "nodeMsg.h"
#include "FwordsToGLvarNode.h"
#include "FfoldNode.h"
#include "FgrowNode.h"
#include "FCayleyCloneNode.h"
#include "FreducedFoldNode.h"
#include "FTTpresNode.h"
#include "FfreeList.h"
#include "FrenumberPresNode.h"
#include "FfreeWord.h"
#include "FTTpres.h"
#include "FGLink.h"
#include "FCayley.h"
#include "FCayleyInitInfo.h"
#include "TietzeTypes.h"
#include <stdio.h>
#include <limits.h>


FTT1pSubnode::FTT1pSubnode(FGLnode *aSupervisor,FTTpres *currentPres,
	long distinguishedRel,long timeRemaining)
	:FGLnode(aSupervisor){
	
	this->currentPres=currentPres;
	this->distinguishedRel=distinguishedRel;
	this->timeRemaining=timeRemaining;
	
// zero the member objects for proper destruction in case
// of initialization failure

	newPres=ZERO;
	newPresDispose=FALSE;
	wordsToGLvar=ZERO;
	fold=ZERO;
	reducedFold=ZERO;
	grow=ZERO;
	CcloneGen=ZERO;
	presGen=ZERO;
	renumber=ZERO;
	itsGLink=ZERO;
	Cayley=ZERO;
	CayleyClone=ZERO;


// the requirement that presentations locally number the free
// generators from g->numItems+1 to globalSymbols->numItems
// means that we don't need to search words whose generators
// are in this range.  Obviously no reduced word containing a
// free generator can be a consequence.

	lettersNo=currentPres->g->numItems;

	if(!lettersNo){
		xErr(59);
	}
	
// control
	relatorIsConsequence=FALSE;

	
	wordsNo=0;
	verticesNo=-1;
	edgesNo=-1;
	foldEntryLocation=FGLink::NA;
	

// construct Cayley, CayleyClone and their GLink
	
	itsGLink=new FGLink;
	
// Cayley
	FGLvarInitInfo* info= new FCayleyInitInfo;
	info->rNoEntries=lettersNo;
	info->iNoEntries=1;
	Cayley= new FCayley(this,itsGLink,(FCayleyInitInfo*)info);
	
// CayleyClone
	CayleyClone=new FGLvar(itsGLink,info);
	Str255 str;
	sprintf((char*)str,"CayleyClone");
	CtoPstr((char*)str);
	BlockMove((Ptr)str,(Ptr)CayleyClone->itsName,12);

	delete info;info=0;
	
// startup function
	mailPerson=wordsToGLvarMail;
	func=TT1pSubfunc;	
	
// control
	reentryFunc=ZERO;
}

FTT1pSubnode::~FTT1pSubnode(void){
	delete wordsToGLvar;wordsToGLvar=0;
	delete grow;grow=0;
	delete CcloneGen;CcloneGen=0;
	delete fold;fold=0;
	delete reducedFold;reducedFold=0; // CAUTION: reducedFold needs member
						  // v existing to properly dispose
	delete presGen;presGen=0;
	delete renumber;renumber=0;
	delete Cayley; Cayley=0;
	delete CayleyClone; CayleyClone=0;
	delete itsGLink;itsGLink=0;
	if(newPresDispose){delete newPres;newPres=0;}
}

Boolean FTT1pSubnode::TT1pSubfunc(FGLnode *node){
	FTT1pSubnode *n=(FTT1pSubnode*)node;
	switch(n->mailPerson){
		case wordsToGLvarMail: return n->wordsToGLvarGo();
		case foldMail: return n->foldGo();
		case growMail: return n->growGo();
		case CayleyCloneMail: return n->CayleyCloneGo();
		case reducedFoldMail: return n->reducedFoldGo();
		case presMail: return n->presGo();
		case renumberMail: return n->renumberGo();
		default: n->nodeErr(6);
	}
}


Boolean FTT1pSubnode::wordsToGLvarGo(void){
	if(!wordsToGLvar){	
		FCayley *v=Cayley;
		v->j=0;			
		v->I();			
		v->e=1;
		v->V();
		v->putFlag(v->cv,FALSE);   // do not duplicate flag
		GLref root=v->cv;

		v->itsRoot=root;  // Cayley needs to know where the root is
		
		// this is a convenient point to grow the initial set of
		// edges of Cayley at the root.
		v->bv=v->cv;
		for(v->e=1;v->e<=lettersNo;++v->e){
			v->V();
			v->putFlag(v->cv,FALSE);
			v->e=-v->e;
			v->V();	
			v->putFlag(v->cv,FALSE);
			v->e=-v->e;
		}
		wordsToGLvar=new FwordsToGLvarNode(this, Cayley,lettersNo,
			currentPres->f,root,sizeof(GLref));
	}
	if(!wordsToGLvar->run()){
		delete wordsToGLvar;wordsToGLvar=0;
		mailPerson=foldMail;
	}
	return(TRUE);

}

Boolean  FTT1pSubnode::foldGo(void){
	if(!fold){
		fold=new FfoldNode(this,Cayley);
	}
	if(!fold->run()){
		delete fold;fold=0;
		if(relatorIsConsequence){
			mailPerson=presMail;
		}
		else mailPerson=growMail;
	}
	return(TRUE);
}


Boolean  FTT1pSubnode::growGo(void){
	if(!grow){	
		CayleyClone->j=0;
		CayleyClone->I();
		recycleStackSwap(Cayley,CayleyClone); // recycles holes created by
											  // foldNode	
		grow=new FgrowNode(this,Cayley,CayleyClone,lettersNo);
	}
	grow->run();
	if(grow->done){
		delete grow;grow=0;
		// at this point, the Cayley graph is fully enumerated but
		// the next consequence has not been detected.  This can only
		// happen if the group is finite.
		
		return FALSE;   
	}
	if(grow->needToClone){
		mailPerson=CayleyCloneMail;
	}
	else{
		mailPerson=reducedFoldMail;
	}
	return TRUE;
}
	
Boolean  FTT1pSubnode::CayleyCloneGo(void){
	if(!CcloneGen){
		CcloneGen= new FCayleyCloneNode(this,Cayley,CayleyClone,0,sizeof(GLref));
	}
	if(!CcloneGen->run()){
		delete CcloneGen;CcloneGen=0;
		mailPerson=growMail;
	}
	return(TRUE);
}

Boolean  FTT1pSubnode::reducedFoldGo(void){
	if(!reducedFold){
		Cayley->bv=foldEntryLocation;
		recycleStackSwap(Cayley,CayleyClone);	
		reducedFold=new FreducedFoldNode(this,Cayley);
	}
	if(!reducedFold->run()){
		delete reducedFold;reducedFold=0;
		if(relatorIsConsequence) mailPerson=presMail;
		else mailPerson=growMail;
	}
	return(TRUE);
}

Boolean FTT1pSubnode::presGo(void){
	if(!presGen){
		presGen=new FTTpresNode(this,newPres);
	}
	if(!presGen->run()){
		delete presGen;presGen=0;
		mailPerson=renumberMail;		
	}
	return(TRUE);

}

Boolean FTT1pSubnode::renumberGo(void){
	if(!renumber){
		renumber=new FrenumberPresNode(this,newPres);
	}
	if(!renumber->run()){
		delete renumber;renumber=0;
		return FALSE;		// all done, successful outcome
	}
	return(TRUE);
}


void FTT1pSubnode::wakeUp(void){
	startClock=clock();
}

void FTT1pSubnode::auRevoir(void){
	timeRemaining=timeRemaining+startClock-clock();
	if(timeRemaining<0){
		// when control returns we go to the trap.  
		
		reentryFunc=func;			// termination is not voluntary
		dirtyAbort=TRUE;
	}
	FGLnode::auRevoir();			// supports the abort mechanism
}


void FTT1pSubnode::goodbyeKiss(void){
	send(TT1pSubGoodbyeKissMsg);
	func=reentryFunc;		// in case were called again we will
							// reenter at the correct entry point
							
	reentryFunc=0;			// reentryFunc must be reinitialized here
							// because no initialization takes place
							// if the node is reentered.  The node
							// doesn't "know" what happens out there
							// in the "real world"
}

void FTT1pSubnode::respond(short aMessage){
	switch (mailPerson){
		case wordsToGLvarMail:
			switch (aMessage){
				case terminateWordMsg:{
					FCayley *v=Cayley;
					if(wordsToGLvar->index!=distinguishedRel){
						// close the circle
						v->bv=wordsToGLvar->itsRoot;
						v->e=-v->e;
						v->Lnk();
					}
					else{
						// add last edge and attach last vertex to
						// a new ideal vertex
						v->bv=v->cv;
						v->V();
						++v->j;
						v->I();		// base is now new ideal vertex
						v->e=-1;
						v->Lnk();
					}
				}
				break;
				case wordsToGLvarAuRevoirMsg:
					DisplayCounters();
				break;
				case wordsToGLvarGoodbyeKissMsg:
					DisplayCounters();
				break;			
				default: send(aMessage);
			}
		break;

		case foldMail:
// unlike TT1, we can find a consequence during the initial fold
			switch(aMessage){
				case identifiedWordMsg:
					// this relator is indeed a consequence of the others.
					// we just set a flag, which will be detected at
					// foldAuRevoir and/or foldGoodbyeKiss
					relatorIsConsequence=TRUE;
				break;
				case foldAuRevoirMsg:
					if(relatorIsConsequence){
						// we have determined that the distinguished relator
						// is indeed a consequence of the others, but the 
						// fold node is still running.
						
						fold->dirtyAbort=TRUE;
					}
					DisplayCounters();
				break;
				case foldGoodbyeKissMsg:
					if(relatorIsConsequence) consequenceFound();
					DisplayCounters();
				break;	
				default: send(aMessage);
			}
		break;
		
		case growMail:
		
			switch(aMessage){
				case foldEntryLocationMsg:
					foldEntryLocation=Cayley->bv;
				break;
				case growAuRevoirMsg:
				case growGoodbyeKissMsg:
					DisplayCounters();
				break;
				case cloneAuRevoirMsg:
					transferCounters();
					DisplayCounters();
				break;
				case cloneGoodbyeKissMsg:
					transferCounters();
					DisplayCounters();
				break;
				default: send(aMessage);
			}	
		break;

		case reducedFoldMail:
			switch (aMessage){
				case identifiedWordMsg:
					// this relator is indeed a consequence of the others.
					// we just set a flag, which will be detected at
					// foldAuRevoir and/or foldGoodbyeKiss
			
					relatorIsConsequence=TRUE;
				break;
				case foldAuRevoirMsg:
					if(relatorIsConsequence){
						// we have determined that the distinguished relator
						// is indeed a consequence of the others, but the 
						// reduced fold node is still running.
						
						reducedFold->dirtyAbort=TRUE;
					}
					DisplayCounters();
				break;
				case foldGoodbyeKissMsg:
					if(relatorIsConsequence)consequenceFound();
					DisplayCounters();
				break;	
				default: send(aMessage);			
			}	
		break;
		case presMail:
			switch(aMessage){
				case presAuRevoirMsg:
				break;
				case presGoodbyeKissMsg:
				break;
				default: send(aMessage);
			}
		break;
		case renumberMail:
			switch(aMessage){
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
					send(presGoodbyeKissMsg);
				break;

				default: send(aMessage);
			}
		break;
		case CayleyCloneMail:
			case cloneAuRevoirMsg:
				transferCounters();
				DisplayCounters();
			break;
			case cloneGoodbyeKissMsg:
				transferCounters();
				DisplayCounters();
			break;
		break;
		default: send(aMessage);
	}
}

void FTT1pSubnode::consequenceFound(void){
	// the fat lady is warming up for the final scene.
	// we make a copy newPres of currentPres and
	// delete the distinguished relator
	
	newPres=new FTTpres(currentPres);
	
	// careful, DeleteItem only deletes
	// the copy of the handle in the list
	FfreeWord *consequence=newPres->f->freeItem(distinguishedRel);	
	delete consequence;consequence=0;					
	newPres->f->DeleteItem(distinguishedRel);
	newPresDispose=TRUE;	// TT1pSubalg owns newPres
							// until this flag goes FALSE.  
							// It ain't over till
							// the fat lady sings.
}


void FTT1pSubnode::DisplayCounters(void){
	long counter;
	counter=Cayley->InUse[GLr]/2;  // because each edge has two packet entries
							  // associated with it
	if(edgesNo!=counter){
		edgesNo=counter;
	}
	counter=Cayley->Total[GLrv];
	if(verticesNo!=counter){
		verticesNo=counter;
	}
}

void FTT1pSubnode::transferCounters(void){
	long index;
	for(index=0;index<GLflags;++index){
		Cayley->Packets[index]=Cayley->Packets[index]+CayleyClone->Packets[index];
		CayleyClone->Packets[index]=0;
		Cayley->Seeks[index]=Cayley->Seeks[index]+CayleyClone->Seeks[index];
		CayleyClone->Seeks[index]=0;
		Cayley->Total[index]=Cayley->Total[index]+CayleyClone->Total[index];
		CayleyClone->Total[index]=0;
		Cayley->InUse[index]=Cayley->InUse[index]+CayleyClone->InUse[index];
		CayleyClone->InUse[index]=0;
	}
}



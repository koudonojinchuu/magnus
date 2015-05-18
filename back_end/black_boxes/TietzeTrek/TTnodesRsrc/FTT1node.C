#include "crossTrainer.h"
#include "FTT1node.h"
#include "FTTnode.h"
#include "FuNode.h"
#include "FwordsToGLvarNode.h"
#include "FgrowNode.h"
#include "FCayleyCloneNode.h"
#include "FfoldNode.h"
#include "FreducedFoldNode.h"
#include "FpTree.h"
#include "FGLink.h"
#include "FCayley.h"
#include "FCayleyInitInfo.h"
#include "FuTree.h"
#include "FuTreeInitInfo.h"
#include "FfreeWord.h"
#include "FfreeList.h"
#include "FTTpres.h"
#include "FTTpresNode.h"
#include <stdio.h>
#include <limits.h>
#include "nodeMsg.h"
#include "Ftime.h"
#include "FrandomRef.h"
// DEBUG
#include "FdebugNode.h"
// END DEBUG


FTT1node::FTT1node(FGLnode *aSupervisor,FpTree *apTree,FTTpres *currentPres)
	:FTTnode(aSupervisor,apTree,currentPres,t1){
	
// zero the member objects for proper destruction in case 
// of initialization failure.

// PROPOSED SKIP

	wordsToGLvar=0;
	grow=0;
	lettersNo=currentPres->g->numItems;
	newPres=0;
	uLink=0;
	uTree=0;
	buffer=0;
	CayleyLink=0;
	Cayley=0;
	CayleyClone=0;
	uNode=0;
	fold=0;
	CcloneGen=0;
	reducedFold=0;
	presGen=0;
	
// check to see if base class initialization succeeded
	if(!initOK) return;

// check for trivial presentation
	if(!currentPres->f->numItems){
		pTree->D.initInfo.TT1.done=TRUE;
		initOK=FALSE;
		return;			// initialization has failed
	}
	

// the requirement that presentations locally number the free
// generators from g->numItems+1 to globalSymbols->numItems
// means that we don't need to search words whose generators
// are in this range.  Obviously no reduced word containing a
// free generator can be a consequence.

	

// stuff owned by the algorithm
	newPresDispose=TRUE;
	
	wordsNo=0;
	verticesNo=-1;
	edgesNo=-1;
	foldEntryLocation=FGLink::NA;

// zero CayleyTime; it is initialized if and when the Cayley node boots
// up.  Afterwards, the non-zero value of CayleyTime prevents Cayley
// from booting up a spurious second time if no consequences are detected.
	
		CayleyTime=0;

// startup
	func=TT1func;
	mailPerson=presMail;
}

FTT1node::~FTT1node(void){
	delete presGen;presGen=0;
	delete wordsToGLvar;wordsToGLvar=0;
	delete fold;fold=0;
	delete grow;grow=0;
	delete CcloneGen;CcloneGen=0;
	delete reducedFold;reducedFold=0;
	delete uNode;uNode=0;	
	delete Cayley;
	delete CayleyClone;
	delete CayleyLink;
	
	// conditional disposal, if nobody else owns
	
	if(newPresDispose){
		delete newPres;newPres=0;
	}
	if(!pTree->D.initInfo.TT1.buffer && buffer){
		delete buffer; buffer=0;
	}
	if(!pTree->D.initInfo.TT1.uTree && uTree){
		delete uTree; uTree=0;
		delete uLink; uLink=0;
	}
}


Boolean FTT1node::TT1func(FGLnode *node){
	FTT1node *n=(FTT1node*)node;
	switch(n->mailPerson){
		case presMail: return n->presGo();
		case wordsToGLvarMail: return n->wordsToGLvarGo();
		case foldMail: return n->foldGo();
		case growMail: return n->growGo();
		case CayleyCloneMail: return n->CcloneGo();
		case reducedFoldMail: return n->reducedFoldGo();
		case uMail: return n->uGo();

		default: n->nodeErr(6);
	}
}

Boolean FTT1node::presGo(void){
	if(!presGen){	
		clock_t startTime=clock();
		// check for global buffer
		buffer=pTree->D.initInfo.TT1.buffer;
		if(buffer){
			bufferSearch();

			if(newPres){
				newPresDispose=TRUE;	// TT1alg owns newPres until this flag 
										// goes FALSE, i.e. when ownership
										// is passes to pTree

				// set up the presNode
				presGen=new FTTpresNode(this,newPres);
			}
			else{
				// release old buffer and uTree
				delete buffer;  buffer=0;
				FGLink *uLink=uTree->itsGLink;
				delete uTree; uTree=0;
				delete uLink; uLink=0;
				
				// zero global handles
				
				pTree->D.initInfo.TT1.buffer=0;
				pTree->D.initInfo.TT1.uTree=0;
				
				// if this is the second attempt to search the buffer,
				// then the local CayleyTime will be nonzero.  
				// In this event, return unsuccessfully.
				
				if(CayleyTime){
					TThasFailed=TRUE;
					return(FALSE);		
				}
				
				// Otherwise set the local CayleyTime and generate the
				// Cayley graph.
				else{
					CayleyTime=(runningBalance+startTime-clock())/3;	
					mailPerson=wordsToGLvarMail; 
				}

			}
		}
		// Set the local CayleyTime and generate the Cayley graph
		else{
			CayleyTime=(runningBalance+startTime-clock())/3;
			mailPerson=wordsToGLvarMail; 
		}

	}
	if(presGen){
		if(!presGen->run()){
			delete presGen;presGen=0;
			return FALSE;		// all done, successful outcome
		}
	}
	return(TRUE);
}

Boolean FTT1node::wordsToGLvarGo(void){
	clock_t startTime=clock();
	if(!wordsToGLvar){
		// set up the timer for the generation of the Cayley graph
		
		timer=0;
		
		// create the Cayley and CayleyClone GLvars and their GLink
		CayleyLink=new FGLink;

		// Cayley
		FGLvarInitInfo *info= new FCayleyInitInfo;
		info->rNoEntries=lettersNo;
		info->iNoEntries=1;
		Cayley= new FCayley(this,CayleyLink,(FCayleyInitInfo*)info);

		FCayley *v=Cayley;
		v->j=0;			
		v->I();						
		v->e=1;
		v->V();

		// CayleyClone
		CayleyClone=new FCayley(this,CayleyLink,(FCayleyInitInfo*)info);
		Str255 str;
		sprintf((char*)str,"CayleyClone");
		CtoPstr((char*)str);
		BlockMove((Ptr)str,(Ptr)CayleyClone->itsName,12);
		delete info;

		v->putFlag(v->cv,FALSE);
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
		wordsToGLvar=new FwordsToGLvarNode(this,Cayley,lettersNo,
			currentPres->f,root,sizeof(GLref));
	}
	Boolean callAgain=wordsToGLvar->run();
	timer+=clock()-startTime;
	if(timer>CayleyTime){
		wordsToGLvar->abort=TRUE;
	}
	if(!callAgain){
		if(wordsToGLvar->abort){
			TThasFailed=TRUE;
			return(FALSE);
		}
		else{
			delete wordsToGLvar;wordsToGLvar=0;
			mailPerson=foldMail;
		}
	}
	return(TRUE);
}

Boolean  FTT1node::foldGo(void){
	clock_t startTime=clock();
	if(!fold){
		fold= new FfoldNode(this,Cayley);
	}
	Boolean callAgain=fold->run();
	timer+=clock()-startTime;
	if(timer>CayleyTime){
		fold->abort=TRUE;
	}
	if(!callAgain){
		if(fold->abort){
			TThasFailed=TRUE;
			return(FALSE);
		}
		else{
			delete fold; fold=0;
			mailPerson=growMail;
		}
	}
	return(TRUE);
}


Boolean  FTT1node::growGo(void){
	clock_t startTime=clock();
	if(!grow){
		CayleyClone->j=0;
		CayleyClone->I();
		recycleStackSwap(Cayley,CayleyClone); // recycles holes created by
											  // foldNode
		grow=new FgrowNode(this,Cayley,CayleyClone,lettersNo);
	}
	grow->run();
	timer+=clock()-startTime;
	if(grow->done){
		delete grow; grow=0;
		// at this point, the Cayley graph is fully enumerated.	
		send(finiteGroupMsg);
		mailPerson=uMail; 
	}
	else if(grow->needToClone){
		mailPerson=CayleyCloneMail;
	}
	else{
		mailPerson=reducedFoldMail;
	}
	if(timer>CayleyTime){
		// we defeat all the elaborate switching above because we
		// are out of time.  Notice we are assuming here that the
		// grow node left Cayley in a consistent state.
		delete grow; grow=0;
		
		mailPerson=uMail;
	}
	return TRUE;
}
	
Boolean  FTT1node::CcloneGo(void){
	clock_t startTime=clock();
	if(!CcloneGen){
		CcloneGen
			= new FCayleyCloneNode(this,Cayley,CayleyClone,0,sizeof(GLref));
	}
	Boolean callAgain=CcloneGen->run();
	timer+=clock()-startTime;
	if(timer>CayleyTime){
		CcloneGen->abort=TRUE; 
	}
	if(!callAgain){
		if(CcloneGen->abort)mailPerson=uMail;
		else mailPerson=growMail;
		delete CcloneGen;CcloneGen=0;
	}
	return(TRUE);
}

Boolean  FTT1node::reducedFoldGo(void){
	clock_t startTime=clock();
	if(!reducedFold){
		Cayley->bv=foldEntryLocation;
		recycleStackSwap(Cayley,CayleyClone);	
		reducedFold=new FreducedFoldNode(this,Cayley);
	}
	Boolean callAgain=reducedFold->run();
	timer+=clock()-startTime;
	if(timer>CayleyTime){
		reducedFold->abort=TRUE;
	}
	if(!callAgain){
		if(reducedFold->abort)mailPerson=uMail;
		else mailPerson=growMail;
		delete reducedFold;reducedFold=0;
	}
	return(TRUE);
}

Boolean FTT1node::uGo(void){

	clock_t startTime=clock();	
	if(!uNode){
		// dispose any nodes that are no longer needed but
		// may still exist.
		
		delete grow; grow=0;
		delete reducedFold; reducedFold=0;
		delete CcloneGen; CcloneGen=0;
		
		// create the buffer of consequences
						
		if(buffer || pTree->D.initInfo.TT1.buffer){
			nodeErr(42);
		}
		buffer=new FrandomRef(CONSEQUENCE_BLOCKSIZE);

		// create the uTree
		if(uTree || pTree->D.initInfo.TT1.uTree){
			nodeErr(43);
		}
		uLink=new FGLink;
		FGLvarInitInfo *info= new FuTreeInitInfo();
		info->rNoEntries=lettersNo;
		if(!info->rNoEntries)xErr(51);
		uTree= new FuTree(uLink,(FuTreeInitInfo*)info);
		delete info;
		uNode=new FuNode(this,uTree,sizeof(GLref),Cayley->itsRoot);
		uTime=UTREE_FRACTION*CayleyTime;
		timer=0;
	}
	Boolean callAgain=uNode->run();
	timer+=clock()-startTime;
	if(timer>uTime){
		uNode->abort=TRUE;
	}
	if(!callAgain){
		delete uNode; uNode=0;
		delete Cayley; Cayley=0;
		delete CayleyClone; CayleyClone=0;
		delete CayleyLink; CayleyLink=0;
		pTree->D.initInfo.TT1.uTree=uTree;		// ownership of uTree and buffer
		pTree->D.initInfo.TT1.buffer=buffer;	// passes to pTree here.
											
		// now search the buffer and create a new presentation
		mailPerson=presMail;

	}
	return(TRUE);	
}

void FTT1node::auRevoir(void){
	FTTnode::auRevoir();
	send(TT1AuRevoirMsg);
}

void FTT1node::goodbyeKiss(void){
	long targetNumber;
	FTTnode::goodbyeKiss();
	if(TThasFailed){
		delete newPres;newPres=0;		// this may be created but not fully
								// initialized at timeout.  We get rid
								// of it here so the driver doesn't get
								// confused.
		pTree->D.bank.TT1account.minRequired
			=2*pTree->D.bank.TT1account.minRequired;
	}
	else{
		if(!newPres)nodeErr(35);
		newPresDispose=FALSE;  			// the fat lady is singing!
	}
	send(TT1GoodbyeKissMsg);
}

void FTT1node::respond(short aMessage){
	if(aMessage==GLvarIsCleanMsg || aMessage==GLvarIsDirtyMsg)return;
	switch (mailPerson){
		case uMail:
			switch(aMessage){
				case uInitializePacketMsg:{
					// Tricky: to expedite processing we initialize
					// DEREFCayleyPacket into the Cayley real link packet 
					// of the Cayley vertex indicated by externalRef.
					// We lock the block containing this packet, if it
					// exists, otherwise we set DEREFCayleyPacket to zero.
					
					Ptr DEREF=Cayley&uNode->externalRef;
					Cayley->xGetPacketRef(CayleyPacketRef,GLr,DEREF);
					Cayley-uNode->externalRef;
					if(isNull(CayleyPacketRef))DEREFCayleyPacket=0;
					else DEREFCayleyPacket=Cayley&CayleyPacketRef;
				}
				break;
				case uBeforeEdgeMsg:{
					// We address off of DEREFCayleyPacket to get the
					// the Cayley slot corresponding to uTree->e.  If this
					// slot is defined we dereference it and reset externalRef
					// to the opposite edge vertex. 
					
					long offset=Cayley->xmodIndexToOffset(GLr,uTree->e);
					Boolean edgeMatch=FALSE;
					if(DEREFCayleyPacket){
						GLref refDatum;
						BlockMove(&DEREFCayleyPacket[offset],
									(Ptr)&refDatum,sizeof(GLref));
						if(!isNull(refDatum)){
							edgeMatch=TRUE;
							Cayley->xEdgeEntryToOwner(uNode->externalRef,refDatum);
						}
					}
					if(!edgeMatch)uNode->externalRef=FGLink::NA;
				}
				break;
				case uAfterEdgeMsg:
					// if externalRef is the Cayley root, then we push
					// uTree->cv onto the consequence buffer
					if(SameAddr(uNode->externalRef,Cayley->itsRoot)){
						buffer->Push(uTree->cv);
					}
				break;
				case uTerminatePacketMsg:
					// we unlock the block containing the Cayley
					// real link packet, if it exists
					
					if(!isNull(CayleyPacketRef))Cayley-CayleyPacketRef;
				break;
				case uAuRevoirMsg:
				case uGoodbyeKissMsg:
				break;
				default: send(aMessage);
			}
		break;

		case wordsToGLvarMail:
		
			switch (aMessage){
				break;					
				case terminateWordMsg:
					FCayley *v;
					v=Cayley;
					v->bv=wordsToGLvar->itsRoot;
					v->e=-v->e;
					v->Lnk();
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
			switch(aMessage){
				case foldAuRevoirMsg:
				break;
				case foldGoodbyeKissMsg:
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
				default: send(aMessage);
			}	
		break;
		case reducedFoldMail:
			switch (aMessage){
				case foldAuRevoirMsg:
					DisplayCounters();
				break;
				case foldGoodbyeKissMsg:
					DisplayCounters();
				break;	
				default: send(aMessage);			
			}	
		break;
		case presMail:
			switch(aMessage){
				break;
				case presAuRevoirMsg:
				break;
				case presGoodbyeKissMsg:{
					// newPres doesn't have a global
					// symbol table yet.  We make a copy of the 
					// currentPres table and load it into newPres
					//
					// CAUTION: newPres may not exist so we test for this.
					// Look before you leap.
					
					if(newPres){
						newPres->globalSymbols= new FList(currentPres->globalSymbols);
					}
				}
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

void FTT1node::DisplayCounters(void){
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

void FTT1node::transferCounters(void){
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

void FTT1node::bufferSearch(void){
	uTree=pTree->D.initInfo.TT1.uTree;
	if(!uTree || !buffer)nodeErr(40);
	if(newPres!=0) nodeErr(44);
	Boolean searching=TRUE;
	while(searching){
		GLref ref;
		buffer->popRandom(ref);
		if(!isNull(ref)){
			// check for duplicate
			FfreeWord *w=uTree->trace(ref);
			if(!currentPres->f->isDuplicate(w)){
				newPres= new FTTpres(TRUE);
				newPres->f= new FfreeList(currentPres->f);
				newPres->f->Append(w);
				newPresDispose=TRUE;// TT1alg owns newPres until this flag 
									// goes FALSE, i.e. when ownership
									// is passes to pTree
				searching=FALSE;

			}
			else{
				// duplicate consequences are ignored
				delete w;w=0;
			}

		}
		else searching=FALSE;
	}
}

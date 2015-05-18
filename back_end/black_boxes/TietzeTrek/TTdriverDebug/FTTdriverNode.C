#include "crossTrainer.h"
#include "FTTdriverNode.h"
#include "nodeMsg.h"
#include "FpLibNode.h"
#include "FpresCompareNode.h"
#include "FGLstepper.h"
#include "FTTpres.h"
#include "FGLink.h"
#include "FTT1node.h"
#include "FTT1pnode.h"
#include "FTT2node.h"
#include "FTT2pnode.h"
#include "FfreeList.h"
#include "FfreeWord.h"
#include "FpTree.h"
#include "FpTreeInitInfo.h"
#include "FpLib.h"
#include "FpLibInitInfo.h"
#include "FrandomRef.h"

#ifdef Frank_MemoryDebugging
#include "debugMemory.h"
extern long totalMemory,noObjects,objectMemory;
#endif  



FTTdriverNode::FTTdriverNode(FGLnode *aSupervisor,FTTpres *aTTpres,
	FGLink *aGLink,FpTree *apTree)
	:FGLwalkNode(aSupervisor,apTree){
	itsInputTTpres=aTTpres;
	itsGLink=aGLink;
	pTree=apTree;
	factorRank=-1;
	groupOrder=-1;
	newGen=itsInputTTpres->g->numItems;
	root=pTree->bv;

	displayParentSerial=-1;
	displaySerial=-1;
	displayPres=0;
	displayType=-1;

	// create the driver GLink

	itsGLink=new FGLink;

#ifdef Frank_MemoryDebugging
	extern FGLink *pLibLink;
	pLibLink=itsGLink;
#endif

	// create pLib
	
	FGLvarInitInfo *info= new FpLibInitInfo;
	pLib= new FpLib(itsGLink,(FpLibInitInfo*)info);
	delete info;info=0;
	
	// set up root for pLib
	pLib->j=0;
	pLib->I();
	pLib->e=1;
	pLib->V();
	pLib->root=pLib->cv;
	
	// set up the free presentation root for pLib
	pLib->j=1;
	pLib->I();
	pLib->e=1;
	pLib->V();
	pLib->freeRoot=pLib->cv;	
	
	// add itsInputTTpres to the library
	foundLinkList=FALSE;
	noFirstCoord=TRUE;
	noSecondCoord=TRUE;
	noThirdCoord=TRUE;
	genIndex=1;
	relIndex=1;
	if(itsInputTTpres->g->numItems==0){
		// the presentation is trivial.  This case is detected
		// externally and signalled by setting the hook to NA
		
		hook=FGLink::NA;
	}
	else hook=pLib->root;

	newPres=itsInputTTpres;
		
// classify the input presentation
	classify();

	// zero the nodes and working objects
	
	TT1=ZERO;
	TT1p=ZERO;
	TT2=ZERO;
	TT2p=ZERO;
	Lib=ZERO;
	currentPres=ZERO;
	newPres=ZERO;


#ifdef Frank_MemoryDebugging
	tout <= "GLink blocksize for pTree and pLib is "; 
	v->itsGLink->itsBlockManager->repBlockSize(tout) <= "\n";
#endif



	send(forgetPresMsg);
	itsInputTTpres=ZERO;	
	
#ifdef Frank_MemoryDebugging
	resetMemory();
	memout <= "Total available memory ";
	repMemSize(memout,totalMemory) <= "\n";
#endif  
	// control
	func=driverFunc;
	mailPerson=walkMail;
}

FTTdriverNode::~FTTdriverNode(void){

	delete currentPres;currentPres=0;
	delete newPres;newPres=0;
	delete TT1;TT1=0;
	delete TT1p;TT1p=0;
	delete TT2;TT2=0;
	delete TT2p;TT2p=0;
	delete Lib;Lib=0;
	delete pLib;pLib=0;
	delete itsGLink;itsGLink=0;
#ifdef Frank_MemoryDebugging
	extern FGLink *pLibLink;
	pLibLink=0;
#endif

}

Boolean FTTdriverNode::driverFunc(FGLnode *node){
	FTTdriverNode *n=(FTTdriverNode*)node;
	Boolean returnVal;
#ifdef Frank_GenuineMac
	TRY{
#endif
		switch(n->mailPerson){
			case walkMail: returnVal=n->walk();
			break;
			case TT1mail: n->currentType=t1; returnVal=n->TT1Go();
			break;
			case TT1pmail: n->currentType=t1p; returnVal=n->TT1pGo();
			break;
			case TT2mail: n->currentType=t2; returnVal=n->TT2Go();
			break;
			case TT2pmail: n->currentType=t2p; returnVal=n->TT2pGo();
			break;
			case LibMail: returnVal=n->LibGo();
			break;
			case doneMail: returnVal=n->doneGo();
			break;
			default: n->nodeErr(6);
		}
#ifdef Frank_GenuineMac
	}
	CATCH{
		NO_PROPAGATE;
	}
	ENDTRY;
#endif
	return returnVal;
}

Boolean FTTdriverNode::walk(void){

// the following code is identical to that of FGLwalkNode::walk().
// We need to repeat it here so that FTTdriverNode overriding
// subroutines virginVertex and processEdge will be called
//
// Is there a more elegant solution that doesn't require 
// code  replication?

	switch(entryPoint){
		case firstVertex:
			if(v==ZERO)nodeErr(4);
			if(v->Qideal())nodeErr(11);
// OUTPUT DISPLAY
//@rn			tout <= "*** pass " <= pTree->passNumber <=" ***" <= "\n";
//@rn ask me if I care
// END OUTPUT DISPLAY
			startUp();
			virginVertex(v->bv);
			entryPoint=nextVertex;
		case nextVertex:
			return(processVertex());
		case firstEdge:{
			// Process the first edge, if there is one.
			// if there is no defined edge, control flows to GLnextVertex
			
			Ptr DEREF=v&v->bv;
			GLref startRef;
			v->xGetPacketRef(startRef,GLr,DEREF);
			v-v->bv;
			if(isNull(startRef)){
				entryPoint=nextVertex;  // no real link
				return(TRUE);  
			}
			
			// start search for first edge at the very beginning
			GLref entryPacketRef;
			v->xsetToFirstPacket(entryPacketRef,startRef);  
			if(itsStepper->firstEdge(entryPacketRef)){
				// first edge detected
				GLref ref;
				v->xEdgeEntryToOwner(ref,itsStepper->refDatum);
				v->cv=ref;
				entryPoint=nextEdge; 
				return(processEdge()); 
			}
			else{	
				// real link has no edges
				
				entryPoint=nextVertex; 
				return(TRUE);  
			}
		}
		case nextEdge:{
			if(itsStepper->nextEdge()){
				// next edge detected
				GLref ref;
				v->xEdgeEntryToOwner(ref,itsStepper->refDatum);
				v->cv=ref;
				entryPoint=nextEdge; 
				return(processEdge()); 
			}
			else{	
				// real link has no more edges
				
				entryPoint=nextVertex; 
				return(TRUE);  
			}

		}
		default: nodeErr(6);
	}
	nodeErr(8);
}



Boolean  FTTdriverNode::vertex(){
	lowFail("FTTdriverNode::atVertex");
// we read the current vertex datum into pTree.D
// and load currentPres from memory


#ifdef Frank_MemoryDebugging
	long totalMem=FreeMem();
	extern long pTreeTotal,libTreeTotal,initialTotal;
	long permanentTotal=pTreeTotal+libTreeTotal;
	extern TEHandle debuggingTextHandle;
	if(debuggingTextHandle) 
		long textSize=GetHandleSize((**debuggingTextHandle).hText);
	tout <= "Remaining memory=" <= totalMem <= " bytes\n"
		<= "dictionary memory=" <= libTreeTotal <= " bytes\n"
		<= "presentation memory=" <= pTreeTotal <= " bytes\n"
		<= "LOST memory=" <= initialTotal-(totalMem+permanentTotal+textSize)
		<= " bytes\n";
	repMemory(memout);
#endif

	pTree->Get();
	if(currentPres){
		nodeErr(31);
	}
	currentPres=pTree->getPres();


// Perform TT1 when control returns	
	mailPerson=TT1mail;
	func=driverFunc;
	return(TRUE);
}

Boolean  FTTdriverNode::TT1Go(void){
	
	if(!pTree->D.initInfo.TT1.memoryOK || pTree->D.initInfo.TT1.done){
		actOnFailure(t1);
		return TRUE;
	}
	mailPerson=TT1mail;
	if(!TT1){
		send(calibrateTT1msg);
		if(pTree->D.bank.TT1account.minRequired<TT1minRequired)
			pTree->D.bank.TT1account.minRequired=TT1minRequired;
		if(pTree->D.bank.TT1account.payCheck<TT1PayCheck)
			pTree->D.bank.TT1account.payCheck=TT1PayCheck;

		TT1=new FTT1node(this,pTree,currentPres);		
		if(!TT1->initOK){
			delete TT1;TT1=0;
			actOnFailure(t1);
			return TRUE;
		}
		TT1RepeatNum=0;
	}
#ifdef Frank_GenuineMac
	TRY{
#endif
		if(!TT1->run()){
			delete TT1;TT1=0;
			if(newPres)mailPerson=LibMail;
			else actOnFailure(t1);
		}
		lowFail("FTTdriverNode::TT1Go::after run");
#ifdef Frank_GenuineMac
	}
	CATCH{
		if(gLastError==memFullErr){
			// we have run out of memory.  
			// set the memory flag FALSE and
			// post warning.
			
			pTree->D.initInfo.TT1.memoryOK=FALSE;
			SysBeep(1);
			tout <= "TT1 is out of memory and cannot produce"
				<=" any more consequences.  The enumeration of"
				<=" presentations has been COMPROMISED." <= "\n";

			// clean up
			delete TT1;TT1=0;
			actOnFailure(t1);
			NO_PROPAGATE;
		}
	}
	ENDTRY;
#endif
	return(TRUE);
}

void FTTdriverNode::releaseTT1memory(void){
	FrandomRef *buffer=pTree->D.initInfo.TT1.buffer;
	if(buffer!=0){
/* DEBUG
gout<="FTTdriverNode::releaseTT1memory deletes buffer= "<=(Handle)buffer<="\n";
END DEBUG */

		delete buffer;
		pTree->D.initInfo.TT1.buffer=0;
	}
	FuTree *uTree=pTree->D.initInfo.TT1.uTree;
	if(uTree!=0){
		FGLink *uLink=uTree->itsGLink;
/* DEBUG
gout<="FTTdriverNode::releaseTT1memory deletes uTree= "<=(Handle)uTree<="\n";
gout<="FTTdriverNode::releaseTT1memory deletes uLink= "<=(Handle)uLink<="\n";
END DEBUG */
		delete uTree;
		delete uLink;
		pTree->D.initInfo.TT1.uTree=0;
	}
}

Boolean  FTTdriverNode::TT1pGo(void){
		
	if(!pTree->D.initInfo.TT1p.memoryOK || pTree->D.initInfo.TT1p.done){
		actOnFailure(t1p);
		return TRUE;
	}
	if(!TT1p){
		send(calibrateTT1pmsg);
		if(pTree->D.bank.TT1paccount.minRequired<TT1pminRequired)
			pTree->D.bank.TT1paccount.minRequired=TT1pminRequired;
		if(pTree->D.bank.TT1paccount.payCheck<TT1pPayCheck)
			pTree->D.bank.TT1paccount.payCheck=TT1pPayCheck;
		if(TT1pRepeatMax==AUTO_SET) TT1pRepeatMax=currentPres->f->numItems;

		TT1p=new FTT1pnode(this,pTree,currentPres);
		if(!TT1p->initOK){
			delete TT1p;TT1p=0;
			actOnFailure(t1p);
			return TRUE;
		}
		TT1pRepeatNum=0;
	}
#ifdef Frank_GenuineMac
	TRY{
#endif
		if(!TT1p->run()){

			delete TT1p;TT1p=0;
			lowFail("FTTdriverNode::TT1pGo::after releasing memory");
			if(newPres){
				mailPerson=LibMail;
			}
			else{
				actOnFailure(t1p);
			}
		}
#ifdef Frank_GenuineMac
	}
	CATCH{
		if(gLastError==memFullErr){
			// we have run out of memory.  
			// set the memory flag FALSE and
			// post warning.
			
			pTree->D.initInfo.TT1p.memoryOK=FALSE;
			SysBeep(1);
			tout <= "TT1p is out of memory and cannot"
				<=" determine if this relator is a "
				<="consequence of the others.  The enumeration"
				<=" of presentations has been COMPROMISED." <= "\n";

			// clean up
			delete TT1p;TT1p=0;
			actOnFailure(t1p);
			NO_PROPAGATE;
		}
	}
	ENDTRY;
#endif
		
	return(TRUE);
}

Boolean  FTTdriverNode::TT2Go(void){
	if(!pTree->D.initInfo.TT2.memoryOK || pTree->D.initInfo.TT2.done){
		actOnFailure(t2);
		return TRUE;
	}
	if(!TT2){
		send(calibrateTT2msg);
		if(pTree->D.bank.TT2account.minRequired<TT2minRequired)
			pTree->D.bank.TT2account.minRequired=TT2minRequired;
		if(pTree->D.bank.TT2account.payCheck<TT2PayCheck)
			pTree->D.bank.TT2account.payCheck=TT2PayCheck;

		TT2=new FTT2node(this,pTree,currentPres,TT2len,newGen);
		if(!TT2->initOK){
			delete TT2;TT2=0;
			actOnFailure(t2);
			return TRUE;
		}
		TT2RepeatNum=0;
	}
	if(!TT2->run()){
		delete TT2;TT2=0;
		if(newPres)mailPerson=LibMail;
		else actOnFailure(t2);
	}
	return(TRUE);
}

Boolean  FTTdriverNode::TT2pGo(void){
	if(!pTree->D.initInfo.TT2p.memoryOK || pTree->D.initInfo.TT2p.done){
		actOnFailure(t2p);
		return TRUE;
	}
	if(!TT2p){
		send(calibrateTT2pmsg);
		if(pTree->D.bank.TT2paccount.minRequired<TT2pminRequired)
			pTree->D.bank.TT2paccount.minRequired=TT2pminRequired;
		if(pTree->D.bank.TT2paccount.payCheck<TT2pPayCheck)
			pTree->D.bank.TT2paccount.payCheck=TT2pPayCheck;
		if(TT2pRepeatMax==AUTO_SET) TT2pRepeatMax=currentPres->g->numItems;
		
		TT2p=new FTT2pnode(this,pTree,currentPres);
		if(!TT2p->initOK){
			delete TT2p;TT2p=0;
			actOnFailure(t2p);
			return TRUE;
		}
		TT2pRepeatNum=0;
	}
	if(!TT2p->run()){
		delete TT2p;TT2p=0;
		if(newPres)mailPerson=LibMail;
		else actOnFailure(t2p);
	}
	return(TRUE);
}

Boolean FTTdriverNode::LibGo(void){
	extern long presCount;

	if(!Lib){
		send(calibrateLibmsg);
#ifdef Frank_MemoryDebugging
	switch(currentType){
		case t1: memout <= "TT1: Lib\n";
		break;
		case t1p: memout <= "TT1p: Lib\n";
		break;
		case t2: memout <= "TT2: Lib\n";
		break;
		case t2p: memout <= "TT2p: Lib\n";
		break;
		default: nodeErr(6);
	}

	repMemory(memout);
#endif
		presentationIsNew=TRUE;
		Lib=new FpLibNode(this,pLib,pTree,newPres);
	}
	if(!Lib->run()){
		delete Lib;Lib=0;
		if(!presentationIsNew){
			delete newPres;newPres=0;
		}
		else{
			long oldSerial=pTree->D.serialNumber;
			Dsave=pTree->D;
			pTree->e=pTree->eNext();
			pTree->V(++presCount);
			
			displayParentSerial=oldSerial;
			displaySerial=presCount;
			displayPres=newPres;
			displayType=currentType;
			send(displayPresMsg);

			pTree->putPres(newPres);
			pTree->cPut();	
			classify();
			hook=FGLink::NA;
			foundLinkList=FALSE;
			delete newPres;newPres=0;
			pTree->D=Dsave;
		}
// Regardless of whether or not you found a duplicate, the
// Transformation operation was successful
		
		actOnSuccess(currentType);
	}
	return(TRUE);
}

void FTTdriverNode::actOnFailure(short aTTtype){
	mailPerson=nextTT(aTTtype);
	switch(aTTtype){
		case t1:{
			// if there are any presentations left in the buffer,
			// and we have not produced TT1RepeatMax presentations,
			// then we try again.
			// Otherwise we release the memory owned by the pTree vertex

			Boolean allDone=TRUE;
			if(TT1RepeatNum<TT1RepeatMax 
				 && pTree->D.initInfo.TT1.buffer){
				if(pTree->D.initInfo.TT1.buffer->numItems){
					mailPerson=TT1mail;
					allDone=FALSE;
				}
			}
			if(allDone){
				releaseTT1memory();
			}
		}
		break; 
		case t1p: 
		break; 
		case t2: 
		break; 
		case t2p: 
		break;
		default: xErr(58);
	}
}

long FTTdriverNode::nextTT(short aTTtype){
#ifdef Frank_MemoryDebugging
	switch(aTTtype){
		case t1: memout <= "TT1 is done\n";
		break;
		case t1p: memout <= "TT1p is done\n";
		break;
		case t2: memout <= "TT2 is done\n";
		break;
		case t2p: memout <= "TT2p is done\n";
		break;
		default: nodeErr(6);
	}

	repMemory(memout);
#endif

	switch(aTTtype){
		case t1: return TT1pmail;
		case t1p: return TT2mail;
		case t2: return TT2pmail;
		case t2p: return doneMail;
		default: nodeErr(6);
	}
}

void FTTdriverNode::actOnSuccess(short aTTtype){
	switch(aTTtype){
		case t1: 
			// if we are not going to repeat TT1 then we
			// release all the memory stored in the pTree vertex
			// for use by TT1
			if((++TT1RepeatNum)<TT1RepeatMax){
				mailPerson=TT1mail;
			}
			else{
				releaseTT1memory();
				mailPerson=nextTT(t1);
			}
		break; 
		case t1p:
			if((++TT1pRepeatNum)<TT1pRepeatMax){
				mailPerson=TT1pmail;
			}
			else{
				mailPerson=nextTT(t1p);
			}
		break; 
		case t2:
			if((++TT2RepeatNum)<TT2RepeatMax){
				mailPerson=TT2mail;
			}
			else{
				mailPerson=nextTT(t2);
			}
		break; 
		case t2p:
			if((++TT2pRepeatNum)<TT2pRepeatMax){
				mailPerson=TT2pmail;
			}
			else{
				mailPerson=nextTT(t2p);
			}
		break;
		default: xErr(58);
	}
}


void FTTdriverNode::classify(void){
	
	long k=newPres->globalSymbols->numItems;
	long n=newPres->g->numItems;
	long r=newPres->f->numItems;

	if(k-n>0 && !pLib->containsFreeFactorPres){
		pLib->containsFreeFactorPres=TRUE;
		factorRank=k;
		send(freeFactorMsg);
	
	}

	if(k<2 && !pLib->containsCyclicPres){
		pLib->containsCyclicPres=TRUE;
		send(cyclicGroupMsg);
	}

	if(isNull(hook)){
		// this is an indication that the empty presentation
		// is to be "added" to the library;
		
		pLib->containsEmptyPres=TRUE;
		send(trivialGroupMsg);
		return;
	}

	if(!n){
		// this is a non-trivial free group.  We half-link to
		// pTree->cv along the rank of the free group
		
		pLib->bv=pLib->freeRoot;
		pLib->e=k;
		pLib->halfLink(pTree->cv);
		send(freeGroupMsg);
		return;
	}
	
	if(foundLinkList){
		// insert companion after hook.
		// CAUTION: hook may not be the terminal vertex in the
		// linklist if the library node encountered a memory
		// exception and aborted.
		
		pTree->D.link.prev=hook;
		pTree->cPut();
		GLref cvRef=pTree->cv;
		pTree->cv=hook;
		pTree->cGet();
		GLref next=pTree->D.link.next;
		pTree->D.link.next=cvRef;
		pTree->cPut();
		pTree->cv=cvRef;
		pTree->cGet();
		pTree->D.link.next=next;
		pTree->cPut();
		return;
	}
		

	FList *count=newPres->gCount;
	FfreeList *rels=newPres->f;
	pLib->bv=hook;

	// first coordinate, number of generators
	
	if(noFirstCoord){
		pLib->e=k;
		pLib->V();
		pLib->bv=pLib->cv;
	}
	
	// second coordinate, number of generators which occur
	// at least once in a relator
	
	if(noSecondCoord){
		pLib->e=n;
		pLib->V();
		pLib->bv=pLib->cv;
	}
	
	// third coordinate, number of generators which occur
	// at least once in a relator
	
	if(noThirdCoord){
		pLib->e=r;
		pLib->V();
		pLib->bv=pLib->cv;
	}

	
	// number of occurrences, up to inverse, of the i'th
	// most frequently occurring generator
				
	for(long i=genIndex;i<=n;++i){
		pLib->e=(long)count->NthItem(i);
		pLib->V();
		pLib->bv=pLib->cv;
	}
	
	// number of letters in the i'th longest relator
	
	for(int i=relIndex;i<r;++i){
		FfreeWord *w=rels->freeItem(i);
		pLib->e=w->wordLength;
		pLib->V();
		pLib->bv=pLib->cv;
	}
	
	// the last vertex edge points to pTree->cv, but pTree->cv does not
	// point back to this last vertex.  Observe that pTree->cv is in a
	// different tree whose edges have meaning unrelated to the library
	// classification.  Hence we are creating a "corrupt edge" which
	// certain GLvar routines trap as an error.  So be careful.

	FfreeWord *w=rels->freeItem(r);
	pLib->e=w->wordLength;
	pLib->halfLink(pTree->cv);
}
	
Boolean  FTTdriverNode::doneGo(void){
	v->Put();
	delete currentPres;currentPres=0;
	mailPerson=walkMail;
	return(TRUE);
}

Boolean FTTdriverNode::processEdge(void){	
	// we override this routine so that it only invokes
	// virginVertex if cv is a virgin.  The edge processing
	// functions are not invoked.

	Ptr DEREF=v&v->cv;
	long IDvertex=v->xGetID(DEREF,IDslot);
	v-v->cv;
	if(IDvertex<IDchild){
		//virgin, edge is maximal
		virginVertex(v->cv);
	}
	else if(IDvertex==IDchild){
		// something is wrong, because CpTree
		// is supposed to be contractible
		nodeErr(30);
	}
	else if(IDvertex==IDadult){
		if(SameAddr(v->bv,v->cv)){
			if(v->e>0){				
				// something is wrong, because CpTree
				// is supposed to be contractible
				nodeErr(30);
			}
		}
	}
	else nodeErr(12);
	return(TRUE);
}


void FTTdriverNode::virginVertex(const GLref &virgin){	
	// we override this method so that it only puts the
	// virgin on the stack if its passNumber is less than
	// or equal to the current pass number.  The pass number
	// of the vertex is stored in the passSlot entry of the
	// walk ID storage area.
	
	Ptr DEREF=v&virgin;
	long passNumber=v->xGetID(DEREF,pTree->passSlot);
	if(passNumber<=pTree->passNumber)childVertices->Push(virgin);
	v->xPutID(DEREF,IDslot,IDchild);
	v-virgin;
}


Boolean FTTdriverNode::finish(void){

// increase the pass number
	++pTree->passNumber;
		
// cancel the IDslot, because it will
// be set again at firstVertex time

	v->cancelSlot(IDslot);
	
// restart
	v->bv=root;
	mailPerson=walkMail;
	entryPoint=firstVertex;
	return(TRUE);
}


void FTTdriverNode::auRevoir(void){
	FGLwalkNode::auRevoir();
	send(driverAuRevoirMsg);
}

void FTTdriverNode::goodbyeKiss(void){
	send(driverGoodbyeKissMsg);
}


void FTTdriverNode::respond(short aMessage){
	if(aMessage==pVertexInitMsg){
		pTreeRealType *D=&pTree->D;
	// TT1 initialization
		D->initInfo.TT1.uTree=0;
		D->initInfo.TT1.buffer=0;
		D->initInfo.TT1.memoryOK=TRUE;
		D->initInfo.TT1.done=FALSE;
		D->bank.TT1account.balance=0;
		D->bank.TT1account.minRequired=TT1minRequired;
		D->bank.TT1account.payCheck=TT1PayCheck;
	
	// TT1p initialization
		D->initInfo.TT1p.loadRel=FGLink::NA; // can't allocate this here
											// because we don't know its size yet.
		D->initInfo.TT1p.memoryOK=TRUE;
		D->initInfo.TT1p.done=FALSE;
		D->bank.TT1paccount.balance=0;
		D->bank.TT1paccount.minRequired=TT1pminRequired;
		D->bank.TT1paccount.payCheck=TT1pPayCheck;
	
	// TT2 initialization
		D->initInfo.TT2.memoryOK=TRUE;
		D->initInfo.TT2.done=FALSE;
		
		D->bank.TT2account.balance=0;
		D->bank.TT2account.minRequired=TT2minRequired;
		D->bank.TT2account.payCheck=TT2pPayCheck;
	
	// TT2p initialization
		D->initInfo.TT2p.targetNumber=1;
		D->initInfo.TT2p.memoryOK=TRUE;
		D->initInfo.TT2p.done=FALSE;
		D->bank.TT2paccount.balance=0;
		D->bank.TT2paccount.minRequired=TT2pminRequired;
		D->bank.TT2paccount.payCheck=TT2pPayCheck;

	}
	else{
		switch(mailPerson){
			case TT1mail:
				switch(aMessage){
					case TT1AuRevoirMsg:
					break;
					case TT1GoodbyeKissMsg:{
						newPres=TT1->newPres;
					}	
					break;
					case finiteGroupMsg:{
						if(!pLib->groupIsFinite){
							pLib->groupIsFinite=TRUE;
							groupOrder=TT1->Cayley->Total[GLrv];
							send(finiteGroupMsg);
						}
					}
					break;
			 		default: send(aMessage);
			 	}
			break;
			case TT1pmail:
				switch(aMessage){
					case TT1pAuRevoirMsg:
					break;
					case TT1pGoodbyeKissMsg:
						newPres=TT1p->newPres;
					break;
			 		default: send(aMessage);
			 	}
			break;
			case TT2mail:
				switch(aMessage){
					case incrementNewGenMsg: 
						++newGen;
						break;
					case TT2AuRevoirMsg:
					break;
					case TT2GoodbyeKissMsg:
						newPres=TT2->newPres;
					break;
			 		default: send(aMessage);
			 	}
			break;
			case TT2pmail:
				switch(aMessage){
					case TT2pAuRevoirMsg:
					break;
					case TT2pGoodbyeKissMsg:
						newPres=TT2p->newPres;
					break;
			 		default: send(aMessage);
			 	}
			break;
			case LibMail:
				switch(aMessage){		 			
					case equivalentPresentationFoundMsg:	
						presentationIsNew=FALSE;
					break;
	
					case LibAuRevoirMsg:
	#ifndef Frank_GenuineMac
	// this is a kluge since there is no error recovery
	// on the SUN.  Instead of recovering from low memory,
	// we just abort if the library check is taking too long.
	
						if(Lib->totalTime>LibTimeLimit)Lib->dirtyAbort=TRUE;
	#endif
					break;
					case LibGoodbyeKissMsg:
						hook=Lib->hook;
						foundLinkList=Lib->foundLinkList;
						noFirstCoord=Lib->noFirstCoord;
						noSecondCoord=Lib->noSecondCoord;
						noThirdCoord=Lib->noThirdCoord;
						genIndex=Lib->genIndex;
						relIndex=Lib->relIndex;
	#ifndef Frank_GenuineMac
						if(Lib->totalTime>LibTimeLimit && presentationIsNew){
							Lib->dirtyAbort=TRUE;
							gout<="Library search for duplicate presentation has timed out.\n";
						}
	#endif
					break;
			 		default: send(aMessage);
				}
			break;
			default: send(aMessage);	
		}
	}
}



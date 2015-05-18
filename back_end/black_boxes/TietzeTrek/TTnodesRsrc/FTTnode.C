#include "crossTrainer.h"
#include "FTTnode.h"
#include "Ftime.h"
#include "TietzeTypes.h"
#include "FpTree.h"
#include "FTT1node.h"
#include "FTT1pnode.h"
#include "FTT2node.h"
#include "FTT2pnode.h"

#include "debugTT.h"


// this class handles all the book keeping for the Tietze
// transformations.  The initialization routine sets initOK
// TRUE if there is enough time accumulated to run the
// TT node.  
	
FTTnode::FTTnode(FGLnode *aSupervisor,FpTree *apTree,FTTpres *currentPres,
		TietzeType aTietzeType)
	:FGLnode(aSupervisor){
	long minRequired,deficit,payCheck;
	itsType=aTietzeType;
	pTree=apTree;
	this->currentPres=currentPres;
	TThasFailed=FALSE;
	switch(itsType){
		case t1:
			payCheck=pTree->D.bank.TT1account.payCheck;
			runningBalance=pTree->D.bank.TT1account.balance+payCheck;
			pTree->D.bank.TT1account.balance=runningBalance;
			minRequired=pTree->D.bank.TT1account.minRequired;
			deficit=minRequired-runningBalance;
			initOK=deficit<=0;
		break;
		case t1p:
			payCheck=pTree->D.bank.TT1paccount.payCheck;
			runningBalance=pTree->D.bank.TT1paccount.balance+payCheck;
			pTree->D.bank.TT1paccount.balance=runningBalance;
			minRequired=pTree->D.bank.TT1paccount.minRequired;
			deficit=minRequired-runningBalance;
			initOK=deficit<=0;
		break;
		case t2:
			payCheck=pTree->D.bank.TT2account.payCheck;
			runningBalance=pTree->D.bank.TT2account.balance+payCheck;
			pTree->D.bank.TT2account.balance=runningBalance;
			minRequired=pTree->D.bank.TT2account.minRequired;
			deficit=minRequired-runningBalance;
			initOK=deficit<=0;
		break;
		case t2p:
			payCheck=pTree->D.bank.TT2paccount.payCheck;
			runningBalance=pTree->D.bank.TT2paccount.balance+payCheck;
			pTree->D.bank.TT2paccount.balance=runningBalance;
			minRequired=pTree->D.bank.TT2paccount.minRequired;
			deficit=minRequired-runningBalance;
			initOK=deficit<=0;
		break;
		default: nodeErr(8);
	}	
}

void FTTnode::wakeUp(void){
	startClock=clock();
}

void FTTnode::auRevoir(void){
	runningBalance=runningBalance+startClock-clock();
	TThasFailed= runningBalance<0;
	if(TThasFailed){
		// when control returns we go to the trap.  

		dirtyAbort=TRUE;
	}
	FGLnode::auRevoir();		// to support abort mechanism
}
	

void FTTnode::goodbyeKiss(void){	
// notice that TThasFailed is not determined here, but in
// auRevoir.  It is possible to run a negative balance and
// succeed if you start out with a positive balance on the
// last run call.
	
	runningBalance=runningBalance+startClock-clock();
	switch(itsType){
		case t1:
			if(TThasFailed){
				pTree->D.bank.TT1account.minRequired
					=2*pTree->D.bank.TT1account.minRequired;
			}
			pTree->D.bank.TT1account.balance=runningBalance;
		break;
		case t1p:
			if(TThasFailed){
				pTree->D.bank.TT1paccount.minRequired
					=2*pTree->D.bank.TT1paccount.minRequired;
			}
			pTree->D.bank.TT1paccount.balance=runningBalance;
		break;
		case t2:
			if(TThasFailed){
				pTree->D.bank.TT2account.minRequired
					=2*pTree->D.bank.TT2account.minRequired;
			}
			pTree->D.bank.TT2account.balance=runningBalance;
		break;
		case t2p:
			if(TThasFailed){
				pTree->D.bank.TT2paccount.minRequired
					=2*pTree->D.bank.TT2paccount.minRequired;
			}
			pTree->D.bank.TT2paccount.balance=runningBalance;
		break;
		default: nodeErr(8);
	}
}

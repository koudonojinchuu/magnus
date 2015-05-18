#include "crossTrainer.h"

#include "FpTree.h"
#include "FGLvar.h"
#include "GLfunctions.h"
#include "FTTpres.h"
#include "FGLink.h"
#include "FfreeWord.h"
#include "FGLink.h"
#include "FpTreeInitInfo.h"
#include <stdlib.h>
#include "FTT1node.h"
#include "FTT1pnode.h"
#include "FTT2node.h"
#include "FTT2pnode.h"
#include "FBlockManager.h"
#include "FcleanUpNode.h"
#include "FrandomRef.h"
#include "FuTree.h"
#include "Ftime.h"
#include "nodeMsg.h"


// standard methods for FpTree

void FpTree::Put(void){ 
	xPut((Ptr)&D);
}
void FpTree::cPut(void){
	xcPut((Ptr)&D);
}
void FpTree::Get(void){ 
	xGet((Ptr)&D);
}
void FpTree::cGet(void){
	xcGet((Ptr)&D);
}
// definitions for programmer defined methods
// programmer defined methods for name
 
FpTree::FpTree(FGLink *aGLink,FObject *aSupervisor, FpTreeInitInfo *info)
	:FGLvar(aGLink,info){
	super=aSupervisor;
	passSlot=reserveSlot();
	if(passSlot==noIDslotAvailable){
		varErr(62);
	}
	passNumber=0;
}

FpTree::~FpTree(void){
	// walk the tree and delete any extant TT1 data
	cleanUp= new FcleanUpNode(this);
	while(cleanUp->run()){
		FGLink::timeout=clock()+ONE_SECOND;  // short circuits event loop,
											 // we can't reenter a destructor
	}
	delete cleanUp;
}

void FpTree::respond(short aMessage){
	switch(aMessage){
		case cleanUpMsg:{
			bv=cleanUp->cleanUpRef;
			Get();
			FrandomRef *buffer=D.initInfo.TT1.buffer;
			if(buffer){
				delete buffer;
				D.initInfo.TT1.buffer=0;
			}
			FuTree *uTree=D.initInfo.TT1.uTree;
			if(uTree){
				FGLink *uLink=uTree->itsGLink;
				delete uTree;
				delete uLink;
				D.initInfo.TT1.uTree=0;
			}
			Put();
		}
		break;
		default: 
			gout<="error in FpTree\n";
			ExitToShell();
	}
}

void FpTree::V(long serialNumber){
// CAUTION: caller is responsible  for putting
// D into cv after call

	FGLvar::V();  // initialize superclass
	send(pVertexInitMsg);
	GLref NA=FGLink::NA;
	D.link.pdata=NA;
	D.link.next=NA;
	D.link.prev=NA;
	
	D.eParent=-e;
	D.eMax=0;
	D.serialNumber=serialNumber;
	Ptr DEREF=this&cv;
	if(passSlot!=0){
		varErr(62);
	}
	xPutID(DEREF,passSlot,passNumber+1);
	this-cv;
}

void FpTree::putPres(FTTpres *p){
// Writes p to a pTree data block.  
// Loads address in D.link.pdata.  Error if 
// this address is already defined
//
// CAUTION: caller is responsible loading
// D into a particular vertex after call

	if(!isNull(D.link.pdata)){
		varErr(53);
	}
// compute packetSize
	long packetSize=p->computePacketSize();
	
// allocate space and put reference in datum
	GLref ref;
	itsGLink->itsBlockManager->ReserveSpace(ref,packetSize);
	D.link.pdata=ref;
	
	Ptr DEREF=this&ref;
	p->storeData(DEREF);
	this-ref;
}	
	
FTTpres * FpTree::getPres(void){
// Reads from pTree data block to a new FTTpres object.  
// Address off D.link.pdata.  Error if 
// this address is not defined
//
// CAUTION: caller is responsible for the
// setting D from a particular vertex prior
// to call
	
	GLref ref=D.link.pdata;
	if(isNull(ref)){
		varErr(54);
	}
	Ptr DEREF=this&ref;
	FTTpres *p=new FTTpres(DEREF);
	this-ref;
	return(p);
}


void FpTree::allocateSpace(GLref& ref,long size){
	
// allocate space
	itsGLink->itsBlockManager->ReserveSpace(ref,size);
	if(isNull(ref)){
#ifdef Frank_GenuineMac
		Failure(memFullErr,0);
#else
		cout <= "Out of memory, goodbye.\n";
		ExitToShell();
#endif
	}
}


long FpTree::eNext(void){
// returns next unused edge
	long next;
	if(!D.eMax) next=1;
	else{
		GLploc p;
		xIndexToGLploc(p,GLr,D.eMax);
		if(p.modIndex<0){
			next=p.base-p.modIndex+1;
		}
		else if(p.modIndex>0){
			next=-p.base-p.modIndex;
		}
		else varErr(55);
	}
	D.eMax=next;
	if(D.eMax==D.eParent)return eNext();
	else return next; 
}



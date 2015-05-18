#include "crossTrainer.h"
#include "FrenumberPresNode.h"
#include "FTTpres.h"
#include "FfreeWord.h"
#include "sortingFunctions.h"
#include "nodeMsg.h"
#include "FfreeList.h"

#define gBlockSize 20

// The algorithm renumbers the local generators of the
// input TTpres to bring it into conformance with the
// RULE of FTTpres.h.  Renumbering affects the g,f, and
// globalSymbol lists.
//
// The renumbered output TTpres is a new TTpres.  Be sure
// to dispose the input TTpres after the node terminates
// if you don't want it anymore.
	
FrenumberPresNode::FrenumberPresNode(FGLnode *aSupervisor,FTTpres *aPres)
	:FGLnode(aSupervisor){

// The node assumes aPres is valid up to renumbering,
// and that the globalSymbol table is defined
// and up to date at input time.
//
// See FTTpresNode for info on transforming a list of relators
// into a FTTpres that is valid up to  renumbering.


	p=aPres;
	func=renumberFunc;

// make a working copy of p and a temporary
// global symbols list

	q= new FTTpres(p);
	normalizedLocal=1;
	temporarySymbols=new FList(p->globalSymbols);
}

FrenumberPresNode::~FrenumberPresNode(void){
	delete temporarySymbols;temporarySymbols=0;
}

Boolean FrenumberPresNode::renumberFunc(FGLnode *node){	
	
	return( ((FrenumberPresNode*)node)->renumber() );
}

Boolean FrenumberPresNode::renumber(void){
	if(normalizedLocal>p->g->numItems) return FALSE;
	long local=(long)p->g->NthItem(normalizedLocal);
	long global=(long)p->globalSymbols->NthItem(local);
	q->g->SetItem(&normalizedLocal,normalizedLocal);
	q->globalSymbols->SetItem(&global,normalizedLocal);
	long zero=0;
	temporarySymbols->SetItem(&zero,local);
	long num=q->f->numItems;
	for(long i=1;i<=num;++i){
		FfreeWord *pw=p->f->freeItem(i);
		pw->leftJustify();
		FfreeWord *qw=q->f->freeItem(i);
		qw->leftJustify();
		long *pvec=*pw->wordHandle;
		long *qvec=*qw->wordHandle;
		long len=pw->wordSize;
		for(long j=0;j<len;++j){
			if(pvec[j]==local)qvec[j]=normalizedLocal;
			++j;
		}
	}
	++normalizedLocal;
	return TRUE;
}


void FrenumberPresNode::auRevoir(void){
	FGLnode::auRevoir();
	send(renumberPresAuRevoirMsg);
}

void FrenumberPresNode::goodbyeKiss(void){
	// we fill out the rest of the global symbol table
	// with the globals that were never relabelled.
	// these have non-zero value in the temporary table
	
	long j=1;
	long num=p->globalSymbols->numItems;
	for(long i=p->g->numItems+1;i<=num;++i){
		Boolean symbolFound=FALSE;
		while(!symbolFound && j<=num){
			long global;
			global=(long)temporarySymbols->NthItem(j);
			if(global){
				symbolFound=TRUE;
				q->globalSymbols->SetItem(&global,i);
			}
			++j;
		}
		if(!symbolFound)nodeErr(37);
	}
	send(renumberPresGoodbyeKissMsg);
}



#include "crossTrainer.h"
#include "FTTdriverNode.h"
#include "FTTpres.h"
#include "FGLink.h"
#include "FpTree.h"
#include "FpTreeInitInfo.h"
#include <stdlib.h>
#include "Ftime.h"
#include "GLfunctions.h"
#include "FfreeList.h"
#include "FfreeWord.h"
#include "FTT2node.h"

void hardTest(void){

// set globals
	FTT2node::newGen=2;
	FfreeWord::freeWordSize=FfreeWord::datumSize();

// manufacture some input for an FTTdriverNode;
	FTTpres *aTTpres=new FTTpres((Boolean)FALSE);

	FList *g=new FList(2);
	g->Append((FObject*)1);
	g->Append((FObject*)2);
	aTTpres->g=g;

	FList *gCount=new FList(2);
	gCount->Append((FObject*)6);
	gCount->Append((FObject*)5);
	aTTpres->gCount=gCount;

	FfreeList *f=new FfreeList(2);

	FfreeWord *w=new FfreeWord(128);
	w->rightMultiply(2,1);
	w->rightMultiply(1,2);
	w->rightMultiply(2,1);
	w->rightMultiply(1,2);
	f->Append(w);
	
	w=new FfreeWord(128);
	w->rightMultiply(2,1);
	w->rightMultiply(1,1);
	w->rightMultiply(2,-2);
	w->rightMultiply(1,1);
	f->Append(w);
	
	
	aTTpres->f=f;

	FList *sym=new FList(2);
	for(long i=1;i<=2;++i){
		sym->Append((FObject*)i);
	}
	aTTpres->globalSymbols=sym;
	
	FGLink*aGLink=new FGLink;
	FpTreeInitInfo *info= new FpTreeInitInfo;
	FpTree* pTree= new FpTree(aGLink,info);
	delete info;info=0;
		
	// we set up the pTree base vertex here so that the
	// driver node base class FGLwalkNode initializes correctly
	
	pTree->j=0;
	pTree->I();
	pTree->e=1;
	pTree->V();
	pTree->bv=pTree->cv;	

	
	
	FTTdriverNode *test=new FTTdriverNode(0,aTTpres,aGLink,pTree);
	FGLink::timeout=clock()+ONE_SECOND; 
	while(test->run()){
		
		FGLink::timeout=clock()+ONE_SECOND; 
	}
	gout<="test aborted itself, goodbye\n";
	ExitToShell();
}

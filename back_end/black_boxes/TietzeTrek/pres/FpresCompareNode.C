#include "crossTrainer.h"
#include "FpresCompareNode.h"
#include "nodeMsg.h"
#include "FrTree.h"
#include "FrTreeInitInfo.h"
#include "FrTreeNode.h"
#include "FgTree.h"
#include "FgTreeInitInfo.h"
#include "FgTreeNode.h"
#include "FTTpres.h"
#include "FfreeList.h"
#include "FGLink.h"

FpresCompareNode::FpresCompareNode(FGLnode *aSupervisor,
	FTTpres *oldTTpres,FTTpres *newTTpres)
	:FGLnode(aSupervisor){
	this->oldTTpres=oldTTpres;
	this->newTTpres=newTTpres;
	rNode=ZERO;
	gNode=ZERO;
	
	itsGLink= new FGLink;
// rTree initialization

	FGLvarInitInfo *info= new FrTreeInitInfo(oldTTpres->f->numItems);
	rTree= new FrTree(itsGLink,oldTTpres->f,(FrTreeInitInfo*)info);

// rTree clone initialization

	rClone=new FrTree(itsGLink,oldTTpres->f,(FrTreeInitInfo*)info);
	delete info;info=0;
	Str255 str;
	sprintf((char*)str,"rClone");
	CtoPstr((char*)str);
	BlockMove((Ptr)str,(Ptr)rClone->itsName,7);

// gTree initialization

	info= new FgTreeInitInfo(oldTTpres->g->numItems);
	gTree= new FgTree(itsGLink,oldTTpres->gCount,(FgTreeInitInfo*)info);
	delete info;info=0;
	
	mailPerson=rTreeMail;		
	func=presCompareFunc;
}
FpresCompareNode::~FpresCompareNode(void){
	delete rNode;rNode=0;
	delete gNode;gNode=0;
	delete rTree;rTree=0;
	delete rClone;rClone=0;
	delete gTree;gTree=0;
	delete itsGLink;itsGLink=0;
}

Boolean FpresCompareNode::presCompareFunc(FGLnode *node){
	FpresCompareNode *n=(FpresCompareNode*)node;	
	switch(n->mailPerson){
		case rTreeMail: return n->rTreeGo();
		case gTreeMail: return n->gTreeGo();
		default: n->nodeErr(6);
	}
}

Boolean FpresCompareNode::rTreeGo(void){
	if(!rNode){
		rTree->j=0;
		rTree->I();
		rTree->e=1;
		rTree->V();
		rTree->bv=rTree->cv;
		rNode=new FrTreeNode(this,rTree);
	}
	if(!rNode->run()){
		delete rNode;rNode=0;
		mailPerson=gTreeMail;
	}
	return(TRUE);

}

Boolean FpresCompareNode::gTreeGo(void){
	if(!gNode){
		gTree->j=0;
		gTree->I();
		gTree->e=1;
		gTree->V();
		gTree->bv=gTree->cv;
		rClone->j=0;
		rClone->I();
		rTree->j=0;
		rTree->jLoc();
		rTree->e=1;
		rTree->iS();
		gTree->putRef(gTree->bv,rTree->bv);
		gNode=new FgTreeNode(this,gTree,oldTTpres,newTTpres,
			rTree,rClone);
	}
	if(!gNode->run()){
		delete gNode;gNode=0;
		return FALSE;
	}
	return TRUE;
}

void FpresCompareNode::auRevoir(void){
	FGLnode::auRevoir();
	send(CompareAuRevoirMsg);
}

void FpresCompareNode::goodbyeKiss(void){
	send(CompareGoodbyeKissMsg);
}

void FpresCompareNode::respond(short aMessage){
	switch(aMessage){
		case equivalentPresentationFoundMsg:
			send(equivalentPresentationFoundMsg);
		break;
		case rTreeAuRevoirMsg:
		break;
		case rTreeGoodbyeKissMsg:
		break;
		case gTreeAuRevoirMsg:
		break;
		case gTreeGoodbyeKissMsg:
		break;
		default:send(aMessage);
	}
}

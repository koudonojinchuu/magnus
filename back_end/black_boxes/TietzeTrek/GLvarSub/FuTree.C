#include "crossTrainer.h"

#include "FGLvar.h"
#include "GLfunctions.h"
#include "FuTree.h"
#include "FuTreeInitInfo.h"
#include "FGLink.h"
#include "FfreeWord.h"
#include "TietzeTypes.h"

FuTree::FuTree(FGLink *aGLink,FuTreeInitInfo *info)
	:FGLvar(aGLink,info){

	// set instance variables 	
	externalOffset=backYardOffset[GLrv];
	breadOffset=backYardOffset[GLrv]+sizeof(GLref);
}


long FuTree::eNext(long eCurrent, long eParent){
// returns next unused edge
	long next;
	if(!eCurrent) next=1;
	else{
		GLploc p;
		xIndexToGLploc(p,GLr,eCurrent);
		if(p.modIndex<0){
			next=p.base-p.modIndex+1;
		}
		else if(p.modIndex>0){
			next=-p.base-p.modIndex;
		}
		else varErr(55);
	}
	if(next==eParent)return eNext(next,eParent); 
	else return next;
}

void FuTree::putBreadCrumb(const GLref& where,breadCrumb& b){
	GLref ref=where;
	Ptr DEREF=this&where;
	BlockMove((Ptr)&b,&DEREF[breadOffset],sizeof(breadCrumb));
	this-ref;
}

void FuTree::getBreadCrumb(const GLref& where,breadCrumb& b){
	GLref ref=where;
	Ptr DEREF=this&where;
	BlockMove(&DEREF[breadOffset],(Ptr)&b,sizeof(breadCrumb));
	this-ref;
}

void FuTree::getExternalRef(const GLref& where,GLref& externalRef){
	GLref ref=where;
	Ptr DEREF=this&where;
	BlockMove(&DEREF[externalOffset],(Ptr)&externalRef,sizeof(GLref));
	this-ref;
}

void FuTree::putExternalRef(const GLref& where,GLref& externalRef){
	GLref ref=where;
	Ptr DEREF=this&where;
	BlockMove((Ptr)&externalRef,&DEREF[externalOffset],sizeof(GLref));
	this-ref;
}

FfreeWord* FuTree::trace(const GLref& startRef){
	extern long freeWordBlockSize;
	FfreeWord *w=new FfreeWord(freeWordBlockSize);
	breadCrumb b;
	getBreadCrumb(startRef,b);
	while(!isNull(b.ref)){
		w->leftMultiply(b.e,1); //word spelled backwards
		getBreadCrumb(b.ref,b);
	}
	w->leftJustify();	// this aligns the word with the beginning of the block
	return w;
}

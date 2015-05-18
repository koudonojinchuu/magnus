#ifndef Frank_CwordsToGLvarNode
#define Frank_CwordsToGLvarNode


#include "FGLnode.h"

class FfreeList;
class FTTpres;


class FwordsToGLvarNode:public FGLnode{
public:
	FfreeList *words;
	GLref itsRoot;
	long lettersNo,index;
	long flagOffset;
	FGLvar *v;
	
	FwordsToGLvarNode(FGLnode* aSupervisor, FGLvar *aGLvar,
		long lettersNo,FfreeList *words,GLref& root,long scratchOffset);
	static Boolean wordsToGLvarFunc(FGLnode *n);
	virtual Boolean wordsToGLvar(void);
	virtual void putFlag(GLref& where,Boolean aFlag);
	virtual void goodbyeKiss(void);
	virtual void auRevoir(void);
private:
	enum wordsToGLvarEntry{GLstart,GLrun};
	wordsToGLvarEntry entryPoint;
};

#endif

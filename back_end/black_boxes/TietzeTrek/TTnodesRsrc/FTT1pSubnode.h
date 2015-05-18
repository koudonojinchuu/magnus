#ifndef Frank_FTT1pSubnode
#define Frank_FTT1pSubnode

#include "FGLnode.h"
#include "Ftime.h"
#include "FGLink.h"
#include "FCayley.h"

class FpTree;
class FfoldNode;
class FreducedFoldNode;
class FgrowNode;
class FCayleyCloneNode;
class FTTpres;
class FwordsToGLvarNode;
class FTTpresNode;
class FfreeWord;
class FfreeList;
class FrenumberPresNode;

class FTT1pSubnode:public FGLnode{
	public:
	clock_t startClock;
	long distinguishedRel,timeRemaining;
	FTT1pSubnode(FGLnode *aSupervisor,FTTpres *currentPres,
		long distinguishedRel,long timeRemaining);
	~FTT1pSubnode(void);
	static Boolean TT1pSubfunc(FGLnode *node);
	nodeFunc reentryFunc;
	Boolean wordsToGLvarGo(void);
	Boolean foldGo(void);
	Boolean growGo(void);
	Boolean CayleyCloneGo(void);
	Boolean reducedFoldGo(void);
	Boolean presGo(void);
	Boolean renumberGo(void);
	
	virtual void wakeUp(void);	
	virtual void auRevoir(void);
	virtual void goodbyeKiss(void);
	FTTpres *currentPres;	// owned by supervisor
	FTTpres *newPres;			// created on the fly, ownership
								// passes to supervisor
	Boolean relatorIsConsequence;
	Boolean newPresDispose;
	long lettersNo;
	FGLink *itsGLink;
	FCayley *Cayley;
	FGLvar *CayleyClone;
	long wordsNo,verticesNo,edgesNo;
	
	FwordsToGLvarNode *wordsToGLvar;
	FfoldNode *fold;
	FreducedFoldNode *reducedFold;
	FgrowNode *grow;
	FCayleyCloneNode *CcloneGen;
	FTTpresNode *presGen;
	FrenumberPresNode *renumber;

	GLref foldEntryLocation;

	virtual void respond(short aMessage);
	virtual void consequenceFound(void);
	void DisplayCounters(void);
	void transferCounters(void);  
private:
	enum{ 
		foldMail,				
		growMail,
		CayleyCloneMail,
		reducedFoldMail,
		wordsToGLvarMail,
		presMail,
		renumberMail
	};	
  
};

#endif

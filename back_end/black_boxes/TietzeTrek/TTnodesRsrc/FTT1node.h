#ifndef Frank_FTT1node
#define Frank_FTT1node


#include "FTTnode.h"
#include "TietzeTypes.h"
#include "FpTree.h"
#include "Ftime.h"
#include "FuTree.h"
#include "FCayley.h"

class FfoldNode;
class FreducedFoldNode;
class FCayleyCloneNode;
class FuNode;
class FwordsToGLvarNode;
class FgrowNode;
class FTTpresNode;

class FTTpres;
class FuTree;
class FpTree;
class FfreeWord;
class FfreeList;
class FTT1node;
class FrandomRef;

#define UTREE_FRACTION .8
#define CONSEQUENCE_BLOCKSIZE 50

class FTT1node:public FTTnode{
	public:
	
	clock_t timer,CayleyTime,uTime;

	FTT1node(FGLnode *aSupervisor,FpTree *apTree,FTTpres *currentPres);	
	~FTT1node(void);
	static Boolean TT1func(FGLnode *node);	

	FTTpresNode *presGen;
	FwordsToGLvarNode *wordsToGLvar;
	FfoldNode *fold;
	FgrowNode *grow;
	FCayleyCloneNode *CcloneGen;
	FreducedFoldNode *reducedFold;
	FuNode *uNode;	

	virtual Boolean presGo(void);
	virtual Boolean wordsToGLvarGo(void);
	virtual Boolean foldGo(void);
	virtual Boolean growGo(void);
	virtual Boolean CcloneGo(void);
	virtual Boolean reducedFoldGo(void);
	virtual Boolean uGo(void);
	
	virtual void auRevoir(void);
	virtual void goodbyeKiss(void);
	void bufferSearch(void);
	
	long lettersNo;
	FTTpres *newPres;			// created on the fly, ownership
								// passes to supervisor
								
	FGLink *uLink;				// ownership passes to pTree
	FuTree *uTree;				// ownership passes to pTree
	GLref CayleyPacketRef;
	Ptr DEREFCayleyPacket;      // for quick access to real link
								// of Cayley vertex during uTree construction
	
	FGLink *CayleyLink;
	FCayley *Cayley;
	FCayley *CayleyClone;
	
	FrandomRef *buffer;
	
	Boolean newPresDispose;
	long wordsNo,verticesNo,edgesNo;
	
	GLref foldEntryLocation;
	
	virtual void respond(short aMessage);
	void DisplayCounters(void);
	void transferCounters(void);    
private:
	enum{ 
		uMail,
		foldMail,				
		growMail,
		CayleyCloneMail,
		reducedFoldMail,
		wordsToGLvarMail,
		presMail
	};	
};

#endif


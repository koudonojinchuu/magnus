#ifndef Frank_FTTdriverNode
#define Frank_FTTdriverNode


#include "FGLwalkNode.h"
#include "FpTree.h"

class FpTree;
class FpLib;
class FTTpres;
class FpLibNode;
class FTT1node;
class FTT1pnode;
class FTT2node;
class FTT2pnode;
class FfreeList;

class FTTdriverNode: public FGLwalkNode{
public:
	FTTpres *itsInputTTpres;		// owned by supervisor
	long newGen;
	GLref root;
	pTreeRealType Dsave;
	short currentType;

	long displayParentSerial;
	long displaySerial;
	FTTpres *displayPres;
	short displayType;

	FTTdriverNode(FGLnode *aSupervisor,FTTpres *aTTpres,
		FGLink *aGLink,FpTree *apTree);
	~FTTdriverNode(void);
	static Boolean driverFunc(FGLnode *n);
	virtual Boolean walk(void);
	virtual Boolean vertex(void);
	Boolean TT1Go(void);
	void releaseTT1memory(void);
	Boolean TT1pGo(void);
	Boolean TT2Go(void);
	Boolean TT2pGo(void);
	Boolean LibGo(void);
	virtual void actOnSuccess(short aTTtype);
	virtual void actOnFailure(short aTTtype);
	virtual long nextTT(short aTTtype);
	void classify(void);
	Boolean doneGo(void);
	virtual Boolean processEdge(void);
	virtual void virginVertex(const GLref& virgin);
	virtual Boolean finish(void);
	virtual void auRevoir(void);
	virtual void goodbyeKiss(void);
	
	FGLink *itsGLink;
	FpTree *pTree;
	FpLib *pLib;
	FTTpres *currentPres;
	FTTpres *newPres;
	FTT1node *TT1;
	FTT1pnode *TT1p;
	FTT2node *TT2;
	FTT2pnode *TT2p;
	FpLibNode *Lib;
	long factorRank,groupOrder;
	Boolean foundLinkList,noFirstCoord,noSecondCoord,noThirdCoord;
	Boolean presentationIsNew;
	long relIndex,genIndex;
	GLref hook;
	virtual void respond(short aMessage);

	long TT1RepeatNum,TT1pRepeatNum,TT2RepeatNum,TT2pRepeatNum;

// calibration

	clock_t TT1minRequired;		
	clock_t TT1pminRequired;
	clock_t TT2minRequired;
	clock_t TT2pminRequired;

	clock_t TT1PayCheck;
	clock_t TT1pPayCheck;
	clock_t TT2PayCheck;
	clock_t TT2pPayCheck;

	long TT1RepeatMax;
	long TT1pRepeatMax;
	long TT2RepeatMax;
	long TT2pRepeatMax;
		
	long TT2len;   // max reduced length of word chosen by TT2

	clock_t LibTimeLimit;

private:
	enum{walkMail,TT1mail,TT1pmail,TT2mail,TT2pmail,LibMail,doneMail};
};

#endif

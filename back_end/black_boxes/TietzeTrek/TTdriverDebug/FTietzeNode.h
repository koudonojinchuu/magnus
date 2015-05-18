#ifndef Frank_FTietzeNode
#define Frank_FTietzeNode


#include "FGLnode.h"
#include "FGLink.h"
#include "FGLink.h"
#include "FfreeWord.h"
#include "text_functions.h"
#include "TietzeTypes.h"

#ifdef Frank_GenuineMac
CLASS CTietzeDoc;
#else
class FTietzeDoc;
#endif

class FstrToWordNode;
class FTTpres;
class FTTpresNode;
class FTTdriverNode;
class FfreeList;

#ifdef Frank_GenuineMac
CLASS CStringArray;
#else
class FStringArray;
#endif

class FGLink;
class FpTree;

class FTietzeNode : public FGLnode{
private:
	enum{inputMail,strToWordMail,presMail,driverMail};

public:
#ifdef Frank_GenuineMac
	FTietzeNode(CTietzeDoc *aTietzeDoc);
	CTietzeDoc *itsDoc;		// its supervising document
	CStringArray *itsStrings;  	// owned by document
#else
	FTietzeNode(FTietzeDoc *aTietzeDoc);
	FTietzeDoc *itsDoc;
	FStringArray *itsStrings;  	// owned by document
#endif
	~FTietzeNode(void);
	static Boolean TietzeFunc(FGLnode *n);		
	Boolean inputGo(void);		
	Boolean strToWordGo(void);
	Boolean presGo(void);
	Boolean driverGo(void);
	
	FfreeList *TTrels;	
	FTTpres *itsTTpres;

	FstrToWordNode *itsStrToWordNode;
	FTTpresNode *itsTTpresNode;
	FTTdriverNode *itsTTdriverNode;
	long itsElementSize;
	long lettersNo,relationsNo;
	long factorRank,groupOrder;
	long **letterToNumber;
	unsigned char **numberToLetter;
	FwordToStringArrayInfo *itsWordToStringArrayInfo;
	
	FGLink *itsGLink;
	FpTree *pTree;
	
	long displayParentSerial;
	long displaySerial;
	FTTpres *displayPres;
	short displayType;

	virtual	Boolean	setTietzeMembers(void); 
	virtual	Boolean	doInputEntry(void);
	
	void send(short aMessage);
	virtual void respond(short aMessage);
	virtual	void emptyPresentationDisplay(void);
	virtual	void relationsNoDisplay(void); 
	virtual void lettersNoDisplay(void);

};

#endif


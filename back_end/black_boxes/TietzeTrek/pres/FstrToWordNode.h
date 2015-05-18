#ifndef Frank_FstrToWordNode
#define Frank_FstrToWordNode

class FGLnode;

#ifdef Frank_GenuineMac
CLASS CStringArray;
#else
class FStringArray;
#endif

#include "FfreeWord.h"
#include "text_functions.h"

class FstrToWordNode:public FGLnode{
public:
#ifdef Frank_GenuineMac
	CStringArray *strs;		// owned by supervisor
#else 
	FStringArray *strs;
#endif
	FfreeWord *f;			// owned by supervisor
	long lettersNo,beginIndex,endIndex;
	long **letterToNumber;	// owned by supervisor
	long strIndex,elementSize;
	Boolean addPreviousWord;
	
FstrToWordNode(FGLnode *aSupervisor,
#ifdef Frank_GenuineMac
CStringArray *strs, 
#else
FStringArray *strs,
#endif
			long lettersNo,long **letterToNumber,long beginIndex,
			long endIndex);
	virtual void processLine(Str255 s,long firstLetter);
	virtual void getLineAttributes(lineAttributesInfo *info);
	static Boolean castToStrToWord(FGLnode *n);
	virtual Boolean strToWord(void);
	virtual long collectExp(StringPtr s,long len,long& lastScannedi);
	virtual void goodbyeKiss(void);
	virtual void auRevoir(void);
private:
	enum strToWordEntry{GLstart,GLrun};
	strToWordEntry entryPoint;
};

#endif

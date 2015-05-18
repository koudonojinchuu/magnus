#ifndef Frank_FTTpresNode
#define Frank_FTTpresNode


#include "FGLnode.h"
class FfreeWord;
class FTTpres;

class FTTpresNode:public FGLnode{
	public:
	FTTpres *p; 		// owned by supervisor
	long fIndex,entryIndex,noEntries;
	long **fHandle;		// owned by a CfreeWord
	FTTpresNode(FGLnode *aSupervisor,FTTpres *aTTpres);	
	static Boolean TTpresFunc(FGLnode *node);
	virtual Boolean TTpresentation(void);	
	virtual void auRevoir(void);
	virtual void goodbyeKiss(void);
private:
	enum presEntry{GLstart,GLrun};
	enum presModifier{							
		addOccurrenceEntry,
		gsortEntry,
		fsortEntry
	};
	presModifier entryModifier;
	presEntry entryPoint;

};

#endif


#ifndef Frank_Debuggingfcns
#define Frank_Debuggingfcns

#include <stdio.h>
#include "GLref.h"
#include "FGLvarTypes.h"

class FGLvar;
class FTTpres;
class FfreeWord;
class FfoldStepper;
class FfreeList;

ostream& repGlobalFreeWord(ostream& s,FTTpres *p,FfreeWord *f);
ostream& repGlobalFreeList(ostream& s,FTTpres *p);
ostream& repGlobalSymbols(ostream& s,FTTpres *p);
ostream& xrepGlobalSymbols(ostream& s,FTTpres *p);
ostream& repGlobalTTpres(ostream& s,FTTpres *p);
ostream& repAnnotatedTTpres(ostream& s,FTTpres *p,
			FGLvar *v,GLref& loadRel);
ostream& repTime(ostream& s,double value);
ostream& repLetter(ostream& s,long e);
ostream& repTTtype(ostream& s,short currentType);

// random stuff
FTTpres* randomRelation(FfreeList* f,long noLetters);
#endif


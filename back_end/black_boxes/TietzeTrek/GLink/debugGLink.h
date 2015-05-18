#ifndef Frank_DebugGLink
#define Frank_DebugGLink

#include "FGLvarTypes.h"
class FGLvarInterface;
class FGLvarLow;
class FGLvar;
class FfoldStepper;

ostream& repReg(ostream& s,FGLvarInterface *v);
ostream& repE(ostream& s,FGLvarLow *v);
ostream& repbE(ostream& s,FGLvarLow *v);
ostream& repcE(ostream& s,FGLvarLow *v);
ostream& xrepE(ostream& s,FGLvarLow *v,GLref& ref);
ostream& repBoolean(ostream& s,Boolean val);
ostream& repFlagToType(ostream& s,short flag);
ostream& repLink(ostream& s,short type,GLref ref,FGLvar *v);
Boolean repScanEdge(scanDoubleInfo *info);
void dumpGraph(ostream& s,FGLvar *v);
Boolean repPackets(ostream& s,FGLvar *v,GLref& startRef);
Boolean repEdgeFromStepper(ostream& s,FGLvar *v,FfoldStepper *f);
Boolean checkPackets(ostream& s,FGLvar *v,GLref& startRef);
Boolean checkEdgeFromStepper(ostream&s,FGLvar *v,FfoldStepper *f);

#endif


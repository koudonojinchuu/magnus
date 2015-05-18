/*******************

Written by Frank Rimlinger, August 1992

*******************/
// Create a subclass for each GLvar

#ifndef Frank_FGLvar
#define Frank_FGLvar

#include "FGLvarHigh.h"


class FGLvar: public FGLvarHigh{

// USER METHODS.  See also Get(),Put(),GetI() and PutI() in GLsubclasses file
public:
			FGLvar(FGLink *aGLink, FGLvarInitInfo *info);

// Moving around in the graph
public:
			virtual Boolean jN(void);		
			virtual Boolean jP(void);		
			virtual Boolean rS(void);		
			virtual Boolean rcS(void);		
			virtual Boolean iS(void);		
			virtual Boolean icS(void);	
			virtual Boolean rN(void);	
			virtual Boolean rcN(void);		
			virtual Boolean rP(void);		
			virtual Boolean rcP(void);	
			virtual Boolean iN(void);	
			virtual Boolean icN(void);		
			virtual Boolean iP(void);		
			virtual Boolean icP(void);
			virtual Boolean rFe(void);
			virtual Boolean rNe(void);
			virtual Boolean rPe(void);
			virtual Boolean rLe(void);
			virtual Boolean rcFe(void);
			virtual Boolean rcNe(void);
			virtual Boolean rcPe(void);
			virtual Boolean rcLe(void);
			virtual Boolean iFe(void);
			virtual Boolean iNe(void);
			virtual Boolean iPe(void);
			virtual Boolean iLe(void);
			virtual Boolean icFe(void);
			virtual Boolean icNe(void);
			virtual Boolean icPe(void);
			virtual Boolean icLe(void);		

// Queries
public:
			virtual Boolean Qempty(void);
			virtual Boolean cQempty(void);
			virtual Boolean Qideal(void);		
			virtual Boolean cQideal(void);	
			virtual Boolean jQideal(void);	
			virtual Boolean QrealTypeEdge(void);	
			virtual long rQnum(void);	
			virtual long rcQnum(void);		
			virtual long iQnum(void);			
			virtual long icQnum(void);
			Boolean Qunlinked(void);
			Boolean cQunlinked(void);
			
public:
// miscellaneous routines for vertices
			virtual void Clear(void);
			virtual void cClear(void);
// Ideal vertices
			virtual Boolean I(void);
			virtual Boolean Iunlinked(void);
			virtual Boolean cI(void);	
			virtual Boolean cIunlinked(void);			
			virtual void PutI(void);		
			virtual void cPutI(void);		
			virtual void jPutI(void);	
			virtual void GetI(void);	
			virtual void cGetI(void);	
			virtual void jGetI(void);		
			virtual long jVal(void);		
			virtual long jcVal(void);	
			virtual Boolean jLoc(void);	
			virtual Boolean jcLoc(void);
// Real vertices	
			virtual void V(void);		
			virtual void Put(void);		
			virtual void cPut(void);		
			virtual void Get(void);			
			virtual void cGet(void);	
		
// Modifying the structure of the graph
public:
			virtual void Lnk(void);			
			virtual Boolean rUnlnk(void);		
			virtual Boolean rcUnlnk(void);	
			virtual Boolean iUnlnk(void);	
			virtual Boolean icUnlnk(void);	
			virtual void Identify(void);
			virtual Boolean jRemove(void);
			virtual void Flush(void);
			virtual void rFlush(void);
			virtual void iFlush(void);
			virtual Boolean Pack(void);
			
// the following routine is a highly specialized routine used by
// the vertex folder

			virtual void Append(GLref& lastPacket,
								const GLref& midpoint, const GLref& vRef);
			
};

#endif


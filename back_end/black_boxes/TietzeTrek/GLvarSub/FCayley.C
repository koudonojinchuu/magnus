#include "crossTrainer.h"
#include "FCayley.h"
#include "FGLvar.h"
#include "GLfunctions.h"
#include "FGLink.h"
#include "FCayleyInitInfo.h"
#include "nodeMsg.h"

// standard methods for Cayley


FCayley::FCayley(FGLnode *aSupervisor,FGLink *aGLink,FCayleyInitInfo *info)
	: FGLvar(aGLink,info){

	// set instance variables 
	
	itsSupervisor=aSupervisor;
	itsRoot=FGLink::NA;
	flagOffset=backYardOffset[GLrv]+sizeof(GLref);
}


// The "logical root" itsRoot is the real vertex connected to the 0
// ideal vertex.  We override Identify() to detect real vertices
// not equal to the logical root which are about to be identified
// to the real root.  If a real vertex vRef is to be identified to
// itsRoot, then unlink each ideal vertex connected to vRef 
// to speed up folding.
// If vRef is the base vertex, then we update itsRoot to vRef
//
// The doNotDuplicate flag of the resulting vertex is set FALSE

void FCayley::Identify(void){
	if(!SameAddr(itsRoot,bv) || !SameAddr(itsRoot,cv)){
		if(SameAddr(itsRoot,bv)){
			ReportToSuper(cv);
		}
		else if(SameAddr(itsRoot,cv)){
			ReportToSuper(bv);
			itsRoot=bv;
		}
	}
	putFlag(bv,FALSE);
	FGLvar::Identify();
}

// same deal with Append, a high speed routine used by the vertex folder

void FCayley::Append(GLref& lastPacket,const GLref& midpoint, const GLref& vRef){
	if(!SameAddr(itsRoot,midpoint) || !SameAddr(itsRoot,vRef)){
		if(SameAddr(itsRoot,midpoint)){
			ReportToSuper(vRef);
		}
		else if(SameAddr(itsRoot,vRef)){
			ReportToSuper(midpoint);
			itsRoot=midpoint;
		}
	}
	putFlag(midpoint,FALSE);
	FGLvar::Append(lastPacket,midpoint,vRef);
}

void FCayley::ReportToSuper(const GLref& vRef){			
	long eSave=e;
	e=0;
	GLref bvSave=bv;
	bv=vRef;
	GLref cvSave=cv;
	while(iN()){									// get next ideal edge
		iUnlnk();									// unlinks edge, cv is now
													// at the ideal vertex
		itsSupervisor->respond(identifiedWordMsg);		// notify supervisor
		e=0;										// restart edge search							
	}
	e=eSave;
	bv=bvSave;
	cv=cvSave;
}

void FCayley::putFlag(const GLref& where,Boolean aFlag){
	Ptr DEREF=this&where;
	BlockMove((Ptr)&aFlag,&DEREF[flagOffset],sizeof(Boolean));
	this-where;
}


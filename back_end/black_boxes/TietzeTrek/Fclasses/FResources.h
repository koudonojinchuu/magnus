#ifndef Frank_FResources
#define Frank_FResources

// This class is intended to give minimal support for Macintosh resources.
// At present, only STR# resources are supported.

// the resources must be converted from the Mac resource using Saderez
// and FTP'd to Resources.  On the SUN these files
// should be regarded as read only.

// the main routine should initialize a CResources object.  This causes
// the class constants to be defined and reads in the resources from
// Resources.  The program can then access the
// resources by passing the appropriate class constant (=Resource ID)
// to GetIndString().

// CAUTION: propogating changes from the Mac .rsrc file involves two
// steps.  First, the resource must be fed through Saderez.  Edit this
// file by putting SIZE='size' in the very first line.  Then FTP to the
// SUN.  Second, take a look at the CResourses object to make sure the
// class constants are up to date, that the noStrs #define is updated.


#define noSTRs 15 // UPDATE when adding new STR resources to this object 
	
#include "FObject.h"
#include <stdio.h>
class  FList;

class FResources:public FObject{
public:

// use these class constants to reference STR resources
	static short xSTRoffset;
	static short xSTRcommon;
	static short xSTRmemWarn;
	static short xSTRtaskNames;
	static short xSTR_TCLfailMsgs;
	static short xSTRunused;
	static short xSTRdlgValidation;
	static short xSTRxErr;
	static short xSTRvarErr;
	static short xSTRnodeErr;
	static short xSTRGLErr;
	static short xSTRbtnTitles;
	static short xSTRdebug;
	static short xSTRdialog;
	static short xSTRerrorAlerts;
	static short xSTRstatic;
	
	FList *STR;
	FResources(void);		// resource aquisition is initialization
private:
#ifndef Frank_GenuineMac
	void loadResource(FILE *stream,long i,long resIndex,short size);
#endif
};
#endif


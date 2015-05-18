#include "crossTrainer.h"
#include "FResources.h"
#include "GLfunctions.h"
#include "rsrcInterface.h"

/********************************************************/
// STREAM OUTPUT
//
// There is some special gri-gri to make stream output
// on the Mac look syntactically like ios::ostream
// We can't use the iostream library on the Mac because
// it blots out the real Mac interface.  
//
// There is nothing wrong with using simple tools, 
// but that doesn't mean we want to live in a cave.
/********************************************************/

#ifdef Frank_GenuineMac
ostream cout;
#else
#ifdef THINK_CPLUS
// we need to configure the console below if running on the Mac
#include <console.h>
#endif

// we create an output file for the SUN version
//@rn ofstream fout("Tietze.out");
ofstream fout("/dev/null");
#endif

ostream &memout=cout;		//  defaults for debugging streams
ostream &gout=cout;			//	you can reroute these if desired
ostream &tout=cout;			//  but you'll have to write some support code.	


/********************************************************/
//  fonts

#ifdef Frank_GenuineMac
	short textFont=monaco;
	short textFontSize=9;
#endif
/********************************************************/

/********************************************************/
//  debugging globals
/********************************************************/

Boolean xDEBUG=TRUE;

// DEBUGGING RESOURCES
// The debugging resources are available for the SUN version
// as derezed Macintosh STR# resources.  

#include "FResources.h"
FResources *debuggingResources=0;
short FResources::xSTRoffset=128; // ID's below 128 are reserved for Mac OS

// use these class constants to reference STR resources
short FResources::xSTRcommon=128;
short FResources::xSTRmemWarn=129;
short FResources::xSTRtaskNames=130;
short FResources::xSTR_TCLfailMsgs=131;
short FResources::xSTRunused=132;
short FResources::xSTRdlgValidation=133;
short FResources::xSTRxErr=134;
short FResources::xSTRvarErr=135;
short FResources::xSTRnodeErr=136;
short FResources::xSTRGLErr=137;
short FResources::xSTRbtnTitles=138;
short FResources::xSTRdebug=139;
short FResources::xSTRdialog=140;
short FResources::xSTRerrorAlerts=141;
short FResources::xSTRstatic=142;

// END DEBUGGING RESOURCES

/********************************************************/
// display globals
/********************************************************/
long presCount=0;

/********************************************************/
// global definitions for memory debugging
// This stuff is mainly for tracking down memory leaks
/********************************************************/

#ifdef Frank_MemoryDebugging
FGLink *pTreeLink=0;
FGLink *pLibLink=0;
long pTreeTotal=0,libTreeTotal=0;initialTotal=0;
long totalMemory,noObjects=0,objectMemory;
FList *handleRoster=ZERO,*ptrRoster=ZERO;
Boolean rosterOK=FALSE;
Boolean toggleSkip;

#ifdef Frank_GenuineMac
TEHandle debuggingTextHandle;
#endif

#endif


/********************************************************/
// sizes for FfreeWord
/********************************************************/

long freeWordBlockSize=128;
#include "FfreeWord.h"
long FfreeWord::freeWordSize;  
	// CAUTION: this must be initialized at startup,
	// by calling datumSize().
	// There is no class method to do this because it
	// only needs to be done once.



/********************************************************/
// global definitions for GLink classes
/********************************************************/

#include "FGLink.h"
#include "FCayley.h"
#include "FpTree.h"
clock_t FGLink::timeout;
GLref FGLink::NA,FGLink::DEADVERTEX;
long FGLink::triggerWarning=50000;
long FGLink::MaxBlocks=100;



/*******************************************************/
// FpTree static data
/*******************************************************/

pTreeRealType FpTree::D;


/********************************************************/
// globals specific to GenuineMac
/********************************************************/
#ifdef Frank_GenuineMac

#include "CTietzeTrekApp.h"

#include "CTietzeTrekDoc.h"
#include "CTietzeDoc.h"

CTietzeTrekApp *trekApp;
Boolean CTietzeTrekApp::UseExceptionHandler=TRUE;
CTietzeTrekDoc *trekDoc;

#else

/********************************************************/
// globals specific to SUN
/********************************************************/

#include "FTietzeDoc.h"
#include "FTietzeNode.h"
#include <stdio.h>
#include "FTTdriverNode.h"

OSErr memFullErr=1; 
Str255 macParam0,macParam1,macParam2,macParam3;
long gSleepTime;
short MemErr=0;

#endif


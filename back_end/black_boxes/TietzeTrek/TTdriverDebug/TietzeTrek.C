/******************************************************************************

  TietzeTrek
 
  Frank Rimlinger, May 1993  
  Revised July 1993
  Version 2.0 Febuary 1994
******************************************************************************/

#include "crossTrainer.h"	// makes SUN and Macintosh buddies.
							// To each, according to their liking,
							// From each, according to their skill.

#include "FResources.h"		
extern FResources *debuggingResources;	// See TietzeGlobals.C for 
										// descriptionof globals
#ifdef Frank_GenuineMac
#include "CTietzeTrekApp.h"	// THINK_C Pascal based classes only are
							// used on the Mac and begin with "C"
							// All other classes begin with "F"
#endif
#include "FTietzeDoc.h"
#include "FTietzeNode.h"

// to run the SUN version on the Mac we set up a
// console window here.  This is primarily a 
// debugging feature.

#ifndef Frank_GenuineMac
#ifdef THINK_CPLUS
#include <console.h>
#endif 
#endif


int main(int v, char**  av){
#ifndef Frank_GenuineMac
// configure the console window if running on the Mac
#ifdef THINK_CPLUS
// TESTING CONSOLE
	console_options.nrows=16;
	console_options.ncols=80;
// END TESTING CONSOLE 

/* APPLICATION CONSOLE
	console_options.nrows=36;
	console_options.ncols=100;
END APPLICATION CONSOLE */
#endif

// DEBUGGING RESOURCES
	debuggingResources=new FResources;
// END DEBUGGING RESOURCES

	FTietzeDoc* TietzeDoc=new FTietzeDoc;
	while(TietzeDoc->itsTietzeNode->run()){
		FGLink::timeout=clock()+ONE_SECOND;
	}

#else
// Genuine_Mac support for TCL classes
	extern CTietzeTrekApp *trekApp;
	trekApp = new CTietzeTrekApp;
	trekApp->ITietzeTrekApp();
	trekApp->Run();
	trekApp->Exit();
#endif
	return 1;
}

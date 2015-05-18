//   Copyright (C) 1995 The New York Group Theory Cooperative
//   See magnus/doc/COPYRIGHT for the full notice.
 
// Contents: Implementation of class MagnusHome
//
// Principal Author: Roger Needham
//
// Status: Complete
//
// Revision History:
//


//@db system porting
//#include "config.h"
#include "MagnusHome.h"


//------------------------------------------------------------------------//
//--------------------------- MagnusHome ---------------------------------//
//---------------------- static initialization ---------------------------//

char* MagnusHome::magnus_home = MAGNUS_HOME;
char* MagnusTmp::magnus_tmp = MAGNUS_TMP;

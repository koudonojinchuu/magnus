//   Copyright (C) 1995 The New York Group Theory Cooperative
//   See magnus/doc/COPYRIGHT for the full notice.
 
// Contents: Definition of class MagnusHome
//
// This class replaces the use of the preprocessor symbol MAGNUS_HOME.
// Main may reset the encapsulated char* when the Magnus root pathname
// is specified on the command line.
//
// Principal Author: Roger Needham
//
// Status: Complete
//
// Revision History:
//
 
#ifndef _MAGNUSHOME_H_
#define _MAGNUSHOME_H_

//@db system porting
#include "config.h"

#include "Chars.h"


struct MagnusHome
{
  friend int main(int argc, char* argv[]);
  static Chars magnusHome( ) { return Chars( magnus_home ); }

private:
  static char* magnus_home;
};

struct MagnusTmp
{
  friend int main(int argc, char* argv[]);
  static Chars magnusTmp( ) { return Chars( magnus_tmp ); }

private:
  static char* magnus_tmp;
};

#endif

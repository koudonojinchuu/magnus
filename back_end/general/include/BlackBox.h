// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the BlackBox class.
//           A BlackBox is a wrapper for an external binary which communicates
//           exclusively through its standard I/O.
//           The binary must accept some sort of `quit' command from its
//           standard input.
//
// Principal Author: Roger Needham
//
// Status: experimental
//
// Revision History:

#ifndef _BLACKBOX_H_
#define _BLACKBOX_H_


#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <iostream.h>
#include <strstream>
#include <fstream.h>
//#include <stdiostream.h>

#include "global.h"
#include "Chars.h"

extern "C" {
  char* tempnam(const char*,const char*); // Lib functions SunOS doesn't declare.
}


class BlackBox {

public:

  BlackBox(const Chars& startCommand, const Chars& restartCommand);
  // Instantiate a BlackBox by supplying sh commands which start and
  // restart the executable. The executable is actually invoked by the
  // start and restart methods.

  BlackBox(const Chars& startCommand);
  // A version of the above where the restart command is the same as the
  // start command, or undesired.

  ~BlackBox( );
  // Unlink the pipes, thus killing the executable.

  Bool start(const char* greeting = NULL);
  // Invoke the executable with the startCommand argument to the constructor.
  // This attaches named pipes to the executable's standard i/o.
  // The arg is a (possibly proper) prefix of the first non-blank line you
  // expect the binary to write to cout. We might use regexps some day.
  // This returns TRUE iff it finds the greeting (if not NULL), and all other
  // initialization was successful.
  // If you call this a second time, it just returns the status: TRUE == good.

  Bool restart(const char* greeting = NULL);
  // Unlink the pipes which were attached by start or any previous restart,
  // thus killing the executable. Invoke the executable again with the
  // restartCommand argument to the constructor, and re-attach the pipes.
  // The arg is the same as for start.
  // If you call this before you call start, it fails and returns FALSE.

  Bool stillRunning( );
  // This attempts to determine whether the executable is still running.
  //@rn It makes no real attempt yet.

  //@rn Maybe there's a better way to do this:

  ostream& toCommand( );
  // Returns an ostream bound to the binary's standard input.

  istream& fromCommand( );
  // Returns an istream bound to the binary's standard output.


private:

  // Data members:

  int status;                  // 1 means good, 0 means bad.
  Bool started_p;              // Set to TRUE when start is attempted.

  Chars theStartCmd;           // sh command to start executable
  Chars theRestartCmd;         // sh command to restart executable

  ofstream* streamToCommand;   // Bound to command's cin, or NULL
  ifstream* streamFromCommand; // Bound to command's cout, or NULL

//  char fyle_in[L_tmpnam];      // Name of pipe bound to command's cin
//  char fyle_out[L_tmpnam];     // Name of pipe bound to command's cout
  char fyle_in[100];      // Name of pipe bound to command's cin
  char fyle_out[100];     // Name of pipe bound to command's cout


  // Private functions:

  void initialize(const Chars& startCommand, const Chars& restartCommand);
  Bool doStart(const Chars& command, const char* greeting);
  void closeStreams( );
  Bool checkGreeting(const char* greeting);

  // Hide these:

  BlackBox(const BlackBox&) {
	 error("called BlackBox copy constructor");
  }
  BlackBox& operator = ( const BlackBox& ) {
	 error("called BlackBox op=");
  }
  
};

#endif



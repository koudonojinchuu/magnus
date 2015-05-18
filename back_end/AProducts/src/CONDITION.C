#include <iostream.h>
#include "CONDITION.h"

void debugMessage( int msgType, bool condition, const char *condDescription, 
		   const char *fileName, int lineno )
{
  if( !condition ) {

    cerr << endl;

    switch( msgType ) {
    case WARNING:
      cerr << "WARNING: ";
      break;
    case ERROR:
      cerr << "ERROR: ";
      break;
    default:
      cerr << "UNIDENTIFIED ERROR: ";
    }

    cerr << fileName << ":" << lineno << ": " << condDescription << " is false." 
	 << endl << endl;

    if( msgType != WARNING ) {
      while( 1 )
	;
      exit(1);
    }
  }
}

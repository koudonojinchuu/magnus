// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Inlined global functions error, warn.
//
// Principal Author: Roger Needham
//
// Status: Useable.

#ifndef _ERROR_H_
#define _ERROR_H_

#ifdef __GNUG__
//@jml This tells g++ that error does not return.
inline volatile void error(const char *msg) {
#else
inline void error(const char* msg) {
#endif

  cout << "fatal_error_ {" << msg << '}' << endl << flush;
  // Try to shove the message directly out to the front end

  cerr << endl << "Fatal error: " << msg << endl;

  while ( 1 );
  // So we can get a stack backtrace when debugging
}

inline void warn(const char* msg) {
  cerr << endl << "Warning: " << msg << endl;
}

#endif

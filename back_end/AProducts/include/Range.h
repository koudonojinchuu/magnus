// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:  
//            
//
// Principal Author: Dmitry Pechkin
//
// Status: under trial.
//
// Revision History:
//
// TO DO:
//
//

#ifndef __RANGE_H__
#define __RANGE_H__

#include <iostream.h>

struct Range {

  int low, high;
  bool Default;
  
  Range() : low(0), high(0), Default(true) { }

  Range( int lo, int hi ) : low(lo), high(hi), Default(false) { }

  bool operator == ( Range r ) const 
  {
    if( low == r.low && high == r.high && Default == r.Default )
      return true;
    return false;
  }
};

// IPC tools:

void operator < ( ostream& ostr, const Range& g );
void operator > ( istream& istr, Range& g );


// I/O routines for class Range.

ostream& operator << ( ostream& ostr, const Range& g );
istream& operator >> ( istream& istr, Range& g );

#endif

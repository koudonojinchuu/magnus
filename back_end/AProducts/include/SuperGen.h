// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:    Definition of class SuperGen.
//
// Principal Author: Dmitry Pechkin
//
// Status: under trial.
//
// Revision History:
//
// TO DO:
//
//  96/09/23 @dp why Generator.h doesn't include <iostream.h> and "global.h"?!
//

#ifndef __SUPERGEN_H__
#define __SUPERGEN_H__

#include <iostream.h>
#include "global.h"
#include "Generator.h"
//#include "Vector.h"
//#include "Word.h"



//////////////////////////////////////////////////////////////////////
//
//                        Class SuperGen
//
//////////////////////////////////////////////////////////////////////

class SuperGen {
public:

  SuperGen() : gen(1), exp(0) { }

  SuperGen( const Generator& g, int exponent = 1 ) : gen(g), exp(exponent) {
    // Need correct if the generator was given in negative form.
    if( ord(g) < 0 ) {
      gen = inv(g);
      exp = -exponent;
    }
  }      

  Generator generator() const { return gen; }

  int exponent() const { return exp; }

  bool operator == ( const SuperGen& g ) const 
  {
    if( &g == this || gen == g.gen && exp == g.exp )
      return true;
    return false;
  }

private:
  Generator gen;
  int exp;
};

void operator < ( ostream& ostr, const SuperGen& g );
void operator > ( istream& istr, SuperGen& g );
ostream& operator << ( ostream& ostr, const SuperGen& g );
istream& operator >> ( istream& istr, SuperGen& g );

#endif

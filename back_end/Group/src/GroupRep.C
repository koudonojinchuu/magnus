// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the GroupRep class
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//
// * 04/96 @dp & @ep fixed
//         Elt GroupRep::raiseToPower( const Elt& e, int n ) const
//
// * 04/97 dp rewrote `GroupRep::raiseToPower()'.


#include "GroupRep.h"


// Methods dealing with the group rep:


// Methods dealing with group elements:


Elt GroupRep::raiseToPower( const Elt& e, int n ) const
// Terribly inefficient. Override.
{
  if ( n == 0 ) return makeIdentity();

  if ( n == 1 ) return e;

  if ( n == -1 ) return inverseOf( e );


  Elt elt; 
  if ( n < 0 ) {
    n = -n;
    elt = inverseOf( e );
  }
  else 
    elt = e;

  Elt result;

  Elt square = elt; 
  int powerOfTwo = 2;
  while ( n ) {
    if( n % 2 ) 
      result = multiply( result, square );
    n /= 2;
    square = multiply( square, square );
  }
  return result;
}


Elt GroupRep::conjugateBy(const Elt& e1, const Elt& e2 ) const
// Terribly inefficient. Override.
{
  return multiply(inverseOf(e2), multiply(e1, e2));
}


Elt GroupRep::commutator(const Elt& e1, const Elt& e2 ) const
// Terribly inefficient. Override.
{
  return multiply(inverseOf(e1), conjugateBy(e1, e2));
}

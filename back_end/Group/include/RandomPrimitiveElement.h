// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declarations of classes RandomPrimitiveElement
//
// Principal Author: Sergei Lioutikov
//
// Status: in progress
//
// Revision History:
//
// Notes:
//
// * generates random primitive elements for a free group ( those which can be 
//   extended to a basis. )
// * copy constructor does logical copy.
//

#ifndef _RANDOMPRIMITIVEELEMENT_H_
#define _RANDOMPRIMITIVEELEMENT_H_


#include "RandomNumbers.h"
#include "FreeGroup.h"

//---------------------------------------------------------------------------//
//----------------------- RandomPrimitiveElement ----------------------------//
//---------------------------------------------------------------------------//


class RandomPrimitiveElement
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  RandomPrimitiveElement( const FreeGroup& F, int seed );
  // Giving the same `seed' you obtain the same sequence of automorphisms.

  ~RandomPrimitiveElement( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Word getElement( int avgNumGens );
  // `avgNumGens' gives the average number of Whitehead automorphisms to apply.

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FreeGroup theGroup;

  int numberOfGroupGens;

  NormalRandom numGensPicker;
  UniformRandom typeGenPicker;

};

#endif


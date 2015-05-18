// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declarations of classes RandomAutomorphism
//
// Principal Author: Sergei Lioutikov
//
// Status: in progress
//
// Revision History:
//
// Notes:
//
// * generates random automorphisms for a free group.
// * copy constructor does logical copy.
// * this is an adoptation of the algorithm written by Roger Needham.
//

#ifndef _RANDOMAUTOMORPHISM_H_
#define _RANDOMAUTOMORPHISM_H_


#include "RandomNumbers.h"
#include "FreeGroup.h"
#include "Map.h"

//---------------------------------------------------------------------------//
//------------------------- RandomAutomorphism ------------------------------//
//---------------------------------------------------------------------------//


class RandomAutomorphism
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  RandomAutomorphism( const FreeGroup& F, int seed );
  // Giving the same `seed' you obtain the same sequence of automorphisms.

  ~RandomAutomorphism( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  VectorOf<Word> getGeneratingVector( int avgNumGens );
  // `avgNumGens' gives the average number of Whitehead automorphisms to apply.

  Map getAutomorphism( int avgnumGens );
  // similar to `getGeneratingVector'.

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


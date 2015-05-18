// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of FreeByCyclic, a group structure class
//           representing the structure of a free group extended
//           by an automorphism.
//
// Principal Author: Roger Needham
//
// Status: In progress.
//
// Revision History:
//


#ifndef _FREE_BY_CYCLIC_H_
#define _FREE_BY_CYCLIC_H_


#include "FreeGroup.h"
#include "Map.h"


class FreeByCyclic
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FreeByCyclic(const FreeGroup F, const Map aut);

  // Destructor supplied by compiler

  FreeByCyclic& operator=( const FreeByCyclic& G );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  VectorOf<Chars> namesOfGenerators( ) const;

  SetOf<Word> getRelators( ) const;

  Word normalForm(const Word& w) const;

  friend ostream& operator<<( ostream& ostr, const FreeByCyclic& G );
  friend istream& operator>>( istream& istr,       FreeByCyclic& G );

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const FreeGroup theGroup;

  const Map theAut;
  const Map  theAutInv;

  int newGenerator;

};

#endif

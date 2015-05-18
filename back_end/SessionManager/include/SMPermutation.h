// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class SMPermutation
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//

#ifndef _SMPermutation_H_
#define _SMPermutation_H_

#include "Permutation.h"
#include "AlgebraicObject.h"


// --------------------------- SMPermutation ------------------------------- //


class SMPermutation : public AlgebraicObject
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMPermutation( Permutation perm, const Chars heritage );
  
  SMPermutation( ) : AlgebraicObject("")
  { }
  //@rn Bogus ctor; see `But no' comment in Menu.h.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Permutation getPermutation( ) const { return thePermutation; }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const char* type( ) { return "SMPermutation"; }

  const char* typeID( ) const { return type(); }        // overrides SMObject

  const IconID iconID( ) const { return IconID::permutation; }  
  // overrides SMObject

  void viewStructure(ostream& ostr) const;              // overrides SMObject

  void printProperties(ostream& ostr) const;            // overrides SMObject

  void printDefinition(ostream& ostr) const;            // overrides SMObject


protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Restricted Access:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void readMessage(istream&) { }                        // overrides SMObject


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Permutation thePermutation;
 
};

#endif

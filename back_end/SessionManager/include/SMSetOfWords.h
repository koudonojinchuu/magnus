// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class SMSetOfWords
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 11/10/96 EP fixed the implementation: Vectors replaced with true Sets

#ifndef _SMSetOfWords_H_
#define _SMSetOfWords_H_

#include "AlgebraicObject.h"
#include "SMFPGroup.h"
#include "conversions.h"


//--------------------------- SMSetOfWords ----------------------------------//


class SMSetOfWords : public AlgebraicObject
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMSetOfWords(SMFPGroup& G, const SetOf<Word>& V, const Chars heritage);
  //@ep changed

  SMSetOfWords(SMFPGroup& G)
    : AlgebraicObject(""), theGroup(G), theWords(0)
  { }
  //@rn Bogus ctor; see `But no' comment in Menu.h

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  VectorOf<Word> getWords( ) const { return makeVectorOf(theWords); }

  SMFPGroup& getParent( ) const { return theGroup; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const char* type( ) { return "SMSetOfWords"; }

  const char* typeID( ) const { return type(); }          // overrides SMObject

  const IconID iconID( ) const { return IconID::SetOfWords; }
  // overrides SMObject

  void viewStructure(ostream& ostr) const;                // overrides SMObject

  void printProperties(ostream& ostr) const;              // overrides SMObject

  void printDefinition(ostream& ostr) const;              // overrides SMObject


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

  const SetOf<Word> theWords;

  SMFPGroup& theGroup;

};

#endif

// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class SMVectorOfWords
//
// Principal Author: Eugene Paderin
//
// Status: in progress
//
// Revision History:
//

#ifndef _SMVectorOfWords_H_
#define _SMVectorOfWords_H_

#include "AlgebraicObject.h"
#include "SMFPGroup.h"
#include "conversions.h"


//--------------------------- SMVectorOfWords ----------------------------------//


class SMVectorOfWords : public AlgebraicObject
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMVectorOfWords(SMFPGroup& G, const VectorOf<Word>& V, const Chars heritage);

  SMVectorOfWords(SMFPGroup& G)
    : AlgebraicObject(""), theGroup(G), theWords(0)
  { }
  //@rn Bogus ctor; see `But no' comment in Menu.h

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  VectorOf<Word> getWords( ) const { return theWords; }

  SMFPGroup& getParent( ) const { return theGroup; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const char* type( ) { return "SMVectorOfWords"; }

  const char* typeID( ) const { return type(); }          // overrides SMObject

  const IconID iconID( ) const { return IconID::VectorOfWords; }
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

  VectorOf<Word> theWords;

  SMFPGroup& theGroup;

};

#endif

// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the FGGroup class.
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//

#include "PresentationParser.h"
#include "FGGroup.h"


// Methods and operators which deal with the group:


Chars FGGroup::nameOfGenerator(int i) const
{
  #if SAFETY > 0
    if ( i < 0 )
		error("passed negative index to "
			"FGGroup::nameOfGenerator");
  #endif
  return look()->theNamesOfGenerators[i];
}

Chars FGGroup::nameOfGenerator(Generator g) const
{
  int i = ord(g);
  #if SAFETY > 0
    if ( i < 0 )
		error("passed negative index to "
			"FGGroup::nameOfGenerator");
  #endif
  if (i>0) return look()->theNamesOfGenerators[i-1];
  if (i<0) error("passed negative index to "
	"FGGroup::nameOfGenerator");
  // for the time being, let's not allow this
}


// Methods which deal with group elements:


Word FGGroup::readWord(istream& istr, Chars& errMesg) const
{
  WordParser P(istr);
  return P.parseWord( namesOfGenerators(), errMesg);
}


SetOf<Word> FGGroup::readSetOfWords(istream& istr, Chars& errMesg) const
{
  PresentationParser P(istr);
  return P.parseSetOfWords(namesOfGenerators(), errMesg);
}


VectorOf<Word> FGGroup::readVectorOfWords(istream& istr, Chars& errMesg) const
{
  PresentationParser P(istr);
  return P.parseVectorOfWords(namesOfGenerators(), errMesg);
}



// Methods which deal with sets of group elements:


void FGGroup::closeUnderCyclicPermutations(SetOf<Word>& S) const
{
  SetIterator<Word> I(S);
  while ( !I.done() ) {
	 Word w = I.value();
	 int i = w.length();
	 while ( --i ) {
		w = w.cyclicallyPermute(1);
		S |= w;
	 }
	 I.next();
  }
}

//---------------------- FGGroupRep:: ---------------//
//------------------------- Static members -------------------------//
 
const Type FGGroupRep::theFGGroupType =
	Type( Type::unique() );
 
 
//----------------------- FGGroup:: -----------------//
//------------------------- Static members -------------------------//
 


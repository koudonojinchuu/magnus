
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the NilpotentGroupRep class.
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Revision History:
//
// Special Notes:
//
//

#include "NilpotentGroupRep.h"
#include "NilpotentGroup.h"
#include "FreeGroup.h"

#include "NilpCollectors.h"
#include "PolyWordIterator.h"

#include "GCD.h"

#include <stdio.h>


//------------------------------------------------------------
//  Special helper stuff
//------------------------------------------------------------

const Type NilpotentGroupRep::theNilpotentGroupType =
Type( Type::unique() );


//-------------------------------------------------------------------
// 	Accessors 
//-------------------------------------------------------------------


// produces FP presentation of the group

PresentationForNG NilpotentGroupRep::makePresentation() const {
  PresentationForNG pres(collector(), relators() );
  pres.build();
  return pres;
}

//-------------------------------------------------------------------
// 	Methods dealing with group elements
//-------------------------------------------------------------------

// returns true iff w lies in the group center
bool NilpotentGroupRep::isCentral(const Word& w) const {

  Word wInv = w.inverse();

  for( int i = 1; i <= theNumberOfGenerators; i++ ) {
    Generator g(i);
    Word comm = w * g * wInv * inv(g);
    if( ! wordProblem(comm) ) 
      return false;
  }
  return true;
}

// returns true iff the word is proper power
bool NilpotentGroupRep::isProperPower(const PolyWord& pw) const {
  PolyWord root;
  int power;
  maximalRoot(pw, root, power);
  return power > 1;
}



//-------------------------------------------------------------------
// IPC tools:                                                          
//-------------------------------------------------------------------
    
void NilpotentGroupRep::write( ostream& ostr ) const
{
  FGGroupRep::write(ostr);
  ostr << actualType() << ' ';
}

void NilpotentGroupRep::read( istream& istr )
{
  /* NilpotentGroupRep *This = (NilpotentGroupRep *)this;

  This->FGGroupRep::read(istr);
  */
  FGGroupRep::read(istr);
  Type savedType = Type::notype();
  istr >> savedType;
  if(savedType != actualType())
    error("NilpotentGroupRep: wrong Rep type transferred via IPC");
}

//-------------------------------------------------------------------
// Internal methods:                                                    
//-------------------------------------------------------------------

// remove all basic generators of weight more than k

void NilpotentGroupRep::mapToClass( PolyWord& pw, int theClass ) const {

  if( theClass >= nilpotencyClass() ) return;

  PolyWordIterator iter(pw);
  
  for( iter.startFromLeft(); ! iter.done(); ) {
    if( commutators().weightOf( ord( iter.thisLetter().gen ) ) > theClass )
      iter.removeThisLetter();
    else
      iter.stepRight();
  }

  pw.freelyReduce();
}
VectorOf<Word> NilpotentGroupRep::getLCSterm(int i) const
{
  if (i > nilpotencyClass())
    error("VectorOf<Word> NilpotentGroupRep::getLCSterm(int i):Wrong argument.");
  NGWordForms bcForms(commutators());
  // Helper class for conversion to words
  int firstIndex = commutators().theFirstOfWeight(i);
  // Index of the first commutator of weight = weightOfQ
  int numberOfCommutators = 0;
  for (int j=i;j<nilpotencyClass()+1;j++)
    numberOfCommutators+=commutators().numberOfWeight(j);
  // Number of  commutators of weight = weightOfQ ... nilpotency class
  VectorOf<Word> generatorsOfLCSTerm(numberOfCommutators);
  
  for (int j=firstIndex;j<firstIndex+numberOfCommutators;j++){
    generatorsOfLCSTerm[j-firstIndex] = bcForms.toWord(j);
  }  
  FreeGroup F1(theNumberOfGenerators );
  generatorsOfLCSTerm = F1.nielsenBasis(generatorsOfLCSTerm);  
  return generatorsOfLCSTerm;
}

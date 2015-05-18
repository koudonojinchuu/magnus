// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the SymmetricRelators class.
//
// Principal Author: Dmitry Pechkin, Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// Special Notes:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//

#include "Set.h"
#include "Vector.h"
#include "SymmetricRelators.h"


//-------------------------- class SymmetricRelators -------------------------//


SymmetricRelators::SymmetricRelators(const SetOf<Word>& relators) {

  SetOf<Word> symmRels = relators;
#if SAFETY > 0
  symmRels -= Word();
#endif
  minimizeSet(symmRels);

  int symmRelsLen = symmRels.cardinality();
  symmetricRelators = VectorOf<Word>(symmRelsLen);
  SetIterator<Word> I(symmRels);
  for ( int i = 0; !I.done(); I.next(), i++)
    symmetricRelators[i] = I.value();

  rootLengths = new int[symmRelsLen];
  for ( int i = 0; i < symmRelsLen; i++ )
    rootLengths[i] = rootLength( symmetricRelators[i] );  
} 


SymmetricRelators::~SymmetricRelators() {
  delete [] rootLengths;
}


// ------------------- Some global functions -----------------------//


Word minimalWord(const Word& w)
// get minimal form of word W :
//  minimal form M of word W is cyclic permutation of W or W^-1 such that
//  if V is any cyclic permutation of W or W^-1  then V >= M
{
  Word result = w;
  Word u = w;
  Word uInverse;
  
  for(int shift=0; shift<u.length(); shift++) {
    uInverse = u.inverse();
    if( u < result ) result = u;
    if( uInverse < result ) result = uInverse;
    u = u.cyclicallyPermute(1);
  }
  
  return result;
}

SetOf<Word>& minimizeSet(SetOf<Word>& S)
// rebuild set of minimal form of words.
{
  unsymmetrise(S);
  SetIterator<Word> I(S);
  S.removeAllElements();
  while( !I.done() ) {
    S |= minimalWord( I.value() );
    I.next();
  }
  return S;
}


template <class T>
T arbitraryElementOf(const SetOf<T>& S)
// Returns an arbitrary element of given set.
// This doesn't guaranteed to get different elements of set if it is called
// repetitive. Really it may return the same element always.
//
//@dp May It should be defined as member of SetOf<> class?
{
  if(S.cardinality() == 0)
    error("arbitraryElementOf(SetOf<T>& ): attempt to take "
	  "an element of empty set.");
  
  SetIterator<T> I(S);
  return I.value();
}

/* @db 2.91
template SetOf<Word> arbitraryElementOf(SetOf<Word>&);
// make instatination of the function template.
*/

SetOf<Word>& unsymmetrise(SetOf<Word>& S)
// @dp provisional implementation, too expensive.
{
  SetOf<Word> SymSet(S);
  SetOf<Word> result;
  
  while( SymSet.cardinality() > 0 ) {
	 	Word w = arbitraryElementOf(SymSet);
	 	Word wInv = w.inverse();
		SymSet -= w;
		SymSet -= wInv;
		// delete all the cyclic permutations of w and wInv from SymSet
	 	for(int i=1; i<w.length(); i++) {
		  SymSet -= w.cyclicallyPermute(i);
		  SymSet -= wInv.cyclicallyPermute(i);
	  	}
		result |= w;
	      }
  S = result;
  return S;
}


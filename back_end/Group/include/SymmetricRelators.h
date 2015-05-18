// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of SymmetricRelators class
//           
//
//   Compact storage for symmetrised relators.
//
// Principal Author: Dmitry Bormotov, Dmitry Pechkin
//
// Status: trial implementation
//
// Usage:
//
//

#ifndef _SYMMETRIC_RELATORS_H
#define _SYMMETRIC_RELATORS_H

#include "Set.h"
#include "Word.h"
#include "Vector.h"


// ------------------- Some global functions -----------------------//


template <class T>
T arbitraryElementOf ( const SetOf<T>& S );
// Returns an arbitrary element of given set.
// This doesn't guaranteed to get different elements of set if it is called
// repetitive. Really it may return the same element always.
//
//@dp May It should be defined as member of SetOf<> class?


SetOf<Word>& unsymmetrise ( SetOf<Word>& S );
// @dp provisional implementation, too expensive.

Word minimalWord( const Word& w );
// get minimal form of word W :
//  minimal form M of word W is cyclic permutation of W or W^-1 such that
//  if V is any cyclic permutation of W or W^-1  then V >= M

SetOf<Word>& minimizeSet( SetOf<Word>& S );
// rebuild set of minimal form of words.

inline int rootLength( const Word& w ) { return w.length() / maximalRoot(w); }


//-------------------------- class SymmetricRelators -------------------------//


class SymmetricRelators {

friend class SymmetricRelatorsIterator;

public:

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Constructors:                                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  // No default constructor.

  // Copy constructor, operator=, and destructor supplied by compiler.
  
  SymmetricRelators ( const SetOf<Word>& relators );

  ~SymmetricRelators ( );


  ///////////////////////////////////////////////////////
  //                                                   //
  //  Accessors:                                       //
  //                                                   //
  ///////////////////////////////////////////////////////

  int cardinality( ) const {
    return symmetricRelators.length();
  } 

  const VectorOf<Word>& getRelators( ) {
    return symmetricRelators; 
  }

private:
  
  ///////////////////////////////////////////////////////
  //                                                   //
  //  Data members:                                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  VectorOf<Word> symmetricRelators;
  int *rootLengths;
  
};


//---------------------- class SymmetricRelatorsIterator ---------------------//


class SymmetricRelatorsIterator {

public:


  ///////////////////////////////////////////////////////
  //                                                   //
  //  Constructors:                                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  // No default constructor.

  // Copy constructor, operator=, and destructor supplied by compiler.

  SymmetricRelatorsIterator(const SymmetricRelators& S) : 
      iterSet(S), relatorNumber(0), cpNum(0), invFlag(false), 
      iterSetLen(S.symmetricRelators.length()) {
    if ( !done() ) {
      curRelator = iterSet.symmetricRelators[relatorNumber];
      rootLen = iterSet.rootLengths[relatorNumber];
      cpNum = rootLen;
    }
  }
  // Constructs iterator from SymmetricRelators over which to iterate.

  SymmetricRelatorsIterator( const SetOf<Word>& S ); 

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Accessors                                        //
  //                                                   //
  ///////////////////////////////////////////////////////

  Bool operator == ( const SymmetricRelatorsIterator& I ) const {
    return ( cpNum == I.cpNum  &&  invFlag == I.invFlag  &&  relatorNumber == I.relatorNumber );
  }
  // TRUE iff the iterators will look at the same elements of the (physically)
  // same set in the same order. Valid at any point of the iteration.

  Word value( ) const {
  #if SAFETY > 0
    if( done() ) error("tried to retrieve value from SymmetricRelatorsIterator "
                       "which is done");
  #endif
    return curRelator;
  }
  // Returns the current relator. Calling this is a fatal error if done().

  Bool next( ) {
    if( done() ) return false;
    bool bSuccess = true;
    cpNum--;
    if (cpNum > 0)
      curRelator = curRelator.cyclicallyPermute(1);
    else {
      cpNum = 0; //@db ?
      if( invFlag || curRelator.length() == 0 ) {
	bSuccess = ( ++relatorNumber < iterSetLen );
	if (bSuccess) {
	  curRelator = iterSet.symmetricRelators[relatorNumber];
	  rootLen = iterSet.rootLengths[relatorNumber];
	  cpNum = rootLen;
	  invFlag = false;
	}
      }
      else {
	curRelator = curRelator.inverse();
	// length of root of curRelator is not changed
	  cpNum = rootLen;
	invFlag = !invFlag;
      }
    }
    return bSuccess;
  }
  // Advances this iterator.
  // Returns TRUE iff the iteration has not finished.
  // This may be called after it returns FALSE with no side effect.

  
  bool done( ) const { 
    return (relatorNumber >= iterSetLen); 
  }
  // Returns TRUE iff the iteration has finished. This may
  // be called after it returns TRUE with no side effect.
  
  void reset( ) {
    relatorNumber = 0;
    cpNum = 0;
    invFlag = false;
    if ( !done() )  {
      curRelator = iterSet.symmetricRelators[relatorNumber];
      rootLen = iterSet.rootLengths[relatorNumber];
      cpNum = rootLen;
    }
  }
  // Resets this iterator to the state it was in after construction.
  
private:

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Data members:                                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  const SymmetricRelators& iterSet;
  int iterSetLen;
  int relatorNumber;
  int cpNum;       // number of cyclic permutation
  int rootLen;     // length of root of curRelator 
  bool invFlag;    // true, if we iterate for inverse curRelator
  Word curRelator; // current relator

};

#endif



// Copyright (C) 1994-1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the MSCGroup class
//           for metric small cancellation groups
//
// Principal Author: Dmitry Bormotov, Dmitry Pechkin
//
// Status: in progress
//
// Usage:
//
// * Any group satisfying a relator overlap of *strictly* less than
//   one-half is admitted as an MSCGroup. But, there is effectively solution
//   of wordProblem and conjugacyProblem for group with C'(1/6)
//   condition only.
//
//
// Special Notes:
//
// Revision History:
//

#ifndef _MSC_GROUP_H_
#define _MSC_GROUP_H_

#include "FPGroupRep.h"
#include "FPGroup.h"
#include "SymmetricRelators.h"
#include "ShortenByRelators.h"
#include "PresentationParser.h"


//----------------------- MSCGroup ---------------------------//


class MSCGroup {

friend class MSCGConjugacyProblem;

public:


  ///////////////////////////////////////////////////////
  //                                                   //
  //  Constructors:                                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  MSCGroup( int ngens = 0 ) : numberOfGenerators ( ngens )
  {
    SetOf<Word> rels;
    makeMSCGroup(ngens, rels);
  }
  // To construct a group of given number of unnamed generators and
  // no relators. Default constructor gives trivial group.

  MSCGroup( int ngens, const SetOf<Word>& rels, int lambda = -1 )
  {
    makeMSCGroup(ngens, rels, lambda);
  }
  // To construct a MSC group of given number of unnamed generators and
  // given relators. When constructing an MSC group lambda value maybe known.

  MSCGroup( FPGroup G, int lambda = -1 ) 
  {
    makeMSCGroup(G.numberOfGenerators(), G.getRelators(), lambda);
  }
  // To construct a metric small cancellation group of given FPGroup.
  // When constructing an MSC group lambda value maybe known.


  ~MSCGroup( ) {
    delete symmetricRelators;
    delete shortenByRels;
  }
 

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods and operators which deal with the group  //
  //                                                   //
  ///////////////////////////////////////////////////////

  int order() const;
  // Returns -1(dontknow) iff the group is not C'(1/6).

  Trichotomy isTrivial() const;
  // Returns dontknow iff the group is not C'(1/6).

  Trichotomy isFinite() const;
  // Returns dontknow iff the group is not C'(1/6).

  Trichotomy isInfinite() const;
  // Returns dontknow iff the group is not C'(1/6).

  Trichotomy isAbelian() const;
  // Returns dontknow iff the group is not C'(1/6).

  Trichotomy isFree() const;
  // Provisional not complete implementation,
  // useful only for some particular cases.

  int getMSCLambda() const {
    return mscLambda;
  } 


  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods which deal with group elements           //
  //                                                   //
  ///////////////////////////////////////////////////////

  Word shortenByRelators(const Word& w) const;

  Word cyclicallyShortenByRelators(const Word& w) const;
  // This method shortens its argument w by relators of the group.
  // The word w is interpreted as the cyclic word.
  // If word argument w contains a piece of some relator of group, and the
  // rest of relator is smaller than this piece, procedure shortens w.

  Elt eval( const Word& w ) const;

  Trichotomy wordProblem( const Word& w ) const;

  Trichotomy areEqual(const Word& w1, const Word& w2) const {
    return wordProblem( w1 * w2.inverse() );
  }


private:


  ///////////////////////////////////////////////////////
  //                                                   //
  //  Private functions:                               //
  //                                                   //
  ///////////////////////////////////////////////////////

  MSCGroup( const MSCGroup& );
  // It is hidden, not implemented.

  MSCGroup& operator = ( const MSCGroup& );
  // It is hidden, not implemented.

  void calculateLambda( );

  void makeMSCGroup( int ngens, const SetOf<Word>& rels, int lambda = -1 );

  int numOfGens() const {
    return numberOfGenerators;
  }
  
  SetOf<Word> getRelators() const {
    const VectorOf<Word> v = symmetricRelators->getRelators();
    SetOf<Word> S;
    int vLen = v.length();
    for ( int i = 0; i < vLen; i++ )
      S |= v[i];
    return S;
  }

  Word cyclicallyShortenByRelators( const Word& w, Word& conjugator) const;


  ///////////////////////////////////////////////////////
  //                                                   //
  //  Data members:                                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  SymmetricRelators *symmetricRelators;
  // Storage of minimized forms of relators. More compact that relators 
  // given by a caller.
  // To get full symmetric set of relators the symmetric set iterator 
  // is provided.
  
  ShortenByRelators *shortenByRels;

  int mscLambda;
  int numberOfGenerators;

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Debugging stuff                                  //
  //                                                   //
  ///////////////////////////////////////////////////////

#ifdef DEBUG
  friend int main(int, char **);
#endif

};

#endif

// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class GroupFastChecks
//
// Principal Authors: Dmitry Bormotov
//
// Status: usable.
//
// Usage: 
//
// Special Notes:
//
// Revision History:
//


#include "GroupFastChecks.h"
#include "PrimeNumbers.h"


// -------------------------- GroupFastChecks ------------------------------ //


bool GroupFastChecks::existsGenWithExpSumZeroInEveryRelator( Generator& g )
{
  for( int i = 0; i < numOfGens; ++i ) {

    Generator gen(i+1);
    SetIterator<Word> I(relators);
    bool allZero = true;

    for( ; !I.done(); I.next() )
      if( Word(I.value()).exponentSum(gen) != 0 ) { allZero = false; break; }

    if( allZero ) { g = gen; return true; }
  }

  return false;
}


VectorOf<int> GroupFastChecks::getExpSumOfGen( const Generator& g )
{
  SetIterator<Word> I(relators);
  VectorOf<int> result( relators.cardinality() );
  int i = 0;
  
  for( ; !I.done(); I.next() )
    result[i++] = Word( I.value() ).exponentSum(g);

  return result;
}


int GroupFastChecks::GCDOfExpSumOfGen( const Generator& g )
{
  VectorOf<int> v = getExpSumOfGen(g);

  // provisional cast to VectorOf<Integer> because no appropriate
  // gcd-function has written yet.
  
  int vLen = v.length();
  VectorOf<Integer> V(vLen);
  for( int i = 0; i < vLen; ++i )
    V[i] = v[i];
  
  return IntProblems().gcdOfVector(V).as_long();
}


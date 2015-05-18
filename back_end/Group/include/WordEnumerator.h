// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declarations of classe WordEnumerator
//
// Principal Author: Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//
// Notes:
//
//

#ifndef _WORDENUMERATOR_H_
#define _WORDENUMERATOR_H_

#include "FreeGroup.h"

// ------------------------ VectorEnumerator -------------------

class VectorEnumerator
{
 public:
  VectorEnumerator( int n = 2) :
    current(0) {
    if ( n < 2)
      error("VectorEnumerator( int n = 2) :"
	    " length of vectors can not be less than 2");
    lengthOfVectors = n;
  }

  // Iteration functions
  void reset(int c = 0) { current = c; }
  void next() { current++; }  
  VectorOf<Integer> vector() const {
    return getVectorOf( current );
  }

  // Coding functions
  VectorOf<Integer> getVectorOf ( Integer c ) const;
  
  Integer getNumberOfVector(VectorOf<Integer> v) const;

  int vectorsLength() const { return lengthOfVectors; }

 private:
  VectorOf<Integer> getPairOf ( Integer c ) const;
  
  Integer getNumberOfPair(const Integer& n,const Integer& m ) const{
    
    return ((n + m)*(n + m + 1))/2 + m;
  }

  int current;
  int lengthOfVectors;
};


// ------------------------ WordEnumerator -------------------

class WordEnumerator
{
public:
  WordEnumerator( const FreeGroup& f, int n = 2):
    ve( n ),
    theGroup( f )
    { }
  
  void reset(int c = 0) { ve.reset( c ); }
  void next() { ve.next(); }
  
  VectorOf<Word> getWords();
    
private:
  FreeGroup theGroup;
  VectorEnumerator ve;
};


#endif

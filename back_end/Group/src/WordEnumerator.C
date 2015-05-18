// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classe WordEnumerator
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

#include "WordEnumerator.h"

// ------------------------ VectorEnumerator -------------------

  // Coding functions
VectorOf<Integer> 
VectorEnumerator::getVectorOf ( Integer c ) const {
  VectorOf<Integer> v(lengthOfVectors);
  Integer tmpC = c;
  for (int ii = 0;ii<lengthOfVectors-1;ii++) {
    VectorOf<Integer> pair = getPairOf(tmpC);
    v[ii] = pair[0];
    tmpC = pair[1];
  }
  v[lengthOfVectors-1] = tmpC;
  return v;
}

Integer VectorEnumerator::getNumberOfVector(VectorOf<Integer> v) const{
  if (v.length() != lengthOfVectors)
    error("Integer getNumberOfVector(VectorOf<Integer> v) const: "
	  " wrong length of vector ");
  
  Integer c =  v[lengthOfVectors - 1];
  for (int ii = lengthOfVectors - 2 ;ii >= 0;ii--) {
    c = getNumberOfPair(v[ii],c);
  }
  return c;
}

VectorOf<Integer> 
VectorEnumerator::getPairOf ( Integer c ) const {
  Integer n,m;
  Integer a = 2*sqrt(c); // sqrt returns a result of the type Integer.
  // Integer a = round( 2*sqrt(c) );
  if ( a != 0) {
    //a+=1;
    while ( c < getNumberOfPair(a,0) )
      a-=1;
    while ( c > getNumberOfPair(0,a) )
      a+=1;
  }
  m = c - (a*(a + 1))/2;
  
  // Compute n : 
  //@dp: porting to g++ 2.8.x: Integer tmp = round( sqrt( (2*m+1)*(2*m+1) - 4*m*m -12*m + 8*c) );
  Integer tmp = sqrt( (2*m+1)*(2*m+1) - 4*m*m -12*m + 8*c );
  n = (-(2*m+1) + tmp)/2;
  if ( n < 0 ) n = abs( n );
  
  // Make vector
  VectorOf<Integer> v(2);
  v[0] = n;
  v[1] = m;
  return v;
}

// ------------------------ WordEnumerator -------------------

VectorOf<Word> 
WordEnumerator::getWords(){
  VectorOf<Integer>  vi = ve.vector();
  
  VectorOf<Word> v(ve.vectorsLength());
  for (int j = 0;j<ve.vectorsLength();j++) {
    v[j] =  theGroup.getN_thElement( vi[j].as_long() );
  }
  return v;
}

    

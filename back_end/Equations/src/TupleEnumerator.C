// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class EnumeratorOfWordTuples.
//
// Principal Authors: Eugeny Paderin, Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
// Description:
//
// We enumerate tuples dividing the tuple space into disjoint layers.
// The layer number L (0-based) can be defined sush a way: it contains
// (finite number of) tuples of words with numbers not greater than L,
// and a tuple must contain at least one word with number L. (Word
// numbering is defined by mean of word enumerating; really, we do not
// need these numbers.)
//
// The process can be visualized as a hypercube expanding into
// n-dimensional space. The layer is a set of (n-1)-faces not
// containing in coordinate hyperplanes.
//
// In fact, we further divide every such layer into sublayers --
// (n-1)-faces. Elements of the current sublayer are listed
// in the lexicographic (ShortLex based) order.
//
// Special remarks:
//

#include "TupleEnumerator.h"

void EnumeratorOfWordTuples::next()
{
	// walk from right to left trying to construct next word
	//
	// the word ...AL...L is followed by ...(A+1)0...0

	for(int i= tupleSize - 1; i >= 0; i--) {
		if(tuple[i] == maxWord[i])
			tuple[i] = Word();   // and step left
		else {
			tuple[i] = Word(tuple[i]).nextFreelyReduced(numberOfGens[i]);
			tuple[fixedWord] = (Word)maxWord[fixedWord];
			return;
		}
	}

	// we arrive here only when all sublayer elements are enumerated
	// and we ought to take next sublayer or layer

	if( ++fixedWord == tupleSize )   // increment means next sublayer
		fixedWord = 0;               // and zeroing means next layer

	maxWord[fixedWord] = Word(maxWord[fixedWord]).nextFreelyReduced(numberOfGens[fixedWord]);

	// the tuple is already zeroed

	tuple[fixedWord] = (Word)maxWord[fixedWord];
}


Integer CantorEnumeration::encodePair( const Integer& x, const Integer& y)
{
  return x + ( (x+y) * (x+y+1) ) / 2;
}

inline Integer minus( const Integer& x, const Integer& y ) 
{
  if( x > y ) 
    return x - y;
  return 0;
}

void CantorEnumeration::decodePair( const Integer& n, Integer& x, Integer& y )
{
  Integer sqrt8n1 = sqrt(8*n+1);
  Integer x1 = (sqrt8n1+1) / 2;
  Integer x2 = minus(sqrt8n1,1) / 2;
  x = minus( n, x1 * x2 / 2 );
  y = minus( (sqrt8n1+1)/2,  x + 1 );
}

Integer CantorEnumeration::encodeTuple( const VectorOf<Integer>& tuple )
{
  int n = tuple.length();
  if( n == 0 ) {
    return 0;
  }

  Integer number = tuple[n-1];
  for( int i = n-2; i >= 0; --i ) {
    number = encodePair( tuple[i], number );
  }
  return encodePair( n, number );
}

VectorOf<Integer> CantorEnumeration::decodeTuple( const Integer& number )
{
  Integer ni, tmp;
  decodePair( number, ni, tmp );
  int n = ni.as_long();

  VectorOf<Integer> tuple(n);
  for( int i = 0; i < n; ++i ) {
    Integer elt;
    decodePair( tmp, elt, tmp );
    tuple[i] = elt;
  }
  return tuple;
}

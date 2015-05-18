// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class NormalRandomWord.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:


#include "NormalRandomWord.h"


Word NormalRandomWord::word( )
{
  int len;
  do {
	 len = L.rand(mean, stddev);
  } while ( len < 1 );
  VectorOf<Generator> v(len);

  int i = 0;
  while ( i < len ) {
	 int gen;
	 do {
		gen = G.rand(-numGens, numGens);
	 } while ( gen == 0 );
	 if ( (i == 0) || (v[i] != -gen) ) v[i++] = gen;
  }
  return Word(v);
}

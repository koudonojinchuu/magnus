// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of class NormalRandomWord for generating
//           random words with normally distributed lengths and
//           uniformly distributed generators.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
// * 07/96 Alexey M. implemented IPC tools.


#ifndef _NORMALRANDOMWORD_H_
#define _NORMALRANDOMWORD_H_


#include "Word.h"
#include "RandomNumbers.h"


class NormalRandomWord {

public:

  NormalRandomWord(int numberOfGenerators, int meanLength, int lengthStddev,
						 int lengthSeed, int generatorSeed)
  : numGens(numberOfGenerators), mean(meanLength), stddev(lengthStddev),
	 G(generatorSeed), L(lengthSeed)
  { }

  Word word( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const NormalRandomWord& nRW )
  {
    ostr < nRW.numGens < nRW.mean < nRW.stddev < nRW.G < nRW.L;
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, NormalRandomWord& nRW )
  {
    istr > nRW.numGens > nRW.mean > nRW.stddev > nRW.G > nRW.L;
    return istr;
  }

private:

  int numGens, mean, stddev;

  UniformRandom G;
  // For choosing letters in the word.

  NormalRandom L;
  // For choosing the length of the word.


};

#endif

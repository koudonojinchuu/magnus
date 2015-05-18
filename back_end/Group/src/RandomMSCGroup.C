// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the RandomMSCGroup class.
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// Special Notes:
//
// Discussion:
//


#include "RandomMSCGroup.h"


FPGroup RandomMSCGroup::getRandomMSCGroup() {
  NormalRandomWord NW(numOfGens, meanLenOfRels, lenOfRelsStddev, gensSeed, lenOfRelsSeed);
  while ( true ) {
    int len;
    do {
      len = numOfRelsSeed.rand(meanNumOfRels, numOfRelsStddev);
    } while ( len < 1 );
    SetOf<Word> S;
    for ( int i = 0; i < len; i++ )
      S |= NW.word();
    FPGroup G(numOfGens, S);
    int lambda = G.cancellationLambda();
    if ( lambda == 0 || lambda >= 6 )
      return G;
  }
}








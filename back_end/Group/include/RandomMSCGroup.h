// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the RandomMSCGroup class
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Usage:
//
// Special Notes:
//
// Revision History:
//

#ifndef _RANDOM_MSC_GROUP_H_
#define _RANDOM_MSC_GROUP_H_

#include "MSCGroup.h"
#include "RandomNumbers.h"
#include "NormalRandomWord.h"


//----------------------- RandomMSCGroup ---------------------------//


class RandomMSCGroup {

public:

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Constructors:                                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  RandomMSCGroup ( 
    int numOfGenerators, int generatorsSeed,
    int meanLengthOfRelators, int lengthOfRelatorsStddev, int lengthOfRelatorsSeed, 
    int meanNumberOfRelators, int numberOfRelatorsStddev, int numberOfRelatorsSeed
  ) :
    numOfGens ( numOfGenerators ), gensSeed ( generatorsSeed ),
    meanLenOfRels ( meanLengthOfRelators ), lenOfRelsStddev ( lengthOfRelatorsStddev ),
    lenOfRelsSeed ( lengthOfRelatorsSeed ),  meanNumOfRels ( meanNumberOfRelators ), 
    numOfRelsStddev ( numberOfRelatorsStddev ), numOfRelsSeed ( numberOfRelatorsSeed )
  { }

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Accessors:                                       //
  //                                                   //
  ///////////////////////////////////////////////////////

  FPGroup getRandomMSCGroup();
  // This is non const functions.

private:
 

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Data members:                                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  int numOfGens;
  int gensSeed;
  int meanLenOfRels;
  int lenOfRelsStddev;
  int lenOfRelsSeed;
  int meanNumOfRels;
  int numOfRelsStddev;
  NormalRandom numOfRelsSeed;

};

#endif

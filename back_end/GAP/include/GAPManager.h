// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class GAPManager
//
// Principal Author: Dmitry Bormotov
//
// Status: Under development
//
// Usage:
//
// Revision History:
//


#ifndef _GAPManager_h_
#define _GAPManager_h_

#include "GAP.h"


// ------------------------------ GAPManager ------------------------------- //


class GAPManager
{
public:

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Constructors:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////
  
  // No constructors for this static class.


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Accessors:                                               //
  //                                                          //
  //////////////////////////////////////////////////////////////
 
private:

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Private functions:                                       //
  //                                                          //
  //////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Data members:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////

  const int maxNumOfGaps; // max num of GAPs allowed
  int theNumOfGaps;       // size of gap below
  GAP* gap;               // array of running copies of GAP
};

#endif




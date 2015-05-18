// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class GAP
//
// Principal Author: Dmitry Bormotov
//
// Status: Under development
//
// Usage:
//
// Revision History:
//


#ifndef _GAP_h_
#define _GAP_h_

#include "global.h"


// --------------------------------- GAP ----------------------------------- //


class GAP
{
public:

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Constructors:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////
  
  GAP( ) { }
  
  // CC, destructor and op = supplied by compiler.


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Accessors:                                               //
  //                                                          //
  //////////////////////////////////////////////////////////////
 
  bool available( ) const;
  // returns true 

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

  
};

#endif




// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of class RipsConstruction
//
// Principal Author: Dmitry Bormotov
//
// Status: In progress
//
// Special Notes:
//
// Revision History:
//

#ifndef _RipsConstruction_h_
#define _RipsConstruction_h_

#include "FPGroup.h"


// -------------------------- RipsConstruction ----------------------------- //


class RipsConstruction {

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  RipsConstruction( const FPGroup& G ) : 
    theGroup( G )
  { }

  // Default & copy contructors, operator = provided by compiler.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPGroup getRipsConstruction( ostream* out = NULL ) const;


private:
 
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPGroup theGroup;
};


#endif


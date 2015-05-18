// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class TTP (Tietze Transformation Package)
//
// Principal Author: Dmitry Bormotov
//
// Status: provisional implementation
//
// Usage: 
//
// Revision History:
//

#ifndef _TTP_H_
#define _TTP_H_


#include "File.h"
#include "FPGroup.h"


// -------------------------------- TTP ------------------------------------ //


class TTP {

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  TTP( const FPGroup& G ) : theGroup( G ) { }
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Chars getFileName( ) const { return file.getFileName(); }
  // returns the name of the presentation file

  bool run( ) const;
  // Enumerates Tietze transformations and writes corresponding
  // presentations to a file.  TIME CONSUMING ALGORITHM, in fact, it
  // never stops unless there is an error, in that case it returns false.

  
private:
 
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  FPGroup theGroup; // original presentation
  File file;        // the list of alternative presentations
};

#endif


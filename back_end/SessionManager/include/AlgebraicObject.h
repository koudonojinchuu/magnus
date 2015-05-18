// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:
//
// Principal Author: Roger Needham
//
// Status: complete
//
// Revision History:
//

#ifndef _ALGEBRAICOBJECT_H_
#define _ALGEBRAICOBJECT_H_


#include "Chars.h"
#include "SMObject.h"
#include "InformationCenter.h"

//---------------------------------------------------------------------------//
//------------------------ AlgebraicObject ----------------------------------//
//---------------------------------------------------------------------------//

class AlgebraicObject : public SMObject
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AlgebraicObject(const Chars heritage) : myRoots( heritage ) { }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Chars heritage( ) const { return myRoots; }

  virtual       InformationCenter* infoCenter()       { return 0; }
  virtual const InformationCenter* infoCenter() const { return 0; }

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const Chars myRoots;

};

#endif

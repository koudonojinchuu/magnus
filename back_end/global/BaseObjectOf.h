// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: A representation pointer wrapper
//
// Principal Author: Stephane Collart
//
// Status: Complete
//
// Description:
//
// Usage:
//
//   @stc see comments in PureRep
//
// Revision History:
//

#ifndef _BASE_OBJECT_OF_H_
#define _BASE_OBJECT_OF_H_


#include "PureRep.h"
#include "ObjectOf.h"


template<class Rep> class BaseObjectOf : private ObjectOf<Rep>
{

// This assumes that Rep is derived from PureRep

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors                                                        //
  //                                                                     //  
  /////////////////////////////////////////////////////////////////////////

  // copy constructor provided by compiler

  // destructor provided by compiler

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Conventional Operators                                              //
  //                                                                     //  
  /////////////////////////////////////////////////////////////////////////

  // assignment operator provided by compiler

protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Representation Handles                                              //
  //                                                                     //  
  /////////////////////////////////////////////////////////////////////////

  // Inherited from base; adjust access:
  ObjectOf<Rep>::look;
  ObjectOf<Rep>::change;
  ObjectOf<Rep>::enhance;
  ObjectOf<Rep>::acquireRep;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Special Constructors                                                //
  //                                                                     //  
  /////////////////////////////////////////////////////////////////////////

  BaseObjectOf( Rep* newrep ) : ObjectOf<Rep>( newrep) { }

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Safety:                                                             //
  //                                                                     //  
  /////////////////////////////////////////////////////////////////////////

	// Dummy Forcing Member:

	void force_derivation( ) { const PureRep* rc = look(); }
	// With this member PureRep is forced to be an accessible base class of Rep

};

#endif


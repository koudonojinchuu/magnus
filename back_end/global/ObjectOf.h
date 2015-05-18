// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of class ObjectOf
//
// Principal Author: Stephane Collart
//
// Description:
//
// * A representation pointer wrapper.
// * Provided for backward compatibility.
//
// Usage:
//
// * ObjectOf<MyClassRep> assumes that
//   1) MyClassRep is derived from RefCounter
//   2) MyClassRep has a member MyClassRep* clone(); returning a (logical)
//      deep copy of *this
//
// * Derive MyClass from ObjectOf<MyClassRep>
//
// @stc see comments in PureRep
//
// Example:
//
// Status: Complete
//
// Bugs:
//
// * ObjectOf does not prevent abuse by e.g. deriving further classes and reusing
//   them to wrap derived representations
//
// Questions:
//
// * Class A has pseudo-virtual method m(...)
//
//   The actual effect of a.m() is not known at the time
//   of calling. This effect may be
//   1) create a new representation for a to adopt in lieu of
//      the old one
//   2) take some extant representation for a to adopt in lieu
//      of its old one
//   3) modify the representation of a in place
//
//   1) means a single call of delref() on the old rep
//   2) means delref() on the old rep and addref() on the new
//   3) means a's rep must if necessary be replaced by an unshared
//   clone before modification begins
//
//   Requirement: fix this up in a `nice', reusable way
//
// Revision History:
//

#ifndef _OBJECT_OF_H_
#define _OBJECT_OF_H_


#include <iostream.h>
#include "RefCounter.h"


template<class Rep> class ObjectOf
{

public:

  ///////////////////////////////////////////////////////////
  //                                                       //
  // Constructors:                                         //
  //                                                       //
  ///////////////////////////////////////////////////////////

  ObjectOf( const ObjectOf& o ) { theRep = o.theRep; theRep->addRef(); }

  ~ObjectOf() { if (theRep->lastRef()) delete theRep; else theRep->delRef(); }

  ///////////////////////////////////////////////////////////
  //                                                       //
  // Standard Operators:                                   //
  //                                                       //
  ///////////////////////////////////////////////////////////

  ObjectOf& operator = ( const ObjectOf& o )
  {
	o.theRep->addRef();
    if ( theRep->lastRef() ) delete theRep; else theRep->delRef();
    theRep = o.theRep;
    return *this;
  }

  ///////////////////////////////////////////////////////////
  //                                                       //
  // Debugging Stuff:                                      //
  //                                                       //
  ///////////////////////////////////////////////////////////

  #ifdef DEBUG
  void debugPrint( ostream& ostr ) {
	ostr << "this : " << this << "; theRep : " << theRep << "; xrefs : "
		 << look()->nxrefs();
  }

  //friend int main( );
  #endif


protected:

  ///////////////////////////////////////////////////////////
  //                                                       //
  // Representation Access:                                //
  //                                                       //
  ///////////////////////////////////////////////////////////

  const Rep* look( ) const { return theRep; }
  // For safe read-only access.

  Rep* enhance( ) const { return theRep; }
  // DANGEROUS: for altering an object without triggering cloning.
  // Use to change theRep without altering semantics.

  Rep* change( ) {
    if ( theRep->lastRef() ) return theRep;
    else { theRep->delRef(); return theRep = (Rep*)theRep->clone(); }
  }
  // For safe read/write access.

  void acquireRep( const Rep* rep )
  {
    ((Rep*&)rep)->addRef();
        // cast away physical constness to permit logically const
        // incrementation of ref count
    if ( theRep->lastRef() ) delete theRep; else theRep->delRef();
    theRep = ((Rep*&)rep);
        // cast away physical constness of representation for
        // acquisition through new object; semantics of look() and
        // and change() guarantee that logical constness is maintained
  }
  // special `assignment-like' member: to permit an object to acquire
  // another's representation consistently
  // useful in delegation of object-level methods

  ///////////////////////////////////////////////////////////
  //                                                       //
  // Special Constructors:                                 //
  //                                                       //
  ///////////////////////////////////////////////////////////

  ObjectOf( Rep* newrep ) { theRep = newrep; }
  // To wrap new representations

private:

  ///////////////////////////////////////////////////////////
  //                                                       //
  // Data Members:                                         //
  //                                                       //
  ///////////////////////////////////////////////////////////

  Rep* theRep;

  ///////////////////////////////////////////////////////////
  //                                                       //
  // Safety:                                               //
  //                                                       //
  ///////////////////////////////////////////////////////////

  void force_derivation( ) { RefCounter* rc = theRep; }
  // With this member RefCounter is forced to be 
  // an accessible base class of Rep

};


#endif

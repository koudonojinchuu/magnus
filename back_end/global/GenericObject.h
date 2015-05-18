// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of classes GenericRep, GenericObject
//
// Description:
//
// * a common root class for all object-level (see PureObject.h) classes
//   with a coherent type scheme.
//
// Principal Author: Stephane Collart
//
// Status: Under development
//
// Usage:
//
// * See PureObject
//
// * In addition, this class provides explicit type query handles; classes
//   derived from this class will normally derive *publicly*, to permit
//   assignment by non-friend
//
// Bugs:
//
// Revision History:
//



#ifndef _GENERIC_OBJECT_H_
#define _GENERIC_OBJECT_H_



#include "PureRep.h"
#include "BaseObjectOf.h"
#include "Type.h"



struct GenericRep : public PureRep
{

	//////////////////////////////////////////////////////////////////////
	//                                                                  //
	// Constructors:                                                    //
	//                                                                  //
	//////////////////////////////////////////////////////////////////////

	// default constructor provided by compiler

	// copy constructor provided by compiler

	//////////////////////////////////////////////////////////////////////
	//                                                                  //
	// Standard Operators:                                              //
	//                                                                  //
	//////////////////////////////////////////////////////////////////////

	// assignment operator disabled

	//////////////////////////////////////////////////////////////////////
	//                                                                  //
	// Type Stuff and Access:                                           //
	//                                                                  //
	//////////////////////////////////////////////////////////////////////

	static const Type theGenericObjectType;
	// every representation class derived from this should carry a similarly
	// defined type field

	static Type type() { return theGenericObjectType; }
	// every representation class derived from this should carry a similarly
	// defined type member

	virtual Type actualType() const { return type(); }
	// this member serves as the root of all type querying members in this
	// type scheme
	// every representation class derived from this should override this
	// member similarly to return the actual type of the class

	//////////////////////////////////////////////////////////////////////
	//                                                                  //
	// Representation Stuff:                                            //
	//                                                                  //
	//////////////////////////////////////////////////////////////////////

	PureRep* clone( ) const { return new GenericRep(*this); }

};


class GenericObject : protected BaseObjectOf<GenericRep>
{
public:

	//////////////////////////////////////////////////////////////////////
	//                                                                  //
	// Constructors:                                                    //
	//                                                                  //
	//////////////////////////////////////////////////////////////////////

	GenericObject( ) : BaseObjectOf<GenericRep>( new GenericRep ) { }

	// copy constructor provided by compiler
	// destructor provided by compiler
	// both of the preceding ref-counting compatible

	// assignment operator provided by compiler
	// ref-counting compatible

	//////////////////////////////////////////////////////////////////////
	//                                                                  //
	// Type Access:                                                     //
	//                                                                  //
	//////////////////////////////////////////////////////////////////////

	static Type type() { return GenericRep::type(); }
	// this `type querying' member is provided solely as an example, since
	// this class is pseudo-abstract (ie. its representation class is abstract)
	// and no actual objects of this type will ever be created

	Type actualType() const { return look()->actualType(); }

protected:

	//////////////////////////////////////////////////////////////////////
	//                                                                  //
	// Representation Access:                                           //
	//                                                                  //
	//////////////////////////////////////////////////////////////////////

	// look(), change() and enhance() inherited

	//////////////////////////////////////////////////////////////////////
	//                                                                  //
	// Special Constructors:                                            //
	//                                                                  //
	//////////////////////////////////////////////////////////////////////

	// Special wrapping constructor to wrap new representations (returned
	// by eg. delegated methods) and for base initialisation by derived
	// classes:

	GenericObject( GenericRep* newrep ) : BaseObjectOf<GenericRep>(newrep) { }

};

#endif

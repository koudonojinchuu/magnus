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


#ifndef _DERIVED_OBJECT_OF_H_
#define _DERIVED_OBJECT_OF_H_


template <class Base, class Rep> class DerivedObjectOf : public Base
{

// This class assumes Base is itself an ObjectOf kind of class, i.e.
// 1) Base has members look(), enhance(), change() returning some BaseRep* type
// 2) Base has a constructor Base( BaseRep* );
// 3) BaseRep is directly or indirectly derived from PureRep
// 4) Rep is derived from BaseRep

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

	// Shadowed representation accessors:

	const Rep* look( ) const { return (Rep*)Base::look(); }
	Rep* enhance( ) const { return (Rep*)Base::enhance(); }
	Rep* change( ) { return (Rep*)Base::change(); }

	/////////////////////////////////////////////////////////////////////////
	//                                                                     //
	// Special Constructors                                                //
	//                                                                     //  
	/////////////////////////////////////////////////////////////////////////

	DerivedObjectOf( Rep* newrep ) : Base( newrep ) { }
	// for base class initialisation

	DerivedObjectOf( const Base& b ) : Base( b ) { }
	// for base class intialisation when cast conversion to derived is desired 

};

#endif

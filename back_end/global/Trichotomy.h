// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class Trichotomy
//
// Principal Author: Stephane Collart
//
// Status: Useable.
//
// Revision History:
//
// * 11/95 Roger made the conversion to bool, and all consequences,
//         switch-outable, pending complete removal.
//
// * 01/96 Dmitry B. implemented IPC tools.
//

#ifndef _TRICHOTOMY_H_
#define _TRICHOTOMY_H_


#include "global.h"

#define ALLOW_BOGUS_CONVERSIONS


//------------------------------------------------------------------------//
//----------------------------- Trichotomy -------------------------------//
//------------------------------------------------------------------------//

class Trichotomy
{

public:

	///////////////////////////////////////////////////////
	//                                                   //
	//  Helper Classes                                   //
	//                                                   //
	///////////////////////////////////////////////////////

	enum TrichotomyValue { DONTKNOW = -1, NO, YES };

	class dontknow { }; // used as marker in ctor of global Trich consts

	// @stc it seems as though g++ 2.6.3 requires local classes declared before
	// first place of use in class definition

	///////////////////////////////////////////////////////
	//                                                   //
	//  Constructors                                     //
	//                                                   //
	///////////////////////////////////////////////////////

	Trichotomy( bool b ) : theValue(convert(b)) { }

	Trichotomy( int i ) : theValue(convert(i)) { }

	Trichotomy( void* p ) : theValue(convert(p)) { }

	// no default constructor

	// copy constructor provided by compiler

	///////////////////////////////////////////////////////
	//                                                   //
	//  Special Initialisation Constructors              //
	//                                                   //
	///////////////////////////////////////////////////////

	Trichotomy( const dontknow& ) : theValue(DONTKNOW) { }

	///////////////////////////////////////////////////////
	//                                                   //
	//  Conversion Operators                             //
	//                                                   //
	///////////////////////////////////////////////////////

#ifdef ALLOW_BOGUS_CONVERSIONS
	operator bool ( ) const
	{
		if (theValue == -1) error("Non-bool value of Trichotomy in"
			" Trichotomy::operator bool()");
		return bool(theValue);
	}

	operator int ( ) const
	{
		return bool(*this);
	}
#endif

	///////////////////////////////////////////////////////
	//                                                   //
	//  Conventional Operators                           //
	//                                                   //
	///////////////////////////////////////////////////////

	// assignment operator provided by compiler

	inline friend bool operator == ( const Trichotomy& t, const Trichotomy& u )
	{
		return t.theValue == u.theValue;
	}
	// global to permit dual promotion

	// op != global below

	///////////////////////////////////////////////////////
	//                                                   //
	//  Logical Operations                               //
	//                                                   //
	///////////////////////////////////////////////////////

#ifdef ALLOW_BOGUS_CONVERSIONS
	Trichotomy operator ! ( ) const
	{
		if (undefined()) return dontknow();
		else return !bool(*this);
	}
  
	// the rest of these are implemented as global functions below
	// for dual type promotion

	///////////////////////////////////////////////////////
	//                                                   //
	//  Accessors                                        //
	//                                                   //
	///////////////////////////////////////////////////////

	bool defined( ) const { return theValue != -1; }
	// true iff *this != dontknow

	bool undefined( ) const { return !defined(); }
	// true iff *this == dontknow
#endif

	///////////////////////////////////////////////////////
	//                                                   //
	//  I/O routines                                     //
	//                                                   //
	///////////////////////////////////////////////////////

        friend ostream& operator << ( ostream& ostr, const Trichotomy& t )
        {
	  switch( t.theValue ) {
	  case DONTKNOW : ostr << "DONTKNOW"; break;
	  case NO       : ostr << "NO";  break;
	  case YES      : ostr << "YES";
	  }
	  return ostr;
	}
	///////////////////////////////////////////////////////
	//                                                   //
	//  IPC operators                                    //
	//                                                   //
	///////////////////////////////////////////////////////

        friend ostream& operator < ( ostream& ostr, const Trichotomy& t )
        {
	  int i; 
	  switch( t.theValue ) {
	  case DONTKNOW : i = -1; break;
	  case NO       : i = 0;  break;
	  case YES      : i = 1;
	  }
	  ostr < i;

	  return ostr;
	}
  
        friend istream& operator > ( istream& istr, Trichotomy& t )
        {
	  int i;
	  istr > i;
	  switch( i ) {
	  case -1 : t.theValue = DONTKNOW; break;
	  case 0  : t.theValue = NO; break;
	  case 1  : t.theValue = YES;
	  }

	  return istr;
	}


private:

	///////////////////////////////////////////////////////
	//                                                   //
	//  Conversion Helpers                               //
	//                                                   //
	///////////////////////////////////////////////////////

	static TrichotomyValue convert( bool b )
	{
		if (b) return YES; else return NO;
	}
	
	static TrichotomyValue convert( void* p )
	{
		if (p) return YES; else return NO;
	}
	
	///////////////////////////////////////////////////////
	//                                                   //
	//  Data Members                                     //
	//                                                   //
	///////////////////////////////////////////////////////

	TrichotomyValue theValue;

};


//------------------ related global operators ---------------------//


inline bool operator != ( const Trichotomy& t, const Trichotomy& u )
{
	return !(t == u);
}

#ifdef ALLOW_BOGUS_CONVERSIONS
inline Trichotomy operator && ( const Trichotomy& t, const Trichotomy& u )
{
	if (t.undefined() || u.undefined()) return Trichotomy::dontknow();
	else return bool(t) && bool(u);
}

inline Trichotomy operator || ( const Trichotomy& t, const Trichotomy& u )
{
	if (t.undefined() || u.undefined()) return Trichotomy::dontknow();
	else return bool(t) || bool(u);
}
#endif

//------------------ global const declarations --------------------//


extern const Trichotomy YES, NO, DONTKNOW, yes, no, dontknow;
// @stc verify that it is necessary to explicitely give consts external linkage

//@rn const bool forever = true; // useful synonym

#endif

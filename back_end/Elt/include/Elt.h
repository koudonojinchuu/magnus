// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the Elt class.
//
// Principal Author: Stephane Collart
//
// Status: Useable.
//
// Revision History:
//
// * 4.6.94 @stc added default constructor to Elt
//
// * 01/96 Dmitry B. implemented IPC tools.
//
// Fundamental Questions:
//
// * What does one do about functions and operators taking two or more
//   arguments, when it is important to correctly resolve the actual
//   type of both arguments? As a special case, see the Q about op ==
//   below. Some type mismatches would be allowed (e.g. EltIdentityRep
//   with anything else) and others not (e.g. WordRep and MatrixRep).
//
// * Where should equality be resolved (as expressed through operator ==)?
//   - at the object level, via the accessors?
//   - via delegation to the representation level?
//   Note the ramifications: you have two objects a,b, ostensibly of
//   type T; should they be compared as T's, or as their *actual* types?
//   If the latter is preferred, what does one do about type mismatches?
//   Simple minded delegation to a virtual function of one of the
//   arguments does not resolve the type of the second argument.
//
// * In a tentative revision of the practice of not giving
//   pseudo-abstract classes a default constructor, Elt now gets a
//   default constructor which gives it an actual EltIdentityRep* as a rep
//   This makes an object-level (ie wrapped-pointer) class behave more
//   closely like a real pointer. A default initialised Elt may be
//   assigned to and even from in perfect consistency. Eventually,
//   it is intended to make such a default initialised Elt behave like
//   the identity; note that this is not implemented fully yet (see
//   restrictions).
//
// * Does Elt need a global operator * taking two Elt arguments? in 
//   every case when one expects a conversion from some type T which has
//   a conversion to Elt on the left hand side of `a * b',
//   the only alternative is to give T itself an operator * ( Elt ),
//   since the ARM will not consider a conversion to apply a method.
//
// * The same question occurs much more frequently and generally with
//   operator ==; for this reason Elt has tentatively been given a global
//   == inlined below.
//
// Restrictions:
//
// * If you try applying methods to a default initialised Elt, you
//   will likely get grief (as if you tried dereferencing the null pointer.)
//   

#ifndef _ELT_H_
#define _ELT_H_

#include "EltRep.h"
#include "Integer.h"
#include "Rational.h"


class Elt : public GenericObject {

public:

///////////////////////////////////////////////////////////////////////
//                                                                   //
// Constructors:                                                     //
//                                                                   //
///////////////////////////////////////////////////////////////////////

  // copy constructor provided by compiler

  Elt( ) : GenericObject( new EltIdentityRep ) { }
  // @stc this is a tentative way of giving Elt a default constructor

  // destructor provided by compiler

///////////////////////////////////////////////////////////////////////
//                                                                   //
// Basic operators:                                                  //
//                                                                   //
///////////////////////////////////////////////////////////////////////

  int operator == ( const Elt& e ) const {
	 return ((look() == e.look()) || (*look() == *e.look()));
  }

  int operator != ( const Elt& e ) const { return !( *this == e ); }

///////////////////////////////////////////////////////////////////////
//                                                                   //
// Representation and type stuff:                                    //
//                                                                   //
///////////////////////////////////////////////////////////////////////

  static Type type( ) { return EltRep::type(); }

  Type actualType( ) const { return look()->actualType(); }

  int hash( ) const { return look()->hash(); }

///////////////////////////////////////////////////////////////////////
//                                                                   //
// Mathematical methods:                                             //
//                                                                   //
///////////////////////////////////////////////////////////////////////

  Elt operator * ( const Elt& e ) const {
	 return Elt( *look() * *e.look() );
  }

  Elt operator *= ( const Elt& e ) {
	 return *this = Elt( *look() * *e.look() );
  }

  Elt raiseToPower( Integer power ) const;

  Elt inverse() const {
	 return Elt( look()->inverse() );
  }

  friend Elt inverse( const Elt& e ) { return e.inverse(); }
  // to make it more convenient to take the inverse of something which
  // can be converted to an Elt

  Elt conjugateBy( const Elt& e ) const {
	 return Elt( look()->conjugateBy(e.look()) );
  }

  friend Elt commutator( const Elt&, const Elt& );

  // The paradigm for output:
  // The Rep of the root of a hierarchy defines
  // virtual void printOn(ostream&) const;
  // The root defines a friend << which
  // calls printOn. Subclasses of Rep override printOn.
  // No other << or printOn's are needed.

  friend ostream& operator << ( ostream&, const Elt& );

  void debugPrint( ostream& o ) const; // mixed pseudo-virtual

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const Elt& e )
  {
    e.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, Elt& e )
  {
    e.change()->read(istr);
    return istr;
  }


protected:

  // Shadow representation accessors to get representations of the
  // right type in the members of this class:

  const EltRep* look( ) const {
	 return (const EltRep*)GenericObject::look();
  }
  EltRep* enhance( ) const {
	 return (EltRep*)GenericObject::enhance();
  }
  EltRep* change( ) {
	 return (EltRep*)GenericObject::change();
  }

  Elt( EltRep* p ) : GenericObject(p) { }

};


//----------------------- Associated Global ------------------------//
//------------------------- Functions ------------------------------//

/*@db porting
inline bool operator == ( const Elt& e1, const Elt& e2 ) {

	return e1.operator == (e2);
}
*/

#endif

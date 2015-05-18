// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the EltRep class.
//
// Principal Author: Stephane Collart
//
// Status: Useable.
//
// Revision History:
//

#ifndef _ELT_REP_H_
#define _ELT_REP_H_

#include "GenericObject.h"
#include "Type.h"


//---------------------------- EltRep -------------------------------//


struct EltRep : GenericRep {

// constructors:

  // no default constructor

  // copy constructor supplied by compiler

  virtual ~EltRep( ) { };
  // destructor must be virtual at root of hierarchy with virtual
  // functions
  
// representation and type stuff:

  static const Type theEltType;

  static Type type( ) { return theEltType; }

  virtual Type actualType( ) const { return theEltType; }

  // @stc there is a type datum and actualType() is not pure virtual
  // to have a type associated with this pseudo-abstract class too

  PureRep* clone( ) const = 0;

// delegated methods and operators:

  virtual Bool operator == ( const EltRep& ) const = 0;

  virtual int hash( ) const = 0;
  
  virtual EltRep* operator * ( const EltRep& ) const = 0;

  virtual EltRep* inverse( ) const = 0;

  virtual EltRep* conjugateBy( const EltRep* ep ) const {
	 EltRep* tmp1 = ep->inverse();
	 EltRep* tmp2 = *tmp1 * *this;
	 EltRep* result = *tmp2 * *ep;
	 delete tmp1;
	 delete tmp2;
	 return result;
  }

  virtual EltRep* commutatorWith( const EltRep* ep ) const {
	 EltRep* tmp1 = ep->conjugateBy( this );
	 EltRep* result = *tmp1 * *ep;
	 delete tmp1;
	 return result;
  }
  
// I/O:

  virtual void printOn(ostream&) const;

  virtual void debugPrint(ostream&) const;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  virtual void write( ostream& ostr ) const { };

  virtual void  read( istream& istr ) { };


private:

  EltRep& operator = ( const EltRep& ); // disable assignment
    
};


//------------------------ EltIdentityRep ---------------------------//


struct EltIdentityRep : EltRep {

// constructors:

  // default constructor provided by compiler

  // copy constructor supplied by compiler

  // destructor supplied by compiler; virtual by inheritence

// representation and type stuff:

  PureRep* clone( ) const { return new EltIdentityRep(*this); }
  // overrides EltRep::clone()
  
  static const Type theEltIdentityType;
  static Type type( ) { return theEltIdentityType; }
  Type actualType( ) const { return theEltIdentityType; }
  // overrides EltRep::actualType();

  virtual Bool operator == ( const EltRep& ) const { return false; }
  // @stc quick hack pending a comprehensive solution
  // 1) to the problem of where op == is resolved (see Elt.h)
  // 2) to the semantics required of EltIdentityRep (see Elt.h)

  virtual int hash( ) const { return 0; }
  
  virtual EltRep* operator * ( const EltRep& e ) const
	{ e.addRef(); return (EltRep*)&e; }
  // @stc quick hack;
  // 1) it may be undesirable to pass representations around
  // through direct manipulation of the reference count; this would
  // preferably done through suitably arranged automatic set-ups in
  // the object/representation interplay
  // 2) this does nothing for the case when the second argument is an
  // EltIdentityRep and not the first

  virtual EltRep* inverse( ) const 
	{ addRef(); return (EltRep*)this; }
  // @stc same comments as above

};


#endif

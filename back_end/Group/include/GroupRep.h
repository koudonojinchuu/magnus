// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the GroupRep class.
//
// Principal Authors: Stephane Collart, Roger Needham
//
// Status: in progress
//
// Revision History:
//
// * 07/96 Alexey M. implemented IPC tools.

#ifndef _GROUP_REP_H_
#define _GROUP_REP_H_


#include "GenericObject.h"
#include "Type.h"
#include "Chars.h"
#include "Elt.h"
#include "Set.h"
#include "IPC.h"
#include "ExtendedIPC.h"

struct GroupRep : GenericRep {
  
public:

// Constructors:

  GroupRep( ) : theOrder(-1) { }
  
  // compiler provides copy constructor (does "deep copy")
  // no need for destructor to do anything
  
// Representation methods:

  // Inherited from PureRep:
  // virtual PureRep* clone( ) = 0;

  static const Type theGroupType;

  static Type type( ) { return theGroupType; }

  virtual Type actualType( ) const { return type(); }

// Operators:

  private: GroupRep& operator = ( const GroupRep& );
  // disable assignement
  public:

// Methods dealing with group structure:

  virtual bool compare( const GroupRep* G ) const = 0;

  virtual int order( ) const = 0;
  virtual Trichotomy isTrivial( ) const = 0;
  virtual Trichotomy isFinite( ) const = 0;
  virtual Trichotomy isInfinite( ) const = 0;
  virtual Trichotomy isAbelian( ) const = 0;
  
// Methods dealing with group elements:

  virtual Elt makeIdentity( ) const = 0;
  virtual Bool isSyntacticIdentity(const Elt&) const = 0;
  virtual Trichotomy isTrivialElt( const Elt& e ) const { return DONTKNOW; }
  virtual Trichotomy areEqual(const Elt&, const Elt&) const = 0;
  virtual Elt firstElt( ) const = 0;
  virtual Elt nextElt(const Elt&) const = 0;

  virtual Elt multiply(const Elt&, const Elt&) const = 0;
  virtual Elt inverseOf(const Elt&) const = 0;

  virtual Elt raiseToPower(const Elt&, int) const;
  virtual Elt conjugateBy(const Elt&, const Elt&) const;
  virtual Elt commutator(const Elt&, const Elt&) const;

// I/O:

  virtual void printOn(ostream&) const = 0;
  virtual GroupRep* readFrom(istream&, Chars&) const = 0;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  virtual void write( ostream& ostr ) const { ostr < theOrder; }
 
  virtual void read( istream& istr ) { istr >theOrder;}
 

// Data members:

  int theOrder;
  //@rn We kludgily use -1 to record dontknow.

  //@rn where/how to store info a group discovers about itself needs
  //    more thought.

// Debugging stuff:

#ifdef DEBUG

  virtual bool consistent( ) const { return true; }
  // this performs a simple consistency check on the internal data of
  // group representation
  // return true = no error detected
  // return false = inconsistency found

#endif
};

#endif

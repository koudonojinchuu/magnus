// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declarations of class SMObject, and helper IconID
//
// Principal Author: Roger Needham
//
// Status: useable
//
// Revision History:
//
// * EP added:
//
//              static const IconID VectorOfWords;


#ifndef _SMOBJECT_H_
#define _SMOBJECT_H_


#include "List.h"
#include "FEData.h"


//---------------------------------------------------------------------------//
//----------------------------- IconID --------------------------------------//
//---------------------------------------------------------------------------//

//@rn In separate attributes wrapper?

class IconID
{
public:

  static const IconID group;
  static const IconID subgroup;
  static const IconID SetOfWords;
  static const IconID VectorOfWords;
  static const IconID elt;
  static const IconID equation;
  static const IconID systemOfEquations;
  static const IconID map;
  static const IconID homomorphism;
  static const IconID permutation;
  static const IconID enumerator_object;
  static const IconID enumerator_problem;
  static const IconID list_object;
  static const IconID problem;
  static const IconID none;
  static const IconID do_not_display;

  bool operator == ( const IconID& ) const;
  bool operator != ( const IconID& ) const;

  friend inline ostream& operator << ( ostream& ostr, const IconID& iid ) {
	 ostr << iid.theName;
	 return ostr;
  }

private:

  IconID( const char* name ) : theName( name ) { }

  const char* theName;

};


//---------------------------------------------------------------------------//
//---------------------------- SMObject -------------------------------------//
//---------------------------------------------------------------------------//


class SMObject : protected FEData
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  operator OID ( ) const { return theOid; }

  OID oid( ) const { return theOid; }

  virtual const char* typeID( ) const = 0;

  virtual const IconID iconID( ) const = 0;

  virtual void viewStructure(ostream& ostr) const = 0;

  virtual void printProperties(ostream& ostr) const = 0;

  virtual void printDefinition(ostream& ostr) const = 0;

  virtual bool displayInFE( ) const { return true; }

  bool isComputationManager( ) const { return isCM; }
  // Used by class TheObjects.


protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMObject(bool is_cm = false);

  virtual ~SMObject( ) { }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // `Public' Members Touched by Friends:                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend class SessionManager;
  friend class TheObjects;
  friend class Ctor;
  friend class SubordinateBase;
  friend class SMFPGroup;
  friend class SMSubgroup;
  friend class SMWord;
  friend class SMMap;
  friend class SMMagnusBreakdown;
  friend class UnboundedSupervisor;
  friend class CheckinMessage;

  virtual void readMessage(istream&) = 0;
  // Used by class SessionManager.
  //@rn return bool, so overriding fns can use default too?


private:

  ListOf<OID> getDependents( ) { return dependents; }
  // Used by class TheObjects for deleting dependents.

  void addDependent(OID oid) { dependents.append( oid ); }
  // Used by classes Ctor, SubordinateBase, SMFPGroup, UnboundedSupervisor.

  SMObject(const SMObject&);
  // Hidden, not to be implemented.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const OID theOid;

  bool isCM;
  // Cheap way to distinguish ComputationManagers from AlgebraicObjects.

  ListOf<OID> dependents;
  // The oids of the SMObjects which depend on this one.
};

#endif

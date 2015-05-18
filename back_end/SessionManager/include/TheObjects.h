// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class TheObjects
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//

#ifndef _THEOBJECTS_H_
#define _THEOBJECTS_H_


#include "Set.h"
#include "Associations.h"
#include "Chars.h"


//---------------------------------------------------------------------------//
//--------------------------- TheObjects ------------------------------------//
//---------------------------------------------------------------------------//

class TheObjects
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // No constructors for this static class.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static bool isRunning(OID oid);
  // True iff oid is the OID of a ComputationManager which is in
  // state RUNNING.

  static bool isStalled(OID oid);
  // Returns true when no running CM has any ARCs from oid.


  static SetOf<OID> remove(OID oid);
  // Removes and deletes the indicated SMObject, and all of its dependents.
  // A bad oid is quietly ignored, for messaging fault-tolerance.
  // Returns the dependency closure of oid, i.e., the objects which were
  // deleted.

  static SMObject* get(OID oid);
  // Returns the SMObject* with the given oid,
  // or 0 if there is no such object, for messaging fault-tolerance.
  // Used by class SessionManager and class Ctor.

  static SetOf<OID> dependencyClosure(OID oid);
  // A bad oid is quietly ignored, for messaging fault-tolerance.

  static Chars name(OID oid);
  // Returns name of the object with the given oid
 
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // `Public' Members Touched by Friends:                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend class SMObject;
  friend class SessionManager;
  friend class Ctor;
  friend class UnboundedSupervisor;
  friend class ResourceManager;
  friend class ExtendToHomProblem; //@db temporary hack

  static OID reserveOid( );
  // Reserves a place in theObjects for a new SMObject, and returns the index.
  // Used by class SMObject to prepare for enrollment.

  static void enroll(SMObject* newObject);
  // Used by class SMObject.

  static void passControl( );
  // Passes control to a ComputationManager in a fair way.

  static void setName(OID oid, Chars name);
  // Changes name of an object

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private Members:                                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static void resize(int newLength);
  // A helper for expanding theObjects.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const int minimumLength = 16;
  static int theObjectsLength;
  static SMObject** theObjects;

  static int CMToGetControl;
  
  // keeps a copy of the names of the objects for SM use
  static AssociationsOf<OID,Chars>* theNames; 
};

#endif

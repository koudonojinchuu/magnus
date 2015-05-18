// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class ComputationManager
//
// Principal Author: Roger Needham, Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 07/96 Dmitry B. made porting to gcc 2.7.2.
//
// * 06/97 @db added new functions:
//
//   bool crashed( ) const;
//   virtual Chars getCrashMessage( ) const;
//   virtual bool isSupervisor() const;
//
// * 03/98 @dp added new functions:
//
//    void attachClient( OID client );
//    void detachClient( OID client );
//    SetOf<OID> getClients( ) const;

#ifndef _COMPUTATIONMANAGER_H_
#define _COMPUTATIONMANAGER_H_


#include "SMObject.h"
#include "ResourceManager.h"
#include "TheObjects.h"
#include "ARCer.h"

//---------------------------------------------------------------------------//
//------------------------ ComputationManager -------------------------------//
//---------------------------------------------------------------------------//

class ARCer;

//@njz: added to avoid complain of GCC-4.0.1
class SMFPGroup;
//

class ComputationManager : public SMObject, public ResourceManager
{

  friend class ARCer;
  friend class RelatorEnumeratorARCer;
  friend class ExternalARCer;
  friend class Supervisor;

public:

  enum State { UNSTARTED, RUNNING, SUSPENDED, TERMINATED };

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ComputationManager( bool display_in_fe = false );

  ~ComputationManager( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  State state( ) const;

  bool crashed( ) const { return isCrashed; }

  virtual Chars getCrashMessage( ) const { return ""; }
  // Override this function to change the default crash message 
  
  virtual bool isSupervisor() const { return false; }
  
  Chars helpID( const Chars& problemName, const SMFPGroup& parent ) const;
  // This function returns a standard helpID ( page reference in the
  // magnus manual). It can be given to FE by supervisors. It is a
  // function of the ComputationManager class, because some of CMs can
  // be supervisors to others.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const char* typeID( ) const;                  // overrides SMObject

  const IconID iconID( ) const;                 // overrides SMObject

  void viewStructure(ostream& ostr) const;      // overrides SMObject

  void printProperties(ostream& ostr) const;    // overrides SMObject  

  void printDefinition(ostream& ostr) const;    // overrides SMObject

  bool displayInFE( ) const { return showMe; }  // overrides SMObject

  static void printGlobalMessageTemplates(ostream& ostr);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Control:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  virtual void takeControl( ) = 0;

  bool fastResult( ) const { return amFast; }
  // A derivative's ctor may decide that the result can be gotten so
  // quickly that there is no need, e.g., to create a problem object.

  bool checkForStall( );
  // Checks whether this is stalled (returns true if so), and sends
  // StateTransition if necessary.

  void attachClient( const SMObject& client ); 
  void detachClient( const SMObject& client );
  SetOf<OID> getClients( ) const;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // To move into any of the four States, there is an administrative
  // member which handles the invariant stuff like informing the front
  // end of the change. For each of these, there is a virtual
  // counterpart, which is overridden to do the actual work required
  // by the state change.

  void adminStart( );

  void adminSuspend( );

  void adminResume( );

  virtual void adminTerminate( );

  virtual void start( ) { }

  virtual void suspend( ) { }     // Probably no need to override

  virtual void resume( ) { }      // Probably no need to override

  virtual void terminate( ) { }


protected:

  void readMessage(istream& istr);  // Overrides SMObject

  void resultIsFast( );
  // A derivative reports its quickness with this.


private:

  void hireArcer( ARCer* arcer );

  void terminateArcers( );

  void setCrashState( );
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ListOf<ARCer*> theArcers;

  SetOf<OID> theClients;

  bool amFast;

  bool showMe;

  State theState;

  bool isCrashed;
  // true when arcer reported about it's crash

  bool wasStalled;
  // Used to prevent repeated stall messages.

  // These are the tags for messages handled by objects in this class:

  enum MessageTag {
	 VIEW_REQUEST, ARC_UPDATE, START, SUSPEND, RESUME, TERMINATE, PARAMETER
  };

};

#endif


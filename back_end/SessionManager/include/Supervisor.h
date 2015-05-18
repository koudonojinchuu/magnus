// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of mixin class Supervisor and helpers
//
// Principal Author: Roger Needham, Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//
// * 06/97 @db added new functions:
//
//   virtual bool Supervisor::isSupervisor() const;
//   void Supervisor::supervise( );
//


#ifndef _SUPERVISOR_H_
#define _SUPERVISOR_H_


#include "ARC.h"
#include "ARCSlotID.h"
#include "OID.h"
#include "Associations.h"
#include "viewStructure.h"
#include "ComputationManager.h"
#include "TheObjects.h"


// A computation manager (CM) might supervise a fixed number (i.e., known
// at compile time) of other CMs. We must support this functionality:
// 
//   - The subordinate CMs may need to be exported to the rest of the
//     system, so that resources may be allocated to them, and they can
//     be queried for results.
// 
//   - A subordinate CM should not be created (and thus consume memory
//     and CPU time) until it is asked for.
// 
//   - Messages may arrive instructing the supervising CM to allocate
//     resources to a subordinate, so the latter must be located
//     easily. Since it may not exist at the time the FE is told about
//     its *potential* existence, the FE cannot refer to an object id.
// 
// 
// The facilities declared here automate this process. When a computation
// manager `boss' of class B is to supervise a subordinate computation
// manager of class W, B has a data member `worker' of class
// Subordinate<B,W>. If worker is to be exported, this member is public.
// The members of the W are accessed via the overloaded -> operator. For
// example:
// 
//     if ( boss.worker->state() == ComputationManager::RUNNING ) ...
// 
// The operator invocation creates the W if necessary.
// 
// The reason that Subordinate<B,W> does not return a W& is that someone
// might erroneously store the W& as, for example, a class data
// member. The problem is that the system may not know about this
// dependence on the W, so the W may be deleted, leaving a dangling
// reference.
// 
// Now, it it possible to extract the W* from operator->(), and this is
// safe within a function body, but you must NEVER, *EVER* STORE THE
// RESULT OF operator->() beyond the duration of a function body. This
// would create the worst possible bug: one that is unlikely to turn up
// *even in testing*, since it depends on just the right sequence of end
// user events. The convoluted syntax of operator->() should remind you
// of this dire peril.
// 
// There appears to be no better solution. One could return an
// intermediate object, with disabled copying, which duplicates the
// needed parts of W's interface. One could arrange that the object is
// always a temporary, so that attempts to store a reference or its
// address would (hopefully) trigger a compiler warning. But it must be
// the case that a sufficiently complex expression would evaluate to the
// illicit reference or pointer, but fool the compiler. In fact, try
// compiling this with gcc 2.6.3 and -Wall:
//  
//  #include <iostream.h>
//  
//  struct Foo { ~Foo( ) { cerr << "Kiss your Foo goodbye!\n" << flush; } };
//  
//  struct Bar {
//    Bar(const Foo& f) : F( f ) { }
//    ~Bar( ) { cerr << "Kiss your Bar goodbye!\n" << flush; }
//    const Foo& F;
//  };
//  
//  struct Baz {
//    Baz(const Foo& f) : B( f ) { }
//    Bar B;
//  };
//  
//  main( ) {
//    Baz* Bp = new Baz( Foo() );
//    cerr << "I have not deleted Bp yet!\n" << flush;
//    delete Bp;
//  }
//
// 
// In some cases, a computation manager might `supervise' another only in
// that it presents a view to the end user for passing ARCs to a CM,
// which is actually supervised by a third CM. For this case, we use
// class MirrorSubordinate.
// 
// The facilites here can not be used for CMs, like the nilpotent
// quotients CM, which must supervise an unbounded number of subordinate
// CMs (class 2, class 3, ...).
//
// The mixin class Supervisor arranges for locating subordinate CMs, given
// a `virtual OID', called, for `historical' reasons, an ARCSlotID.


//---------------------------------------------------------------------------//
//--------------------------- Subordinate -----------------------------------//
//---------------------------------------------------------------------------//

// As usual, we cannot hide a template class in class Supervisor.

class SubordinateBase
{
public:

  SubordinateBase(class Supervisor& boss);

  ARCSlotID arcSlotID( ) const { return asi; }
  // This is used by class Supervisor as a `virtual OID'. That is, a
  // derivative of SubordinateBase can announce that either it wraps a
  // ComputationManager now, or will do in the future. The ARCSlotID
  // is the means by which the FE refers to this `virtual'
  // ComputationManager, since it may not have a real OID yet.

  virtual Supervisor& getBoss( ) const = 0;

  virtual ComputationManager* getWorker( ) const = 0;

  virtual void terminateWorker( ) = 0;

  virtual void deleteWorker() = 0;
  
  virtual bool exists( ) const = 0;
  // For querying existence of subordinate without triggering same.

  
protected:

  friend class Supervisor;

  virtual void acceptAllocation(OID oid, ARCSlotID, ARC, bool overrides=true)=0;
  // Used by class Supervisor.

  void addDependent( SMObject& smo, OID oid ) const;
  // So derivatives need not be friends of SMObject.

private:

  SubordinateBase( const SubordinateBase& );
  // Hidden, not to be implemented.

  ARCSlotID asi;
};


// Class Subordinate<B,W> assumes that:
//
//  1) Class B derives from class Supervisor.
//  2) Class W derives from class ComputationManager.
//  3) Class W has a constructor which takes a B.

template <class B, class W> class Subordinate : public SubordinateBase
{

friend class MirrorSubordinate;
  
public:

  Subordinate(B& boss)
    : SubordinateBase( boss ), theBoss( boss ), theWorker( 0 )
  { }

  // This does not need a dtor; since *theWorker is a dependent of the
  // supervisor, TheObjects deletes *theWorker when it deletes the
  // supervisor.

  // *** DANGER *** see below *** DANGER *** //
  W* operator -> ( ) {
	 if ( ! theWorker ) {
		theWorker = new W( theBoss );
		theWorker->attachClient( theBoss );
		addDependent( theBoss, theWorker->oid() );
	 }
	 return theWorker;
  }
  // *** YOU MUST NEVER, EVER STORE THE W* EXCEPT AS AN AUTOMATIC *** //
  // *** VARIABLE IN A FUNCTION BODY. See the comments above.     *** //
  // This is inlined for now to avoid the explicit template instantiation
  // required by gcc 2.6.3.

  Supervisor& getBoss( ) const { return theBoss; } // overrides SubordinateBase

  ComputationManager* getWorker( ) const { return theWorker; }
  // overrides SubordinateBase

  void terminateWorker( )
  {
      if ( theWorker ){
         theWorker->adminTerminate();
         deleteWorker();
      }
  }

  bool exists( ) const { return theWorker != 0; }  // overrides SubordinateBase

protected:

  void acceptAllocation(OID oid, ARCSlotID asi, ARC arc, bool overrides) {
	 // Overrides SubordinateBase.
	 // Do not make theWorker only to give it 0 ARCs:
	 if ( theWorker || arc ) {
		( operator->() )->acceptAllocation( oid, asi, arc, overrides );
	 }
  }

private:

  void deleteWorker()
  {
    TheObjects::remove( theWorker->oid() );
    theWorker = 0;
  }
  
  //Data members:
  
  B& theBoss;
  
  W* theWorker;
};


class MirrorSubordinate : public SubordinateBase
{
public:

  MirrorSubordinate(class Supervisor& boss, SubordinateBase&);

  Supervisor& getBoss( ) const { return theBoss; }
  // overrides SubordinateBase

  bool exists( ) const { return mirror.exists(); }
  // overrides SubordinateBase

  ComputationManager* getWorker( ) const;
  // overrides SubordinateBase
  //@db temporary hack

  void terminateWorker( );

  void deleteWorker(); 
  
protected:

  void acceptAllocation(OID oid, ARCSlotID asi, ARC arc, bool overrides);
  // Overrides SubordinateBase.

private:

  class Supervisor& theBoss;

  SubordinateBase& mirror;

  bool firstAccessToWorker;
};


//---------------------------------------------------------------------------//
//--------------------------- Supervisor ------------------------------------//
//---------------------------------------------------------------------------//

// The purpose of class Supervisor is only to allow for locating the
// appropriate Subordinate, given its ARCSlotID. In cooperation with
// the SubordinateBase ctor, this assigns ARCSlotIDs.

class Supervisor : public ComputationManager
{
public:
  
  bool isSupervisor( ) const { return true; }

  void supervise( );
  // checks and conrols dependents, then calls it's own takeControl()
  
protected:

  friend class MirrorSubordinate;

  Supervisor( bool display_in_fe )
    : ComputationManager( display_in_fe ), ARCSlotID_Counter( 1 ),
      theParameters( NULL )
  { 
    if( display_in_fe ) 
      attachClient( *this );
  }

  void readMessage(istream& istr);  // Overrides ComputationManager

  void forwardAllocation(OID oid, ARCSlotID to, ARCSlotID from, ARC arc,
			 bool overrides);

  void adminTerminate( );
  
  void submit(ostream& ostr,const EnumertatorProblemView& pv) const {
    Supervisor* This = (Supervisor* )this;
    This->theParameters = new ParameterStructure(pv.extractParameters());
    pv.done( ostr );
  }
  
  const ParameterStructure& getParameters()const{
    if (theParameters == NULL)
      error("const ParameterStructure& getParameters()const : "
	    "viewStructure was not submited yet.");
    return *theParameters;
  }
  
private:

  SubordinateBase* dereference(ARCSlotID asi) const;
  // This returns the SubordinateBase with the given ARCSlotID. For
  // robustness, it may return 0 if something has gone wrong.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // `Public' Members Touched by Friends:                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend class SubordinateBase;

  ARCSlotID hire( SubordinateBase* worker );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AssociationsOf<int,SubordinateBase*> theWorkers;
  AssociationsOf<int,bool> theCrashedWorkers;

  int ARCSlotID_Counter;
  // By convention, ARCSlotID 0 always refers to the supervisor, so
  // this counter starts at 1.

   ParameterStructure* theParameters;

};


//---------------------------------------------------------------------------//
//----------------------- UnboundedSupervisor -------------------------------//
//---------------------------------------------------------------------------//

// This is an experimental class (ok, kludge) which allows supervisors of
// an unbounded number of CMs to get ad hoc access to private methods of
// TheObjects and SMObject.

class UnboundedSupervisor : public ComputationManager
{
protected:

  UnboundedSupervisor( bool display_in_fe )
    : ComputationManager( display_in_fe )
  { }

  void remove(OID oid) { TheObjects::remove( oid ); }

  void addDependent(OID oid) { SMObject::addDependent( oid ); }

};

#endif

// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of mixin class Supervisor and helpers
//
// Principal Author: Roger Needham, Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#include "Supervisor.h"
#include "OutMessages.h"


//---------------------------------------------------------------------------//
//--------------------------- Subordinate -----------------------------------//
//---------------------------------------------------------------------------//


SubordinateBase::SubordinateBase(Supervisor& boss)
  : asi( boss.hire( this ) )
{ }


void SubordinateBase::addDependent( SMObject& smo, OID oid ) const
{
  smo.addDependent( oid );
}


MirrorSubordinate::MirrorSubordinate(Supervisor& boss, SubordinateBase& worker)
  : SubordinateBase( boss ),
    theBoss( boss ),
    mirror( worker ),
    firstAccessToWorker( true )
{
/*
  //@@db
  ComputationManager* pWorker;
  if( pWorker = worker.getWorker() )
    pWorker->adminStart();
*/
}


void MirrorSubordinate::acceptAllocation(OID oid, ARCSlotID asi, ARC arc,
					 bool overrides)
{
  mirror.getBoss().forwardAllocation(oid,mirror.arcSlotID(),asi,arc,overrides);

  ComputationManager *newWorker = mirror.getWorker();
  if( firstAccessToWorker && newWorker ) {
    newWorker->attachClient( theBoss );
    firstAccessToWorker = false;
  }
}
 
ComputationManager* MirrorSubordinate::getWorker( ) const 
{ 
  return mirror.getWorker(); 
}

void MirrorSubordinate::deleteWorker() 
{ 
  if( exists() ) {
    getWorker()->detachClient( theBoss );
  }
}

void MirrorSubordinate::terminateWorker( )
{
  ComputationManager* worker = mirror.getWorker();
  if( worker )
    if( !worker->isNeeded() ) {
      worker->adminTerminate();
      mirror.deleteWorker();
    }
}


//---------------------------------------------------------------------------//
//--------------------------- Supervisor ------------------------------------//
//---------------------------------------------------------------------------//


void Supervisor::forwardAllocation(OID oid, ARCSlotID to, ARCSlotID from,
				   ARC arc, bool overrides)
{
  SubordinateBase* recipient = dereference( to );

  if ( recipient ) {
	 recipient->acceptAllocation( oid, from, arc, overrides );
  }
}


SubordinateBase* Supervisor::dereference(ARCSlotID asi) const
{
  int i = asi.unwrap();

  return ( theWorkers.bound( i ) ? theWorkers.valueOf( i ) : 0 );
}


ARCSlotID Supervisor::hire( SubordinateBase* worker )
{
  theWorkers.bind( ARCSlotID_Counter, worker );

  return ARCSlotID( ARCSlotID_Counter++ );
}


void Supervisor::readMessage(istream& istr)
{
  MessageTag tag;
  
  istr >> (int&)tag;

 

  if ( tag == ARC_UPDATE ) {
	 
	 int asi, arc;
	 
	 istr >> asi;
	 istr >> arc;

	 if ( asi == 0 )
		acceptAllocation( oid(), ARCSlotID(asi), ARC(arc), true );
	 else
		forwardAllocation( oid(), ARCSlotID(asi), ARCSlotID(asi), ARC(arc), true );
	 
  } else if ( tag == START ) {
	 adminStart();
  } else if ( tag == SUSPEND ) {
	 adminSuspend();
  } else if ( tag == RESUME ) {
	 adminResume();
  } else if ( tag == TERMINATE ) {
         adminTerminate();
  } else if ( tag == PARAMETER ) {
         theParameters->read( istr );
  }
}


void Supervisor::adminTerminate( )
{
  if( crashed() ) {

    LogMessage msg( *this );
    Chars message = getCrashMessage();
    
    if( message != Chars("") )
      msg << message;
    else
      msg << "The problem " << "\\\"" << Name(*this)
	  << "\\\"" << " crashed; it should not affect any other "
	  << "algorithm in magnus.";
    
    msg.send();
  }
  
  ComputationManager::adminTerminate();

  AssociationsIterator<int,SubordinateBase*> I(theWorkers);
  for( ; !I.done(); I.next() ) {
    I.value()->terminateWorker();
/*
    Computation1Manager* pWorker = I.value()->getWorker();
    if( pWorker != 0 )
      pWorker->adminTerminate();
*/      
  }
  detachClient( *this );
}


void Supervisor::supervise( )
{
  AssociationsIterator<int,SubordinateBase*> I(theWorkers);
  bool allFinished = true;

  for( ; !I.done(); I.next() ) {
    ComputationManager* pWorker = I.value()->getWorker();
    if( pWorker ) {

      if( pWorker->state() != ComputationManager::TERMINATED )
	allFinished = false;
	
      if( pWorker->crashed() && !theCrashedWorkers.bound(I.key()) ) {

	theCrashedWorkers.bind(I.key(), true);
	
	LogMessage msg( *this );
	Chars message = pWorker->getCrashMessage();

	if( message != Chars("") )
	  msg << message;
	else
	  msg << "The computation manager " << "\\\"" 
	      << SubProblemName( *this, I.value()->arcSlotID( ) ) 
	      << "\\\"" << " crashed; it should not affect any other "
	      << "algorithm in magnus.";

	msg.send();
      }
    }
    else allFinished = false;
  }
  
  takeControl();

  /*@db temporary disabled

  if( this->state() != ComputationManager::TERMINATED && allFinished ) {
    LogMessage msg( *this );
    msg << "The currently implemented algorithms could not solve"
	<< " the problem.";
    msg.send();
    adminTerminate();
  }
  */
}

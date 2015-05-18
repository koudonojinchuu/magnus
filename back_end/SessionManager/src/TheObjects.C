// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class TheObjects
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


//@rn  It is still not clear (to me) how this should manage passing control
//@rn  to CMs. Eschewing premature optimization, the eggregiously clumsy code
//@rn  below is only semantically correct, which is all that we need just
//@rn  now.


#include "SMObject.h"
#include "ComputationManager.h"
#include "TheObjects.h"
#include "Supervisor.h"
//#include "unistd.h"


// #define debug_TheObjects


//---------------------------------------------------------------------------//
//--------------------------- TheObjects ------------------------------------//
//---------------------------------------------------------------------------//


bool TheObjects::isRunning(OID oid)
{
  SMObject* smo = TheObjects::get( oid );

  return
	 ( smo && smo->isComputationManager()
		&& ((ComputationManager*)smo)->state() == ComputationManager::RUNNING
		);
}


bool TheObjects::isStalled(OID oid)
{
  // See ComputationManager::checkForStall(). This checks whether any
  // CM which has ever had ARCs with benefactor oid, has any free ARCs now.

  for( int j = 0; j < theObjectsLength; ++j ) {
	 SMObject* smo = theObjects[j];
	 if ( smo != 0 &&
	      smo->isComputationManager() &&
	      ((ComputationManager*)smo)->workingFor( oid ) 
	      )
	   return false;
  }
  return true;
}


OID TheObjects::reserveOid( )
{
  // Maybe need to initialize:

  if ( theObjectsLength == 0 ) resize( minimumLength );

  // Look for an empty spot

  int i = 0;
  while ( i < theObjectsLength && theObjects[i] != 0 ) ++i;
  if ( i == theObjectsLength ) resize( i++ << 1 );

  return OID(i);
}


void TheObjects::enroll(SMObject* newObject)
{
  int oid = (newObject->oid()).unwrap();
  theObjects[oid] = newObject;
}


SetOf<OID> TheObjects::dependencyClosure(OID oid)
{
  SetOf<OID> result;
  int i = oid.unwrap();

  if ( i >= 0 && i < theObjectsLength && theObjects[i] ) {

	 result.adjoinElement( oid );
	 if  (theObjects[oid.unwrap()]->isComputationManager() )
	      return result;

	 ListIterator< ListOf<OID> > I( theObjects[i]->getDependents() );
	 while ( ! I.done() ) {
		result |= TheObjects::dependencyClosure( I.value() );
		I.next();
	 }
  }
  return result;
}


SetOf<OID> TheObjects::remove(OID oid)
{
  SetOf<OID> toDelete = TheObjects::dependencyClosure( oid );
  
#ifdef debug_TheObjects
  cerr << "\nabout to delete " << toDelete.cardinality()
       << " objects" << endl << endl << flush;
#endif
  
  SetIterator<OID> I( toDelete );
  
  while ( ! I.done() ) {
    int i = I.value().unwrap();
    
    if ( i >= 0 && i < theObjectsLength && theObjects[i] ) {
      
#ifdef debug_TheObjects
      cerr << "\ndeleting " << i << endl << endl << flush;
#endif
      
      //@db
      SMObject* smo = theObjects[i];
      if( smo->isComputationManager() ) {
	ComputationManager* CM = (ComputationManager*)smo;
	if( CM->state() == ComputationManager::RUNNING ||
	    CM->state() == ComputationManager::SUSPENDED )
	  CM->adminTerminate();
      }

      delete theObjects[i];
      theObjects[i] = 0;
      if( theNames->bound(i) )
	theNames->unbind(i);
    }
    
    I.next();
  }
  
  return toDelete;
}


SMObject* TheObjects::get(OID oid)
{
  int i = oid.unwrap();
  return ( i >= 0 && i < theObjectsLength ? theObjects[i] : 0 );
}


void TheObjects::passControl( )
{
  //@rn  Terrible, temporary hackery: A CM may have a fastResult(), which
  //@rn  means it should get control immediately, and then be removed.
  //@rn  Here we find all of these.

  for( int j = 0; j < theObjectsLength; ++j ) {
	 SMObject* smo = theObjects[j];
	 if ( smo != 0 &&
	      smo->isComputationManager() &&
	      ((ComputationManager*)smo)->fastResult()
	      ) {
	   ((ComputationManager*)smo)->takeControl();
	   remove( j );
	 }
  }
  
  
  int i = CMToGetControl;
  int count = theObjectsLength;
  
  while ( count-- ) {
	 if ( i >= theObjectsLength ) i = 0;
	 SMObject* smo = theObjects[i];
	 if ( smo != 0 &&
	      smo->isComputationManager() &&
	      ((ComputationManager*)smo)->state() 
	      == ComputationManager::RUNNING
	      ) 
	   {
	     ComputationManager* cm = (ComputationManager*)smo;
	     if ( !(cm->checkForStall()) ) {
	       
	       if( cm->isSupervisor() )  
		 ((Supervisor*)cm)->supervise();
	       else
		 cm->takeControl();
	       break;
	     }
	   }
	 ++i;
  }
  CMToGetControl = i + 1;
  
  //@dp Magnus should sleep, do not eat CPU time, when there are no CMs
  //    with allocated ARCs.

  static int passesHandledBeforeSleeping = 10;
  //  bool runningCMs = false;

  if(passesHandledBeforeSleeping >= 0 ) 
    --passesHandledBeforeSleeping;

  for( int j = 0; j < theObjectsLength; ++j ) {
    SMObject* smo = theObjects[j];
    if ( smo != 0
	 && smo->isComputationManager()
	 && ((ComputationManager*)smo)->state() == ComputationManager::RUNNING
	 && ((ComputationManager*)smo)->freeARCs() > 0 
	 ) {
      //runningCMs = true;
      passesHandledBeforeSleeping = 10;
      break;
    }
  }
  
  if( passesHandledBeforeSleeping < 0 /*!runningCMs*/  )
  #ifdef SYSV
    sleep(1); // @db: don't have usleep under Solaris
  #else    
    usleep(200);
  #endif   
}


void TheObjects::resize(int newLength)
{
  SMObject** temp = new SMObject*[newLength];
  for( int i = 0; i < theObjectsLength; ++i )
	 temp[i] = theObjects[i];
  for( int i = theObjectsLength; i < newLength; ++i )
	 temp[i] = 0;

  theObjectsLength = newLength;
  delete [] theObjects;
  theObjects = temp;
}


void TheObjects::setName(OID oid, Chars name)
{
  theNames->bind(oid,name);
}


Chars TheObjects::name(OID oid)
{
  if( theNames->bound(oid) )
    return theNames->valueOf(oid);
  else
    return "";
}


//---------------------------------------------------------------------------//
//---------------------------- TheObjects -----------------------------------//
//----------------------- static initializations ----------------------------//


int TheObjects::theObjectsLength = 0;

SMObject** TheObjects::theObjects = 0;

int TheObjects::CMToGetControl = 0;

AssociationsOf<OID,Chars>* TheObjects::theNames = new AssociationsOf<OID,Chars>(); 

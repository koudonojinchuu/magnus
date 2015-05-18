// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class ResourceManager
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#include "ResourceManager.h"
#include "OutMessages.h"
#include "TheObjects.h"
#include "SMObject.h"
#include "ComputationManager.h"


#define debug_RM


//---------------------------------------------------------------------------//
//------------------------- ResourceManager ---------------------------------//
//---------------------------------------------------------------------------//


ResourceManager::ResourceManager(OID oid) :
  toUse(NULL),
  lastDrawnFrom(NULL),
  theOID(oid)
{ }


ResourceManager::~ResourceManager( )
{
  delete toUse;
}


OID ResourceManager::getOID( ) const
{
  return theOID;
}


ARC ResourceManager::freeARCs( ) const
{
  // Of course we could keep a running total, instead of computing it
  // each time.

  ARC result = ZeroARC();
  Resources* p = toUse;
  while ( p ) {
	 if ( isLiquid( p->benefactor ) )
		result += p->theARCs;
	 p = p->next;
  }
  return result;
}


bool ResourceManager::workingFor(OID oid) const
{
  // See ComputationManager::checkForStall(). This checks whether this
  // CM has ever had ARCs with benefactor oid, and has any free ARCs now.

  if ( freeARCs() == 0 ) return false;

  Resources* p = toUse;
  while ( p ) {
	 if ( p->benefactor == oid ) return true;
	 p = p->next;
  }
  return false;
}


bool ResourceManager::isNeeded() const
{
  Resources* p = toUse;
  while ( p ) {

    SMObject* smo = TheObjects::get(p->benefactor);
    
    if ( (smo) && smo->isComputationManager() ) {

      ComputationManager* CM = (ComputationManager*)smo;
      if( CM->state() == ComputationManager::RUNNING ||
	  CM->state() == ComputationManager::SUSPENDED )
	return true;
    }

    p = p->next;
  }

  return false;
}


void ResourceManager::freeze( )
{
  // For possible future use. Now, we tell whether a CM is frozen by
  // looking at its state.
}


void ResourceManager::liquefy( )
{
  // For possible future use. Now, we tell whether a CM is frozen by
  // looking at its state.
}


void ResourceManager::allocate(ResourceManager& recipient, ARC arcs)
{
  // Decide which of our Resources to take ARCs from:

  ARC give;
  ARC amount_left = arcs;
  Resources* p = toUse;

#ifdef debug_RM
  cerr
	 << "\n\n" << theOID << " is about to allocate " << arcs
	 << " in " << recipient.theOID << endl;
#endif

  while ( p ) {

	 // Don't fool around yet. Just grab the first ARCs we see.

#ifdef debug_RM
	 cerr
		<< "Looking at Resources:\nFrom: " << p->benefactor
		<< "\nASI: " << p->theARCSlotID
		<< "\nARCs: " << p->theARCs
		<< "\nState: ";
	 if ( isLiquid( p->benefactor ) ) cerr << "Liquid\n\n";
	 else  cerr << "Frozen\n\n";
#endif

	 if ( isLiquid( p->benefactor ) ) {

		if ( p->theARCs >= amount_left ) {
		  p->theARCs -= amount_left;
		  give = amount_left;
		  amount_left = ZeroARC();
		} else {
		  amount_left -= p->theARCs;
		  give = p->theARCs;
		  p->theARCs = ZeroARC();
		}

		// Note that we do not remove zeroed Resources, since they are
		// liable to be added to later.

		// Tell the recipient:

		recipient.acceptAllocation(p->benefactor, p->theARCSlotID, give);

		//@rn  In fact, need to distinguish between ARCSlotID-wise
		//@rn  benefactor, and the freeze-wise benefactor...

		if ( amount_left == 0 ) break;
		p = p->next;
	 }
  }

  if ( amount_left != 0 ) {
	 Warning msg;
	 msg << "ResourceManager::allocate tried to transfer too much; ignoring";
	 msg.send();
  }
}


void ResourceManager::acceptAllocation(OID benefactor,
													ARCSlotID asi,
													ARC arcs,
													bool overrides
													)
{
#ifdef debug_RM
  cerr << theOID << " is accepting " << arcs << " ARCs from "
		 << benefactor << " for ARCSlotID " << asi;
  if ( overrides ) cerr << " (overrides)";
  cerr << endl << endl;
#endif

  ARC change = arcs;

  // See if we already have Resources from the benefactor.

  Resources* p = toUse;
  while ( p ) {
	 if ( p->benefactor == benefactor && p->theARCSlotID == asi ) break;
	 p = p->next;
  }

  if ( p ) {  // Have one

	 if ( overrides ) {
		change -= p->theARCs;
		p->theARCs = arcs;
	 } else
		p->theARCs += arcs;

  } else {  // Make new Resources

	 toUse = new Resources(benefactor, asi, arcs, toUse);
  }
}


void ResourceManager::usedARCs(ARC arcs)
{
#ifdef debug_RM
  Resources* q = toUse;
  cerr << "\nHere are all Resources in ResourceManager::usedARCs, for "
		 << theOID << ':' << endl << endl;
  while ( q ) {
	 cerr
		<< "From: " << q->benefactor
		<< "\nASI: " << q->theARCSlotID
		<< "\nARCs: " << q->theARCs
		<< "\nState: ";
	 if ( isLiquid( q->benefactor ) )
		cerr << "Liquid\n\n";
	 else
		cerr << "Frozen\n\n";

	 q = q->next;
  }
#endif

  if ( freeARCs() >= arcs ) { // Internal check

	 ARC used;
	 ARC amount_left = arcs;

#ifdef debug_RM
	 cerr << "\n\n" << theOID << " is about to use " << arcs << endl;
#endif

	 do {
		do {
		  if ( lastDrawnFrom )
			 lastDrawnFrom = lastDrawnFrom->next;
		  if ( ! lastDrawnFrom )
			 lastDrawnFrom = toUse;
		} while (
					! isLiquid( lastDrawnFrom->benefactor ) ||
					lastDrawnFrom->theARCs == 0
					); // Should not loop

#ifdef debug_RM
		cerr
		  << "Looking at Resources:\nFrom: " << lastDrawnFrom->benefactor
		  << "\nASI: " << lastDrawnFrom->theARCSlotID
		  << "\nARCs: " << lastDrawnFrom->theARCs
		  << "\nState: ";
		if ( isLiquid( lastDrawnFrom->benefactor ) )
		  cerr << "Liquid\n\n";
		else
		  cerr << "Frozen\n\n";
#endif

		if ( lastDrawnFrom->theARCs >= amount_left ) {
		  used = amount_left;
		  lastDrawnFrom->theARCs -= used;
		  amount_left = ZeroARC();
		} else {
		  used = lastDrawnFrom->theARCs;
		  amount_left -= used;
		  lastDrawnFrom->theARCs = ZeroARC();
		}

		// Send ARC update

		ARCUpdate update(lastDrawnFrom->benefactor,
							  lastDrawnFrom->theARCSlotID,
							  used
							  );

		update.send();

	 } while ( amount_left > 0 ); // Should not loop

  } else {
    //@rn Let's not crash just yet, but just warn.

    if( ((ComputationManager*)this)->crashed() ) return; 
    //@db in case of crash let supervisor worry about warnings
    
    Warning msg;
    msg << "CM " << theOID << " tried to draw " << arcs
	<< " ARCs, when it has only " << freeARCs() << " free; ignoring";
    msg.send();
  }
}


void ResourceManager::usedOneARC( )
{
  usedARCs( OneARC() );
}


bool ResourceManager::isLiquid(OID oid) const
{
  return TheObjects::isRunning( oid );
}

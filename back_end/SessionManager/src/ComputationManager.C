// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class ComputationManager
//
// Principal Author: Roger Needham, Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#include "ComputationManager.h"
#include "OutMessages.h"
#include "SMFPGroup.h"

#define DEBUG_CLIENTS

//---------------------------------------------------------------------------//
//------------------------ ComputationManager -------------------------------//
//---------------------------------------------------------------------------//


ComputationManager::ComputationManager( bool display_in_fe )
  : SMObject( true ),
    ResourceManager( oid() ),
    amFast( false ),
    showMe( display_in_fe ),
    wasStalled( false ),
    isCrashed( false ),
    theState( UNSTARTED )
{ }


ComputationManager::~ComputationManager( )
{

}


ComputationManager::State ComputationManager::state( ) const
{
  return theState;
}


const IconID ComputationManager::iconID( ) const
{
  return IconID::problem;
}


void ComputationManager::viewStructure(ostream& ostr) const
{
  // Blank provided here to save the nuisance of supplying it
  // in derivatives, many of which do not have `viewStructures'.
}


const char* ComputationManager::typeID( ) const
{
  return "ComputationManager";
}


void ComputationManager::printProperties(ostream& ostr) const
{
  // Blank provided here to save the nuisance of supplying it
  // in derivatives, most of which do not have `properties'.
}


void ComputationManager::printDefinition(ostream& ostr) const
{
  // Blank provided here to save the nuisance of supplying it
  // in derivatives, most of which do not have `definitions'.
}


void ComputationManager::resultIsFast( )
{
  amFast = true;
  showMe = false;
}


void ComputationManager::printGlobalMessageTemplates(ostream& ostr)
{
  ostr
	 << "init_view_req_msg_ {${viewID} " << VIEW_REQUEST
	 << " ${ARCSlotID}}" << endl

	 << "init_arc_msg_ {${viewID} " << ARC_UPDATE
	 << " ${ARCSlotID} ${value}}" << endl

	 << "init_start_msg_ {${viewID} " << START << '}' << endl

	 << "init_suspend_msg_ {${viewID} " << SUSPEND << '}' << endl

	 << "init_resume_msg_ {${viewID} " << RESUME << '}' << endl

	 << "init_terminate_msg_ {${viewID} " << TERMINATE << '}' << endl

	 << "init_parameter_msg_ {${viewID} " << PARAMETER 
	 << " ${value} {EOF_PARAM} }" << endl;
}


void ComputationManager::readMessage(istream& istr)
{
  MessageTag tag;
  
  istr >> (int&)tag;

  if ( tag == ARC_UPDATE ) {
	 
	 int asi, arcs;
	 
	 istr >> asi;
	 istr >> arcs;
	 
	 acceptAllocation( oid(), ARCSlotID(asi), ARC(arcs), true );
	 
  } else if ( tag == START ) {
	 adminStart();
  } else if ( tag == SUSPEND ) {
	 adminSuspend();
  } else if ( tag == RESUME ) {
	 adminResume();
  } else if ( tag == TERMINATE ) {
	 adminTerminate();
  }
}

void ComputationManager::attachClient( const SMObject& client ) { 
  theClients |= client.oid(); 
}

void ComputationManager::detachClient( const SMObject& client ) {  
  theClients -= client.oid(); 
}

SetOf<OID> ComputationManager::getClients( ) const { 
  return theClients; 
}

bool ComputationManager::checkForStall( )
{
  // Definition of `stalled': A CM X which is displayed in the FE, is
  // in state RUNNING, and for which no CM (including X), which has
  // ever had ARCs with benefactor X, has any free ARCs now.

  // We can check the first two conditions here. For the last, we must
  // ask TheObjects, since it is the only one (so far) who knows where
  // all of the CMs are. TheObjects asks the ResourceManager of each
  // CM whether it works for X; it can answer, because (so far) it
  // never throws out zeroed Resource records.

  if ( displayInFE() && theState == RUNNING ) {

	 if ( TheObjects::isStalled( *this ) ) {

		if ( ! wasStalled )
		  StateTransition( StateTransition::STALLED, *this ).send();
		wasStalled = true;
		return true;

	 } else {  // Any state other than RUNNING overrides in the FE.

		if ( wasStalled )
		  StateTransition( StateTransition::RESUME, *this ).send();
		wasStalled = false;
	 }
  }
  return false;
}


void ComputationManager::adminStart( )
{
  theState = RUNNING;

  liquefy();


  if ( displayInFE() )
	 StateTransition( StateTransition::START, oid() ).send();

  // @am 
  start();
}


void ComputationManager::adminSuspend( )
{
  theState = SUSPENDED;

  freeze();

  suspend();

  if ( displayInFE() )
	 StateTransition( StateTransition::SUSPEND, oid() ).send();
}


void ComputationManager::adminResume( )
{
  theState = RUNNING;

  liquefy();

  resume();

  if ( displayInFE() )
    StateTransition( StateTransition::RESUME, oid() ).send();
}


void ComputationManager::adminTerminate( )
{
  //@db
  if( theState == TERMINATED ) return;
  
  theState = TERMINATED;

  freeze();

  terminate();

  terminateArcers();

  if ( displayInFE() )
    StateTransition( StateTransition::TERMINATE, oid() ).send();
}


void ComputationManager::hireArcer( ARCer* arcer )
{
  theArcers.append(arcer);
}


void ComputationManager::terminateArcers( )
{
  int len = theArcers.length();
  for( int i = 0; i < len; ++i )
    theArcers[i]->terminate();
}


Chars ComputationManager::helpID( const Chars& problemName, 
				  const SMFPGroup& parent ) const
{
  return problemName + "::" + parent.getCheckinTypeStr();
}


void ComputationManager::setCrashState( ) 
{ 
  isCrashed = true; 
  adminTerminate();
}

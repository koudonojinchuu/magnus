// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class OutMessage, and derivatives
//           ARCUpdate, CheckinMessage, FEDataUpdate, LogMessage,
//           ParseErrorMessage, StateTransition, Warning
//
// Principal Author: Roger Needham
//
// Status: useable
//
// Revision History:
//
// 01/25/98 @am added ParseParamErrorMessage class. It reports to
// the front end about parsing errors in parameters.

#include "SMObject.h"
#include "OutMessages.h"
#include "TheObjects.h"
#include "Timer.h"
#include "Set.h"
#include "TheObjects.h"
#include "ComputationManager.h"

//---------------------------------------------------------------------------//
//--------------------------- OutMessage ------------------------------------//
//---------------------------------------------------------------------------//

void OutMessage::send( ) const
{
  print( cout );
  cout << endl << flush;
}


//---------------------------------------------------------------------------//
//--------------------------- LogMessage ------------------------------------//
//---------------------------------------------------------------------------//


LogMessage::LogMessage(int message_level)
  : num_addressees( 0 ), level( message_level ),
    oids( 0 )
{ }


LogMessage::LogMessage(OID o, int message_level)
  : num_addressees( 1 ), level( message_level ), 
    oids( 1 )
{
  oids[0] = o;
}


LogMessage::LogMessage(OID o1, OID o2, int message_level)
  : num_addressees( 2 ), level( message_level ),
    oids( 2 )
{
  oids[0] = o1;
  oids[1] = o2;
}
	  

LogMessage::LogMessage(OID o1, OID o2, OID o3, int message_level)
  : num_addressees( 3 ), level( message_level ),
    oids( 3 )
{
  oids[0] = o1;
  oids[1] = o2;
  oids[2] = o3;
}


LogMessage::LogMessage(const char* msg, int message_level)
  : num_addressees( 0 ), level( message_level ),
    oids( 0 )
{
  *this << msg;
}


LogMessage::LogMessage(OID o, const char* msg, int message_level)
  : num_addressees( 1 ), level( message_level ),
    oids(1)
{
  oids[0] = o; 
  *this << msg;
}

void LogMessage::print(ostream& ostr) const
{
  //ostrstream& o = (ostrstream&)*this;  // Cast away constness

  //o << ends;
  ostr << "post_to_log_ {";

  SetOf<OID> bosses;
  for( int i = 0; i < num_addressees; ++i ) {
    OID o = oids[i];
    SMObject *worker = TheObjects::get( o );
    SetOf<OID> wbosses;
    if( worker->isComputationManager() ) 
      wbosses = ((ComputationManager *)worker)->getClients();
    else 
      wbosses |= o;
    bosses |= wbosses;
  }

  SetIterator<OID> I(bosses);
  for( ; !I.done(); I.next() ) {
	 ostr << I.value() << ' ';
  }

  ostr << "} {" << rdbuf() << "} " << level;
  //this->freeze(0);
  
  //@rn could check for bogus characters, brace balance, etc.
}


//---------------------------------------------------------------------------//
//------------------------ ParseErrorMessage --------------------------------//
//---------------------------------------------------------------------------//


ParseErrorMessage::ParseErrorMessage(const Chars name, const Chars diagnosis)
  : theName( name ), theDiagnosis( diagnosis )
{ }


void ParseErrorMessage::print(ostream& ostr) const
{
  ostr
	 << "syntax_error_in_defn_ name {"
	 << theName
	 << "} errMesg {"
	 << theDiagnosis
	 << '}';
}


//---------------------------------------------------------------------------//
//------------------------ PackageInfoMessage     ---------------------------//
//---------------------------------------------------------------------------//


PackageInfoMessage::PackageInfoMessage(const Chars& name, 
				       const Chars& command,
				       const Chars& ch_type, 
				       const Chars& obj)  :
  theChType( ch_type ),
  theName( name ),
  theObject( obj ),
  theCommand( command )
{ }


void PackageInfoMessage::print(ostream& ostr) const
{
  ostr
    << "view_package_info_  "
    << " { " << theName << " } "
    << " { " << theCommand << " } "
    << " { " << theChType << " } "
    << " { " << theObject << " } ";\
}


//---------------------------------------------------------------------------//
//------------------------ ParseParamErrorMessage ---------------------------//
//---------------------------------------------------------------------------//


ParseParamErrorMessage::ParseParamErrorMessage(OID o,const Chars& name, 
					       const Chars& type,
					       const Chars& msg)
  : theOID( o ), theName( name ), theType( type  ), theMsg( msg )
{ }


void ParseParamErrorMessage::print(ostream& ostr) const
{
  ostr
    << "syntax_error_in_param_ OID {"
    << theOID 
    << "} name {"
    << theName
    << "} type {"
    << theType
    << "} errMesg {"
    << theMsg
    << '}';
}

//---------------------------------------------------------------------------//
//------------------------- CheckinMessage ----------------------------------//
//---------------------------------------------------------------------------//

//@rn This might just as well take a vector of dependencies.

CheckinMessage::CheckinMessage(const SMObject& smo,
			       const Chars name,
			       const ListOf<OID> dependencies,
			       bool bPrintDefinition
			       )
  : theSMO( smo ),
    theName( name ),
    theDependencies( dependencies ),
    BPrintDefinition( bPrintDefinition )
{ }


void CheckinMessage::print(ostream& ostr) const  // overrides OutMessage
{
  //@db report dependents to TheObjects
  {
    ListIterator< ListOf<OID> > I( theDependencies );
    while ( ! I.done() ) {
      SMObject* smo  = TheObjects::get(OID(I.value()));
      smo->addDependent( theSMO.oid() );
      I.next();
    }
  }

  ostr << "create_ objectID " << theSMO.oid()
       << " typeID " << theSMO.typeID()
       << " iconID " << theSMO.iconID()
       << " name {" << theName
       << "} dependencies {";
  
  ListIterator< ListOf<OID> > I( theDependencies );
  while ( ! I.done() ) {
    ostr << ' ' << I.value();
    I.next();
  }
  
  ostr << "} properties {";
  
  theSMO.printProperties( ostr );

  ostr << "} viewStructure {";
  
  theSMO.viewStructure( ostr );
  
  ostr << '}';
  
  if ( theName.length() == 0 ) {

    // By convention, when an object is created internally, it is not
    // given a name. This is how we know to echo the definition.
    //@rn Shouldn't we always print defn, for double-check?
    
    ostr << " definition {";
    if( BPrintDefinition )
      theSMO.printDefinition( ostr );
    ostr << '}';
  }
}


//---------------------------------------------------------------------------//
//-------------------------- FEDataUpdate -----------------------------------//
//---------------------------------------------------------------------------//

void FEDataUpdate::print(ostream& ostr) const  // overrides OutMessage
{
  ostr << "new_data_ key {" << theKey << "} datum {" << theDatum << '}';
}


//---------------------------------------------------------------------------//
//------------------------- StateTransition ---------------------------------//
//---------------------------------------------------------------------------//

void StateTransition::print(ostream& ostr) const
{
  if ( kind == START )
	 ostr << "confirm_start_";
  else if ( kind == SUSPEND )
	 ostr << "confirm_suspend_";
  else if ( kind == RESUME )
	 ostr << "confirm_resume_";
  else if ( kind == TERMINATE )
	 ostr << "terminate_problem_";
  else if ( kind == STALLED )
	 ostr << "problem_stalled_";

  ostr << " viewID " << oid;
}


//---------------------------------------------------------------------------//
//---------------------------- ARCUpdate ------------------------------------//
//---------------------------------------------------------------------------//

void ARCUpdate::print(ostream& ostr) const
{
  ostr
	 << "arc_update_ viewID " << oid
	 << " ARCSlotID " << asi
	 << " value " << value;
}


//---------------------------------------------------------------------------//
//----------------------------- Warning -------------------------------------//
//---------------------------------------------------------------------------//

void Warning::print(ostream& ostr) const
{
  // Partial safeguard against repeated warnings from within a loop:

  static Timer timer( 0 );   // First call should go out

  if ( timer.expired() ) {

	 //ostrstream& o = (ostrstream&)*this;  // Cast away constness

	 //*this << ends;
	 ostr << "warn_ {" << *this << '}';

	 timer.reset( 1000 );
  }
}


//----------------------------- Message -------------------------------------//


void Message::print(ostream& ostr) const
{
  //ostrstream& o = (ostrstream&)*this;  // Cast away constness
  //o << ends;
  ostr << "tk_dialog .d {" << theTitle << "} {" << rdbuf() << "} warning 0 OK";
//  ostr "alert {" << o.str() << '}';
}


//----------------------------- InvokingMessage ------------------------------//


void InvokingMessage::print(ostream& ostr) const
{
  //ostrstream& o = (ostrstream&)*this;  // Cast away constness
  //o << ends;
  ostr << rdbuf();
}

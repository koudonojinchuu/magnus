// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class SMMap
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#include "SMMap.h"

// ------------------------------- MCM ------------------------------------- //


MCM::MCM( class SMMap& M )
  : Supervisor( false ),
    theMap( M ),
    abelianHomIsMono( *this ),
    abelianHomIsEpi( *this )
{
  adminStart();
}


void MCM::takeControl( )
{
  if ( ! didFastChecks ) {
    
    didFastChecks = true;
    
    // Cheat and do some quickie checks here without reporting ARC consumption.
  }
}

//---------------------------------------------------------------------------//
//------------------------------ SMMap --------------------------------------//
//---------------------------------------------------------------------------//

SMMap::SMMap(SMFPGroup& G, Map m, const Chars heritage)
  : AlgebraicObject( heritage ),
	 theMap( m ),
	 theDomain( G ),
	 theRange( G ),
	 mic( oid() ),
         theMCM( new MCM( *this ) )
{
  addDependent( *theMCM );
}

SMMap::SMMap(SMFPGroup& G, SMFPGroup& H, Map m, const Chars heritage)
  : AlgebraicObject( heritage ),
    theMap( m ),
    theDomain( G ),
    theRange( H ),
    mic( oid() ),
    theMCM( new MCM( *this ) )
{
  addDependent( *theMCM );
}

SMMap::SMMap(SMFPGroup& G)
: AlgebraicObject(""), theMap( *((Map*)0) ), theDomain( G ), theRange( G ),
    mic( oid() ),theMCM( new MCM( *this ) )
{
  addDependent( *theMCM );
} 

SMMap::SMMap(SMFPGroup& G, SMFPGroup& H)
: AlgebraicObject(""), theMap( *((Map*)0) ), theDomain( G ), theRange( H ),
  mic( oid() ),theMCM( new MCM( *this ) )
{
  addDependent( *theMCM );
} 

void SMMap::printProperties(ostream& ostr) const
{
  ostr << DataPair( Domain( *this ), Object( theDomain ) )
		 << DataPair( Range( *this ), Object( theRange ) );
}


void SMMap::viewStructure(ostream& ostr) const
{
  ostr
	 << "viewBuilder objectView viewParameters {viewID "
	 << oid()
	 << " heritage {"
	 << heritage()
	 << "}}";
}


void SMMap::printDefinition(ostream& ostr) const
{
  ostr << "{ ";
  theMap.printOn( ostr );
  ostr << " } ";
}


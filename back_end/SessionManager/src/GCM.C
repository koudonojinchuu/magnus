// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class GCM
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#include "GCM.h"
#include "SMFPGroup.h"


//---------------------------------------------------------------------------//
//-------------------------------- GCM --------------------------------------//
//---------------------------------------------------------------------------//


GCM::GCM( class SMFPGroup& G )
  : Supervisor( false ),
    normalClosure( *this ),
    abelianInvariants( *this ),
    abelianPrimes( *this ),
    abelianRank( *this ),
    kbSupervisor( *this ),
    agSupervisor( *this ),
    theToddCoxeter( *this ),
    ghToddCoxeter( *this ),
    nilpotentQuotients( *this ),
    computeBasis( *this ),
    theSMFPGroup( G ),
    didFastChecks( false )
{
  adminStart();
}



void GCM::takeControl( )
{
  if ( ! didFastChecks ) {
    
    didFastChecks = true;
    
    // Cheat and do some quickie checks here without reporting ARC consumption.

    if( theSMFPGroup.getCheckinType() != SMFPGroup::ABELIAN &&
	theSMFPGroup.getCheckinType() != SMFPGroup::NILPOTENT ) {
    
      SetOf<Word> relators = theSMFPGroup.getFPGroup().getRelators();
      
      if ( relators.cardinality() == 1 ) {
	
	SetIterator<Word> I( relators );
	Word r = I.value().cyclicallyReduce();
	if ( r.isProperPower() )
	  theSMFPGroup.gic.putIsOneRelatorWithTorsion( r );
	else
	  theSMFPGroup.gic.putIsOneRelator( r );
      }
    }
  }
}


void GCM::start( ) { }

void GCM::terminate( ) { }

void GCM::viewStructure(ostream& ostr) const { }


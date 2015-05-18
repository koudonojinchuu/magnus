// Copyright (C) 1999 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes FNWPArcer, FNWPCM
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#include "FNWP.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "PowerSeriesWP.h"


// ----------------------------- FNWPArcer --------------------------------- //


FNWPArcer::FNWPArcer( ComputationManager& boss )
  : ARCer( boss ), retValue(dontknow)
{ } 

void FNWPArcer::setArguments( const FreeGroup& group,
			      int nClass,
			      const SetOf<Word>& words )
{
  theGroup = group;
  c = nClass;
  theWords = words;
}

void FNWPArcer::runComputation( ) 
{
  PowerSeriesWP pswp(theGroup,c);
  SetIterator<Word> I(theWords);
  for(; !I.done(); I.next() )
    if( !pswp.isTrivial(I.value()) ) {
      retValue = no;
      return;
    }
  retValue = yes;
}

void FNWPArcer::writeResults( ostream& out )
{
  out < retValue;
}

void FNWPArcer::readResults( istream& in )
{
  in > retValue;
}


// ------------------------------- FNWPCM ---------------------------------- //


FNWPCM::FNWPCM( Supervisor& boss )
: theBoss( boss ),
  tAreTrivial( dontknow ),
  bStarted( false ),
  bInited( false ),
  arcer( *this )
{
  adminStart();
}

void FNWPCM::terminate( ) { }

void FNWPCM::start( ) { }

void FNWPCM::init( const FreeGroup& group, 
		   int nClass, 
		   const SetOf<Word> words )
{
  theGroup = group;
  c = nClass;
  theWords = words;
  bInited = true;
}

void FNWPCM::takeControl( )
{
  if ( freeARCs() > 0 ) {
    if( !bStarted ) {

    #if SAFETY > 0
      if( !bInited )
	error("void FNWPCM::takeControl( ) : "
	      "The computation manager was not initialized.");
    #endif  
    
      arcer.setArguments( theGroup, c, theWords );
      bStarted = true;
    }
    if( arcer.takeControl() ) {
      tAreTrivial = arcer.getRetValue();
      adminTerminate();
      return;
    }
    else
      usedOneARC();
  }
}

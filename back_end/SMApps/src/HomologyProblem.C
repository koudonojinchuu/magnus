// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class HomologyProblem
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#include "HomologyProblem.h"
#include "Homology.h"
#include "SMFPGroup.h"
#include "OutMessages.h"
#include "viewStructure.h"


HomologyARCer::HomologyARCer( ComputationManager& boss )
  : ARCer( boss ), kbmachine( 0 )
{
}

HomologyARCer::~HomologyARCer( )
{
  delete kbmachine;
}

void HomologyARCer::setArguments( const KBMachine& M, int startDimension, 
				  int endDimension )
{
  kbmachine = new KBMachine( M );
  startdim = startDimension;
  enddim = endDimension;
}

void HomologyARCer::runComputation( )
{
  Homology theHomology( *kbmachine , startdim, enddim );
  while( !theHomology.workOnNextGroup() );
  torsionFreeRank   = theHomology.getTorsionFreeRank();
  torsionInvariants = theHomology.getTorsionInvariants();
}

void HomologyARCer::writeResults( ostream& ostr )
{
  ostr < torsionFreeRank;
  ostr < torsionInvariants; 
}

void HomologyARCer::readResults( istream& istr )
{
  istr > torsionFreeRank;
  istr > torsionInvariants;
}



//---------------------------------------------------------------------------//
//--------------------------- HomologyProblem -------------------------------//
//---------------------------------------------------------------------------//


HomologyProblem::HomologyProblem( const HomologySupervisor& homology )
  : theGroup( homology.group() ), theDimension( homology.dimension() ),
    homologyArcer( *this ), arcerStarted( false ), solved( false )
{
  if ( ! theGroup.gic.haveConfluentKBMachine() ) {
    LogMessage msg( *this, theGroup );
    msg << "The system requires a confluent rewriting system for " 
	<< Name(theGroup) << " before it can start to compute its homology.";
    msg.send();
  }
  adminStart();
}


void HomologyProblem::takeControl( )
{
  if( theGroup.gic.haveConfluentKBMachine() && freeARCs() > 0 ) {
    
    if( !arcerStarted ) {
      homologyArcer.setArguments( theGroup.gic.getConfluentKBMachine(), 
				  theDimension, theDimension );
      arcerStarted = true;
      usedOneARC();
    }
    else {
      // homologyArcer.runComputation();
      if( homologyArcer.takeControl() ) {
	
	LogMessage msg( *this, theGroup );
	msg << "Dimension " << theDimension << " integral homology: Z^"
	    << homologyArcer.getTorsionFreeRank();
	
	VectorOf<Integer> invariants = homologyArcer.getTorsionInvariants();
	int len = invariants.length();
	for( int i = 0; i < len; ++i )
	  msg << " x Z_" << invariants[i];
	
	msg << ".";
	msg.send();
	solved = true;
	adminTerminate();
	
      }
      else
	usedOneARC();
    }
  }
}



//---------------------------------------------------------------------------//
//------------------------ HomologySupervisor -------------------------------//
//---------------------------------------------------------------------------//

HomologySupervisor::HomologySupervisor(SMFPGroup& G, int d)
  : Supervisor( true ), theGroup( G ), theDimension( d ), 
    homology( *this ), kbSupervisor( *this, G.gcm().kbSupervisor )
{
}

void HomologySupervisor::takeControl( ) 
{
  if( homology->isSolved() )
    adminTerminate();
}

void HomologySupervisor::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Integral homology"),
		  Text("Compute the dimension") + theDimension + "homology of"
		  + Name( theGroup ),
		  helpID("HomologyProblem", theGroup),
		  "Homology"
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of this problem"), homology.arcSlotID(), 50);

  //@rn Dodge updating the FE database for now:

  if( ! theGroup.gic.haveConfluentKBMachine() ) {
    
    pv.add(Text("Seek a rewriting system for") + Name( theGroup ),
	   kbSupervisor.arcSlotID(),
	   50
	   );
  }

  pv.done();
}


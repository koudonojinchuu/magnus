// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes KBSupervisor, KBProblem
//
// Principal Author: Roger Needham, Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//

#include <sys/wait.h>
#include "KBModule.h"
#include "SMFPGroup.h"
#include "OutMessages.h"
#include "viewStructure.h"


// ------------------------ KBSupervisorARCer ------------------------------ //


void KBSupervisorARCer::setArguments( const FPGroup& group )
{
  G = group;
}


void KBSupervisorARCer::runComputation( )
{
  // Pre-compute one of the parameters to RKBPackage:

  int maxRelLen = 0;
  SetIterator<Word> I( G.getRelators() );
  while ( !I.done() ) {
    Word w = I.value();
    int l = w.length();
    if ( l > maxRelLen ) maxRelLen = l;
    I.next();
  }
  int rkbp_par = 2 * maxRelLen;

  // Make the RKBPackage:

  RKBP = new RKBPackage(G.namesOfGenerators(), G.getRelators());

  do {
    RKBP->runKB(rkbp_par, -1, rkbp_par, rkbp_par);
    rkbp_par += (rkbp_par >> 1);
  } while( !RKBP->isProvedConfluent() );
}


void KBSupervisorARCer::writeResults( ostream& out ) 
{
  out < (RKBP->KnuthBendixMachine());
  delete RKBP;
}


void KBSupervisorARCer::readResults( istream& in )
{
  in > KBM;
}


//----------------------------- KBSupervisor ---------------------------------//


KBSupervisor::KBSupervisor(class GCM& gcm)
  : theGroup( gcm.getSMFPGroup() ),
    arcer( *this )
{
  adminStart();
}


void KBSupervisor::start( )
{
  arcer.setArguments(theGroup.getFPGroup());
}


void KBSupervisor::takeControl( )
{
  if ( freeARCs() > 0 ) 

    if( arcer.takeControl() ) {
      theGroup.gic.putHaveConfluentKBMachine( arcer.getKBMachine() );
      adminTerminate();
    }
    else
      usedOneARC();

}


void KBSupervisor::terminate( )
{
//  arcer.terminate();
}


//---------------------------- KBProblem ------------------------------------//


KBProblem::KBProblem(class SMFPGroup& G)
  : Supervisor( true ),
    theGroup( G ),
    kbSupervisor( *this, G.gcm().kbSupervisor )
{ }

void KBProblem::start( )
{
/*@db
  LogMessage msg( *this, theGroup );
  msg <<
	 "Warning: Magnus currently executes the Rutgers Knuth-Bendix"
	 " Package *synchronously*. This means that Magnus will appear to"
	 " freeze if rkbp takes a long time to respond. We are working on"
	 " an asynchronous version.";
  msg.send();
*/
}


void KBProblem::takeControl( )
{
  if( theGroup.gic.haveConfluentKBMachine() )
    adminTerminate();
}


void KBProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Seek rewriting system"),
		  Text("Seek a rewriting system for") + Name( theGroup ),
		  helpID("KBProblem", theGroup),
		  "Rewr sys"
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of this problem"), kbSupervisor.arcSlotID(), 100);

  pv.done();
}

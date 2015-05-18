// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes AutGroupARCer, AGSupervisor, AGProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//

#include "AGModule.h"
#include "SMFPGroup.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "ARCer.h"

// ---------------------------AutGroupARCer -------------------------------- //

void AutGroupARCer::setArguments( const FPGroup group )
{
  G = group;
}


void AutGroupARCer::runComputation( )
{
  KBM = new KBmagPackage(G.namesOfGenerators(), G.getRelators());
  retValue = KBM->autgroup();
}

void AutGroupARCer::writeResults( ostream& out ) 
{
  out < retValue;
  if( retValue == yes )
    out < KBM->differenceMachine(1) < KBM->differenceMachine(2) 
        < KBM->wordAcceptor() < KBM->generalMultiplier();
  delete KBM;
}

void AutGroupARCer::readResults( istream& in )
{
  in > retValue;
  if( retValue == yes ) in > DM1 > DM2 > WA > GM;
}

DiffMachine AutGroupARCer::getDiffMachine1( ) 
{ 
#if SAFETY > 0
  if ( retValue != yes )
    error( "DiffMachine AutGroupARCer::getDiffMachine1( ) : "
	   "cannot give result of the computation that failed.");
#endif

  return DM1; 
}

DiffMachine AutGroupARCer::getDiffMachine2( ) 
{ 
#if SAFETY > 0
  if ( retValue != yes )
    error( "DiffMachine AutGroupARCer::getDiffMachine2( ) : "
	   "cannot give result of the computation that failed.");
#endif

  return DM2; 
}

GroupDFSA AutGroupARCer::getWordAcceptor( ) 
{
#if SAFETY > 0
  if ( retValue != yes )
    error( "GroupDFSA AutGroupARCer::getWordAcceptor( ) : "
	   "cannot give result of the computation that failed.");
#endif

  return WA; 
}

GenMult AutGroupARCer::getGeneralMultiplier( ) 
{ 
#if SAFETY > 0
  if ( retValue != yes )
    error( "GenMult AutGroupARCer::getGeneralMultiplier( ) : "
	   "cannot give result of the computation that failed.");
#endif

  return GM; 
}

// ----------------------------- AGSupervisor ------------------------------ //

AGSupervisor::AGSupervisor( class GCM& gcm )
  : theGroup( gcm.getSMFPGroup() ),
    arcer( *this )
{
  adminStart();
}

void AGSupervisor::start( )
{
  if (theGroup.gic.isAbelian()==yes) {
    CommutatorIterator I(theGroup.getFPGroup().numberOfGenerators(), 2);
    SetOf<Word> newRelators = theGroup.getFPGroup().getRelators();
    for( I.reset(); !I.done(); I.next() ) newRelators |= I.value();
    FPGroup g(theGroup.getFPGroup().namesOfGenerators(), newRelators);
    arcer.setArguments(g);
  }
  else arcer.setArguments(theGroup.getFPGroup());
}

void AGSupervisor::takeControl( )
{
  if ( freeARCs() > 0 ) 
    if( arcer.takeControl() ) {
      Trichotomy answer = arcer.isAutomatic();
      theGroup.gic.putIsAutomatic(answer);
      {
			LogMessage msg( *this, theGroup );
			msg << theGroup.gic.getAutomaticMessage();
			msg.send();
      }
      if( answer == yes ) {
			File fileWA, fileDM1, fileDM2, fileGM;
			arcer.getWordAcceptor().printOn(fileWA);
			fileWA << end;
			arcer.getDiffMachine1().printOn(fileDM1);
			fileDM1 << end;
			arcer.getDiffMachine2().printOn(fileDM2);
			fileDM2 << end;
			arcer.getGeneralMultiplier().printOn(fileGM);
			fileGM << end;
			{
				LogMessage msg( *this, theGroup );
				msg << "Have a ShortLex automatic structure for " << Name( theGroup )
					 << ", discovered by KBMAG, Version 1.2.";
				msg.send();
			}
			{
				LogMessage msg( *this, theGroup );
				msg << Link("Click here to see a word acceptor.",
		      	"AutWordAcceptor", fileWA.getFileName(), false);
				msg.send(); 
			}
			{
	  			LogMessage msg( *this, theGroup );
	  			msg << Link("Click here to the first difference machine.",
		      	"AutDiffMachine1", fileDM1.getFileName(), false);
	  			msg.send(); 
			}
			{
	  			LogMessage msg( *this, theGroup );
	  				msg << Link("Click here to the second difference machine.",
		      		"AutDiffMachine2", fileDM2.getFileName(), false);
	  				msg.send(); 
			}
			{
	  			LogMessage msg( *this, theGroup );
	  				msg << Link("Click here to the general multiplier.",
		      		"AutGeneralMultiplier", fileGM.getFileName(), false);
	  				msg.send(); 
			}
			theGroup.gic.putHaveAutomatic( arcer.getWordAcceptor(), 
													arcer.getDiffMachine1() );
      }
      adminTerminate();
    }
    else usedOneARC();
}

// ---------------------------- AGProblem ---------------------------------- //

AGProblem::AGProblem(SMFPGroup& G)
  : Supervisor( true ),
    theGroup( G ),
    agSupervisor( *this, G.gcm().agSupervisor )
{ 
  if( theGroup.gic.isAutomatic() != dontknow ) resultIsFast();
}

void AGProblem::takeControl( )
{
  if( fastResult() ) {
    LogMessage( theGroup.gic.getAutomaticMessage() ).send();
    adminTerminate();
    return;
  }
  if( theGroup.gcm().agSupervisor->state() == ComputationManager::TERMINATED ) adminTerminate();
}

void AGProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Seek automatic structure"),
		  Text("Seek a ShortLex automatic structure for")
		  + Name( theGroup ), helpID( "AGProblem", theGroup ),
		  Text("Aut struct") );
  pv.startItemGroup();
  pv.add(Text("For the use of this problem"), agSupervisor.arcSlotID(), 100);
  pv.done();
}

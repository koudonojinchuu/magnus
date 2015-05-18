// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of classes AbelianInvariants, AbelInvariantsProb,
//           AbelianRank, AbelianWordProblem, AbelianIsomProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#include "AbelianInvariants.h"
#include "GCM.h"
#include "SMFPGroup.h"
#include "SMWord.h"
#include "SMSubgroup.h"
#include "viewStructure.h"
#include "AbelianInfinitenessProblem.h"
#include "OutMessages.h"


// ---------------------- AbelianInvariantsARCer --------------------------- //


void AbelianInvariantsARCer::setArguments( const FPGroup& group, 
					   bool makeFile )
{
  delete AG;
  AG = new AbelianGroup( group, makeFile );
}


void AbelianInvariantsARCer::runComputation( )
{
#if SAFETY > 0
  if( AG == 0 )
    error( "void AbelianInvariantsARCer::runComputation( ) : "
	   "The cyclic decomposition has not been computed yet");
#endif

  AG->computeCyclicDecomposition();
}


void AbelianInvariantsARCer::writeResults( ostream& out ) 
{
  out < *AG;
}


void AbelianInvariantsARCer::readResults( istream& in )
{
  in > *AG;
}


AbelianGroup AbelianInvariantsARCer::getCyclicDecomposition() 
{
#if SAFETY > 0
  if( AG == 0 )
    error( "AbelianGroup AbelianInvariantsARCer::getCyclicDecomposition() : "
	   "The cyclic decomposition has not been computed yet");
#endif
  
  return *AG;
}


// -------------------------- AbelianInvariants ---------------------------- //


AbelianInvariants::AbelianInvariants(GCM& gcm)
  : theGCM( gcm ),    
    arcer( *this )
{
  arcer.setArguments( theGCM.getSMFPGroup().getFPGroup(), true );
  adminStart();
}


void AbelianInvariants::takeControl( )
{
  if ( freeARCs() > 0 )

    if( arcer.takeControl() ) {
     
      theGCM.getSMFPGroup().gic.putHaveCyclicDecomposition
	( arcer.getCyclicDecomposition() );

      adminTerminate();
    }
    else
      usedOneARC();
}


// ---------------------- AbelianInvariantsOfFactor ------------------------ //


AbelianInvariantsOfFactor::AbelianInvariantsOfFactor(SCM& scm)
  : theSCM( scm ), 
    arcer( *this )
{
  FPGroup G( theSCM.getSubgroup().getParent().getFPGroup() );
  SetOf<Word> S;
  VectorOf<Word> v = theSCM.getSubgroup().getSubgroup().generators();
  int vLen = v.length();
  for( int i = 0; i < vLen; ++i )
    S |= v[i];
  G.addRelators(S);
  
  arcer.setArguments(G, true);
  adminStart();
}


void AbelianInvariantsOfFactor::takeControl( )
{
  if ( freeARCs() > 0 )
    
    if( arcer.takeControl() ) {
      
      theSCM.getSubgroup().sic.putHaveCyclicDecompositionOfFactor
	( arcer.getCyclicDecomposition() );
      
      adminTerminate();
    }
    else
      usedOneARC();
}


// ------------------------- AbelInvariantsProb ---------------------------- //


AbelInvariantsProb::AbelInvariantsProb(SMFPGroup& G)
  : Supervisor( true ),
	 theGroup( G ),
 	 abelianInvariants( *this, G.gcm().abelianInvariants )
{
  if( theGroup.gic.haveCyclicDecomposition() )
    resultIsFast();
}


void AbelInvariantsProb::takeControl( )
{
  if( fastResult() ) {
    LogMessage( theGroup.gic.getCyclicDecompositionMessage() ).send();
    adminTerminate();
    return;
  }

  if ( theGroup.gcm().abelianInvariants->state() == TERMINATED ) {
    LogMessage( *this, theGroup.gic.getCyclicDecompositionMessage(), 2 ).send();
    adminTerminate();
  }
}


void AbelInvariantsProb::viewStructure(ostream& ostr) const
{
  char *temp;
  
  if( theGroup.getCheckinType() == SMFPGroup::ABELIAN )
    temp = "";
  else 
    temp = " abelianized";
    
  ProblemView pv( ostr, oid(), Text("Compute the canonical decomposition"),
		  Text("Compute the canonical decomposition of")
		  + Name( theGroup ) + Text(temp),
		  helpID( "AbelInvariantsProb", theGroup ),
		  "Can dec"
		  );
  
  pv.startItemGroup();

  pv.add(Text("For the use of this problem"), abelianInvariants.arcSlotID(), 100);

  pv.done();
}


// -------------------------- AbelianRank ---------------------------------- //


AbelianRank::AbelianRank(GCM& gcm)
  : ComputationManager( false ),
	 theGroup( gcm.getSMFPGroup() ),
	 theProb( 0 )
{
  adminStart();
}


AbelianRank::~AbelianRank( ) { delete theProb; }


void AbelianRank::start()
{
  theProb = new AbelianInfinitenessProblem( theGroup.getFPGroup() );

  theProb->startComputation();
}


void AbelianRank::takeControl( )
{
  if ( freeARCs() > 0 )

    if ( theProb->done() ) {
      
      bool infinite = theProb->isInfinite();
      
      if( theGroup.gic.isAbelian() == yes || theGroup.gic.isNilpotent() == yes
	  || infinite ) {
	
	Chars text;
	
	if( theGroup.gic.isAbelian() == yes )
	  text = Text("this follows by computing the torsion-free rank of")
	    + Name( theGroup );
	else
	  text = Text("this follows by computing the torsion-free rank of"
		      " the abelianization of") + Name( theGroup );
	
	theGroup.gic.putIsFinite( !infinite, text );
      }
      
      delete theProb;
      theProb = 0;
      adminTerminate();
      
    }
    else {
      theProb->continueComputation();
      usedOneARC();
    }
}
// ---------------------- AbelianPrimesARCer --------------------------- //


void AbelianPrimesARCer::setArguments( const AbelianGroup& group )
{
  AG = new AbelianGroup( group );
}


void AbelianPrimesARCer::runComputation( )
{
#if SAFETY > 0
  if( AG == 0 )
    error( "void AbelianPrimesARCer::runComputation( ) : "
	   "You have not set the arguments yet");
#endif

  AG->findPrimaryBasis();
}


void AbelianPrimesARCer::writeResults( ostream& out ) 
{
  out < *AG;
}


void AbelianPrimesARCer::readResults( istream& in )
{
  in > *AG;
}


AbelianGroup AbelianPrimesARCer::getPrimaryDecomposition() 
{
#if SAFETY > 0
  if( AG == 0 )
    error( "AbelianGroup AbelianPrimesARCer::getPrimaryDecomposition() : "
	   	   "You have not set the arguments yet");
#endif
  
  return *AG;
}

// -------------------------- AbelianPrimes ---------------------------- //


AbelianPrimes::AbelianPrimes(GCM& gcm)
    : theGCM( gcm ),
      primesStarted( false),
      A(*this)
{
  adminStart();
}

void AbelianPrimes::takeControl( )
{
  if (freeARCs()>0){
   if (theGCM.getSMFPGroup().gic.haveCyclicDecomposition()){
     if (!primesStarted){
         A.setArguments( theGCM.getSMFPGroup().gic.getCyclicDecomposition());
         primesStarted = true;   
     }
    
     if( A.takeControl() ) {
        theGCM.getSMFPGroup().gic.putHavePrimaryDecomposition( A.getPrimaryDecomposition());
            LogMessage msg(*this,theGCM.getSMFPGroup());
            msg << "The primary decomposition of " << Name( theGCM.getSMFPGroup() ) << " :";
            A.getPrimaryDecomposition().printPrimaryDec(msg); 
            msg << ".";
            msg.send();       
            LogMessage msg1(*this,theGCM.getSMFPGroup());
            msg1 << Link(Chars( "Click here to see the generators of the primary decomposition of ")+
                          Text( Name(theGCM.getSMFPGroup()) ),
		          "PrimaryDecomposition",
			   A.getPrimaryDecomposition().getFileNameOfPDGens() );
	    msg1.send();  
            adminTerminate();
            return;
      } 
    }
    usedOneARC();
  }
}
// ------------------------- AbelianPrimeDecomp ---------------------------- //

AbelianPrimeDecomp::AbelianPrimeDecomp(SMFPGroup& G)
  : Supervisor( true ),
	 theGroup( G ),
         abDone(false),
         abelianPrimes( *this, G.gcm().abelianPrimes),
         abelianInvariants( *this, G.gcm().abelianInvariants )
{
  if( theGroup.gic.havePrimaryDecomposition( ) ){
    resultIsFast();
  }
  if (theGroup.gic.haveCyclicDecomposition() && !fastResult()){
    if (theGroup.gic.getCyclicDecomposition().isFree()){
      AbelianGroup tmpAG = theGroup.gic.getCyclicDecomposition();
      tmpAG.findPrimaryBasis();
      theGroup.gic.putHavePrimaryDecomposition( tmpAG );
      resultIsFast();
    }
    abDone = true;
  }
}


void AbelianPrimeDecomp::takeControl( )
{
  if (fastResult()){
     LogMessage msg;
     msg << "The primary decomposition of " << Name( theGroup ) << " :";
     theGroup.gic.getCyclicDecomposition().printPrimaryDec(msg); 
     msg << ".";
     msg.send( );
     adminTerminate();
     return;
  }     
   if (theGroup.gic.havePrimaryDecomposition( )){
     adminTerminate();
  }
}


void AbelianPrimeDecomp::viewStructure(ostream& ostr) const
{
  char *temp;
    
  ProblemView pv( ostr, oid(), Text("Compute the primary decomposition"),
		  Text("Compute the primary decomposition of")
		  + Name( theGroup ),
		  helpID("AbelianPrimeDecomp", theGroup),
		  "Prime dec"
		  );
  
  pv.startItemGroup();

  if (!abDone)
     pv.add(Text("Compute abelian invariants of") + Name( theGroup ),
                            abelianInvariants.arcSlotID(), 100);

     pv.add(Text("Find the primary decomposition of") + Name( theGroup ),
                            abelianPrimes.arcSlotID(), 100);

  pv.done();
}

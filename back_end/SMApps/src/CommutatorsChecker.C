// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes CommutatorsChecker,
//           CommutatorsCheckerARCer
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#include "CommutatorsChecker.h"
#include "CommutatorIterator.h"
#include "SMFPGroup.h"
#include "SMFPGroup.h"
#include "OutMessages.h"
#include "viewStructure.h"


// ------------------------- CommutatorsChecker ---------------------------- //


CommutatorsChecker::CommutatorsChecker( class SMFPGroup& group, int length)
  : G( group.getFPGroup() ),
    gic( group.gic ),
    gcm( group.gcm() ),
    triedAbelianization( false ),
    theLength( length ),
    generators( group.getFPGroup().numberOfGenerators() )
{
  int genLen = generators.length();
  for( int i = 0; i < genLen; ++i )
    generators[i] = Word(Generator(i+1));
}


CommutatorsChecker::CommutatorsChecker( class SMFPGroup& group, int length,
					const VectorOf<Word>& V )
  : G( group.getFPGroup() ),
    gic( group.gic ),
    gcm( group.gcm() ),
    triedAbelianization( false ),
    theLength( length ),
    generators( V )
{ }


Trichotomy CommutatorsChecker::areTrivial( )
{
  // We might consider something like storing a pointer to a member
  // function which does exactly one of the below, when we know that that
  // solves the problem. Trouble is, many of these are only theoretical
  // `solutions', and may not succeed in practice.

  CommutatorIterator I(generators, theLength);

  
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use one relator with torsion                                      //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( gic.isOneRelatorWithTorsion() == true ) {

    bool allTrivial = true;
    for( I.reset(); !I.done(); I.next() )
      if( G.shortenByRelators(I.value()).length() != 0 ) {
	allTrivial = false;
	break;
      }

    explanation = "by Dehn's algorithm (one relator with torsion)";
    return allTrivial;
  }

  
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use small cancellation                                            //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( gic.haveMSC() ) {

    bool allTrivial = true;
    for( I.reset(); !I.done(); I.next() )
      if( !gic.getMSC().wordProblem(I.value()) ) {
	allTrivial = false;
	break;
      }
    
    explanation = "by Dehn's algorithm (small cancellation)";
    return allTrivial;
  }
  

  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use the normal closure of the relators                            //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  // There are two cases:
    
  // 1) The group is finite, and a complete Cayley graph has been reported
  //    to the GIC.

  if ( gic.haveCompleteCayleyGraph() ) {
    // We get a definite answer.

    bool allTrivial = true;
    for( I.reset(); !I.done(); I.next() )
      if( !gic.getCompleteCayleyGraph().contains(I.value()) ) {
	allTrivial = false;
	break;
      }

    explanation = "by running through complete Cayley graph";
    return allTrivial;
  }
    
  // 2) Not 1, so we run the word through the existing normal closure
  //    approximation.

  if ( gcm.normalClosure.exists() ) {

    bool allTrivial = true;
    for( I.reset(); !I.done(); I.next() )
      if( gcm.normalClosure->isTrivial( I.value() ) != yes ) {
	allTrivial = false;
	break;
      }

    if( allTrivial ) {
      explanation = "labels loop in normal closure approximation";
      return yes;
    }
  }

  
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use the abelian quotient                                          //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( ! triedAbelianization && gic.haveCyclicDecomposition( ) ) {
    
    triedAbelianization = true;
    
    // If the group itself is abelian, we get a definite answer;
    // otherwise, we only succeed when the images are non-trivial.
	 
    if ( gic.isAbelian() == yes ) {
      
      bool allTrivial = true;
      for( I.reset(); !I.done(); I.next() )
	if( !gic.getCyclicDecomposition().isTrivial( I.value() ) ) {
	  allTrivial = false;
	  break;
	}
      
      explanation = "this follows from the canonical decomposition";
      return allTrivial;
    }
    else {

      bool allTrivial = true;
      for( I.reset(); !I.done(); I.next() )
	if( !gic.getCyclicDecomposition().isTrivial( I.value() ) ) {
	  allTrivial = false;
	  break;
	}
 
      if ( !allTrivial ) {
	explanation = "this follows from an inspection of the abelianization of the group";

	return no;
      }
    }
  }

  
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use complete KBMachine                                            //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( gic.haveConfluentKBMachine() ) {
    // Yay! But we have to ask the KBMachine for the answer

    bool allTrivial = true;
    for( I.reset(); !I.done(); I.next() ) {
      Word temp( I.value() );
      gic.getConfluentKBMachine().rewrite( temp );
      if( temp.length() != 0 ) {
	allTrivial = false;
	break;
      }
    }

    explanation = "according to the confluent rewrite system";
    return allTrivial;
  }

  
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use automatic structure                                           //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( gic.haveAutomatic() ) {
    // Yay! But we have to ask the DiffMachine for the answer

    bool allTrivial = true;
    for( I.reset(); !I.done(); I.next() ) {
      Word temp( I.value() );
      gic.getDiffMachine().rewrite( temp );
      if( temp.length() != 0 ) {
	allTrivial = false;
	break;
      }
    }
    
    explanation = "according to the difference machine";
    return allTrivial;
  }

  
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use any nilpotent quotients                                       //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use (possibly partial) KBMachines                                 //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Give up for now                                                   //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  return dontknow;
}


// ---------------------- CommutatorsCheckerARCer -------------------------- //


void CommutatorsCheckerARCer::setArguments(CommutatorsChecker* checker)
{
  theChecker = checker;
}


void CommutatorsCheckerARCer::runComputation( )
{
  retValue = theChecker->areTrivial();
}


void CommutatorsCheckerARCer::writeResults( ostream& out ) 
{
  out < (*theChecker) < retValue;
}


void CommutatorsCheckerARCer::readResults( istream& in )
{
  in > (*theChecker) > retValue;
}

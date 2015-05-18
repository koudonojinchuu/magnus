// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class SetOfWordsChecker
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#include "SetOfWordsChecker.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "conversions.h"
#include "ShortenByRelators2.h"

// ----------------------- SetOfWordsChecker ------------------------------- //


SetOfWordsChecker::SetOfWordsChecker( const VectorOf<Word>& words,
				      const SMFPGroup& group )
  : theWords( words ),
    G( group.getFPGroup() ),
    gic( group.gic ),
    gcm( group.gcm() ),
    triedAbelianization( false ),
    solutionAlgorithm( GIC::NONE ),
    keepDetails( false ),
    dehnsDetails( group.getFPGroup() ),
    wordsDecomposition( group.getFPGroup() )
{ 
  init();
}


SetOfWordsChecker::SetOfWordsChecker( const SetOf<Word>& words,
				      const class SMFPGroup& group )
  : G( group.getFPGroup() ),
    gic( group.gic ),
    gcm( group.gcm() ),
    triedAbelianization( false ),
    solutionAlgorithm( GIC::NONE ),
    theWords( makeVectorOf(words) ),
    dehnsDetails( group.getFPGroup() ),
    wordsDecomposition( group.getFPGroup() )
{
  init();
}


SetOfWordsChecker::SetOfWordsChecker( const class SMFPGroup& group )
  : G( group.getFPGroup() ),
    gic( group.gic ),
    gcm( group.gcm() ),
    triedAbelianization( false ),
    solutionAlgorithm( GIC::NONE ),
    dehnsDetails( group.getFPGroup() ),
    wordsDecomposition( group.getFPGroup() )
{ 
  init();
}

void SetOfWordsChecker::init()
{
  theTrivialWords = VectorOf<bool>( theWords.length() );
  for( int i = 0; i < theTrivialWords.length(); ++i ) {
    theTrivialWords[i] = false;
  }
}

void SetOfWordsChecker::replaceTheSet( const VectorOf<Word>& V) 
{
  theWords = V; 
  solutionAlgorithm = GIC::NONE;
  init();
}

void SetOfWordsChecker::replaceTheSet( const SetOf<Word>& words )
{
 replaceTheSet( makeVectorOf( words ) );
}

Trichotomy SetOfWordsChecker::isTrivial( )
{
  // We might consider something like storing a pointer to a member
  // function which does exactly one of the below, when we know that that
  // solves the problem. Trouble is, many of these are only theoretical
  // `solutions', and may not succeed in practice.

  int len = theWords.length();
  
  bool allTrivial = true;
  for( int i = 0; i < len; ++i )
    if( Word(theWords[i]).length() != 0 ) {
      allTrivial = false;
      break;
    }
  if ( allTrivial ) return yes;

  
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use one relator with torsion                                      //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( gic.isOneRelatorWithTorsion() == true ) {

    bool allTrivial = true;
    for( int i = 0; i < len; ++i )
      if( G.shortenByRelators(theWords[i]).length() != 0 ) {
	allTrivial = false;
	break;
      }
    
    if( len == 1 ) 
      dehnsDetails.printDehnTransformationDetails( theWords[0] );

    explanation = 
      "this follows by using Dehn's algorithm (the word problem for "
      "one-relator groups with torsion can be solved by using Dehn's "
      "algorithm)";

    return allTrivial;
  }

  
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use small cancellation                                            //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( gic.haveMSC() ) {

    bool allTrivial = true;
    for( int i = 0; i < len; ++i )
      if( !gic.getMSC().wordProblem(theWords[i]) ) {
	allTrivial = false;
	break;
      }

    if( len == 1 ) 
      dehnsDetails.printDehnTransformationDetails( theWords[0] );

    explanation = "by Dehn's algorithm (small cancellation)";
    return allTrivial;
  }
  

  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use free-by-cyclic structure                                      //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( gic.isFreeByCyclic() ) {

    bool allTrivial = true;
    for( int i = 0; i < len; ++i )
      if( gic.getFreeByCyclic().normalForm( theWords[i] ).length() > 0 ) {
	allTrivial = false;
	break;
      }

    explanation = "by normal form in free-by-cyclic";
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

    explanation = "by running through complete Cayley graph";

    bool allTrivial = true;
    bool header = ! wordsDecomposition.haveDetails();
    for( int i = 0; i < len; ++i ) {
      if( !theTrivialWords[i] ) {
	if( !gic.getCompleteCayleyGraph().contains( theWords[i] ) ) {
	  if( keepDetails ) {
	    wordsDecomposition.printNonTrivialWordDetails( 
              theWords[i], explanation, header );
	    header = false;
	  }
	  allTrivial = false;
	  break;
	}
	else if( keepDetails && gic.haveWordDecomposer() ) {
	  wordsDecomposition.printTrivialWordDetails( 
	    theWords[i], 
	    gic.getWordDecomposer().fullDecomposition(theWords[i]),
	    header
	    );
	  header = false;
	}
      }
    } // for( int i = ...

    return allTrivial;
  }
    
  // 2) Not 1, so we run the word through the existing normal closure
  //    approximation.

  if ( !keepDetails && gcm.normalClosure.exists() 
       || keepDetails && gic.haveWordDecomposer() ) {

    bool allTrivial = true;
    bool header = ! wordsDecomposition.haveDetails();
    for( int i = 0; i < len; ++i ) {
      if( !theTrivialWords[i] ) {
	if( gcm.normalClosure->isTrivial( theWords[i] ) != yes ) {
	  allTrivial = false;
	  break;
	}
	else {
	  if( keepDetails && gic.haveWordDecomposer() ) {
	    wordsDecomposition.printTrivialWordDetails( 
	      theWords[i], 
	      gic.getWordDecomposer().fullDecomposition(theWords[i]),
	      header
	      );
	    header = false;
	  }
	  theTrivialWords[i] = true;
	}
      }
    } // for(...

    if( allTrivial ) {
      solutionAlgorithm = GIC::NORM_CLOSURE;

      if( theWords.length() == 1 ) {
	//	printReductionDetailsByNormalClosure();
      }

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
      for( int i = 0; i < len; ++i )
	if( !gic.getCyclicDecomposition().isTrivial( theWords[i] ) ) {
	  allTrivial = false;
	  break;
	}

      solutionAlgorithm = GIC::AB_INV;
      explanation = "this follows from the canonical decomposition";
      return allTrivial;
    }
    else {

      bool allTrivial = true;
      for( int i = 0; i < len; ++i )
	if( !gic.getCyclicDecomposition().isTrivial( theWords[i] ) ) {
	  allTrivial = false;
	  break;
	}
 
      if ( !allTrivial ) {
	solutionAlgorithm = GIC::AB_INV;
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
    for( int i = 0; i < len; ++i ) {
      Word temp( theWords[i] );
      gic.getConfluentKBMachine().rewrite( temp );
      if( temp.length() != 0 ) {
	allTrivial = false;
	break;
      }
    }
    solutionAlgorithm = GIC::REWR_SYSTEM;
    explanation = "according to the confluent rewriting system";
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
    for( int i = 0; i < len; ++i ) {
      Word temp( theWords[i] );
      gic.getDiffMachine().rewrite( temp );
      if( temp.length() != 0 ) {
	allTrivial = false;
	break;
      }
    }

    solutionAlgorithm = GIC::AUT_STRUCTURE;
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

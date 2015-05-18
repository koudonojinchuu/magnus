// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes IsTrivialChecker, IsTrivialProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#include "IsTrivialProblem.h"
#include "SMFPGroup.h"
#include "OutMessages.h"
#include "viewStructure.h"
//#include "ORWordProblem.h"
#include "ORProblems.h"
#include "DecomposeInSubgroup.h"

// ------------------------ IsTrivialChecker ------------------------------- //


IsTrivialChecker::IsTrivialChecker( class SMFPGroup& group)
  : theGroup( group ),
    G( group.getFPGroup() ),
    checker( group.getFPGroup() ),
    gic( group.gic ),
    gcm( group.gcm() ),
    triedAbelianization( false ),
    triedPreliminaryCheckings( false ),
    trivialDetails( group.getFPGroup() )
{ }

Trichotomy IsTrivialChecker::preliminaryCheckings()
{
  SetOf<Word> S = G.getRelators();
  int numOfGens = G.numberOfGenerators();


  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Couple of fast checks                                             //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if( checker.hasTrivialPresentation() ) return yes;

  if( checker.numOfRelsLessNumOfGens() ) {
    explanation = "the number of relators is less than"
                  " the number of generators";
    return no;
  }


  Generator g;

  if( checker.existsGenWithExpSumZeroInEveryRelator(g) ) {
    
    //@njz
    //    ostrstream msg;
    std::ostrstream msg;
    //
    msg << "the exponent sum of generator ";
    G.printWord(msg, Word(g));
    msg << " is equal to 0 in every relator of the group" << ends;

    explanation = msg.str();
    return no;
  }
  
  for( int i = 0; i < G.numberOfGenerators(); ++i ) {
    Integer gcd = abs( checker.GCDOfExpSumOfGen( Generator(i+1) ) );
    if( gcd > 1 ) {

      //@njz
      //    ostrstream msg;
      std::ostrstream msg;
      //
      msg << "the greatest common divisor of exponent sums of generator ";
      G.printWord(msg, Word(Generator(i+1)));
      msg << " in the group relations is " << gcd << ends;
      
      explanation = msg.str();
      return no;
    }
  }
  
  
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use one relator                                                   //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( gic.isOneRelator() ) {
    explanation = "the group is one relator";
    return ORProblems(G.numberOfGenerators(), gic.getOneRelator()).isTrivial();
  }


  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use small cancellation                                            //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( !gic.haveMSC() ) {
    int lambda = G.cancellationLambda();
    if ( lambda == 0 || lambda >= 6 )
      gic.putHaveMSC( G, lambda );
  }
  
  if ( gic.haveMSC() ) {
    explanation = "the group is small cancellation";
    return gic.getMSC().isTrivial();
  }

  return dontknow;
}


Trichotomy IsTrivialChecker::isTrivial( )
{
  if( !triedPreliminaryCheckings ) {
    triedPreliminaryCheckings = true;
    Trichotomy prelResult = preliminaryCheckings();
    if ( prelResult != dontknow )
      return prelResult;
  }

  int numberOfGenerators = G.numberOfGenerators();


  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use the abelian quotient                                          //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( ! triedAbelianization && gic.haveCyclicDecomposition( ) ) {
	 
    triedAbelianization = true;
    
    // If the group itself is abelian, we get a definite answer;
    // otherwise, we only succeed when the image is non-trivial.
    
    if ( gic.isAbelian() == yes ) {
      explanation =
	FEData::Text("this follows from the canonical decomposition of")
   	+ FEData::Name(theGroup);
      return gic.getCyclicDecomposition().isTrivial();
    }
    else
      if ( gic.isNilpotent() == yes ) {
	explanation = "this follows from an inspection of the abelianization of the group";
	return gic.getCyclicDecomposition().isTrivial();
      }
      else
	if ( !gic.getCyclicDecomposition().isTrivial() ) {
	  explanation = "this follows from an inspection of the abelianization of the group";

	  return no;
	}
  } 


  if( theGroup.getCheckinType() != SMFPGroup::FP )
    return dontknow;
  
/* 
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use free-by-cyclic structure                                      //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( gic.isFreeByCyclic() ) {
	 explanation = "by normal form in free-by-cyclic";
	 return ( gic.getFreeByCyclic().normalForm( w ).length() == 0 );
  }
*/

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

    bool header = true;
    SubgroupGraph SG( gic.getCompleteCayleyGraph() );
    for( int i = 0; i < numberOfGenerators; ++i ) {
      if( !SG.contains( Generator(i+1) ) ) {
	trivialDetails.printNonTrivialGeneratorDetails( 
          Generator(i+1), explanation, header
	);
	
	return no;
      }
      else if( gic.haveWordDecomposer() ) {
	trivialDetails.printTrivialGeneratorDetails(
	  Generator( i+1 ),
	  gic.getWordDecomposer().fullDecomposition( Generator(i+1) ),
	  header
	);
	header = false;
      }
    }

    return yes;
  }
    
  // 2) Not 1, so we run the word through the existing normal closure
  //    approximation.

  if ( gcm.normalClosure.exists() && gic.haveWordDecomposer() ) {

    bool haveResult = true;
    for( int i = 0; i < numberOfGenerators; ++i ) {
      if( gcm.normalClosure->isTrivial( Generator(i+1) ) != yes ) {
	haveResult = false;
	break;
      }
    }

    if( haveResult ) {
      explanation = "labels loop in normal closure approximation";

      bool header = true;
      for( int i = 0; i < numberOfGenerators; ++i ) {
	trivialDetails.printTrivialGeneratorDetails( 
	  Generator(i+1), 
	  gic.getWordDecomposer().fullDecomposition( Generator(i+1) ),
	  header
	);
	header = false;
      }

      return yes;
    }
  }


  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use complete KBMachine                                            //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( gic.haveConfluentKBMachine() ) {
    explanation = "according to the confluent rewrite system";
    for( int i = 0; i < numberOfGenerators; ++i ) {
      Word temp( Generator(i+1) );
      gic.getConfluentKBMachine().rewrite( temp );
      if( temp.length() > 0 )
	return no;
    }
    return yes;
  }

  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use automatic structure                                           //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( gic.haveAutomatic() ) {
    explanation = "according to the difference machine";
    for( int i = 0; i < numberOfGenerators; ++i ) {
      Word temp( Generator(i+1) );
      gic.getDiffMachine().rewrite( temp );
      if( temp.length() > 0 )
	return no;
    }
    return yes;
  }

  return dontknow;
}


// ----------------------- IsTrivialProblem -------------------------------- //


IsTrivialProblem::IsTrivialProblem(class SMFPGroup& G)
  : Supervisor( ! G.gic.haveFastWordProblem() ),
    theGroup( G ),
    theChecker( G ),
    theAnswer( dontknow ),
    normalClosure( *this, G.gcm().normalClosure ),
    abelianInvariants( *this, G.gcm().abelianInvariants ),
    kbSupervisor( *this, G.gcm().kbSupervisor ),
    agSupervisor( *this, G.gcm().agSupervisor ),
    nilpotentQuotients( *this, G.gcm().nilpotentQuotients ),
    nilpotentWPInQuotients(*this),
    genetic( *this )
{
  SetOf<Word> comms;
  int numOfGens = theGroup.getFPGroup().numberOfGenerators();
  for( int i = 0; i < numOfGens; ++i ) {
    Word gen = Word(Generator(i+1));
    comms |= gen;
  }
  nilpotentWPInQuotients->initialize(comms,&theGroup);
  genetic->init(G.getFPGroup(), comms, GeneticWPCM::SET_OF_WORDS);
  if ( ! displayInFE() ) adminStart();
  if( (theAnswer = theChecker.isTrivial()) != dontknow ) resultIsFast();
}


void IsTrivialProblem::takeControl( )
{
  if ( ! displayInFE() || freeARCs() > 0 ) {
    if (nilpotentWPInQuotients->state() == ComputationManager::TERMINATED)
      if (nilpotentWPInQuotients->isTrivial()==no){
	int retClass;
	theAnswer = nilpotentWPInQuotients->isTrivial(retClass);
	//@njz
	//	ostrstream msgTmp;
	std::ostrstream msgTmp;
	//
	msgTmp << ": because it's not trivial in lower central quotient of class " 
	       << retClass << ends;
	Chars explanation = msgTmp.str();
	LogMessage msg( *this, theGroup );
	msg << Name( theGroup ) << " is ";
	if ( theAnswer == no ) msg << "not ";
	msg << "trivial" << explanation;
	msg.send();
	adminTerminate();     
	return;
      }
    if ( theAnswer != dontknow || 
	 ( theAnswer = theChecker.isTrivial() ) != dontknow ) {

      explanation = theChecker.getExplanation();
      LogMessage msg( *this, theGroup );
      msg << Name( theGroup ) << " is ";
      if ( theAnswer == no ) msg << "not ";
      msg << "trivial";

      if( explanation.length() > 0 )
	msg << ": " << explanation;

      msg << ".";
      msg.send();

      if( theChecker.haveDetails() ) {
	LogMessage msgLink( *this, theGroup );
	msgLink << Link(Chars("Click here to see a decomposition of "
			      "trivial generators of ") 
			+ Text(Name(theGroup))  
			+ Text(" as a product of conjugates of the relators."),
			"IsTrivialDetails", 
			theChecker.getDetailsFileName()
			);
	msgLink.send();
      }

      adminTerminate();
    } else
      if ( ( theAnswer = genetic->areTrivial() ) != dontknow ) {
	
	Chars explanation = "according to the genetic algorithm";
	LogMessage msg( *this, theGroup );
	msg << Name( theGroup ) << " is trivial";
	msg << " : " << explanation << ".";
	msg.send();
	adminTerminate();
      }
    else
      usedOneARC();
  }
}


void IsTrivialProblem::start( )
{

}


void IsTrivialProblem::terminate( )
{

}


void IsTrivialProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Triviality problem"),
		  Text("Is") + Name( theGroup ) + "trivial ?",
		  helpID( "IsTrivialProblem", theGroup ), 
		  Text(Name(theGroup)) + "= 1 ?"
		  );

  
  pv.startItemGroup();

  pv.add( Text("For the use of this problem"), ThisARCSlotID(), 50
	  );


  pv.startItemGroup( CheckinType( theGroup ) == SMFPGroup::ABELIAN
		     );
  
  pv.add( Text("Compute abelian invariants of") + Name( theGroup ),
	  abelianInvariants.arcSlotID(),
	  15
	  );


  pv.startItemGroup( CheckinType( theGroup ) != SMFPGroup::ABELIAN
		     );
  
  pv.add( Text("Is abelianization of") + Name( theGroup )
	  + Text("trivial ?"),
	  abelianInvariants.arcSlotID(),
	  15
	  );

  
  pv.startItemGroup( CheckinType( theGroup ) == SMFPGroup::FP
		     );
 
  pv.add( Text("Enumerate normal closure of relators"),
	  normalClosure.arcSlotID(), 50);

  
  pv.add( Text("Seek a rewriting system for") + Name( theGroup ),
	  kbSupervisor.arcSlotID(),
	  50
	  );
  
  
  pv.add( Text("Seek a ShortLex automatic structure for")
	  + Name( theGroup ),
	  agSupervisor.arcSlotID(),
	  50
	  );

  pv.add( Text("Run genetic algorithm"), genetic.arcSlotID(), 50 );
  
  int upperBound, currentClass = 2;
  if (theGroup.gic.isNilpotent()==yes)
    upperBound = theGroup.gic.getNilpotentcyClass()-1;
  else
    upperBound = 10;
  
  while (currentClass <= upperBound && 
	 theGroup.gic.haveNilpotentQuotInited( currentClass ) )
    currentClass++;
  
  if (!(currentClass > upperBound) )
    pv.add(Text("Compute lower central quotients for") + Name( theGroup ),
	   nilpotentQuotients.arcSlotID(),
	   50
	   );
  
  pv.add(Text("Solve using lower central quotients of")
	 + Name( theGroup ),
	 nilpotentWPInQuotients.arcSlotID(),
	 50
	 );
  
  
  pv.done();
}

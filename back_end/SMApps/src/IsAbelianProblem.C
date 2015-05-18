// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes IsAbelianChecker, IsAbelianProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#include "IsAbelianProblem.h"
#include "CommutatorIterator.h"
#include "SMFPGroup.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "ORProblems.h"
#include "DecomposeInSubgroup.h"

// ------------------------- IsAbelianChecker ------------------------------ //


IsAbelianChecker::IsAbelianChecker(class SMFPGroup& group)
  : G( group.getFPGroup() ),
    gic( group.gic ),
    gcm( group.gcm() ),
    triedAbelianization( false ),
    triedOneRelator( false ),
    abelianDetails( group.getFPGroup() )
{ }


Chars IsAbelianChecker::commutator( VectorOf<int> components )
{

  //@njz
  //  ostrstream msg;
  std::ostrstream msg;
  //
  msg << "\\[";

  int len = components.length();
  for( int i = 0; i < len; ++i ) {
    G.printWord( msg, Word(Generator(components[i])) );
    if( i != len-1 )
      msg << ",";
  }

  msg << "\\]" << ends;
  return msg.str();
}

Trichotomy IsAbelianChecker::isAbelian( )
{
  int numOfGens = G.numberOfGenerators();

  if( (numOfGens - G.getRelators().cardinality()) > 1 ) {
    
    explanation = "the group contains non-abelian free subgroup"; 
    return no;
  }


  CommutatorIterator I(numOfGens, 2);

  
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use one relator with torsion                                      //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( gic.isOneRelatorWithTorsion() ) {

    bool allTrivial = true;
    for( I.reset(); !I.done(); I.next() )
      if( G.shortenByRelators(I.value()).length() != 0 ) {
	allTrivial = false;
	break;
      }

    explanation = "according to Dehn's algorithm (one relator with torsion) ";

    if( allTrivial ) 
      explanation += "all commutators are trivial";
    else 
      explanation += Chars("the commutator ") + commutator(I.components()) 
	+ " is not trivial";
    
    return allTrivial;
  }


  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use one relator                                                   //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////
  
  if ( gic.isOneRelator() ) {

    bool result = ORProblems( numOfGens, gic.getOneRelator() ).isAbelian();

    if( numOfGens <= 1)
      explanation = "number of generators of the one relator group is less"
	" than two";
    else if( numOfGens > 2)
      explanation = "number of generators of the one relator group is greater"
	" than two";
    else
      if( result ) 
	explanation = "all commutators are trivial";
      else {
	VectorOf<int> v(2);
	v[0] = 1; v[1] = 2;
	explanation = Chars("the commutator ") + commutator(v) 
	  + " is not trivial";
      }
    
    return result;
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
    
    explanation = "according to Dehn's algorithm (small cancellation) ";
    
    if( allTrivial ) 
      explanation += "all commutators are trivial";
    else 
      explanation += Chars("the commutator ") + commutator(I.components()) 
	+ " is not trivial";

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


    explanation = "according to complete Cayley graph ";

    bool allTrivial = true;
    bool header = true;

    for( I.reset(); !I.done(); I.next() ) {

      if( !gic.getCompleteCayleyGraph().contains(I.value()) ) {

	explanation += 
	  Chars("the commutator ") + commutator(I.components()) 
	  + " is not trivial";
	
	abelianDetails.printNonTrivialCommutatorDetails( 
          commutator(I.components()), explanation, header 
	);

	allTrivial = false;
	break;
      }
      else if( gic.haveWordDecomposer() ) {
	abelianDetails.printTrivialCommutatorDetails(
	  commutator(I.components()), 
	  gic.getWordDecomposer().fullDecomposition(I.value()),
	  header
	);
	header = false;
      }
    }
    
    if( allTrivial ) 
      explanation += "all commutators are trivial";

    return allTrivial;
  }
    
  // 2) Not 1, so we run the word through the existing normal closure
  //    approximation.

  if ( gcm.normalClosure.exists() && gic.haveWordDecomposer() ) {

    bool allTrivial = true;
    for( I.reset(); !I.done(); I.next() )
      if( gcm.normalClosure->isTrivial( I.value() ) != yes ) {
	allTrivial = false;
	break;
      }

    if( allTrivial ) {
      explanation = "according to normal closure approximation "
	"all commutators are trivial";

      bool header = true;
      for( I.reset(); !I.done(); I.next() ) {
	abelianDetails.printTrivialCommutatorDetails(
	  commutator(I.components()), 
	  gic.getWordDecomposer().fullDecomposition(I.value()),
	  header
	);
	header = false;
      }

      return yes;
    }
  }

  //@db need to replace abelianization by nilpotent quotients
  //@am abelianization is used in nilpotent groups: if abelianization
  // iz cyclic, then nilpotent group is abelian
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use the abelian quotient                                          //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( ! triedAbelianization && gic.haveCyclicDecomposition( ) ) {
    
    triedAbelianization = true;
    
    if ( gic.isFreeNilpotent() == yes ||
	 gic.isNilpotent() == yes) {
      const AbelianGroup& ag = gic.getCyclicDecomposition( );
      if ((ag.rankOfFreeAbelianFactor() == 0 && ag.invariants().length() ==1) ||
	  (ag.rankOfFreeAbelianFactor() == 1 && ag.invariants().length() ==0)){
	gic.putActualNilpotencyClass(1);
      explanation = "this follows from the fact that it's abelianization is cyclic";
	return true;
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

    explanation = "according to the confluent rewriting system ";

    if( allTrivial ) 
      explanation += "all commutators are trivial";
    else 
      explanation += Chars("the commutator ") + commutator(I.components()) 
	+ " is not trivial";
    
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
    
    explanation = "according to the difference machine ";

    if( allTrivial ) 
      explanation += "all commutators are trivial";
    else 
      explanation += Chars("the commutator ") + commutator(I.components()) 
	+ " is not trivial";
    
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


// -------------------------- IsAbelianProblem ---------------------------- //


IsAbelianProblem::IsAbelianProblem(SMFPGroup& G)
  : Supervisor( ! G.gic.haveFastWordProblem() ),
    theGroup( G ),
    theChecker( G ),
    normalClosure( *this, G.gcm().normalClosure ),
    abelianInvariants( *this, G.gcm().abelianInvariants ),
    kbSupervisor( *this, G.gcm().kbSupervisor ),
    agSupervisor( *this, G.gcm().agSupervisor ),
    nilpotentQuotients(*this, G.gcm().nilpotentQuotients),
    nilpotentWPInQuotients(*this),
    computeBasis(*this, G.gcm().computeBasis),
    nilpotentWP(*this),
    genetic( *this )
{
  nilpotentWPInQuotients->initialize(2,&theGroup);
  nilpotentWP->initialize(2,&theGroup);

  SetOf<Word> comms;
  CommutatorIterator I(G.getFPGroup().numberOfGenerators(), 2);
  for( ; !I.done(); I.next() ) comms |= I.value();
  genetic->init(G.getFPGroup(), comms, GeneticWPCM::COMMUTATORS);
  
  if ( ! displayInFE() ) adminStart();
}


void IsAbelianProblem::takeControl( )
{
  Trichotomy theAnswer = dontknow;
  Chars explanation;

  if ( ! displayInFE() || freeARCs() > 0 ) {
    if (nilpotentWP->state() == ComputationManager::TERMINATED){
      theAnswer = nilpotentWP->isTrivial();
      explanation = ": according to an inspection of a basis, ";
      if( theAnswer ) 
	explanation += Chars("all commutators are trivial.");
      else 
	explanation += Chars("the commutator ") + 
	  theChecker.commutator(nilpotentWP->getCommutator())+ 
	  " is not trivial.";
      
    }
    if (nilpotentWPInQuotients->state() == ComputationManager::TERMINATED)
      if (nilpotentWPInQuotients->isTrivial()==no){
	int retClass;
	theAnswer = nilpotentWPInQuotients->isTrivial(retClass);
	//@njz
	//	ostrstream msgTmp;
	std::ostrstream msgTmp;
	//
	msgTmp << ": because the commutator " << 
	  theChecker.commutator(nilpotentWPInQuotients->getCommutator()) << 
	  " is not trivial in lower central quotient of class " 
	       << retClass << ends;
	explanation = msgTmp.str();
      }    
    if (theAnswer != dontknow){
 	LogMessage msg( *this, theGroup );
	msg << Name( theGroup ) << " is ";
	if ( theAnswer == no ) msg << "not ";
	msg << "abelian" << explanation;
	msg.send();
	if( theAnswer == yes ) theGroup.gic.putIsAbelian();
	//if (theGroup.gic.isNilpotent())
	//theGroup.gic.putActualNilpotencyClass(1);
	adminTerminate();     
	return;     
    }

    if ( ( theAnswer = theChecker.isAbelian() ) != dontknow ) {

      LogMessage msg( *this, theGroup );
      msg << Name( theGroup ) << " is ";
      if ( theAnswer == no ) msg << "not ";
      msg << "abelian";

      explanation = theChecker.getExplanation();
      if( explanation.length() > 0 )
	msg << " : " << explanation;

      msg << ".";
      msg.send();

      if( theChecker.getDetailsFileName().length() > 0 ) {
	LogMessage msgLink( *this, theGroup );
	msgLink << Link(Chars("Click here to see a decomposition of "
			      "trivial commutators of ") 
			+ Text(Name(theGroup))  
			+ Text(" as a product of conjugates of the relators."),
			"IsAbelianDetails", 
			theChecker.getDetailsFileName()
			);
	msgLink.send();
      }

      if( theAnswer == yes ) theGroup.gic.putIsAbelian();
      //if (theGroup.gic.isNilpotent())
      //theGroup.gic.putActualNilpotencyClass(1);
      adminTerminate();
    } else
      
      if ( ( theAnswer = genetic->areTrivial() ) != dontknow ) {
	
	LogMessage msg( *this, theGroup );
	msg << Name( theGroup ) << " is ";
	msg << "abelian";
	Chars explanation = "according to the genetic algorithm";
	msg << " : " << explanation << ".";
	msg.send();
	if( theAnswer == yes ) theGroup.gic.putIsAbelian();
	adminTerminate();
      }
    
      else
	usedOneARC();
  }
}


void IsAbelianProblem::start( ) { }


void IsAbelianProblem::terminate( ) { }


void IsAbelianProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Is abelian problem"),
		  Text("Is") + Name( theGroup ) + "abelian ?",
		  helpID("IsAbelianProblem", theGroup),
		  Text(Name(theGroup)) + "abelian ?"
		  );
  

  pv.startItemGroup( CheckinType(theGroup) == SMFPGroup::FP ||
		     CheckinType(theGroup) == SMFPGroup::NILPOTENT
		     );
  
  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 50);


  if ( (theGroup.gic.isFreeNilpotent() == yes ||
	theGroup.gic.isNilpotent() == yes) && 
       !theGroup.gic.haveCyclicDecomposition()) 
    
    pv.add(Text("Compute abelian invariants of") + Name(theGroup)
	   + "abelianized",
	   abelianInvariants.arcSlotID(),
	   15
	   );
  

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
 
  pv.startItemGroup( CheckinType(theGroup) == SMFPGroup::FP);
  
  pv.add(Text("Enumerate normal closure of relators"),
	 normalClosure.arcSlotID(), 50);
  
  pv.add(Text("Seek a rewriting system for") + Name(theGroup),
	 kbSupervisor.arcSlotID(),
	 50
	 );
  
  pv.add(Text("Seek a ShortLex automatic structure for")
	 + Name(theGroup),
	 agSupervisor.arcSlotID(),
	 50
	 );

  pv.add(Text("Run genetic algorithm"),
	 genetic.arcSlotID(), 50);

  pv.startItemGroup( CheckinType(theGroup) == SMFPGroup::NILPOTENT);

  if (!theGroup.gic.haveNilpotentQuotInited(theGroup.gic.getNilpotentcyClass()))
    pv.add(Text("Compute a basis of ") + Name( theGroup ),
	   computeBasis.arcSlotID(),
	   50
	   );
  
  pv.add(Text("Solve using a basis of")
	 + Name( theGroup ),
	 nilpotentWP.arcSlotID(),
	 50
	 );
  
  pv.done();
}

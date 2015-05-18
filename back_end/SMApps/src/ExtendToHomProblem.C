// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes ExtendToHomChecker, ExtendToHomProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#include "ExtendToHomProblem.h"
#include "SMHomomorphism.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "OneRelatorGroup.h"

// ------------------------ ExtendToHomChecker ----------------------------- //


ExtendToHomChecker::ExtendToHomChecker(ExtendToHomProblem& boss,
					class SMMap& map)
  : theMap( map ),
    theBoss(boss),
    theDomain( map.getDomain() ),
    theRange( map.getRange() ),
    M( map.getMap() ),
    G1( map.getDomain().getFPGroup() ),
    G2( map.getRange().getFPGroup() ),
    gic1( map.getDomain().gic ),
    gcm1( map.getDomain().gcm() ),
    gic2( map.getRange().gic ),
    gcm2( map.getRange().gcm() ),
    triedAbelianization( false ),
    theClass( map.getRange().gic.getNilpotentcyClass() ),
    relators( getAllRelators(theDomain) )
//    triedPreliminaryCheckings( false )
{ }


Trichotomy ExtendToHomChecker::preliminaryCheckings()
{
  int numOfGens1 = G1.numberOfGenerators();
  SetOf<Word> S1 = G1.getRelators();
  SetIterator<Word> I1(S1);

  int numOfGens2 = G2.numberOfGenerators();
//  SetOf<Word> S2 = G2.getRelators();
//  SetIterator<Word> I2(S2);

/*
  if( numOfGens1 == 0 || numOfGens2 == 0 )
    return yes;
*/

  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Free Group:                                                       //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if( theDomain.getCheckinType() == SMFPGroup::FREE ) {
    //@njz
    //    explanation = FEData::Text("this follows from the fact that the given"
    //      " generators freely generate") + FEData::Name(theDomain);
    explanation = FEData::Text("this follows from the fact that the given generators freely generate") + FEData::Name(theDomain);
    //
    return yes;
  }

  if( theRange.getCheckinType() == SMFPGroup::FREE ) {
    
    FreeGroup F = theRange.getFreePreimage();

    // Check images of all relators

    for( I1.reset(); !I1.done();  I1.next() )
      if( F.wordProblem( M.imageOf(I1.value()) ) == no )
	return no;

    if( gic1.isAbelian() == yes || gic1.isNilpotent() == yes || 
	gic1.isFreeNilpotent() == yes) {

      // In case abelian or nilpotent group images of all generators
      // of G1 must be powers with the same root.
      
      Word image = M.generatingImages(0);
      Word root = image.initialSegment( image.length()/maximalRoot( image ) );
      
      for( int i = 1; i < numOfGens1; ++i ) {
	image = M.generatingImages(i);
	Word tmp = image.initialSegment( image.length()/maximalRoot( image ) );
	if( tmp != root )
	  return no;
      }
    }

    return yes;
  }

  
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Map to MSC Group:                                                 //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if( gic2.haveMSC() ) {
    
    // Check images of all relators first
    
    for( I1.reset(); !I1.done();  I1.next() )
      if( gic2.getMSC( ).wordProblem( M.imageOf(I1.value()) ) == no )
	return no;
    
    if( gic1.isAbelian() == yes || gic1.isNilpotent() == yes || 
	gic1.isFreeNilpotent() == yes) {
      
      // In case abelian or nilpotent group all images
      // of generators of G1 have to commute in G2
      
      VectorOf<Word> V = M.generatingImages();
      for( int i = 0; i < numOfGens1; ++i )
	for( int j = 0; j < numOfGens1; ++j )
	  if( i != j ) {
	    Word comm = ( Word(V[i]).inverse() * Word(V[j]).inverse()
			  * V[i] * V[j] );
	    if( gic2.getMSC( ).wordProblem(comm) == no )
	      return no;
	  }
    }
    
    return yes;
  }

  
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Map to OR Group:                                                  //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if( gic2.isOneRelator() ) {
    
    // ORWP should work swiftly in this case.
    OneRelatorGroup orwp( G2.namesOfGenerators(), gic2.getOneRelator() );

    // Check images of all relators first
    
    for( I1.reset(); !I1.done();  I1.next() ) {
      
      if( !orwp.wordProblem( M.imageOf(I1.value()).freelyReduce() ) ) 
	return no;
    }
    
    if( gic1.isAbelian() == yes || gic1.isNilpotent() == yes || 
	gic1.isFreeNilpotent() == yes) {
      
      // In case abelian or nilpotent group all images
      // of generators of G1 have to commute in G2
      
      VectorOf<Word> V = M.generatingImages();
      for( int i = 0; i < numOfGens1; ++i )
	for( int j = 0; j < numOfGens1; ++j )
	  if( i != j ) {
	    Word comm = ( Word(V[i]).inverse() * Word(V[j]).inverse()
			  * V[i] * V[j]);
	    if( !orwp.wordProblem( comm.freelyReduce() ) )
	      return no;
	  }
    }
    
    return yes;
  }


  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Abelian Group:                                                    //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if( gic1.isAbelian() == yes && gic2.isAbelian() == yes
      && S1.cardinality() == 0 ) // free abelian -> abelian
    return yes;
  
/*  
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Nilpotent Group:                                                  //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if( gic1.isNilpotent() == yes )
    error( "Trichotomy ExtendToHomChecker::preliminaryCheckings() : "
	   "Nilpotent case is not implemented yet.");
*/  
  
  return dontknow;
}


SetOf<Word> ExtendToHomChecker::getAllRelators( class SMFPGroup& group ) const
{
  if( group.gic.isAbelian() == yes )
    return AbelianGroup( group.getFPGroup() ).getAllRelators();
  else
    return group.getFPGroup().getRelators();
}


bool ExtendToHomChecker::checkRelatorsInNilpotent() const
{
  // The target group has to be nilpotent.
  
  SetIterator<Word> I(relators);
  NilpotentGroup NG( gic2.getNilpotentGroupInited() );
  for( ; !I.done(); I.next() ) {
    if( !NG.wordProblem(I.value()) )
      return false;
  }
  
  int class1;
  if( (gic1.isNilpotent() == yes || gic1.isFreeNilpotent() == yes)&&
      (class1 = gic1.getNilpotentcyClass()) < theClass ) {
    CommutatorIterator I(G1.numberOfGenerators(), class1 + 1);
    for( ; !I.done(); I.next() ) {
      if( !NG.wordProblem(I.value()) )
	return false;
    }
  }

  return true;
}


Trichotomy ExtendToHomChecker::doesExtendToHom( )
{
/*
  if( !triedPreliminaryCheckings ) {
    triedPreliminaryCheckings = true;
    Trichotomy prelResult = preliminaryCheckings();
    if ( prelResult != dontknow )
      return prelResult;
  }
*/
  int numOfGens1 = G1.numberOfGenerators();
  SetOf<Word> S1 = G1.getRelators();
  SetIterator<Word> I1(S1);

  int numOfGens2 = G2.numberOfGenerators();
//  SetOf<Word> S2 = G2.getRelators();
//  SetIterator<Word> I2(S2);

  SetIterator<Word> I(relators);

  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use the abelian quotient                                          //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( ! triedAbelianization && gic2.haveCyclicDecomposition( ) ) {
    
    triedAbelianization = true;
    const AbelianGroup& theGroup = gic2.getCyclicDecomposition();
    
    // If the group itself is abelian, we get a definite answer;
    // otherwise, we only succeed when the all images are non-trivial.
    
    if ( gic2.isAbelian() == yes ) {

      bool answer = yes;
      for( I1.reset(); !I1.done(); I1.next() )
	if( !theGroup.isTrivial( M.imageOf(I1.value()) ) ) {
	  answer = no;
	  break;
	}
     
//      explanation = "the canonical decomposition";
      return answer;
    }

    bool answer = yes;
    for( I.reset(); !I.done(); I.next() )
      if( !theGroup.isTrivial( M.imageOf(I.value()) ) ) {
	answer = no;
	break;
      }
    
    if( !answer ) {
//      explanation = "by mapping into the abelianization";
      return answer;
    }
  }


  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Nilpotent case                                                    //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  /*  if( gic2.isNilpotent() == yes && gic2.haveNilpotentQuot(theClass) ){

    if( !bNilpStarted ) {
      bNilpStarted = true;
      arcer.setArguments(this);
    }
    else
      if( arcer.takeControl() )
	return arcer.getRetValue();
  }
  */
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use nilpotent quotients                                           //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////
  if (theBoss.nilpWPInQuotAnswer() != dontknow ){
    //    explanation = "because of computed nilpotent quotients";
    return theBoss.nilpWPInQuotAnswer();
  }
  if (theBoss.nilpWPAnswer() != dontknow ){
    //    explanation = "because of computed nilpotent structure";
    return theBoss.nilpWPAnswer();
  }


  if( theRange.getCheckinType() != SMFPGroup::FP )
    return dontknow;
  

  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use the normal closure of the relators                            //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  // There are two cases:
    
  // 1) The group is finite, and a complete Cayley graph has been reported
  //    to the GIC.

  if ( gic2.haveCompleteCayleyGraph() ) {

    // We get a definite answer.
//    explanation = "by running through complete Cayley graph";
    SubgroupGraph SG = gic2.getCompleteCayleyGraph();

    for( I.reset(); !I.done(); I.next() )
      if( !SG.contains( (M.imageOf(I.value())).freelyReduce() ) )
	return no;
    
    if( gic1.isNilpotent() == yes || gic1.isFreeNilpotent() == yes) {
      CommutatorIterator CI(numOfGens1, gic1.getNilpotentcyClass());
      for( CI.reset(); !CI.done(); CI.next() )
	if( !SG.contains( (M.imageOf(CI.value())).freelyReduce() ) )
	  return no;
    }
    
    return yes;
  }
    
  // 2) Not 1, so we run the word through the existing normal closure
  //    approximation.

  if ( gcm2.normalClosure.exists() ) {
    
    bool bAnswer = true;
    for( I.reset(); !I.done(); I.next() )
      if( (gcm2.normalClosure->
	    isTrivial( (M.imageOf(I.value())).freelyReduce() ) ) != yes ) {
	bAnswer = false;
	break;
      }
    
    if( bAnswer && (gic1.isNilpotent() == yes || 
		    gic1.isFreeNilpotent() == yes)) {
      CommutatorIterator CI(numOfGens1, gic1.getNilpotentcyClass());
      for( CI.reset(); !CI.done(); CI.next() )
	if( (gcm2.normalClosure->
	      isTrivial( (M.imageOf(CI.value())).freelyReduce() ) )!=yes ) {
	  bAnswer = false;
	  break;
	}
    }
    
    if( bAnswer ) {
//      explanation = "labels loop in normal closure approximation";
      return yes;
    }
  }


  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use complete KBMachine                                            //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( gic2.haveConfluentKBMachine() ) {

//    explanation = "according to the confluent rewrite system";

    for( I.reset(); !I.done(); I.next() ) {
      Word temp( (M.imageOf(I.value())).freelyReduce() );
      gic2.getConfluentKBMachine().rewrite( temp );
      if( temp.length() != 0 )
	return no;
    }

    if( gic1.isNilpotent() == yes || gic1.isFreeNilpotent() == yes) {
      CommutatorIterator CI(numOfGens1, gic1.getNilpotentcyClass());
      for( CI.reset(); !CI.done(); CI.next() ) {
	Word temp( (M.imageOf(CI.value())).freelyReduce() );
	gic2.getConfluentKBMachine().rewrite( temp );
	if( temp.length() != 0 )
	  return no;
      }
    }

    return yes;
  }


  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use automatic structure                                           //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( gic2.haveAutomatic() ) {

//    explanation = "according to the difference machine";

    for( I.reset(); !I.done(); I.next() ) {
      Word temp( (M.imageOf(I.value())).freelyReduce() );
      gic2.getDiffMachine().rewrite( temp );
      if( temp.length() != 0 )
	return no;
    }
    
    if( gic1.isNilpotent() == yes || gic1.isFreeNilpotent() == yes) {
      CommutatorIterator CI(numOfGens1, gic1.getNilpotentcyClass());
      for( CI.reset(); !CI.done(); CI.next() ) {
	Word temp( (M.imageOf(CI.value())).freelyReduce() );
	gic2.getDiffMachine().rewrite( temp );
	if( temp.length() != 0 )
	  return no;
      }
    }
    
    return yes;
  }

  return dontknow;
}


// ----------------------- ExtendToHomProblem ------------------------------ //


ExtendToHomProblem::ExtendToHomProblem(class SMMap& map)
  : Supervisor( ! map.getRange().gic.haveFastWordProblem() ),
    theMap( map ),
    theDomain( map.getDomain() ),
    theRange( map.getRange() ),
    theChecker( *this, map ),
    theAnswer( dontknow ),
    normalClosure( *this, map.getRange().gcm().normalClosure ),
    abelianInvariants( *this, map.getRange().gcm().abelianInvariants ),
    kbSupervisor( *this, map.getRange().gcm().kbSupervisor ),
    agSupervisor( *this, map.getRange().gcm().agSupervisor ),
    nilpotentQuotients( *this, map.getRange().gcm().nilpotentQuotients ),
    nilpotentWPInQuotients(*this),
    computeBasis(*this, map.getRange().gcm().computeBasis),
    nilpotentWP(*this),
    genetic(*this),
    theClass( theRange.gic.getNilpotentcyClass() )
{
  SetIterator<Word> I(theDomain.getFPGroup().getRelators());
  SetOf<Word> comms;
  Map m = theMap.getMap();
  for( I.reset(); !I.done(); I.next() )
    comms |=  (m.imageOf(I.value())).freelyReduce();
  nilpotentWPInQuotients->initialize(comms,&theRange);
  nilpotentWP->initialize(comms,&theRange);
  genetic->init(theRange.getFPGroup(), comms, GeneticWPCM::SET_OF_WORDS);

  if ( ! displayInFE() ) adminStart();
  if( theMap.mic.doesExtendToHom() != dontknow )
    resultIsFast();
  if( (theAnswer = theChecker.preliminaryCheckings()) != dontknow )
    resultIsFast();
  if( theRange.gic.isAbelian() == yes 
      && theRange.gic.haveCyclicDecomposition() )
    resultIsFast();
}
  
Trichotomy ExtendToHomProblem::nilpWPAnswer( ) 
{
  if (nilpotentWP->state() == ComputationManager::TERMINATED){
    Trichotomy answer = nilpotentWP->isTrivial();
    return answer;
  }
  else
    return dontknow;
}

Trichotomy ExtendToHomProblem::nilpWPInQuotAnswer( ) 
{
  if (nilpotentWPInQuotients->state() == ComputationManager::TERMINATED)
    if (nilpotentWPInQuotients->isTrivial()==no)
      return no;
  return dontknow;
}

void ExtendToHomProblem::sendResultMessage( Chars explanation )
{
  if( explanation.length() == 0 )
    if( theAnswer == yes )
      explanation = "the images of the generators satisfy the given relations";
    else
      explanation = "the images of the generators do not"
	" satisfy the given relations";

  theMap.mic.putDoesExtendToHom(theAnswer, explanation);
  
  if( theAnswer == yes ) {
    
    Chars text = Text("The extension of") + Name(theMap) + 
      Text("to an homomorphism of") + Name( theDomain ) + "to"
      + Name( theRange );
      
    if( theDomain.oid() == theRange.oid() ) {
      SMHomomorphism* smo = new SMHomomorphism(theMap, text);
      ListOf<OID> dependencies( theMap.getDomain() );
      CheckinMessage( *smo, "", dependencies ).send();
    }
    else {
      SMHomomorphism2* smo = new SMHomomorphism2(theMap, text);
      ListOf<OID> dependencies( theMap.getDomain() );
      dependencies.append( theMap.getRange() );
      CheckinMessage( *smo, "", dependencies ).send();
    }
  }
}


void ExtendToHomProblem::takeControl( )
{
  if( theMap.mic.doesExtendToHom() != dontknow ) {
    LogMessage(theMap.mic.getExtendToHomMessage()).send();
    return;
  }

  if( theAnswer != dontknow ) {
    sendResultMessage(theChecker.getExplanation());
//    adminTerminate();
    return;
  }

  if ( !displayInFE() || freeARCs() > 0 ) {
    if ( ( theAnswer = theChecker.doesExtendToHom() ) != dontknow ) {
      sendResultMessage(theChecker.getExplanation());
      adminTerminate();
    }
    else if ( ( theAnswer = genetic->areTrivial() ) != dontknow ) {
      explanation = "according to the genetic algorithm";
      sendResultMessage(explanation);
      adminTerminate();
    }    
    else
      usedOneARC();
  }
}


void ExtendToHomProblem::start( ) { }


void ExtendToHomProblem::terminate( ) { }


void ExtendToHomProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Extend to an homomorphism problem"),
		  Text("Does") + Name( theMap ) + "extend to an homomorphism ?",
		  helpID("ExtendToHomProblem", theMap.getDomain()),
		  "Ext to hom ?"
		  );

  
  pv.startItemGroup();

  pv.add( Text("For the use of this problem"), ThisARCSlotID(), 50
	  );


  pv.startItemGroup( );
  
  pv.add( Text("Compute abelian invariants of") + Name( theRange ),
	  abelianInvariants.arcSlotID(),
	  15
	  );


  pv.startItemGroup( CheckinType( theRange ) == SMFPGroup::NILPOTENT ||
		     CheckinType( theRange ) == SMFPGroup::FREE_NILPOTENT
		     );

  if ( ! theRange.gic.haveNilpotentGroupInited( ) ) {
    
    pv.add(Text("Compute a basis of ")
	   + Name(theRange),
	   computeBasis.arcSlotID(),
	   100
	   );
  }
  pv.add(Text("Solve using basis of ")
	 + Name( theRange ),
	 nilpotentWP.arcSlotID(),
	 100
	 );
 
  pv.startItemGroup( CheckinType( theRange ) == SMFPGroup::FP
		     );
 
  pv.add( Text("Enumerate normal closure of relators"),
	  normalClosure.arcSlotID(), 50);

  
  pv.add( Text("Seek a rewriting system for") + Name( theRange ),
	  kbSupervisor.arcSlotID(),
	  50
	  );
  
  pv.add( Text("Seek a ShortLex automatic structure for")
	  + Name( theRange ),
	  agSupervisor.arcSlotID(),
	  50
	  );

  pv.add( Text("Run genetic algorithm"), genetic.arcSlotID(), 50 );
  
  pv.startItemGroup( CheckinType(theRange ) == SMFPGroup::NILPOTENT ||
		     CheckinType(theRange ) == SMFPGroup::FREE_NILPOTENT ||
		     CheckinType(theRange ) == SMFPGroup::FP
		     );
  
  int upperBound, currentClass = 2;
  if (theRange.gic.isNilpotent()==yes || theRange.gic.isFreeNilpotent()==yes)
    upperBound = theRange.gic.getNilpotentcyClass()-1;
  else
    upperBound = 10;
  
  while (currentClass <= upperBound && 
	 theRange.gic.haveNilpotentGroupInited( ) )
    currentClass++;
  
  if (!(currentClass > upperBound) )
    pv.add(Text("Compute the lower central quotients for") + Name( theRange ),
	   nilpotentQuotients.arcSlotID(),
	   50
	   );
  pv.add(Text("Solve using lower central quotients of")
	 + Name( theRange ),
	 nilpotentWPInQuotients.arcSlotID(),
	 50
	 );
  
  pv.done();
}

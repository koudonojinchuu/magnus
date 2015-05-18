// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes ORWordProblemARCer, ORWordProblemCM,
//           WordProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#include "WordProblem.h"
#include "SMWord.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "OneRelatorGroup.h"
#include "OneRelatorGroupWithTorsion.h"
#include "GAWP.h"
#include "GeneticProblems.h"
#include "FNWP.h"


// ------------------------ ORWordProblemARCer ----------------------------- //


ORWordProblemARCer::ORWordProblemARCer( ComputationManager& boss )
  : ARCer( boss ), answer(dontknow)
{ } 


void ORWordProblemARCer::setArguments( const Word& relator,
				       const Word& testWord )
{
  theRelator = relator;
  theTestWord = testWord;
}

void ORWordProblemARCer::runComputation( ) 
{
  Generator g1 = theRelator.maxOccurringGenerator();
  Generator g2 = theTestWord.maxOccurringGenerator();
  int numOfGens = max( abs(ord(g1)), abs(ord(g2)) );

  const OneRelatorGroup G( numOfGens, theRelator );
  const bool keepDetails = true;
  answer = G.wordProblem( theTestWord, keepDetails, wDeco );
}

void  ORWordProblemARCer::writeResults( ostream& out )
{
  out < answer;
  if( answer == yes ) 
    out < wDeco;
}


void ORWordProblemARCer::readResults( istream& in )
{
  in > answer;
  if( answer == yes ) {
    retValue = true;
    in > wDeco;
  }
  else if( answer == no )
    retValue = false;
  else 
    error( "Internal error in OneRelatorGroup::wordProblem() " );
}


void ORWordProblemARCer::printWord(const Word& w, ostream& ostr)
{
  int len = w.length();
  ostr << len << endl;
  for( int i = 0; i < len; ++i ) {
	 int g = ord(w[i]);
	 ostr << char( abs(g) + 'a' - 1 );
	 if ( g < 0 ) ostr << -1;
	 else ostr << 1;
  }
  ostr << endl;
}


// -------------------------- ORWordProblemCM ------------------------------ //


ORWordProblemCM::ORWordProblemCM( WordProblem& PO )
: theRelator( PO.getTestWord().getParent().gic.getOneRelator() ),
  testWord( PO.getTestWord() ),
  itIsTrivial( dontknow ),
  bStarted( false ),
  arcer( *this )
{
  adminStart();
}

void ORWordProblemCM::terminate( )
{
  arcer.terminate(); 
}

void ORWordProblemCM::start( )
{
}

void ORWordProblemCM::takeControl( )
{
  if ( freeARCs() > 0 ) {
    if( !bStarted ) {
      arcer.setArguments(theRelator, testWord.getWord());
      bStarted = true;
    }
    if( arcer.takeControl() ) {
      itIsTrivial = arcer.getRetValue();
      wDeco = arcer.wordDecomposition();
      adminTerminate();
      return;
    }
    else
      usedOneARC();
  }
}


// ----------------------------- WordProblem ------------------------------- //


WordProblem::WordProblem(SMWord& w)
  : Supervisor( ! w.getParent().gic.haveFastWordProblem() ),
    theWord( w ),
    theChecker( w.getWord(), w.getParent() ),
    theAnswer( dontknow ),
    triedAbelianization( false ),
    normalClosure( *this, w.getParent().gcm().normalClosure ),
    abelianInvariants( *this, w.getParent().gcm().abelianInvariants ),
    kbSupervisor( *this, w.getParent().gcm().kbSupervisor ),
    agSupervisor( *this, w.getParent().gcm().agSupervisor ),
    computeBasis( *this, w.getParent().gcm().computeBasis ),
    nilpotentQuotients( *this, w.getParent().gcm().nilpotentQuotients ),
    nilpotentWPInQuotients(*this),
    nilpotentWP( *this ),	 
    orwp( *this ),
    genetic( *this ),
    fnwp( *this ),
    geneticForORG( *this ),
    linkGenORHasBeenSent( false )
{
  if (theWord.wic.isTrivial()!=dontknow)
    resultIsFast();
  else{
    nilpotentWP->initialize(theWord.getWord(),&theWord.getParent());
    nilpotentWPInQuotients->initialize(theWord.getWord(),&theWord.getParent());
    genetic->init(w.getParent().getFPGroup(), w.getWord(), GeneticWPCM::WORD);
    fnwp->init( w.getParent().getFreePreimage(), 
		w.getParent().gic.getNilpotentcyClass( ), w.getWord() );
    if ( ! displayInFE() ) adminStart();
  }

  theChecker.enablePutDetailsToFile();
}

void WordProblem::takeControl( )
{
  if ( ! displayInFE() || freeARCs() > 0 ) {
    Chars explanation;
    if (!fastResult()){
      if (nilpotentWP->state() == ComputationManager::TERMINATED){
	explanation = " this follows from an inspection of a basis." ;
	theAnswer = nilpotentWP->isTrivial();
	theWord.wic.putIsTrivial(theAnswer,explanation);
      }
      if (nilpotentWPInQuotients->state() == ComputationManager::TERMINATED)
	if (nilpotentWPInQuotients->isTrivial()==no){
	  int retClass;
	  theAnswer = nilpotentWPInQuotients->isTrivial(retClass);
	  //@njz
	  //	  ostrstream msgTmp;
	  std::ostrstream msgTmp;
	  //
	  msgTmp << " because it's not trivial in lower central quotient of class " 
		 << retClass << ends;
	  explanation = msgTmp.str();
	  theWord.wic.putIsTrivial(theAnswer,explanation);
	}
    }
    if ( (theAnswer = theWord.wic.isTrivial(explanation) )!=dontknow){
      LogMessage msg( *this, theWord );
      msg << Name( theWord ) << " is ";
      if ( theAnswer == no ) msg << "not ";
      msg << "trivial in " << Name( Parent( theWord ) );
      if( explanation.length() > 0 )
	msg << " : " << explanation;
      msg.send();
      adminTerminate();     
      return;
    }
    
    if ( ( theAnswer = theChecker.isTrivial() ) != dontknow ) {

      LogMessage msg( *this, theWord );
      msg << Name( theWord ) << " is ";
      if ( theAnswer == no ) msg << "not ";
      msg << "trivial in " << Name( Parent( theWord ) );

      Chars explanation = theChecker.getExplanation();

      if ( theWord.getParent().gic.isAbelian() == yes){
	//@njz
	//	  ostrstream msgTmp;
	std::ostrstream msgTmp;
	//
	msgTmp << "because it's canonical decomposition is ";

	const AbelianGroup& A = 
	  theWord.getParent().gic.getCyclicDecomposition();
	
	A.printWordInNewGens(msgTmp,A.oldToNewGens
			     (A.oldInAbelianForm(theWord.getWord() )));
	msgTmp << ends;
	explanation = msgTmp.str();
      }

      if( theWord.getParent().gic.haveMSC() 
	  || theWord.getParent().gic.isOneRelatorWithTorsion() ) {
	explanation = explanation + ". " + 
	  Link( Chars("Click here to see word transformations of ")
		+ Text(Name(theWord)),
		"DehnTransformations", 
		theChecker.getDehnTransformationFileName()
		);
      }

      if( explanation.length() > 0 )
	msg << " : " << explanation;

      msg << ".";
      msg.send();
      theWord.wic.putIsTrivial(theAnswer,explanation);
      
      if( theWord.getParent().gic.isOneRelatorWithTorsion() == true ) {
	OneRelatorGroupWithTorsion G( theWord.getParent().getFreePreimage()
				      .namesOfGenerators(),
				      theWord.getParent().gic.getOneRelator()
				    );
	ProductOfRelatorConjugates wDeco;
	const bool answer = G.wordProblem( theWord.getWord(), wDeco );

	if( answer ) {
	  FPGroup G1 = theWord.getParent().getFPGroup();
	  DetailedReport wordDecomposition( G1 );
	  wordDecomposition.printTrivialWordDetails( theWord.getWord(), wDeco,
						     true );
	  LogMessage msg( *this, theWord );
	  msg << Link( Chars("Click here to see a decomposition of ")
		       + Text(Name(theWord)) 
		       + Text(" as a product of conjugates of the relator."),
		       "ORWordProblem", 
		       wordDecomposition.getFileName()
		       );
	  msg.send();
	}
      }

      if( theChecker.haveDetails() ) {
	LogMessage msg( *this, theWord );
	msg << Link( Chars("Click here to see a decomposition of ")
		     + Text(Name(theWord)) + 
		     Text(" as a product of conjugates of the relators."),
		     "WordProblemDetails", 
		     theChecker.getDecompositionFileName()
		     );
	msg.send();
      }
      
      adminTerminate();
    }
    else if ( theWord.getParent().gic.isOneRelator() ) {
      if( ( theAnswer = orwp->isTrivial() ) != dontknow ) {
	
	LogMessage msg( *this, theWord );
	msg << Name( theWord ) << " is ";
	if ( theAnswer == no ) msg << "not ";
	msg << "trivial in " << Name( Parent( theWord ) );
	
	Chars explanation = "according to the Magnus breakdown";
	msg << " : " << explanation << ".";
	msg.send();
	theWord.wic.putIsTrivial(theAnswer,explanation);
	
	if( theAnswer == yes ) {
	  DetailedReport details( theWord.getParent().getFPGroup() );
	  details.printTrivialWordDetails( theWord.getWord(),
					   orwp->wordDecomposition(), 
					   true );
	  LogMessage msg( *this, theWord );
	  msg << Link( Chars("Click here to see a decomposition of ")
		       + Text(Name(theWord)) 
		       + Text(" as a product of conjugates of the relator."),
		       "ORWordProblem", 
		       details.getFileName()
		       );
	  msg.send();
	  theWord.wic.putIsTrivial( theAnswer, explanation );
	}
	adminTerminate();
      }
      if( !linkGenORHasBeenSent ) {
	linkGenORHasBeenSent = true;
	
	LogMessage msg( *this , theWord );
	msg << Link("Click here to see details of the genetic algorithm 2.",
		    "GAWPforORG", geneticForORG->getFileName(), true);
	msg.send();
      }

      if( ( theAnswer = geneticForORG->answer() ) != dontknow ) {
	
	LogMessage msg( *this, theWord );
	msg << Name( theWord ) << " is ";
	if ( theAnswer == no ) msg << "not ";
	msg << "trivial in " << Name( Parent( theWord ) );
	
	Chars explanation = "according to the genetic algorithm 2";
	msg << " : " << explanation << ".";
	msg.send( );
	
	theWord.wic.putIsTrivial( theAnswer , explanation );
	adminTerminate();
      }      
    } 
    else if ( ( theAnswer = genetic->areTrivial() ) != dontknow ) {
	
      LogMessage msg( *this, theWord );
      msg << Name( theWord ) << " is ";
      //if ( theAnswer == no ) msg << "not ";
      msg << "trivial in " << Name( Parent( theWord ) );
      
      Chars explanation = "according to the genetic algorithm 1";
      msg << " : " << explanation << ".";
      msg.send();
      theWord.wic.putIsTrivial(theAnswer,explanation);
      adminTerminate();
    }
    else if ( ( theAnswer = fnwp->areTrivial() ) != dontknow ) {
	
      LogMessage msg( *this, theWord );
      msg << Name( theWord ) << " is ";
      if ( theAnswer == no ) msg << "not ";
      msg << "trivial in " << Name( Parent( theWord ) );
      
      Chars explanation = "solved using power series";
      msg << " : " << explanation << ".";
      msg.send();
      theWord.wic.putIsTrivial(theAnswer,explanation);
      adminTerminate();
    }
    else {
      usedOneARC();
    }
  }
}

void WordProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Word problem"),
		  Text("Is") + Name( theWord ) + Text("trivial in")
		  + Name( Parent( theWord ) ) + Text("?"),
		  helpID( "WordProblem", theWord.getParent() ), 
		  Text(Name(theWord)) + Text("= 1 ?")
		  );
  

  pv.startItemGroup();
  
  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 50);

  pv.add(Text("Compute abelian invariants of") + Name( Parent( theWord ) ),
	 abelianInvariants.arcSlotID(),
	 15
	 );


  pv.startItemGroup( CheckinType( Parent( theWord ) ) ==
		     SMFPGroup::ONE_RELATOR);

  pv.add(Text("Compute the Magnus breakdown"), orwp.arcSlotID(), 50);

  pv.startItemGroup( CheckinType( Parent( theWord ) ) != SMFPGroup::ABELIAN );

  pv.startItemGroup( CheckinType( Parent( theWord ) ) != SMFPGroup::ABELIAN &&
		     CheckinType( Parent( theWord ) ) != SMFPGroup::NILPOTENT &&
		     CheckinType( Parent( theWord ) ) != SMFPGroup::FREE_NILPOTENT
		     );

  pv.add(Text("Enumerate normal closure of relators"),
	 normalClosure.arcSlotID(), 50);
  
  pv.add(Text("Seek a rewriting system for") + Name( Parent( theWord ) ),
	 kbSupervisor.arcSlotID(),
	 50
	 );
  
  pv.add(Text("Seek a ShortLex automatic structure for")
	 + Name( Parent( theWord ) ),
	 agSupervisor.arcSlotID(),
	 50
	 );
  
  pv.startItemGroup( CheckinType( Parent( theWord ) ) != SMFPGroup::ABELIAN &&
		     CheckinType( Parent( theWord ) ) != SMFPGroup::NILPOTENT &&
		     CheckinType( Parent( theWord ) ) != SMFPGroup::FREE_NILPOTENT
		     );
  pv.add(Text("Run genetic algorithm 1"), genetic.arcSlotID(), 50);

  pv.startItemGroup( CheckinType( Parent( theWord ) ) ==
		     SMFPGroup::ONE_RELATOR || 
		     CheckinType( Parent( theWord ) ) ==
		     SMFPGroup::ONE_RELATOR_WITH_TORSION
		     );

  pv.add(Text("Run genetic algorithm 2") , geneticForORG.arcSlotID(), 50);
  
  pv.startItemGroup( CheckinType( Parent( theWord ) ) == SMFPGroup::NILPOTENT ||
		     CheckinType(Parent( theWord ) ) == SMFPGroup::FREE_NILPOTENT
		     );

  if (!theWord.getParent().gic.haveNilpotentGroupInited())
    pv.add(Text("Compute a basis of") + Name( Parent( theWord ) ),
	   computeBasis.arcSlotID(),
	   50
	   );
  
  pv.add(Text("Solve using basis of")
	 + Name( Parent( theWord ) ),
	 nilpotentWP.arcSlotID(),
	 50
	 );
  
  pv.startItemGroup
    ( CheckinType( Parent( theWord ) ) == SMFPGroup::NILPOTENT ||
      CheckinType( Parent( theWord ) ) == SMFPGroup::FREE_NILPOTENT ||
      CheckinType( Parent( theWord ) ) == SMFPGroup::FP
      );

  int upperBound, currentClass = 2;
  if (theWord.getParent().gic.isNilpotent()==yes || 
      theWord.getParent().gic.isFreeNilpotent()==yes)
    upperBound = theWord.getParent().gic.getNilpotentcyClass()-1;
  else
    upperBound = 10;
  
  while (currentClass <= upperBound && 
	 theWord.getParent().gic.haveNilpotentQuotInited( currentClass ) )
    currentClass++;

  if (!(currentClass > upperBound) )
    pv.add(Text("Compute lower central quotients for") 
	   + Name( Parent( theWord ) ),
	   nilpotentQuotients.arcSlotID(),
	   50
	   );
  
  pv.add(Text("Solve using lower central quotients of")
	 + Name( Parent( theWord ) ),
	 nilpotentWPInQuotients.arcSlotID(),
	 50
	 );
  

  pv.startItemGroup
    ( CheckinType( Parent( theWord ) ) == SMFPGroup::FREE_NILPOTENT
      );

  pv.add(Text("Solve using power series"),
	 fnwp.arcSlotID(),
	 50
	 );

  pv.done();
}

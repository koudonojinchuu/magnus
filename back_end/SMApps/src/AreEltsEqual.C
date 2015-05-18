// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class AreEltsEqual
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#include "AreEltsEqual.h"
#include "SMWord.h"
#include "OutMessages.h"
#include "viewStructure.h"


//--------------------------- AreEltsEqual ----------------------------------//


AreEltsEqual::AreEltsEqual(const SMWord& w1, const SMWord& w2)
  : Supervisor( ! w1.getParent().gic.haveFastWordProblem() ),
    word1( w1 ),
    word2( w2 ),
    theChecker( w1.getParent() ),
    normalClosure( *this, w1.getParent().gcm().normalClosure ),
    abelianInvariants( *this, w1.getParent().gcm().abelianInvariants ),
    kbSupervisor( *this, w1.getParent().gcm().kbSupervisor ),
    agSupervisor( *this, w1.getParent().gcm().agSupervisor ),
    computeBasis( *this, w1.getParent().gcm().computeBasis ),
    nilpotentQuotients( *this, w1.getParent().gcm().nilpotentQuotients ),
    nilpotentWPInQuotients(*this),
    nilpotentWP( *this ),
    genetic( *this )
{
  Word checkWord( (w1.getWord() * w2.getWord().inverse()).freelyReduce());
  theChecker.replaceTheSet( checkWord );
  nilpotentWP->initialize(checkWord,&word1.getParent());
  nilpotentWPInQuotients->initialize(checkWord,&word1.getParent());
  genetic->init(word1.getParent().getFPGroup(), checkWord, GeneticWPCM::WORD);

  if ( !displayInFE() ) adminStart();
}


void AreEltsEqual::takeControl( )
{
  Trichotomy theAnswer = dontknow;
  Chars explanation;
  if ( ! displayInFE() || freeARCs() > 0 )
    if (nilpotentWP->state() == ComputationManager::TERMINATED){
      explanation = Chars(": this follows from an inspection of a basis of ") +
	Text(Name(word1.getParent()));
      theAnswer = nilpotentWP->isTrivial();
    }
    if (nilpotentWPInQuotients->state() == ComputationManager::TERMINATED)
      if (nilpotentWPInQuotients->isTrivial()==no){
	int retClass;
	theAnswer = nilpotentWPInQuotients->isTrivial(retClass);
	//@njz
	//	ostrstream msgTmp;
	std::ostrstream msgTmp;
	//
	msgTmp << ": their images are not equal in the lower central quotient of "
	       << Name(word1.getParent()) << "of class " 
	       << retClass << ends;
	explanation = msgTmp.str();
      }
    if (theAnswer != dontknow){
	LogMessage msg( *this, word1, word2 );
	msg << Name( word1 ) << " is ";
	if ( theAnswer == no ) msg << "not ";
	msg << "equal to " << Name( word2 ) << explanation;
	msg.send();
	adminTerminate();     
	return;      
    }
    if ( ( theAnswer = theChecker.isTrivial() ) != dontknow ) {
      
      LogMessage msg( *this, word1, word2 );
      msg << "The elements "<< Name( word1 ) << " and " << Name( word2 ) << " are ";
      if ( theAnswer == no ) msg << "not ";
      msg << "equal";

      Chars explanation = theChecker.getExplanation();
      if( explanation.length() > 0 )
	msg << " : " << explanation;

      msg << ".";
      msg.send();
      adminTerminate();
    }
    else if ( ( theAnswer = genetic->areTrivial() ) != dontknow ) {
      
      LogMessage msg( *this, word1, word2 );
      msg << "The elements "<< Name( word1 ) << " and " << Name( word2 ) 
	  << " are equal";
      Chars explanation = "according to the genetic algorithm";
      msg << " : " << explanation << ".";
      msg.send();
      adminTerminate();
    }
    else
      usedOneARC();
}


void AreEltsEqual::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Elements equality problem"),
		  Text("Is") + Name( word1 ) + "equal to"
		  + Name( word2 ) + "?",
		  helpID("AreEltsEqual", word1.getParent()),
		  Text(Name(word1)) + "=" + Name(word2) + "?"
		  );
  

  pv.startItemGroup();
  
  pv.add( Text("For the use of this problem"), ThisARCSlotID(), 50);

  pv.add( Text("Compute abelian invariants of") + Name( Parent( word1 ) ),
	  abelianInvariants.arcSlotID(),
	  15
	  );

    
  pv.startItemGroup
    ( CheckinType( Parent( word1 ) ) != SMFPGroup::ABELIAN &&
       CheckinType( Parent( word1 ) ) != SMFPGroup::NILPOTENT &&
      CheckinType( Parent( word1 ) ) != SMFPGroup::FREE_NILPOTENT
      );

  pv.add( Text("Enumerate normal closure of relators of")
	  + Name( Parent( word1 ) ),
	  normalClosure.arcSlotID(),
	  50);
  
  pv.add( Text("Seek a rewriting system for") + Name( Parent( word1 ) ),
	  kbSupervisor.arcSlotID(),
	  50
	  );
    
  pv.add( Text("Seek a ShortLex automatic structure for")
	  + Name( Parent( word1 ) ),
	  agSupervisor.arcSlotID(),
	  50
	  );

  pv.add(Text("Run genetic algorithm"), genetic.arcSlotID(), 50);

  pv.startItemGroup
    ( CheckinType( Parent( word1 ) ) == SMFPGroup::FP ||
      CheckinType( Parent( word1 ) ) == SMFPGroup::NILPOTENT ||
      CheckinType( Parent( word1 ) ) == SMFPGroup::FREE_NILPOTENT
      );
  
  int upperBound, currentClass = 2;
  if (word1.getParent().gic.isNilpotent()==yes ||
      word1.getParent().gic.isFreeNilpotent()==yes)
    upperBound = word1.getParent().gic.getNilpotentcyClass()-1;
  else
    upperBound = 10;
  
  while (currentClass <= upperBound && 
	 word1.getParent().gic.haveNilpotentQuotInited( currentClass ) )
    currentClass++;

  if (!(currentClass > upperBound) )
    pv.add(Text("Compute the lower central quotients for") + Name( Parent( word1 ) ),
	   nilpotentQuotients.arcSlotID(),
	   50
	   );
  
  pv.add(Text("Solve using lower central quotients of")
	 + Name( Parent( word1 ) ),
	 nilpotentWPInQuotients.arcSlotID(),
	 50
	 );
  
  pv.startItemGroup( CheckinType( Parent( word1 ) ) == SMFPGroup::NILPOTENT ||
		     CheckinType( Parent( word1 ) ) == SMFPGroup::FREE_NILPOTENT
		     );
  
  if (!word1.getParent().gic.haveNilpotentGroupInited())
    pv.add(Text("Compute a basis of ") + Name( Parent( word1 ) ),
	   computeBasis.arcSlotID(),
	   50
	   );
  
  pv.add(Text("Solve using relation subgroup of ")
	 + Name( Parent( word1 ) ),
	 nilpotentWP.arcSlotID(),
	 50
	 );
  pv.done();
}

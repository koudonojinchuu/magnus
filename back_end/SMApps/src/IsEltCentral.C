// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class IsEltCentral
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#include "IsEltCentral.h"
#include "SMWord.h"
#include "OutMessages.h"
#include "viewStructure.h"


//--------------------------- IsEltCentral ----------------------------------//


IsEltCentral::IsEltCentral(const SMWord& word)
  : Supervisor( ! word.getParent().gic.haveFastWordProblem() ),
    theWord( word ),
    theChecker( word.getParent() ),
    normalClosure( *this, word.getParent().gcm().normalClosure ),
    abelianInvariants( *this, word.getParent().gcm().abelianInvariants ),
    kbSupervisor( *this, word.getParent().gcm().kbSupervisor ),
    agSupervisor( *this, word.getParent().gcm().agSupervisor ),
    computeBasis( *this, word.getParent().gcm().computeBasis ),
    nilpotentQuotients( *this, word.getParent().gcm().nilpotentQuotients ),
    nilpotentWPInQuotients(*this),
    nilpotentWP( *this ),
    genetic( *this )
{
  SetOf<Word> comms;
  int numOfGens = word.getParent().getFPGroup().numberOfGenerators();
  Word w = word.getWord();
  
  for( int i = 0; i < numOfGens; ++i ) {
    Word gen = Word(Generator(i+1));
    comms |= (w.inverse() * gen.inverse() * w * gen).freelyReduce();
  }

  theChecker.replaceTheSet(comms);
  nilpotentWP->initialize(comms,&theWord.getParent());
  nilpotentWPInQuotients->initialize(comms,&theWord.getParent());
  genetic->init(theWord.getParent().getFPGroup(), comms, 
		GeneticWPCM::COMMUTATORS);

  if ( !displayInFE() ) adminStart();
}


void IsEltCentral::takeControl( )
{
  Trichotomy theAnswer = dontknow;
  Chars explanation;
  if ( ! displayInFE() || freeARCs() > 0 )
    if (nilpotentWP->state() == ComputationManager::TERMINATED){
      explanation = ": this follows from an inspection of a basis." ;
      theAnswer = nilpotentWP->isTrivial();
    }
  if (nilpotentWPInQuotients->state() == ComputationManager::TERMINATED)
    if (nilpotentWPInQuotients->isTrivial()==no){
      int retClass;
      theAnswer = nilpotentWPInQuotients->isTrivial(retClass);
      //@njz
      //      ostrstream msgTmp;
      std::ostrstream msgTmp;
      //
      msgTmp << ": because it's not central in lower central quotient of class " 
	     << retClass << ends;
      explanation = msgTmp.str();
    }
  if (theAnswer != dontknow){
      LogMessage msg( *this, theWord );
      msg << Name( theWord ) << " is ";
      if ( theAnswer == no ) msg << "not ";
      msg << "central in " << Name( Parent( theWord ) ) << explanation;
      msg.send();
      adminTerminate();     
      return;    
  }
  if ( ( theAnswer = theChecker.isTrivial() ) != dontknow ) {
    
    LogMessage msg( *this, theWord );
    msg << Name( theWord ) << " is ";
    if ( theAnswer == no ) msg << "not ";
    msg << "central in " << Name( Parent( theWord ) );
    
    Chars explanation = theChecker.getExplanation();
    if( explanation.length() > 0 )
      msg << " : " << explanation;
    
    msg << ".";
    msg.send();

    if( theChecker.haveDetails() ) {
      LogMessage msgLink( *this, theWord );
      msgLink << Link( Chars("Click here to see details of centrality problem "
			     "for ")
		       + Text(Name(theWord)) + 
		       Text("."),
		       "WordProblemDetails", 
		       theChecker.getDecompositionFileName()
		       );
      msgLink.send();
    }
    
    
    adminTerminate();
  } 
  else if ( ( theAnswer = genetic->areTrivial() ) != dontknow ) {
    
    Chars explanation = "according to the genetic algorithm";
    LogMessage msg( *this, theWord );
    msg << Name( theWord ) << " is central";
    msg << " : " << explanation << ".";
    msg.send();
    adminTerminate();
  }
 
  else if ( ( theAnswer = genetic->areTrivial() ) != dontknow ) {
    
    Chars explanation = "according to the genetic algorithm";
    LogMessage msg( *this, theWord );
    msg << Name( theWord ) << " is central";
    msg << " : " << explanation << ".";
    msg.send();
    adminTerminate();
  }
  else
    usedOneARC();
}


void IsEltCentral::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Is element central problem"),
		  Text("Is") + Name( theWord ) + "central in"
		  + Name( Parent( theWord ) ) + "?",
		  helpID("IsEltCentral",theWord.getParent()), 
		  Text(Name(theWord)) + "central ?"
		  );
  

  pv.startItemGroup();
  
  pv.add( Text("For the use of this problem"), ThisARCSlotID(), 50);

  pv.add( Text("Compute abelian invariants of") + Name( Parent( theWord ) ),
	  abelianInvariants.arcSlotID(),
	  15
	  );

    
  pv.startItemGroup
    ( CheckinType( Parent( theWord ) ) != SMFPGroup::ABELIAN &&
      CheckinType( Parent( theWord ) ) != SMFPGroup::NILPOTENT &&
      CheckinType( Parent( theWord ) ) != SMFPGroup::FREE_NILPOTENT
      );

  pv.add( Text("Enumerate normal closure of relators of")
	  + Name( Parent( theWord ) ),
	  normalClosure.arcSlotID(), 50);
  
  pv.add( Text("Seek a rewriting system for") + Name( Parent( theWord ) ),
	  kbSupervisor.arcSlotID(),
	 50
	  );
    
  pv.add( Text("Seek a ShortLex automatic structure for")
	  + Name( Parent( theWord ) ),
	  agSupervisor.arcSlotID(),
	  50
	  );

  pv.add( Text("Run genetic algorithm"), genetic.arcSlotID(), 50 );

  pv.startItemGroup( CheckinType( Parent( theWord ) ) == SMFPGroup::NILPOTENT ||
		     CheckinType( Parent( theWord ) ) == SMFPGroup::FREE_NILPOTENT );

  if (!theWord.getParent().gic.haveNilpotentGroupInited())
    pv.add(Text("Compute a basis of ") + Name( Parent( theWord ) ),
	   computeBasis.arcSlotID(),
	   50
	   );
  
  pv.add(Text("Solve using relation subgroup for")
	 + Name( Parent( theWord ) ),
	 nilpotentWP.arcSlotID(),
	 50
	 );
  
  pv.startItemGroup( CheckinType( Parent( theWord ) ) == SMFPGroup::NILPOTENT ||
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
    pv.add(Text("Compute lower central quotients for") + Name( Parent( theWord ) ),
	   nilpotentQuotients.arcSlotID(),
	   50
	   );
  
  pv.add(Text("Solve using lower central quotients of")
	 + Name( Parent( theWord ) ),
	 nilpotentWPInQuotients.arcSlotID(),
	 50
	 );
 
  pv.done();
}

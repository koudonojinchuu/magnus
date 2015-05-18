// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes 
// 
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
//

#include "SubgroupContainmentProblem.h"
#include "SMSubgroup.h"
#include "SMWord.h"
#include "OutMessages.h"
#include "SubgroupGraph.h"
#include "DecomposeInSubgroup.h"
#include "conversions.h"

// ------------------------- SubgroupContainment ---------------------- //

SubgroupContainment::SubgroupContainment( SubgroupContainmentProblem& boss )
  : // arcer( *this ), arcerStarted( false ),
    theSubgroup( boss.getSubgroup() ),
    theTestWords( 1 ),
    wordsToCheck( boss.getTestWord().getWord() ),
    theAnswer( dontknow ),
    wordsDecomposition( boss.getSubgroup().getParent().getFPGroup(),
			boss.getSubgroup().getSubgroup().generators() )
{
  theTestWords[0] = boss.getTestWord().getWord();

  adminStart();
}



SubgroupContainment::SubgroupContainment( IsSGNormal& boss )
  : theSubgroup( boss.getSubgroup() ),
    theTestWords( makeVectorOf(boss.getTestWords()) ),
    wordsToCheck( boss.getTestWords() ),
    theAnswer( dontknow ),
    wordsDecomposition( boss.getSubgroup().getParent().getFPGroup(),
			boss.getSubgroup().getSubgroup().generators() )
{
  adminStart();
}

Trichotomy SubgroupContainment::contains( ) const 
{
  return theAnswer;
}

// Find words which are belonged to the current approximation
// and remove them from the test set.
SetOf<Word> SubgroupContainment::reduceTestingSet( 
  const DecomposeInSubgroupOfFPGroup& D, SetOf<Word>& testingSet )
{
  SetOf<Word> result;
  SetIterator<Word> I(testingSet);

  for( I.reset(); !I.done(); I.next() ) {
    Word w = I.value();
    if( !D.contains( w ) ) {
      result |= w;
    }
    else {
      wordsDecomposition.printSubgroupElement( 
        w, D.fullDecomposition(w), !wordsDecomposition.haveDetails() 
      );
    }
  }

  return result;
}

void SubgroupContainment::takeControl( )
{
  if ( freeARCs() > 0 ) {

    if( theSubgroup.sic.haveWordDecomposer() ) {

      const DecomposeInSubgroupOfFPGroup& D = 
	theSubgroup.sic.getWordDecomposer();

      wordsToCheck = reduceTestingSet( D, wordsToCheck );

      if( wordsToCheck.cardinality() == 0 ) {
	theAnswer = yes;

	adminTerminate();
	return;
      }    
    }
    
    if( theSubgroup.sic.haveCompleteCayleyGraph() ) {

      const DecomposeInSubgroupOfFPGroup& D = 
	theSubgroup.sic.getWordDecomposer();

      wordsToCheck = reduceTestingSet( D, wordsToCheck );

      if( wordsToCheck.cardinality() == 0 ) {
	theAnswer = yes;
      }    
      theAnswer = no;

      adminTerminate();
      return;
    }
    
    usedOneARC();
  }
  
}


SubgroupContainmentProblem::SubgroupContainmentProblem( SMSubgroup& subgroup,
							SMWord& w )
  : Supervisor( true ),
    theSubgroup( subgroup ),
    theTestWord( w ),
    isWordInSubgroup( dontknow ),
    subgroupContain( *this ),
    normalClosure( *this, subgroup.scm().normalClosure ),
    abelianInvariants( *this, subgroup.scm().abelianInvariantsOfFactor ),
    sgNilpotentQuotients( *this, subgroup.scm().sgNilpotentQuotients ),
    scpInNilpotentQuotients( *this ),
    toddCoxeter( *this, subgroup.scm().sgToddCoxeter ),
    wordRepCM( *this )
{

  if( theSubgroup.sic.haveCyclicDecompositionOfFactor() ) {
    const AbelianGroup& A = theSubgroup.sic.getCyclicDecompositionOfFactor();
    if( A.isTrivial(theTestWord.getWord()) == false ) {
      isWordInSubgroup = no;
      explanation = " it follows from canonical decomposition";
      resultIsFast();
    }
  }
  scpInNilpotentQuotients->initialize( theTestWord.getWord(), &theSubgroup );
}


void SubgroupContainmentProblem::takeControl( )
{
  if( isWordInSubgroup == dontknow ) {

    // Check abelian computation managers.
    if( theSubgroup.sic.haveCyclicDecompositionOfFactor() ) {
      const AbelianGroup& A=theSubgroup.sic.getCyclicDecompositionOfFactor();
      if( A.isTrivial(theTestWord.getWord()) == false ) {
	isWordInSubgroup = no;
	explanation = " it follows from canonical decomposition";
      }
    }
    
    // Check nilpotent computation managers.
    if( scpInNilpotentQuotients->state() == ComputationManager::TERMINATED ){
      int retClass;
      Trichotomy theAnswer = scpInNilpotentQuotients->contains(retClass);
      
      if( theAnswer == no ){
	isWordInSubgroup = no;
	//@njz
	//	ostrstream msgTmp;
	std::ostrstream msgTmp;
	//
	msgTmp << " because it's not trivial in lower central quotient" 
	       << "of class " << retClass << ends;
	explanation = msgTmp.str();
	msgTmp.freeze(0);
      }
    }
    
    // Check Todd-Coxeter computation managers.
    if( wordRepCM->state() == ComputationManager::TERMINATED ) {
      
      if( !wordRepCM->isInfinite() ) {
	Word rep = wordRepCM->getWordsReps()[0];
	//@njz
	//	ostrstream msgTmp;
	std::ostrstream msgTmp;
	//
	msgTmp << " because Schreier representative of " << Name(theTestWord)
	       << " modulo " << Name( theSubgroup );
	if( rep.length() == 0 ) {
	  isWordInSubgroup = yes;
	  msgTmp << " is 1";
	}
	else {
	  isWordInSubgroup = no;
	  msgTmp << " is not 1";
	}
	explanation = msgTmp.str();
	msgTmp.freeze(0);
      }
    }
  }
  
  if( isWordInSubgroup != dontknow ) {
    
    // Send message to front end about found results and quit.
    LogMessage msg(*this, theTestWord, theSubgroup);
    msg << Name( theTestWord );
    if( isWordInSubgroup == yes ) 
      msg << " lies ";
    else
      msg << " does not lie ";
    msg << "in " << Name( theSubgroup ) 
	<< explanation << ".";
    msg.send();
    adminTerminate();
  }
  
  if( subgroupContain->state() == ComputationManager::TERMINATED ) {

    LogMessage msg( *this, theSubgroup, theTestWord );
    msg << Name(theTestWord)
	<< (subgroupContain->contains() == yes ? " lies " : " does not lie ")
	<< "in " << Name(theSubgroup) 
	<< " according to a normal closure approximation.";
    msg.send();
    
    if( subgroupContain->contains() == yes
	&& subgroupContain->haveDetails() ) {
      LogMessage msgLink( *this, theSubgroup, theTestWord );
      msgLink << Link( Chars("Click here to see a decomposition of ")
		       + Text(Name(theTestWord))
		       + Text(" in terms of subgroup generators and "
			      "as a product of conjugates of given relators."),
		       "SubgroupContainmentDetails", 
		       subgroupContain->getDecompositionFileName()
		       );
      msgLink.send();
    }
    
    adminTerminate();
  }
  
}

void SubgroupContainmentProblem::start( )
{
}

void SubgroupContainmentProblem::terminate( )
{
}


void SubgroupContainmentProblem::viewStructure(ostream& ostr) const
{

  ProblemView pv( ostr, oid(), Text("Subgroup containment problem"),
		  Text("Does") + Name( theSubgroup ) + "contain "
		  + Name( theTestWord ) + "?",
		  helpID( "none", theSubgroup.getParent() ), 
		  Text( "SubgroupContainment" )
		);

  pv.startItemGroup();
  
  //pv.add("For the use of this problem", ThisARCSlotID(), 50);

  pv.add(Text("For the use of this problem"), subgroupContain.arcSlotID(), 50);

  pv.add( Text("Normal closure approximation method") + Name(theSubgroup),
	  normalClosure.arcSlotID(),
	  50
	  );

  pv.add( Text("Compute abelian invariants of") + Name(theSubgroup),
	  abelianInvariants.arcSlotID(),
	  15
	  );

  pv.startItemGroup(CheckinType(Parent(theTestWord))==SMFPGroup::NILPOTENT
		 || CheckinType(Parent(theTestWord))==SMFPGroup::FREE_NILPOTENT
		 || CheckinType(Parent(theTestWord))==SMFPGroup::FP
  );

  SMFPGroup& G = theTestWord.getParent();
  int upperBound;
  int currentClass = 2;

  upperBound = getNilpotencyUpperBound( G );

  while( currentClass <= upperBound && 
	 theSubgroup.sic.haveSGNilpotentQuotInited( currentClass ) )
    currentClass++;     

  if( currentClass <= upperBound ) {
    
    pv.add( Text("Compute lower central quotients for") + Name( theSubgroup ),
	    sgNilpotentQuotients.arcSlotID(),
	    50
	    );
  }

  pv.add( Text("Solve using lower central quotients of") + Name(theSubgroup),
	  scpInNilpotentQuotients.arcSlotID(),
	  50
	  );
  
  pv.startItemGroup();

  pv.add( Text("Enumerate cosets by using Todd-Coxeter procedure"),
	  toddCoxeter.arcSlotID(),
	  40
	  );

  pv.add( Text("Method that uses Todd-Coxeter procedure"),
	  wordRepCM.arcSlotID(),
	  40
	  );

  pv.done();
}


// ============================= IsSGNormal ============================== //

IsSGNormal::IsSGNormal( SMSubgroup& subgroup )
  : Supervisor( true ),
    theSubgroup( subgroup ),
    theTestWords( makeTestWords(subgroup) ),
    isNormal( dontknow ),
    subgroupContain( *this ),
    normalClosure( *this, subgroup.scm().normalClosure ),
    abelianInvariants( *this, subgroup.scm().abelianInvariantsOfFactor ),
    sgNilpotentQuotients( *this, subgroup.scm().sgNilpotentQuotients ),
    scpInNilpotentQuotients( *this ),
    toddCoxeter( *this, subgroup.scm().sgToddCoxeter ),
    wordRepCM( *this )
{

  if( theSubgroup.sic.isNormal() != dontknow ) {
    resultIsFast();
    return;
  }

  if( theSubgroup.sic.haveCyclicDecompositionOfFactor() ) {
    const AbelianGroup& A = theSubgroup.sic.getCyclicDecompositionOfFactor();
    SetIterator<Word> I( theTestWords );
    for( ; !I.done(); I.next() ) {
      Word w = I.value();
      if( A.isTrivial( w ) == false ) {
	isNormal = no;
	explanation = " it follows from canonical decomposition";
	resultIsFast();
	return;
      }
    }
  }
  scpInNilpotentQuotients->initialize( theTestWords, &theSubgroup );
}

SetOf<Word> IsSGNormal::makeTestWords( const SMSubgroup& H ) const
{
  FPGroup G = H.getParent().getFPGroup();
  VectorOf<Word> gens = H.getSubgroup().generators();

  SetOf<Word> result;
  Word w;
  for( int ii = 0; ii < gens.length(); ++ii ) {
    for( int j = 0; j < G.numberOfGenerators(); ++j ) {
      w = gens[ii].conjugateBy( Word(Generator(j+1)) );
      result |= w.freelyReduce();
      w = gens[ii].conjugateBy( Word(inv(Generator(j+1))) );
      result |= w.freelyReduce();
    }
  }

  return result;
}

void IsSGNormal::takeControl( )
{
  if( theSubgroup.sic.isNormal() != dontknow ) {
    // Send message to front end about found results and quit.
    LogMessage msg(*this, theSubgroup);
    msg << "The subgroup " << Name( theSubgroup ) << " is "
	<< ( theSubgroup.sic.isNormal() ? "" : "not " ) << "normal.";
    msg.send();
    adminTerminate();
  }
  
  if( isNormal == dontknow ) {

    // Check abelian computation managers.
    if( theSubgroup.sic.haveCyclicDecompositionOfFactor() ) {
      const AbelianGroup& A=theSubgroup.sic.getCyclicDecompositionOfFactor();
      SetIterator<Word> I( theTestWords );
      for( ; !I.done(); I.next() ) {
	Word w = I.value();
	if( A.isTrivial( w ) == false ) {
	  isNormal = no;
	  explanation = " it follows from canonical decomposition";
	  break;
	}
      }
    }
    
    // Check nilpotent computation managers.
    if( scpInNilpotentQuotients->state() == ComputationManager::TERMINATED ){
      int retClass;
      Trichotomy theAnswer = scpInNilpotentQuotients->contains(retClass);
      
      if( theAnswer == no ){
	isNormal = no;
	//@njz
	//	ostrstream msgTmp;
	std::ostrstream msgTmp;
	//
	msgTmp << " it follows from inspection of lower central quotient " 
	       << "of class " << retClass << ends;
	explanation = msgTmp.str();
	msgTmp.freeze(0);
      }
    }
    
    // Check Todd-Coxeter computation managers.
    if( wordRepCM->state() == ComputationManager::TERMINATED ) {
      
      if( !wordRepCM->isInfinite() ) {

	VectorOf<Word> reps = wordRepCM->getWordsReps();
	isNormal = yes;
	for( int ii = 0; ii < reps.length(); ++ii ) {
	  if( reps[ii].length() != 0 ) {
	    isNormal = no;
	    break;
	  }
	}
	explanation = 
	  "it follows from inspection of Schreier representatives system";
      }
    }

    if( subgroupContain->state() == ComputationManager::TERMINATED ) {
      explanation = "according to a normal closure approximation";
      isNormal = subgroupContain->contains();
    }
  }
  
  if( isNormal != dontknow ) {
    
    ((SMSubgroup&)theSubgroup).sic.putIsNormal( isNormal );
    // Send message to front end about found results and quit.
    LogMessage msg(*this, theSubgroup);
    msg << "The subgroup " << Name( theSubgroup ) << " is "
	<< ( isNormal ? "" : "not " ) << "normal " << explanation << ".";
    msg.send();
    adminTerminate();
  }
  
  
}

void IsSGNormal::start( )
{
}

void IsSGNormal::terminate( )
{
}


void IsSGNormal::viewStructure(ostream& ostr) const
{

  ProblemView pv( ostr, oid(), Text("Is normal problem"),
		  Text("Is") + Name( theSubgroup ) + "normal ?",
		  helpID( "none", theSubgroup.getParent() ), 
		  Text( "IsNormal" )
		);

  pv.startItemGroup();
  
  //pv.add("For the use of this problem", ThisARCSlotID(), 50);

  pv.add(Text("For the use of this problem"), subgroupContain.arcSlotID(), 50);

  pv.add( Text("Normal closure approximation method") + Name(theSubgroup),
	  normalClosure.arcSlotID(),
	  50
	  );

  pv.add( Text("Compute abelian invariants of") + Name(theSubgroup),
	  abelianInvariants.arcSlotID(),
	  15
	  );

  pv.startItemGroup(CheckinType(Parent(theSubgroup))==SMFPGroup::NILPOTENT
		 || CheckinType(Parent(theSubgroup))==SMFPGroup::FREE_NILPOTENT
		 || CheckinType(Parent(theSubgroup))==SMFPGroup::FP
  );

  SMFPGroup& G = theSubgroup.getParent();
  int upperBound;
  int currentClass = 2;

  upperBound = getNilpotencyUpperBound( G );

  while( currentClass <= upperBound && 
	 theSubgroup.sic.haveSGNilpotentQuotInited( currentClass ) )
    currentClass++;     

  if( currentClass <= upperBound ) {
    
    pv.add( Text("Compute lower central quotients for") + Name( theSubgroup ),
	    sgNilpotentQuotients.arcSlotID(),
	    50
	    );
  }

  pv.add( Text("Solve using lower central quotients of") + Name(theSubgroup),
	  scpInNilpotentQuotients.arcSlotID(),
	  50
	  );
  
  pv.startItemGroup();

  pv.add( Text("Enumerate cosets by using Todd-Coxeter procedure"),
	  toddCoxeter.arcSlotID(),
	  40
	  );

  pv.add( Text("Method that uses Todd-Coxeter procedure"),
	  wordRepCM.arcSlotID(),
	  40
	  );

  pv.done();
}


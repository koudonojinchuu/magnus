// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes IsSGAbelian, IsSGCentral, IsSGNilpotent,
//           IsSGTrivial.
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#include "SubgroupProblems.h"
#include "SMSubgroup.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "Subgroup.h"
#include "conversions.h"
#include "PresentationsOfSubgroup.h"

//--------------------------- IsSGAbelian -----------------------------------//

IsSGAbelian::IsSGAbelian( SMSubgroup& S)
  : Supervisor( ! S.getParent().gic.haveFastWordProblem() ),
    theSubgroup( S ),
    theChecker( S.getParent() ),
    normalClosure( *this, S.getParent().gcm().normalClosure ),
    abelianInvariants( *this, S.getParent().gcm().abelianInvariants ),
    kbSupervisor( *this, S.getParent().gcm().kbSupervisor ),
    agSupervisor( *this, S.getParent().gcm().agSupervisor ),
    nilpotentQuotients(*this, S.getParent().gcm().nilpotentQuotients),
    nilpotentWPInQuotients(*this),
    computeBasis(*this, S.getParent().gcm().computeBasis),
    nilpotentWP(*this),
    genetic(*this)
{
  if (theSubgroup.sic.isAbelian()!=dontknow)
    resultIsFast();
  else {
    SetOf<Word> comms;
    VectorOf<Word> gens = S.getSubgroup().generators();
    int gensLen = gens.length();
    
    for( int i = 0; i < gensLen-1; ++i ) 
      for( int j = i+1; j < gensLen; ++j ) {
	Word w1 = gens[i];
	Word w2 = gens[j];
	comms |= (w1.inverse() * w2.inverse() * w1 * w2).freelyReduce();
      }
    nilpotentWPInQuotients->initialize(comms,&(theSubgroup.getParent()));
    nilpotentWP->initialize(comms,&(theSubgroup.getParent()));
    genetic->init(theSubgroup.getParent().getFPGroup(), comms, 
		  GeneticWPCM::COMMUTATORS);
    theChecker.replaceTheSet(comms);
  }
  if ( !displayInFE() ) adminStart();
}


void IsSGAbelian::takeControl( )
{
  Chars explanation;
  if (!fastResult())
  if ( ! displayInFE() || freeARCs() > 0 ){
    if (nilpotentWP->state() == ComputationManager::TERMINATED){
      explanation = Chars(":this follows from an inspection of a basis of ")+
	Text(Name(theSubgroup.getParent())) ;
      theSubgroup.sic.putIsAbelian(nilpotentWP->isTrivial());
    }
    if (nilpotentWPInQuotients->state() == ComputationManager::TERMINATED)
      if (nilpotentWPInQuotients->isTrivial()==no){
	int retClass;
	theSubgroup.sic.putIsAbelian
	  (nilpotentWPInQuotients->isTrivial(retClass));
	//@njz
	//	ostrstream msgTmp;
	std::ostrstream msgTmp;
	//
	  msgTmp << ":the image of "<<Name(theSubgroup)<<" in the lower central quotient of "
		 << Name(theSubgroup.getParent()) << " of class "<< retClass
		 << " is not abelian"<<ends;
	explanation = msgTmp.str();
      }    

    Trichotomy theAnswer(dontknow);
    if ( ( theAnswer = theChecker.isTrivial() ) != dontknow ) {
      explanation = theChecker.getExplanation();
      theSubgroup.sic.putIsAbelian(theAnswer);
    }

    if ( ( theAnswer = genetic->areTrivial() ) != dontknow ) {
      explanation = "according to the genetic algorithm";
      theSubgroup.sic.putIsAbelian(theAnswer);
    }
    
    usedOneARC();
  }
  if (theSubgroup.sic.isAbelian()!=dontknow){
    LogMessage msg( *this, theSubgroup );
    msg << Name( theSubgroup ) << " is ";
    if ( theSubgroup.sic.isAbelian() == no ) msg << "not ";
    msg << "abelian ";
    if (explanation.length() >0)
      msg << explanation;
    msg << ".";
    msg.send();
    adminTerminate();     
    return;
  }
}


void IsSGAbelian::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Is subgroup abelian problem"),
		  Text("Is the subgroup ") + Name( theSubgroup ) + " of "
		  + Name( Parent( theSubgroup ) ) + " abelian?",
		  helpID("IsSGAbelian", theSubgroup.getParent()),
		  Text(Name(theSubgroup)) + "abelian ?"
		  );
  

  pv.startItemGroup();
  
  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 50);

  pv.add(Text("Compute abelian invariants of") + Name( Parent( theSubgroup ) ),
	 abelianInvariants.arcSlotID(),
	 15
	 );

    
  pv.startItemGroup
    ( CheckinType( Parent( theSubgroup ) ) != SMFPGroup::ABELIAN &&
      CheckinType( Parent( theSubgroup ) ) != SMFPGroup::NILPOTENT &&
      CheckinType( Parent( theSubgroup ) ) != SMFPGroup::FREE_NILPOTENT
      );

  pv.add( Text("Enumerate normal closure of relators of")
	  + Name( Parent( theSubgroup ) ),
	  normalClosure.arcSlotID(), 50);
  
  pv.add(Text("Seek a rewriting system for") + Name( Parent( theSubgroup ) ),
	 kbSupervisor.arcSlotID(),
	 50
	 );
    
  pv.add(Text("Seek a ShortLex automatic structure for")
	 + Name( Parent( theSubgroup ) ),
	 agSupervisor.arcSlotID(),
	 50
	 );
  
  pv.add( Text("Run genetic algorithm"), genetic.arcSlotID(), 50 );

  pv.startItemGroup( CheckinType(Parent( theSubgroup )) == SMFPGroup::FP ||
		     CheckinType(Parent( theSubgroup )) == SMFPGroup::NILPOTENT ||
		     CheckinType(Parent( theSubgroup )) == SMFPGroup::FREE_NILPOTENT 
		     );
  
  int upperBound, currentClass = 2;
  if (theSubgroup.getParent().gic.isNilpotent()==yes ||
      theSubgroup.getParent().gic.isFreeNilpotent()==yes)
    upperBound = theSubgroup.getParent().gic.getNilpotentcyClass()-1;
  else
    upperBound = 10;
  
  while (currentClass <= upperBound && 
	 theSubgroup.getParent().gic.haveNilpotentQuotInited(currentClass ) )
    currentClass++;
  
  if (!(currentClass > upperBound) )
    pv.add(Text("Compute the lower central quotients for") + 
	   Name( theSubgroup.getParent() ),
	   nilpotentQuotients.arcSlotID(),
	   50
	   );
  pv.add(Text("Solve using lower central quotients of")
	 + Name( theSubgroup.getParent() ),
	 nilpotentWPInQuotients.arcSlotID(),
	 50
	 );  

  pv.startItemGroup( CheckinType(Parent(theSubgroup)) == SMFPGroup::NILPOTENT ||
		     CheckinType(Parent(theSubgroup)) == SMFPGroup::FREE_NILPOTENT);
  
  if (!theSubgroup.getParent().gic.
      haveNilpotentGroupInited())
    pv.add(Text("Compute a basis of ") + 
	   Name(theSubgroup.getParent()),
	   computeBasis.arcSlotID(),
	   50
	   );
  
  pv.add(Text("Solve using basis of ")
	 + Name( theSubgroup.getParent() ),
	 nilpotentWP.arcSlotID(),
	 50
	 );
  pv.done();
}


// -------------------------- IsSGCentral ---------------------------------- //


IsSGCentral::IsSGCentral( SMSubgroup& S)
  : Supervisor( ! S.getParent().gic.haveFastWordProblem() ),
    theSubgroup( S ),
    theChecker( S.getParent() ),
    normalClosure( *this, S.getParent().gcm().normalClosure ),
    abelianInvariants( *this, S.getParent().gcm().abelianInvariants ),
    kbSupervisor( *this, S.getParent().gcm().kbSupervisor ),
    agSupervisor( *this, S.getParent().gcm().agSupervisor ),
    nilpotentQuotients(*this, S.getParent().gcm().nilpotentQuotients),
    nilpotentWPInQuotients(*this),
    computeBasis(*this, S.getParent().gcm().computeBasis),
    nilpotentWP(*this),
    genetic(*this)
{
  if (theSubgroup.sic.isCentral() != dontknow)
    resultIsFast();
  else {
    SetOf<Word> comms;
    int numOfGens = S.getParent().getFPGroup().numberOfGenerators();
    VectorOf<Word> gens = S.getSubgroup().generators();
    int gensLen = gens.length();
    
    for( int i = 0; i < gensLen; ++i ) 
      for( int j = 0; j < numOfGens; ++j ) {
	Word w1 = gens[i];
	Word w2 = Word(Generator(j+1));
	comms |= (w1.inverse() * w2.inverse() * w1 * w2).freelyReduce();
      }
    nilpotentWPInQuotients->initialize(comms,&(theSubgroup.getParent()));
    nilpotentWP->initialize(comms,&(theSubgroup.getParent()));
    genetic->init(theSubgroup.getParent().getFPGroup(), comms, 
		  GeneticWPCM::COMMUTATORS);
    theChecker.replaceTheSet(comms);
  }
  if ( !displayInFE() ) adminStart();
}


void IsSGCentral::takeControl( )
{
  Chars explanation;
  if (!fastResult())
  if ( ! displayInFE() || freeARCs() > 0 ){
    if (nilpotentWP->state() == ComputationManager::TERMINATED){
      explanation = ":this follows from an inspection of a basis" ;
      theSubgroup.sic.putIsCentral(nilpotentWP->isTrivial());
    }
    if (nilpotentWPInQuotients->state() == ComputationManager::TERMINATED)
      if (nilpotentWPInQuotients->isTrivial()==no){
	int retClass;
	theSubgroup.sic.putIsCentral
	  (nilpotentWPInQuotients->isTrivial(retClass));
	//@njz
	//	ostrstream msgTmp;
	std::ostrstream msgTmp;
	//
	msgTmp << ":the image of "<<Name(theSubgroup)<<" in the lower central quotient of "
	       << Name(theSubgroup.getParent()) << " of class "<< retClass
	       << " is not central"<<ends;
	explanation = msgTmp.str();
      }    

    Trichotomy theAnswer(dontknow);
    if ( (  theAnswer = theChecker.isTrivial() ) != dontknow ) {
      theSubgroup.sic.putIsCentral(theAnswer);
      explanation = theChecker.getExplanation();
    }

    if ( ( theAnswer = genetic->areTrivial() ) != dontknow ) {
      explanation = "according to the genetic algorithm";
      theSubgroup.sic.putIsCentral(theAnswer);
    }

    usedOneARC();
  }
  if (theSubgroup.sic.isCentral() != dontknow){
    LogMessage msg( *this, theSubgroup );
    msg << Name( theSubgroup ) << " is ";
    if (theSubgroup.sic.isCentral() == no ) msg << "not ";
    msg << "central in " <<  Name( theSubgroup.getParent() )<<" ";
    if (explanation.length() >0)
      msg << explanation;
    msg << ".";
    msg.send();
    adminTerminate();     
    return;     
  }
}

void IsSGCentral::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Is subgroup central problem"),
		  Text("Is") + Name( theSubgroup ) + "central in"
		  + Name( Parent( theSubgroup ) ) + "?",
		  helpID("IsSGCentral",  theSubgroup.getParent()),
		  Text(Name(theSubgroup)) + "central ?"
		  );
  

  pv.startItemGroup();
  
  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 50);

  pv.add(Text("Compute abelian invariants of") + Name( Parent( theSubgroup ) ),
	 abelianInvariants.arcSlotID(),
	 15
	 );

    
  pv.startItemGroup
    ( CheckinType( Parent( theSubgroup ) ) != SMFPGroup::ABELIAN &&
      CheckinType( Parent( theSubgroup ) ) != SMFPGroup::NILPOTENT &&
      CheckinType( Parent( theSubgroup ) ) != SMFPGroup::FREE_NILPOTENT
      );

  pv.add( Text("Enumerate normal closure of relators of")
	  + Name( Parent( theSubgroup ) ),
	  normalClosure.arcSlotID(), 50);
  
  pv.add(Text("Seek a rewriting system for") + Name( Parent( theSubgroup ) ),
	 kbSupervisor.arcSlotID(),
	 50
	 );
    
  pv.add(Text("Seek a ShortLex automatic structure for")
	 + Name( Parent( theSubgroup ) ),
	 agSupervisor.arcSlotID(),
	 50
	 );

  pv.add( Text("Run genetic algorithm"), genetic.arcSlotID(), 50 );

  pv.startItemGroup( CheckinType(Parent( theSubgroup )) == SMFPGroup::FP ||
		     CheckinType(Parent( theSubgroup )) == SMFPGroup::NILPOTENT ||
		     CheckinType(Parent( theSubgroup )) == SMFPGroup::FREE_NILPOTENT
		     );
  
  int upperBound, currentClass = 2;
  if (theSubgroup.getParent().gic.isNilpotent()==yes ||
      theSubgroup.getParent().gic.isFreeNilpotent()==yes )
    upperBound = theSubgroup.getParent().gic.getNilpotentcyClass()-1;
  else
    upperBound = 10;
  
  while (currentClass <= upperBound && 
	 theSubgroup.getParent().gic.haveNilpotentQuotInited( currentClass ) )
    currentClass++;
  
  if (!(currentClass > upperBound) )
    pv.add(Text("Compute the lower central quotients for") + 
	   Name( theSubgroup.getParent() ),
	   nilpotentQuotients.arcSlotID(),
	   50
	   );
  pv.add(Text("Solve using lower central quotients of")
	 + Name( theSubgroup.getParent() ),
	 nilpotentWPInQuotients.arcSlotID(),
	 50
	 );  

  pv.startItemGroup( CheckinType(Parent(theSubgroup)) == SMFPGroup::NILPOTENT ||
		     CheckinType(Parent(theSubgroup)) == SMFPGroup::FREE_NILPOTENT);
  
  if (!theSubgroup.getParent().gic.
      haveNilpotentGroupInited())
    pv.add(Text("Compute a basis of ") + Name(theSubgroup.getParent()),
	   computeBasis.arcSlotID(),
	   50
	   );
  
  pv.add(Text("Solve using basis of")
	 + Name( theSubgroup.getParent() ),
	 nilpotentWP.arcSlotID(),
	 50
	 ); 
  pv.done();
}


// ---------------------------- IsSGNilpotent ------------------------------ //


IsSGNilpotent::IsSGNilpotent(const SMSubgroup& S, int nilpClass)
  : Supervisor( true/*! S.getParent().gic.haveFastWordProblem()*/ ),
    theSubgroup( S ),
    theClass(nilpClass),
    theChecker( S.getParent(), nilpClass+1, S.getSubgroup().generators() ),
    theArcer( *this ),
    normalClosure( *this, S.getParent().gcm().normalClosure ),
    abelianInvariants( *this, S.getParent().gcm().abelianInvariants ),
    kbSupervisor( *this, S.getParent().gcm().kbSupervisor ),
    agSupervisor( *this, S.getParent().gcm().agSupervisor ),
    nilpotentQuotients(*this, S.getParent().gcm().nilpotentQuotients),
    nilpotentWPInQuotients(*this)
{
  nilpotentWPInQuotients->initialize(theClass+1,&(theSubgroup.getParent()),
				     theSubgroup.getSubgroup().generators(),
				     theClass+1);
  if ( ! displayInFE() ) adminStart();
  theArcer.setArguments(&theChecker);
}


void IsSGNilpotent::takeControl( )
{
  Trichotomy theAnswer = dontknow;
  Chars explanation;

  if ( ! displayInFE() || freeARCs() > 0 ) {
    if (nilpotentWPInQuotients->state() == ComputationManager::TERMINATED)
      if (nilpotentWPInQuotients->isTrivial()==no){
	int retClass;
	theAnswer = nilpotentWPInQuotients->isTrivial(retClass);
	//@njz
	//	ostrstream msgTmp;
	std::ostrstream msgTmp;
	//
	msgTmp << "because there are non trivial commutators in lower central quotient of class " 
	       << retClass << ends;
	explanation = msgTmp.str();
      }    
    if (theAnswer != dontknow){
 	LogMessage msg( *this, theSubgroup );
	msg << Name( theSubgroup ) << " is ";
	if ( theAnswer == no ) msg << "not ";
	msg << "nilpotent of class " << theClass << explanation;
	msg.send();
	adminTerminate();     
	return;     
    }
    
    if( theArcer.takeControl() ) 

      if( (theAnswer = theArcer.getRetValue()) != dontknow ) {
	LogMessage msg( *this, theSubgroup );
	msg << Name( theSubgroup ) << " is ";
	if ( theAnswer == no ) msg << "not ";
	msg << "nilpotent of class " << theClass;
	
	explanation = theChecker.getExplanation();
	if( explanation.length() > 0 )
	  msg << " : " << explanation;
	
	msg << ".";
	msg.send();
	adminTerminate();
	return;
      }

    usedOneARC();
  }
}


void IsSGNilpotent::viewStructure(ostream& ostr) const
{
  char classStr[10];
  sprintf(classStr,"%u",theClass);

  ProblemView pv( ostr, oid(), Text("Is subgroup nilpotent problem"),
		  Text("Is") + Name( theSubgroup ) + "nilpotent of class"
		  + classStr + "?",
		  helpID("IsSGNilpotent", theSubgroup.getParent()),
		  Text(Name(theSubgroup)) + "nilpotent ?"
		  );
  pv.startItemGroup();

  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 50);
  
  pv.add(Text("Compute abelian invariants of") + Name(Parent( theSubgroup ))
	 + "abelianized",
	 abelianInvariants.arcSlotID(),
	 15
	 );  

  pv.startItemGroup
    ( CheckinType(Parent( theSubgroup )) == SMFPGroup::FP ||
      CheckinType(Parent( theSubgroup )) == SMFPGroup::SMALL_CANCELLATION ||
      CheckinType(Parent( theSubgroup )) == SMFPGroup::ONE_RELATOR
      );
  
  pv.add(Text("Enumerate normal closure of relators"),
	 normalClosure.arcSlotID(), 50);
  
  pv.add(Text("Seek a rewriting system for") + Name(Parent( theSubgroup )),
	 kbSupervisor.arcSlotID(),
	 50
	 );
    
  pv.add(Text("Seek a ShortLex automatic structure for")
	 + Name(Parent( theSubgroup )),
	 agSupervisor.arcSlotID(),
	 50
	 );

  pv.startItemGroup( CheckinType(Parent( theSubgroup )) == SMFPGroup::FP 
		     );
  
  int upperBound, currentClass = 2;
  if (theSubgroup.getParent().gic.isNilpotent()==yes ||
      theSubgroup.getParent().gic.isFreeNilpotent()==yes)
    upperBound = theSubgroup.getParent().gic.getNilpotentcyClass()-1;
  else
    upperBound = 10;
  
  while (currentClass <= upperBound && 
	 theSubgroup.getParent().gic.haveNilpotentQuotInited( currentClass ) )
    currentClass++;
  
  if (!(currentClass > upperBound) )
    pv.add(Text("Compute the lower central quotients for") + 
	   Name( theSubgroup.getParent() ),
	   nilpotentQuotients.arcSlotID(),
	   50
	   );
  pv.add(Text("Solve using lower central quotients of")
	 + Name( theSubgroup.getParent() ),
	 nilpotentWPInQuotients.arcSlotID(),
	 50
	 );  

  pv.done();
}


//--------------------------- IsSGTrivial -----------------------------------//


IsSGTrivial::IsSGTrivial( SMSubgroup& S)
  : Supervisor( ! S.getParent().gic.haveFastWordProblem() ),
    theSubgroup( S ),
    theChecker( S.getSubgroup().generators(), S.getParent() ),
    normalClosure( *this, S.getParent().gcm().normalClosure ),
    abelianInvariants( *this, S.getParent().gcm().abelianInvariants ),
    kbSupervisor( *this, S.getParent().gcm().kbSupervisor ),
    agSupervisor( *this, S.getParent().gcm().agSupervisor ),
    nilpotentQuotients(*this, S.getParent().gcm().nilpotentQuotients),
    nilpotentWPInQuotients(*this),
    computeBasis(*this, S.getParent().gcm().computeBasis),
    nilpotentWP(*this),
    genetic(*this)
{
  if (theSubgroup.sic.isTrivial() != dontknow)
    resultIsFast();
  else {
    nilpotentWPInQuotients->initialize(theSubgroup.getSubgroup().generators(),
				       &(theSubgroup.getParent()));
    nilpotentWP->initialize(theSubgroup.getSubgroup().generators(),
			    &(theSubgroup.getParent()));
    genetic->init( theSubgroup.getParent().getFPGroup(), 
		   makeSetOf(theSubgroup.getSubgroup().generators()),
		   GeneticWPCM::SET_OF_WORDS );
  }
  if ( ! displayInFE() ) adminStart();
}


void IsSGTrivial::takeControl( )
{
  Chars explanation;
  if (!fastResult())
    if ( ! displayInFE() || freeARCs() > 0 ){
      if (nilpotentWP->state() == ComputationManager::TERMINATED){
	explanation = Chars("all generators of ")+ Text(Name(theSubgroup)) +" are trivial";
	theSubgroup.sic.putIsTrivial(nilpotentWP->isTrivial());
      }
      if (nilpotentWPInQuotients->state() == ComputationManager::TERMINATED)
	if (nilpotentWPInQuotients->isTrivial()==no){
	  int retClass;
	  theSubgroup.sic.putIsTrivial
	    (nilpotentWPInQuotients->isTrivial(retClass));
	  //@njz
	  //	ostrstream msgTmp;
	  std::ostrstream msgTmp;
	  //
	  msgTmp << "the image of "<<Name(theSubgroup)<<" in the lower central quotient of "
		 << Name(theSubgroup.getParent()) << " of class "<< retClass
		 << " is not trivial"<<ends;
     	  explanation = msgTmp.str();
	}    

      Trichotomy theAnswer(dontknow);
      if ( ( theAnswer = theChecker.isTrivial() ) != dontknow ) {
	theSubgroup.sic.putIsTrivial(theAnswer);
	explanation = theChecker.getExplanation();
      }
      
      if ( ( theAnswer = genetic->areTrivial() ) != dontknow ) {
	explanation = "according to the genetic algorithm";
	theSubgroup.sic.putIsTrivial(theAnswer);
      }

      usedOneARC();
    }
  if (theSubgroup.sic.isTrivial() != dontknow){
    LogMessage msg( *this, theSubgroup );
    msg << Name( theSubgroup ) << " is a ";
    if (theSubgroup.sic.isTrivial() == no ) msg << "non-";
    msg << "trivial subgroup of " <<  Name( Parent( theSubgroup ) );
    if (explanation.length() > 0)
      msg <<": " << explanation;
    msg << ".";
    msg.send();
    adminTerminate();     
    return;     
  }  
}


void IsSGTrivial::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Subgroup triviality problem"),
		  Text("Is the subgroup") + Name( theSubgroup ) + "of"
		  + Name( Parent( theSubgroup ) ) + "trivial ?",
		  helpID("IsSGTrivial",  theSubgroup.getParent()),
		  Text(Name(theSubgroup)) + "= 1 ?"
		  );
  

  pv.startItemGroup();
  
  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 50);

  pv.add(Text("Compute abelian invariants of") + Name( Parent( theSubgroup ) ),
	 abelianInvariants.arcSlotID(),
	 15
	 );

/*
  pv.startItemGroup( CheckinType( Parent( theSubgroup ) ) ==
		     SMFPGroup::ONE_RELATOR);

  pv.add("Compute the Magnus breakdown", orwp.arcSlotID(), 50);
*/
    
  pv.startItemGroup
    ( CheckinType( Parent( theSubgroup ) ) != SMFPGroup::ABELIAN &&
      CheckinType( Parent( theSubgroup ) ) != SMFPGroup::NILPOTENT &&
      CheckinType( Parent( theSubgroup ) ) != SMFPGroup::FREE_NILPOTENT
      );

  pv.add(Text("Enumerate normal closure of relators"),
	 normalClosure.arcSlotID(), 50);
  
  pv.add(Text("Seek a rewriting system for") + Name( Parent( theSubgroup ) ),
	 kbSupervisor.arcSlotID(),
	 50
	 );
    
  pv.add(Text("Seek a ShortLex automatic structure for")
	 + Name( Parent( theSubgroup ) ),
	 agSupervisor.arcSlotID(),
	 50
	 );

  pv.add( Text("Run genetic algorithm"), genetic.arcSlotID(), 50 );
  
  pv.startItemGroup( CheckinType(Parent(theSubgroup)) == SMFPGroup::NILPOTENT ||
		     CheckinType(Parent(theSubgroup)) == SMFPGroup::FREE_NILPOTENT ||
		     CheckinType(Parent(theSubgroup)) == SMFPGroup::FP);
  
  int upperBound, currentClass = 2;
  if (theSubgroup.getParent().gic.isNilpotent()==yes ||
      theSubgroup.getParent().gic.isFreeNilpotent()==yes)
    upperBound = theSubgroup.getParent().gic.getNilpotentcyClass()-1;
  else
    upperBound = 10;
  
  while (currentClass <= upperBound && 
	 theSubgroup.getParent().gic.haveNilpotentQuotInited( currentClass ) )
    currentClass++;
  
  if (!(currentClass > upperBound) )
    pv.add(Text("Compute the lower central quotients for") + 
	   Name( theSubgroup.getParent() ),
	   nilpotentQuotients.arcSlotID(),
	   50
	   );
  pv.add(Text("Solve using lower central quotients of")
	 + Name( theSubgroup.getParent() ),
	 nilpotentWPInQuotients.arcSlotID(),
	 50
	 );  

  pv.startItemGroup( CheckinType(Parent(theSubgroup)) == SMFPGroup::NILPOTENT ||
		     CheckinType(Parent(theSubgroup)) == SMFPGroup::FREE_NILPOTENT);
  
  if (!theSubgroup.getParent().gic.
      haveNilpotentGroupInited())
    pv.add(Text("Compute a basis of ") + Name(theSubgroup.getParent()),
	   computeBasis.arcSlotID(),
	   50
	   );
  
  pv.add(Text("Solve using basis of")
	 + Name( theSubgroup.getParent() ),
	 nilpotentWP.arcSlotID(),
	 50
	 ); 
 
  pv.done();
}


// -------------------------- SGIndexProblem ------------------------------- //


SGIndexProblem::SGIndexProblem(SMSubgroup& subgroup)
  : Supervisor( ! subgroup.sic.haveIndex() ),
    theSubgroup( subgroup ),
    sgToddCoxeter(*this,subgroup.scm().sgToddCoxeter),
    sgGHToddCoxeter(*this,subgroup.scm().sgGHToddCoxeter)
{
  if ( theSubgroup.sic.haveIndex() ) resultIsFast();
}


void SGIndexProblem::takeControl( )
{

  if ( theSubgroup.sic.haveIndex() ) {
    int theIndex = theSubgroup.sic.index();

    LogMessage msg( *this, theSubgroup );
     msg << Name( theSubgroup ) << " has";
    
    if( theIndex == 0 )
      msg << " infinite index";
    else
      msg << " index " << theIndex;
    if( !fastResult() )
      msg << ": according to the Todd-Coxeter algorithm";
    msg << ".";
    msg.send();
    adminTerminate();
    return;
  }

}


void SGIndexProblem::viewStructure(ostream& ostr) const
{
 ProblemView pv( ostr, oid(), Text("Index of a subgroup problem"),
		  Text("Compute the index of") + Name( theSubgroup )
		  + Text("in") + Name( theSubgroup.getParent() ),
		  helpID("SGIndexProblem", theSubgroup.getParent()),
		  Text("Index") + Name(theSubgroup)
		  );
  


  pv.startItemGroup( CheckinType( theSubgroup.getParent() ) == SMFPGroup::FP
		     );
  pv.add(Text("Enumerate cosets"),
	 sgToddCoxeter.arcSlotID(),
	 100
	 );
 
  pv.add(Text("GH Todd-Coxeter"),
	 sgGHToddCoxeter.arcSlotID(),
	 100
	 );

  pv.done();
}


// ---------------------------SGPresentationProblem--------------------------//

// -------------------------------ApproxMethodARCer--------------------------//

void ApproxMethodARCer::setArguments( const FPGroup& g , 
				      const VectorOf<Word>& v )
{
  sg = Subgroup(g,v);
}

FPGroup ApproxMethodARCer::getRetValue( )
{
  return result;
}

Chars ApproxMethodARCer::tmpOutputFilename()
{
  return tmpOutput.getFileName();
}

void ApproxMethodARCer::runComputation( )
{
  PresentationsOfSubgroup PS( sg );
  result = PS.makePresentation(tmpOutput);
}

void ApproxMethodARCer::writeResults( ostream& out)
{
  out < result;
}

void ApproxMethodARCer::readResults( istream& in )
{
  in > result;
}


// -----------------------------ApproxMethod-------------------------------//

ApproxMethod::ApproxMethod(SGPresentationProblem& sgp)
  : ComputationManager( false ), 
    arcer(*this) , theSubgroup(sgp.getTestSubgroup()) , result() 
{
  arcer.setArguments(theSubgroup.getParent().getFPGroup(),
		     theSubgroup.getSubgroup().generators());
  adminStart();
}
    
FPGroup ApproxMethod::getResult()
{
  return result;
}

Chars ApproxMethod::tmpOutputFilename()
{
  return arcer.tmpOutputFilename();
}

void ApproxMethod::takeControl( )
{
  if ( freeARCs() > 0 )
    if (arcer.takeControl())
      {
	result = arcer.getRetValue();
	adminTerminate();
	return;
      }
    else
      usedOneARC();
}

void ApproxMethod::start( )
{
  
}

void ApproxMethod::terminate( )
{

}


// -----------------------------TCMethodARCer---------------------------//

void TCMethodARCer::setArguments( const FPGroup& g , 
				  const VectorOf<Word>& v,
				  const PermutationRepresentation& prep )
{
  sg = Subgroup(g,v);
  pr = prep;
}

FPGroup TCMethodARCer::getRetValue( )
{
  return result;
}

Chars TCMethodARCer::tmpOutputFilename()
{
  return tmpOutput.getFileName();
}

void TCMethodARCer::runComputation( )
{
  PresentationsOfSubgroup PS( sg );
  result = PS.makePresentationTC( pr, tmpOutput );
}

void TCMethodARCer::writeResults( ostream& out)
{
  out < result;
}

void TCMethodARCer::readResults( istream& in )
{
  in > result;
}


// -------------------------------TCMethod------------------------------//

TCMethod::TCMethod(SGPresentationProblem& sgp)
  : ComputationManager( false ), 
    arcer(*this) , theSubgroup(sgp.getTestSubgroup()) , result() , init(false)
{ 
  adminStart();
}
    
FPGroup TCMethod::getResult()
{
  return result;
}

Chars TCMethod::tmpOutputFilename()
{
  return arcer.tmpOutputFilename();
}

void TCMethod::takeControl( )
{
   if ( freeARCs() > 0 && theSubgroup.sic.haveSchreierTransvl() )
     {
       if( !init )
	 {
	   arcer.setArguments(theSubgroup.getParent().getFPGroup(),
			      theSubgroup.getSubgroup().generators(),
			      theSubgroup.sic.getSchreierTransvl());
	   init = true;
	 }
       
       if (arcer.takeControl())
	 {
	   result = arcer.getRetValue();
	   adminTerminate();
	   return;
	 }
       else
	 usedOneARC();
     }

}   
void TCMethod::start( )
{
  
}

void TCMethod::terminate( )
{

}


// ---------------------------SGPresentationProblem--------------------------//

SGPresentationProblem::SGPresentationProblem(SMSubgroup& s)
  : Supervisor( true ) ,
    theSubgroup( s ) ,
    done( false ) ,
    term( false ) ,
    am( *this ) ,
    tcm( *this ) ,
    sgToddCoxeter(*this,theSubgroup.scm().sgToddCoxeter)
{
  if( theSubgroup.sic.haveIndex() )
    { 
      int index = theSubgroup.sic.index();
      if ( !index ) resultIsFast();
    }   
  
  if ( ! displayInFE() ) adminStart();
}

SMSubgroup& SGPresentationProblem::getTestSubgroup()
{
  return theSubgroup;
}

void SGPresentationProblem::takeControl()
{
  if( theSubgroup.sic.haveIndex() && !term )
    { 
      int index = theSubgroup.sic.index();
      
      if ( !index )
	{
	  LogMessage msg3( *this, theSubgroup );
	  // @am explanation was changed
	  msg3 << Name( theSubgroup ) << " is of infinite index in " 
	       << Name( theSubgroup.getParent() ) << ". "
	       << "There is no finite RS-presentation for" 
	       <<  Name( theSubgroup ) <<".";
	  msg3.send();
	  
	  adminTerminate();
	  return;
	}
      
      term = true;
      
    }
  
  if( am->state() == ComputationManager::TERMINATED )
    {
      LogMessage msg1( *this, theSubgroup );
      
      msg1 << Name( theSubgroup ) << " has presentation according to approximation method:";
      msg1 << am->getResult(); 
      msg1.send();
      
      LogMessage msg2( *this, theSubgroup );
      msg2 << Link("Click here to see the steps of RS-process that uses approximation method" , "ApproximationsRS-process" , am->tmpOutputFilename() );
      msg2.send();
      
      adminTerminate();
      return;
    }
      
  if( tcm->state() == ComputationManager::TERMINATED )
    {
      LogMessage msg4( *this, theSubgroup );
      
      msg4 << Name( theSubgroup ) << " has presentation according to method that uses Todd-Coxeter procedure:";
      msg4 << tcm->getResult(); 
      msg4.send();
      
      LogMessage msg5( *this, theSubgroup );
      msg5 << Link("Click here to see the steps of RS-process that uses Todd-Coxeter procedure" , "ToddCoxeterRS-process" , tcm->tmpOutputFilename() );
      msg5.send();

      adminTerminate();
      return;
    }
}

void SGPresentationProblem::viewStructure( ostream& ostr ) const
{
  ProblemView pv( ostr, oid(), Text("Subgroup presentation problem"),
		  Text("Compute an RS-presentation for") + Name( theSubgroup )
		  + Text("in") + Name( theSubgroup.getParent() ),
		  helpID("SGPresentationProblem", theSubgroup.getParent()),
		  Text("Pres") + Name( theSubgroup )
		  );  
  
  pv.startItemGroup();
  
  pv.add(Text("Approximation method"), am.arcSlotID(), 45 );

  pv.add(Text("Compute Schreier transversal"), sgToddCoxeter.arcSlotID(), 45 );

  pv.add(Text("Method that uses Todd-Coxeter procedure"), tcm.arcSlotID(), 10 );
  
  pv.done();
  
}


void SGPresentationProblem::start( )
{
  
}

void SGPresentationProblem::terminate( )
{

}


//-----------------------------SGRewriteWordProblem-----------------------//

void RewriteWordARCer::setArguments(const PermutationRepresentation& pr1,const FreeGroup& K,const Word& w)
{
  theWord = w;
  F = K;
  pr =  new  PermutationRepresentation(pr1);
}

Word RewriteWordARCer::rewrite(const PermutationRepresentation& pr,const VectorOf<Word>& b,
			       const FreeGroup& F,const Word& w,bool flag)
{
  Word h;
  Word rw;
  FreeGroup K(b.length());
  
  if( flag )
    {
      tmpOutput << " "; 
      F.printWord(tmpOutput,w);
      tmpOutput << " = " << endl << " = ";
    }
  
  for( int i = 0 ; i < w.length() ; i++ )
    {
      Word h = pr.representativeOf(w.initialSegment(i));
      Word h1 = pr.representativeOf(w.initialSegment(i+1));
      Word s = h * w[i] * F.inverseOf(h1);
      s = s.freelyReduce();
      
      if( flag )
	{
      
	  if( rw.length() )
	    {
	      K.printWord(tmpOutput,rw);
	      tmpOutput << "  ";
	    }
	  
	  tmpOutput << " ( ";
	  F.printWord(tmpOutput,h);
	  tmpOutput << "   ";
	  F.printWord(tmpOutput,w[i]);
	  tmpOutput << "   ( ";
	  F.printWord(tmpOutput,h1);
	  tmpOutput << " )^-1 )   ";
      
	  if(h1.length())
	    {
	      F.printWord(tmpOutput,h1);
	      tmpOutput << "   ";
	    }
	  
	  if( i != w.length() - 1 )
	    F.printWord(tmpOutput,
			(w.initialSegment(i+1).inverse() * w).freelyReduce());
	  tmpOutput << " = " << endl << " = ";
	}
      
      int k = b.indexOf(s);
      
      Word g;
      
      if( k < 0 )
	{
	  s = F.inverseOf(s);
	  k = b.indexOf(s);
	  
	  if( k >= 0 )
	    g = Generator(-(k+1));
	  
	}
      else
	g = Generator(k+1);
      
      rw *= g;
    
      if( flag )
	{
	  K.printWord(tmpOutput,rw);
	  tmpOutput << "   ";
	  
	  if( h1.length() )
	    {
	      F.printWord(tmpOutput,h1);
	      tmpOutput << "   ";
	    }
	  
	  if( i != w.length() - 1 )
	    F.printWord(tmpOutput,
			(w.initialSegment(i+1).inverse() * w).freelyReduce());
	  if( i != w.length() - 1 )
	    tmpOutput << " = " << endl << " = ";
	}
    }
  
  if( flag )
    tmpOutput << endl;
  
  return rw;
}

Chars RewriteWordARCer::tmpOutputFilename()
{
  return tmpOutput.getFileName();
}

int RewriteWordARCer::getRank()
{
  return rank;
}

Word RewriteWordARCer::getRepresentative()
{
  return pr->representativeOf(theWord);
}



void RewriteWordARCer::runComputation( )
{
  VectorOf<Word> sch = pr->getRepresentatives();
  tmpOutput << "Schreier generators of the subgroup:" << endl;
  VectorOf<Word> basis;
  Word w;
  
  int count = 1;
  for( int i = 0 ; i < F.rank() ; i++ ) 
    {
      Word x = Generator(i+1);
      
      for( int r = 0 ; r < sch.length() ; r++ )
	{
	  Word u = sch[r];
	  Word tmp = pr->representativeOf(u * x);
	  w = u * x * F.inverseOf(tmp);
	  w = w.freelyReduce();
	  
	  if( w.length() ) 
	    {
	      basis.append(w);
	      tmpOutput << "x" << count << " = j(s" << r + 1 << ",";
	      F.printWord(tmpOutput,x);
	      tmpOutput << ") = ";
	      F.printWord(tmpOutput,w);
	      tmpOutput << endl;
	      count++;
	    }
	}
    }
  
  tmpOutput << endl << " The steps of the rewriting process:" << endl << endl;
  
  SGofFreeGroup H(F,basis);
  if(H.contains(theWord))
    {
      FreeGroup K(basis.length());
      
      tmpOutput << " ";
      F.printWord(tmpOutput,theWord);
      tmpOutput << "  is in the subgroup and can be expressed in terms of Schreier generators as follows: " << endl << " ";
      F.printWord(tmpOutput,theWord);
      tmpOutput << " = ";
      
      K.printWord(tmpOutput,rewrite(*pr,basis,F,theWord,0));
      tmpOutput << endl << endl;
      
      tmpOutput << " The process for expressing is as follows: " << endl;
      retValue = rewrite(*pr,basis,F,theWord,1);
    }  
  else
    retValue = Word();
  
  tmpOutput << end;
}

void RewriteWordARCer::writeResults( ostream& out ) 
{
  out < retValue;

}

void RewriteWordARCer::readResults( istream& in )
{
  in > retValue;

}


SGRewriteWordProblem::SGRewriteWordProblem(SMSubgroup& s,SMWord& w)
  : Supervisor( true ),
    theSubgroup( s ),
    theWord( w ),
    init( false ),
    arcer(*this),
    sgToddCoxeter(*this,theSubgroup.scm().sgToddCoxeter)
{
  if( theSubgroup.sic.haveIndex() )
    {
      int index = theSubgroup.sic.index();  
      if ( !index ) resultIsFast();
    }
  
  if( !theWord.getWord().length() )
    resultIsFast();
}


void SGRewriteWordProblem::takeControl( )
{
  if( !theWord.getWord().length() )
    {
      LogMessage msg1( *this, theSubgroup, theWord );
      msg1 << Name( theWord ) << " is trivial.";
      msg1.send();
      
      adminTerminate();
      return;
    }
  
  if ( theSubgroup.sic.haveIndex() ) 
    {
      int index = theSubgroup.sic.index();
      
      if( index == 0 )
	{
	  LogMessage msg2( *this, theSubgroup, theWord );
	  msg2 << Name( theSubgroup ) << " has infinite index , so we can't rewrite " << Name( theWord ) << ".";
	  msg2.send();
	  
	  adminTerminate();
	  return;
	}
      else
	{ 
	  if( !init )
	    {
	      arcer.setArguments(theSubgroup.sic.getSchreierTransvl(),
				 theSubgroup.getParent().getFreePreimage(),
				 theWord.getWord());
	      init = true;
	    }
	  
	  if(freeARCs() > 0)
	    if(arcer.takeControl())
	      {
		Word result = arcer.getRetValue();
		if( !arcer.getRepresentative().length() )
		  {
		    LogMessage msg3( *this, theSubgroup, theWord );
		    msg3 << Name( theWord );
		    msg3 << " in terms of Schreier generators: ";
		    
		    FreeGroup T(arcer.getRank());
		    T.printWord(msg3,result);
		    
		    msg3 << ".";
		    msg3.send();
		    
		    LogMessage msg4( *this, theSubgroup ,theWord );
		    msg4 << Link("Click here to see the Schreier generators" , "SchreierGens" , arcer.tmpOutputFilename() );
		    msg4.send();
     		  }
		else
		  {
		    LogMessage msg5( *this, theSubgroup, theWord );
		    msg5 << Name( theWord );
		    msg5 << " is not in ";
		    msg5 << Name( theSubgroup ) << "; its representative in the Schreier transversal is ";
		    
		    FreeGroup F(theSubgroup.getParent().getFreePreimage());
		    F.printWord(msg5,arcer.getRepresentative());
		    
		    msg5 << ".";

		    msg5.send();
		  }
		
		adminTerminate();
		return;
	      }
	    else
	      usedOneARC();
	}
    }
} 

void SGRewriteWordProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Rewriting of a word in terms of Schreier generators"),
		  Text("Rewrite a word in terms of Schreier generators ") + 
		  Name( theWord ) ,
		  helpID("SGRewriteWordProblem",theWord.getParent()),
		  Text("rewrite") + Name(theWord)
		  );
  pv.startItemGroup();
  
  if (!theSubgroup.sic.haveIndex())
    pv.add(Text("Enumerate cosets of")
	   + Name( theSubgroup ),
	   sgToddCoxeter.arcSlotID(),
	   100
	   );   
  
  pv.add(Text("Rewrite ")+Name( theWord ),
  	 ThisARCSlotID(),
  	 10);
  
  pv.done();
}

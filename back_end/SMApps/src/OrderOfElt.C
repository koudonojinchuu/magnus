// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes OrderOfEltARCer, OrderOfElt
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#include "OrderOfElt.h"
#include "SMWord.h"
#include "OutMessages.h"
#include "viewStructure.h"

//---------------------------------------------------------------------------//
//---------------------------- OrderOfEltInQuotients  -----------------------//
//---------------------------------------------------------------------------//


OrderOfEltInQuotients::OrderOfEltInQuotients(OrderOfElt& sup)
  : 
    currentClass(2),
    arcer( *this ),
    isStarted (false),
    theWord(sup.getWord()),
    theGroup(sup.getWord().getParent()),
    upperBound( 10 )
{
  adminStart();
}

void OrderOfEltInQuotients::takeControl( )
{
  if ( freeARCs() > 0 ) {
    if (currentClass > upperBound ){
      adminTerminate();
      return;
    }   
    if ( ! isStarted && theGroup.gic.haveNilpotentQuotInited( currentClass ) ) {
      arcer.setArguments( theGroup.gic.getNilpotentQuotInited( currentClass ),
			  theWord.getWord());
      isStarted = true;
    }
    
    if ( isStarted ) 
      if (arcer.takeControl()){
	if (arcer.getOrder() == 0){
	  theWord.wic.putHaveOrder(0);
	  adminTerminate();
	  return;
	}
	currentClass++;
	isStarted = false;
    }
    usedOneARC();
  }
}

// -------------------------- OrderOfEltARCer ------------------------------ //


OrderOfEltARCer::OrderOfEltARCer( ComputationManager& boss,
				  const class SMFPGroup& group, 
				  const Word& word ) 
  : ARCer( boss ), 
    theGroup( group ),
    theWord( word ),
    current( word ),
    retValue( -1 ),
    power( 1 )
{ } 


void OrderOfEltARCer::runComputation( )
{
  while( true ) {

    SetOfWordsChecker theChecker(current, theGroup);
    Trichotomy answer = dontknow;

    if( (answer = theChecker.isTrivial()) != no ) {
      if( answer == yes )
	retValue = power;
      return;
    }

    ++power;
    current = (current * theWord).freelyReduce();
  }
}


void OrderOfEltARCer::writeResults( ostream& out ) 
{
  out < retValue < power;
}


void OrderOfEltARCer::readResults( istream& in )
{
  in > retValue > power;
}


// --------------------------- OrderOfElt --------------------------------- //


OrderOfElt::OrderOfElt( SMWord& word)
  : Supervisor( true /*! word.getParent().gic.haveFastWordProblem()*/ ),
    theWord( word ),
    theArcer( *this, word.getParent(), word.getWord() ),
    explanation(""),
    triedAbelianization( false ),
    normalClosure( *this, word.getParent().gcm().normalClosure ),
    abelianInvariants( *this, word.getParent().gcm().abelianInvariants ),
    kbSupervisor( *this, word.getParent().gcm().kbSupervisor ),
    agSupervisor( *this, word.getParent().gcm().agSupervisor ),
    nilpotentQuotients(*this, word.getParent().gcm().nilpotentQuotients),
    orderOfEltInQuotients(*this)
{
  if (theWord.wic.haveOrder())
    resultIsFast();
  if ( !displayInFE() ) adminStart();
}


int OrderOfElt::getOrder( )
{
  if ( ! triedAbelianization && 
      theWord.getParent().gic.haveCyclicDecomposition( ) ) {
    
    triedAbelianization = true;
    if( theWord.getParent().gic.getCyclicDecomposition()
       .orderOfElt( theWord.getWord() ) == 0 ) {
      explanation = "this follows from an inspection of the abelianization of the group";
      return 0;
    }
  }
  
  if( theArcer.takeControl() ){
    return theArcer.getRetValue();
  }
  
  return -1;
}


void OrderOfElt::takeControl( )
{
  Integer theAnswer;
  
  if ( ! displayInFE() || freeARCs() > 0 ){
    if (theWord.wic.haveOrder())
      theAnswer = theWord.wic.getOrder();
    else
      theAnswer = getOrder();
    
    if ( theAnswer  != -1 ) {
      LogMessage msg( *this, theWord );
      msg << Name( theWord ) << " has ";
      if ( theAnswer == 0 ) 
	msg << "infinite order";
      else
	msg << "order " << theAnswer;
      
      if( explanation.length() > 0 )
	msg << " : " << explanation;
      msg << ".";
      msg.send();
      
      if (!fastResult())
	theWord.wic.putHaveOrder(theAnswer);
      adminTerminate();
    }
    else
      usedOneARC();
  }
}


void OrderOfElt::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Is element of finite order problem"),
		  Text("Is") + Name( theWord ) + "of finite order in"
		  + Name( Parent( theWord ) ) + "?",
		  helpID("OrderOfElt", theWord.getParent()),
		  Text("Order") + Name(theWord)
		  );
  

  pv.startItemGroup();
  
  pv.add( Text("For the use of this problem"), ThisARCSlotID(), 50);

  pv.add( Text("Compute abelian invariants of") + Name( Parent( theWord ) ),
	  abelianInvariants.arcSlotID(),
	  15
	  );

    
  pv.startItemGroup
    ( CheckinType( Parent( theWord ) ) != SMFPGroup::ABELIAN &&
      CheckinType( Parent( theWord ) ) != SMFPGroup::NILPOTENT
      );

  int currentClass = 2;
  while (currentClass <= 10 && 
	 theWord.getParent().gic.haveNilpotentQuotInited( currentClass ) )
    currentClass++;
  
  if (!(currentClass > 10) )
    pv.add(Text("Compute the nilpotent quotients for") + 
	   Name( theWord.getParent() ),
	   nilpotentQuotients.arcSlotID(),
	   100
	   );
  pv.add(Text("Compute the problem, using nilpotent quotients for")
	 + Name( theWord.getParent() ),
	 orderOfEltInQuotients.arcSlotID(),
	 100
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
  
  pv.done();
}

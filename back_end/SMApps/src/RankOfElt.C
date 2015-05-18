// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes RankOfElement, RankOfElementProblem
//
// Principal Author: Alexander Ushakov
//
// Status: in progress
//
// Revision History:
//

#include "Set.h"
#include "RankOfElt.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "SMSubgroup.h"



// -------------------- RankOfElementProblem --------------------------- //


RankOfElementProblem::RankOfElementProblem(const SMWord& w )
    : Supervisor( true ), 
      theWord( w ), 
      rankOfElement( *this ),
      linkHasBeenSent( false )      
{ 

}

 
const SMWord& RankOfElementProblem::getWord( ) const 
{ 
  return theWord; 
}


void RankOfElementProblem::takeControl( )
{
  if( !linkHasBeenSent ) {
  
    linkHasBeenSent = true;
    
    LogMessage msg( *this, theWord );
    msg << Link( Text("Click here to see the rank of ") + Name( theWord ),
		"RankOfSubgroup", rankOfElement->getFileName(), true);
    msg.send();

  }

  Trichotomy answer = rankOfElement->getAnswer();
  if ( rankOfElement.exists() && answer!=dontknow) {
    
    LogMessage msg( *this, theWord );
    msg.send();
    
    adminTerminate();
    return;
  }

}


void RankOfElementProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Rank"),
		  Text("Rank of ") + Name( theWord ),
		  helpID("RankOfElementProblem", theWord.getParent()),
		  "Rank"
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of the algorithm"), rankOfElement.arcSlotID(), 100);

  pv.done();
}


// ------------------------ RankOfElement ------------------------------ //


VectorOf<Word> RankOfElement::constructVector( const Word& word )
{
  VectorOf<Word> result(1);
  result[0] = word;
  return result;
}


RankOfElement::RankOfElement(RankOfElementProblem& problemObject)
  : theWord( problemObject.getWord( ) ),
    theGroup( theWord.getParent().getFreePreimage() ),
    theARCer(*this, theWord.getParent(), SGofFreeGroup(theGroup , constructVector( theWord.getWord() )) )
{
  adminStart();
}


RankOfElement::~RankOfElement( )
{

}


void RankOfElement::takeControl( )
{
  
  if ( freeARCs() > 0 ) {
    
    if ( theARCer.takeControl() ) {
      
      adminTerminate();
      return;
    }   
    
    usedOneARC();
  }

}











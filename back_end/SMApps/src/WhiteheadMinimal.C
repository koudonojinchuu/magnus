// Copyright (C) 1999 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes
//        
// Principal Author: Alexei Miasnikov
//
// Status: in progress
//
// Revision History:
//

#include "WhiteheadMinimal.h"
#include "OutMessages.h"
#include "viewStructure.h"



// -------------------------- GAFindWhiteheadMinimalArcer ---------------------- //


GAFindWhiteheadMinimalArcer::GAFindWhiteheadMinimalArcer( ComputationManager& boss )
  : ARCer( boss )
{ } 

void GAFindWhiteheadMinimalArcer::setArguments( FreeGroup f,
						const VectorOf<Word>& v )
{
  theGroup  =  f;
  theTuple = v;
}

void GAFindWhiteheadMinimalArcer::runComputation( ) 
{
  ACConfig c( 50, 10000, 1, 
	      85, 95, 1, 
	      0, 1,
	      75, 5,15,5,0,0
	      );


 GAIsPartOfBasis wh_minimal(theGroup,c,theTuple);
 
 wh_minimal.isPartOfBasis( compFile,resultFile.getFileName() );

 theAuto = wh_minimal.getAutomorphism();

}

void  GAFindWhiteheadMinimalArcer::writeResults( ostream& out )
{
  out < theAuto;
}

void GAFindWhiteheadMinimalArcer::readResults( istream& in )
{
  in > theAuto;
}


// -------------------- FindWhiteheadMinimalProblem --------------------------- //


FindWhiteheadMinimalProblem::FindWhiteheadMinimalProblem(const SMVectorOfWords& w)
    : Supervisor( true ), 
      theTuple( w ), 
      arcer( *this ),
      linkHasBeenSent( false )
{ 
  arcer.setArguments( w.getParent().getFreePreimage(),
		      w.getWords());
}

void FindWhiteheadMinimalProblem::terminate( )
{
  LogMessage msg( *this, theTuple );
  msg << Link("Click here to see the best solution found by the genetic algorithm.",
	      "WhiteheadMinimal", 
	      arcer.getResultFileName());
  msg.send();
}
void FindWhiteheadMinimalProblem::takeControl( )
{
  
    if ( freeARCs() > 0 ) {
      
      if( !linkHasBeenSent ) {
	
	linkHasBeenSent = true;
	
	LogMessage msg( *this, theTuple );
	msg << Link("Click here to see details of the genetic algorithm.",
		    "WhiteheadMinimal", 
		    arcer.getComputationFileName(), true);
	msg.send();
	
      }
      
      if( arcer.takeControl() ) {
	
	adminTerminate();
	return;
      }
    else
      usedOneARC();
    }
}


void FindWhiteheadMinimalProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Find a Whitehead minimal"),
		  Text("Find a Whitehead minimal of ") + Name( theTuple )
		  +  + "?",
		  helpID("FindWhiteheadMinimalProblem", theTuple.getParent()),
		  "Wh min"
		  );

  pv.startItemGroup();

  // pv.add("For the use of the Whitehead algorithm", freeIsPartOfBasis.arcSlotID(), 100);
  pv.add(Text("For the use of the genetic algorithm"), ThisARCSlotID(), 100);

  pv.done();
}

// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes
//           ACConjectureARCer, ACConjectureProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//

#include "ACConjecture.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "conversions.h"


// -------------------------- ACConjectureARCer ---------------------- //


ACConjectureARCer::ACConjectureARCer( ComputationManager& boss )
  : ARCer( boss )
{ } 

void ACConjectureARCer::setArguments( FPGroup fp )
{
  theGroup  =  fp;
}

void ACConjectureARCer::runComputation( ) 
{
  /*
  ACConfig( int pop = 50, int gen = MAXINT, int fit_scale = 1, 
	    int cross_prob = 70, int mut_prob = 85, int elit = true, 
	    int penalty = 0, int cross_type = 1,
	    int mut_app_prob = 30, int mut_insert_prob = 20,
	    int mut_delete_prob = 20,
	    int mut_change_prob = 10,
	    int mut_permute_prob = 20,
	    int use_sum_fit = 1,
	    int num_min = 1000,
	    int us_w = 1
	    );
  */
  ACConfig c( 50, 10000000, 1, 
	      85, 95, true, 
	      0,1,
	      70, 5,
	      10,
	      15,
	      0,
	      1,
	      5000,
	      1
	      );


  ACGA acga(theGroup,c);
  result = acga.transform( compFile, resultFile );
  

}

void  ACConjectureARCer::writeResults( ostream& out )
{
  out < result;
}

void ACConjectureARCer::readResults( istream& in )
{
  in > result;
}

// -------------------- ACConjectureProblem --------------------------- //


ACConjectureProblem::ACConjectureProblem( const SMVectorOfWords& t)
    : Supervisor( true ), 
      theTuple( t ), 
      arcer ( *this )
{
  if (t.getWords().length() != t.getParent().getFreePreimage().numberOfGenerators() ){
    Message msg("Warning");
    msg << "Number of words in the tuple must be equal to the number of the generators!";
    msg.send();
    resultIsFast();
  }  else{
    SetOf<Word> s = makeSetOf(theTuple.getWords());
    FPGroup g(theTuple.getParent().getFreePreimage().namesOfGenerators(),
	      s);
    arcer.setArguments( g );
  }
}

void ACConjectureProblem::start( )
{
  
  LogMessage msg( *this, theTuple );
  msg << Link("Click here to obtain details of the genetic algorithm.",
	      "ACConjecture", arcer.getComputationFileName(), true);
  msg.send();
  
  LogMessage msg1( *this, theTuple );
  msg1 << Link("Click here to see the fittest solutions of the genetic algorithm.",
	       "ACConjecture", 
	       arcer.getResultFileName(), true);
  msg1.send();
  
}

void ACConjectureProblem::takeControl( )
{

  if ( freeARCs() > 0 ) {
    if ( arcer.takeControl() ) {
      
      /*     LogMessage msg( *this, theTuple );
      msg << Name( theTuple );
      if ( arcer.getResult()  )
	msg << " satisfies ";
      else
	msg << " does not satisfy ";
      msg << " the Andrews-Curtis conjecture.";
      msg.send();
      */
      adminTerminate();
      return;
    }
    usedOneARC();
  }
}


void ACConjectureProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Find Andrews-Curtis reduction"),
		  Text("Find Andrews-Curtis reduction of ") + 
		  Name( theTuple ),
		  helpID("ACConjectureProblem", theTuple.getParent()),
		  "AC"
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of the problem"), ThisARCSlotID(), 1000);

  pv.done();
}



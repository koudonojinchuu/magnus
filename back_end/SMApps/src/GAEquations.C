// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes 
//           GAEquationArcer, GAEquationCM, GAEquationProblem,
//           TwoCommArcer, TwoCommCM, TwoCommProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#include "SMWord.h"
#include "GAEquations.h"
#include "TwoCommSolver.h"
#include "SMFPGroup.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "ARCer.h"


// --------------------------- GAEquationArcer ----------------------------- //


void GAEquationArcer::setArguments( FreeGroup F, int n, Word w )
{
  theGroup = F;
  numOfVars = n;
  theWord = w;
}


void GAEquationArcer::runComputation( )
{
  GHNConfig c;
  GAEquationSolver solver(theGroup,numOfVars,c);
  solution = solver.getSolution(theWord,&file);
  retValue = true;
}


void GAEquationArcer::writeResults( ostream& out ) 
{
  out < retValue;
}


void GAEquationArcer::readResults( istream& in )
{
  in > retValue;
}


Map GAEquationArcer::getSolution( ) const
{ 
#if SAFETY > 0
  if ( retValue != yes )
    error( "Map GAEquationArcer::getSolution( ) const : "
	   "the solution has not been found yet.");
#endif

  Map s;
  return s;
  //return solution; 
}


// ----------------------------- GAEquationCM ------------------------------ //


GAEquationCM::GAEquationCM( class GAEquationProblem& PO )
  : theEquation( PO.getEquation() ),
    arcer( *this ),
    theAnswer( dontknow ),
    bStarted( false )
{
  adminStart();
  //filename = arcer.getFileName();
}


void GAEquationCM::start( )
{
  //arcer.setArguments(theEquation);
}


void GAEquationCM::takeControl( )
{
  if ( freeARCs() > 0 ) { 
    
    if( !bStarted ) {
      bStarted = true;
      arcer.setArguments(theEquation.getFreeGroup(), 
			 theEquation.numberOfVariables(),  
			 theEquation.getWord());
    }
    
    if( arcer.takeControl() ) {
      
      theAnswer = arcer.haveSolution();
      //solution = arcer.getSolution();
      //LogMessage msg( *this, theEquation );
      //msg << "Found solution: " << getSolution() << ".";
      //msg.send();
      adminTerminate();
    }
    else
      usedOneARC();
  }
}


Map GAEquationCM::getSolution( ) const
{ 
#if SAFETY > 0
  if ( theAnswer != yes )
    error( "Map GAEquationCM::getSolution( ) const : "
	   "the solution is not found yet.");
#endif

  return solution; 
}


// ------------------------- GAEquationProblem ----------------------------- //


GAEquationProblem::GAEquationProblem( SMEquation2& E )
  : Supervisor( true ),
    theEquation( E ),
    linkHasBeenSent( false ),
    GA( *this )
{ }


void GAEquationProblem::takeControl( )
{
  //if ( freeARCs() > 0 ) { 
 
    if( !linkHasBeenSent ) {
      
      linkHasBeenSent = true;
      
      LogMessage msg( *this, theEquation );
      msg << Link("Click here to obtain details of the computation.",
		  "GAEquationProblem", GA->getFileName(), true);
      msg.send();
    }
  
    //Trichotomy answer = GA->haveSolution();
    if ( GA->haveSolution() != dontknow ) {
      /*
      LogMessage msg( *this, theEquation );
      msg << "Found solution: " << GA->getSolution() << ".";
      msg.send();
      */
      adminTerminate();
      return;
    }
  
  
    if( GA->state() == ComputationManager::TERMINATED )
      adminTerminate();
    
    //}
}


void GAEquationProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Find solution"),
		  Text("Find a solution for")
		  + Name( theEquation ), 
		  helpID( "GAEquations", theEquation.getParent() ),
		  "Solution"
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of genetic algorithm"), GA.arcSlotID(), 100);

  pv.done();
}


// ----------------------------- TwoCommArcer ------------------------------ //


void TwoCommArcer::setArguments( FreeGroup F, Word w )
{
  theGroup = F;
  theWord = w;
}


void TwoCommArcer::runComputation( )
{
  TwoCommSolver solver(theGroup);
  Word x1,x2,y1,y2;
  solver.isProductOfTwoComms(theWord,x1,x2,y1,y2,&file);
  retValue = true;
}


void TwoCommArcer::writeResults( ostream& out ) 
{
  out < retValue;
}


void TwoCommArcer::readResults( istream& in )
{
  in > retValue;
}


// ------------------------------- TwoCommCM ------------------------------- //


TwoCommCM::TwoCommCM( class TwoCommProblem& PO )
  : theWord( PO.getWord() ),
    arcer( *this ),
    theAnswer( dontknow ),
    bStarted( false )
{
  adminStart();
}


void TwoCommCM::start( )
{ }


void TwoCommCM::takeControl( )
{
  if ( freeARCs() > 0 ) { 
    
    if( !bStarted ) {
      bStarted = true;
      arcer.setArguments( theWord.getParent().getFreePreimage(), 
			  theWord.getWord() );
    }
    
    if( arcer.takeControl() ) {
      theAnswer = arcer.haveSolution();
      adminTerminate();
    }
    else
      usedOneARC();
  }
}


// --------------------------- TwoCommProblem ------------------------------ //


TwoCommProblem::TwoCommProblem( SMWord& w )
  : Supervisor( true ),
    theWord( w ),
    linkHasBeenSent( false ),
    GA( *this )
{ }


void TwoCommProblem::takeControl( )
{
  //if ( freeARCs() > 0 ) { 
 
    if( !linkHasBeenSent ) {
      
      linkHasBeenSent = true;
      
      LogMessage msg( *this, theWord );
      msg << Link("Click here to obtain details of the computation.",
		  "TwoCommProblem", GA->getFileName(), true);
      msg.send();
    }
  
    if ( GA->haveSolution() != dontknow ) {
      adminTerminate();
      return;
    }
  
  
    if( GA->state() == ComputationManager::TERMINATED )
      adminTerminate();
    
    //}
}


void TwoCommProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Is product of two commutators ?"),
		  Text("Is") + Name( theWord ) 
		  + "product of two commutators ?", 
		  helpID( "TwoCommProblem", theWord.getParent() ),
		  "Two comms"
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of genetic algorithm"), GA.arcSlotID(), 100);

  pv.done();
}

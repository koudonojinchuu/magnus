// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of the EqSystemInAbelianARCe, EqSystemInAbelianCM,
//           EqSystemInFPProblem, EquationInAbelianCM, EquationInFPProblem 
//           classes.
//
// Principal Authors: Denis Serbin
//
// Status: In progress
//
// Revision History:
//
// Special Notes:
//
//

#include "EquationsInFPProblem.h"
#include "OutMessages.h"
#include "SMFPGroup.h"

//--------------------------EqSystemInAbelianARCer---------------------------//

void EqSystemInAbelianARCer::setArguments( const AbelianEquationsSolver& a )
{
  AES = a;
}
  
void EqSystemInAbelianARCer::runComputation( )
{
  AES.findSolutions(file);
  
  retValue = AES.haveSolutions();
}
 
void EqSystemInAbelianARCer::writeResults( ostream& out )
{
  out < retValue;
}

void EqSystemInAbelianARCer::readResults( istream& in )
{
  in > retValue;
}

//---------------------------EqSystemInAbelianCM-----------------------------//

EqSystemInAbelianCM::EqSystemInAbelianCM( class EqSystemInFPProblem& p )
  : theSystem( p.getSystem() ),
    answer( 0 ),
    arcer( *this )
{
  int numOfVar = theSystem.numberOfVariables();
  
  SetOf<Word> S = theSystem.getParent().getFPGroup().getRelators();
  SetIterator<Word> I(S);
  SetOf<Word> newS;
  
  while( !I.done() )
    {
      Word w = I.value();
      for( int j = 0 ; j < w.length() ; j++ )
	{
	  int p = Generator( w[j] ).hash();
	  if( p > 0 )
	    w[j] = Generator( p + numOfVar );
	  else
		w[j] = Generator( p - numOfVar );
	}
      
      newS.adjoinElement( w );
	  I.next();
    }
  
  FPGroup G( theSystem.getFreeGroup().namesOfGenerators() , newS );
  
  AbelianGroup A( G );
  
  VectorOf<Word> v = theSystem.getSystem();
  
  arcer.setArguments( AbelianEquationsSolver( A , v , numOfVar ) ); 

  adminStart();
}

void EqSystemInAbelianCM::takeControl( )
{
  if(freeARCs() > 0)
    if(arcer.takeControl())
      {
	theSystem.seic.putIsSolvedInAbelianization();
	theSystem.seic.putHaveSolutionsInAbelianization( arcer.getRetValue() );
	
	if( theSystem.getParent().getCheckinType( ) == SMFPGroup::ABELIAN )
	  {
	    theSystem.seic.putIsSolved();
	    theSystem.seic.putHaveSolutions( arcer.getRetValue() );
	  }
      }
    else
      usedOneARC();
}

//-------------------------EqSystemInFPProblem--------------------------//

EqSystemInFPProblem::EqSystemInFPProblem( SMEqSystem& s )
  : Supervisor( true ), 
    theSystem( s ),
    abelianCM( *this )
{
  if( theSystem.getParent().getCheckinType( ) == SMFPGroup::ABELIAN && 
      theSystem.seic.isSolvedInAbelianization() )
    resultIsFast();
}

void EqSystemInFPProblem::takeControl( )
{
  if( theSystem.seic.isSolvedInAbelianization() )
    {
      LogMessage msg1( *this, theSystem );
      
      if( theSystem.seic.haveSolutionsInAbelianization() )
	{
	  if( theSystem.getParent().getCheckinType( ) == SMFPGroup::ABELIAN )
	    {
	      msg1 << Name( theSystem );
	      msg1 << " has solutions.";
	      msg1.send();
	      
	      LogMessage msg2( *this, theSystem );
	      
	      msg2 << Link("Click here to see the solutions of the system" , "AbEqSysSol" , abelianCM->getFileName() );
	      msg2.send();
	    }
	  else
	    {
	      msg1 << "The algorithm can't give an exact answer.";
	      msg1.send();
	    }
	  
	  adminTerminate();
	  return;
	}
      else
	{
	  if( theSystem.getParent().getCheckinType( ) == SMFPGroup::ABELIAN )
	    {
	      msg1 << Name( theSystem );
	      msg1 << " has no solutions.";
	      msg1.send();
	      
	      LogMessage msg3( *this, theSystem );
	      
	      msg3 << Link("Click here to see the details" , "AbEqSysSol" , abelianCM->getFileName() );
	      msg3.send();
	    }
	  else
	    {
	      msg1 << Name( theSystem );
	      msg1 << " has no solutions because it cannot be solved in abelianization of ";
	      msg1 << Name( theSystem.getParent() ) << " .";
	      msg1.send();
	    }
	  
	  adminTerminate();
	  return;
	}
    }
}

void EqSystemInFPProblem::viewStructure( ostream& ostr ) const
{
  ProblemView pv( ostr, oid(), Text("Finding solutions for the system of equations"),
		  Text("Finding solutions of") + 
		  Name( theSystem ) ,
		  helpID("EquationsInAbelianProblem",theSystem.getParent()),
		  Text("solve") + Name(theSystem)
		  );
  
  pv.startItemGroup();
  
  if( theSystem.getParent().getCheckinType( ) == SMFPGroup::ABELIAN )
    pv.add(Text("For the use of the problem") , abelianCM.arcSlotID() , 50 );
  else
    pv.add(Text("Find solutions in abelianization") , abelianCM.arcSlotID() , 50 );
  
  pv.done();
}

//---------------------------EquationInAbelianCM-----------------------------//

EquationInAbelianCM::EquationInAbelianCM( class EquationInFPProblem& p )
  : theEquation( p.getEquation() ),
    answer( 0 ),
    arcer( *this )
{
  int numOfVar = theEquation.numberOfVariables();
  
  SetOf<Word> S = theEquation.getParent().getFPGroup().getRelators();
  SetIterator<Word> I(S);
  SetOf<Word> newS;
  
  while( !I.done() )
    {
      Word w = I.value();
      for( int j = 0 ; j < w.length() ; j++ )
	{
	  int p = Generator( w[j] ).hash();
	  if( p > 0 )
	    w[j] = Generator( p + numOfVar );
	  else
	    w[j] = Generator( p - numOfVar );
	}
      
      newS.adjoinElement( w );
      I.next();
    }
  
  FPGroup G( theEquation.getFreeGroup().namesOfGenerators() , newS );
  
  AbelianGroup A( G );
  
  VectorOf<Word> v;
  v.append( theEquation.getWord() );
  
  arcer.setArguments( AbelianEquationsSolver( A , v , numOfVar ) ); 

  adminStart();
}

void EquationInAbelianCM::takeControl( )
{
  if(freeARCs() > 0)
    if(arcer.takeControl())
      {
	theEquation.eic.putIsSolvedInAbelianization();
	theEquation.eic.putHaveSolutionsInAbelianization( arcer.getRetValue() );
      }
    else
      usedOneARC();
}

//---------------------------EquationInFPProblem-----------------------------//

EquationInFPProblem::EquationInFPProblem( SMEquation2& e )
  : Supervisor( true ), 
    theEquation( e ),
    abelianCM( *this )
{
  if( theEquation.getParent().getCheckinType( ) == SMFPGroup::ABELIAN && 
      theEquation.eic.isSolvedInAbelianization() )
    resultIsFast();
}

void EquationInFPProblem::takeControl( )
{
  if( theEquation.eic.isSolvedInAbelianization() )
    {
      LogMessage msg1( *this, theEquation );
      
      if( theEquation.eic.haveSolutionsInAbelianization() )
	{
	  if( theEquation.getParent().getCheckinType( ) == SMFPGroup::ABELIAN )
	    {
	      msg1 << Name( theEquation );
	      msg1 << " has solutions.";
	      msg1.send();
	      
	      LogMessage msg2( *this, theEquation );
	      
	      msg2 << Link("Click here to see solutions of the equation" , "AbEqSol" , abelianCM->getFileName() );
	      msg2.send();
	    }
	  else
	    {
	      msg1 << "The algorithm can't give an exact answer.";
	      msg1.send();
	    }
	  
	  adminTerminate();
	  return;
	}
      else
	{
	  if( theEquation.getParent().getCheckinType( ) == SMFPGroup::ABELIAN )
	    {
	      msg1 << Name( theEquation );
	      msg1 << " has no solutions.";
	      msg1.send();
	      
	      LogMessage msg3( *this, theEquation );
	      
	      msg3 << Link("Click here to see the details" , "AbEqSol" , abelianCM->getFileName() );
	      msg3.send();
	    }
	  else
	    {
	      msg1 << Name( theEquation );
	      msg1 << " has no solutions because it cannot be solved in abelianization of ";
	      msg1 << Name( theEquation.getParent() ) << " .";
	      msg1.send();
	    }
	  
	  adminTerminate();
	  return;
	}
    }
}

void EquationInFPProblem::viewStructure( ostream& ostr ) const
{
  ProblemView pv( ostr, oid(), Text("Finding solutions for the equation"),
		  Text("Finding solutions of") + 
		  Name( theEquation ) ,
		  helpID("EquationInAbelianProblem",theEquation.getParent()),
		  Text("solve") + Name( theEquation )
		  );
  
  pv.startItemGroup();
  
  if( theEquation.getParent().getCheckinType( ) == SMFPGroup::ABELIAN )
    pv.add(Text("For the use of the problem") , abelianCM.arcSlotID() , 50 );
  else
    pv.add(Text("Find solutions in abelianization") , abelianCM.arcSlotID() , 50 );
  
  pv.done();
}

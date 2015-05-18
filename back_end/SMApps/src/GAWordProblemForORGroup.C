// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes GAWordForORGroupProblem, GAWordForORGroupARCer, GAWordForORGroup
//
// Principal Author: Alexander Ushakov
//
// Status: in progress
//
// Revision History:
//

#include "Set.h"
#include "GAWordProblemForORGroup.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "SMSubgroup.h"
#include "WordProblem.h"



//---------------------------------------------------------------------------//
//------------------------- GAWordForORGroupARCer ---------------------------//
//---------------------------------------------------------------------------//


GAWordForORGroupARCer::GAWordForORGroupARCer( GAWordForORGroup& boss ) : 
  ARCer( boss ), 
  theAnswer( dontknow ),
  theGroup( boss.getGroup( ) ),
  solver( 0 )
{

}

GAWordForORGroupARCer::~GAWordForORGroupARCer( )
{
  delete solver;
}

void GAWordForORGroupARCer::setArguments( const OneRelatorGroup& G, const Word& u )
{
  theGroup = G;
  theWord = u;
  solver = new GAConjProblemForORGroupSolver( theGroup , Word( ) , theWord, true , false );
}

void GAWordForORGroupARCer::runComputation( )
{
  if( solver->isConj( ) )
    theAnswer = yes;
  else
    theAnswer = no;
}

void GAWordForORGroupARCer::writeResults( ostream& ostr )
{
  ostr < theAnswer;
}

void GAWordForORGroupARCer::readResults( istream& istr )
{
  istr > theAnswer;
}


//---------------------------------------------------------------------------//
//----------------------- GAWordForORGroup -----------------------------//
//---------------------------------------------------------------------------//


GAWordForORGroup::GAWordForORGroup(WordProblem& CP)
  : ComputationManager( ), 
  arcer( *this ), 
  problem( CP ), 
  theAnswer( dontknow )
{
  arcer.setArguments( getGroup( ) , problem.getTestWord().getWord() );
  adminStart();
}


OneRelatorGroup GAWordForORGroup::getGroup( ) const 
{
  FPGroup g1 = problem.getTestWord( ).getParent().getFPGroup( );
  int gens = g1.numberOfGenerators( );
  SetOf< Word > rels = g1.getRelators( );
  SetIterator< Word > I( rels );

  return OneRelatorGroup( gens , I.value( ) );
}


void GAWordForORGroup::takeControl( )
{
  if ( freeARCs() > 0 ) {

    if( arcer.takeControl() ) {

      theAnswer = arcer.answer();
      adminTerminate();
      return;
    }

    usedOneARC();
  }

}






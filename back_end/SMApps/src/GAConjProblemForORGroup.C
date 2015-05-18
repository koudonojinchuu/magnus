// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes GAConjugacyForORGroupProblem, GAConjugacyForORGroupARCer, GAConjugacyForORGroup
//
// Principal Author: Alexander Ushakov
//
// Status: in progress
//
// Revision History:
//

#include "Set.h"
#include "GAConjProblemForORGroup.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "SMSubgroup.h"
#include "ConjugacyProblem.h"


//---------------------------------------------------------------------------//
//--------------------- GAConjugacyForORGroupARCer --------------------------//
//---------------------------------------------------------------------------//


GAConjugacyForORGroupARCer::GAConjugacyForORGroupARCer( GAConjugacyForORGroup& boss ) : 
  ARCer( boss ), 
  theAnswer( dontknow ),
  theGroup( boss.getGroup( ) ),
  solver( 0 )
{

}

GAConjugacyForORGroupARCer::~GAConjugacyForORGroupARCer( )
{
  delete solver;
}

void GAConjugacyForORGroupARCer::setArguments( const OneRelatorGroup& G, const Word& u, const Word& v )
{
  theGroup = G;
  firstWord = u;
  secondWord = v;
  solver = new GAConjProblemForORGroupSolver( theGroup , firstWord , secondWord );
}

void GAConjugacyForORGroupARCer::runComputation( )
{
  if( solver->isConj( ) )
    theAnswer = yes;
  else
    theAnswer = no;
}

void GAConjugacyForORGroupARCer::writeResults( ostream& ostr )
{
  ostr < theAnswer;
}

void GAConjugacyForORGroupARCer::readResults( istream& istr )
{
  istr > theAnswer;
}


//---------------------------------------------------------------------------//
//----------------------- GAConjugacyForORGroup -----------------------------//
//---------------------------------------------------------------------------//


GAConjugacyForORGroup::GAConjugacyForORGroup( const ConjugacyProblem& CP)
  : ComputationManager( ), 
  arcer( *this ), 
  problem( CP ), 
  theAnswer( dontknow )
{
  arcer.setArguments( CP.firstWord().getParent().getFPGroup(),
		      CP.firstWord().getWord(), 
		      CP.secondWord().getWord() 
		      );

  adminStart();
}


OneRelatorGroup GAConjugacyForORGroup::getGroup( ) const 
{
  const SMWord& u = problem.firstWord( );

  int gens = u.getParent( ).getFPGroup( ).numberOfGenerators( );
  SetOf<Word> set = u.getParent( ).getFPGroup( ).getRelators( );

  SetIterator<Word> iterator( set );
  Word relator = iterator.value( );
  return OneRelatorGroup( gens , relator );
}


void GAConjugacyForORGroup::takeControl( )
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



// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class ConjugacyProblem
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//
//
// 11/19/95 db fixed:
//
// MSCConjugacyWrapper::MSCConjugacyWrapper(ConjugacyProblem& ),
//
// void MSCConjugacyWrapper::takeControl( ),
//
// FPConjugacyWrapper::FPConjugacyWrapper(ConjugacyProblem& CP),
//
// void FPConjugacyWrapper::takeControl( ).
//
// Now they correspond to the new interface of MSCGConjugacyProblem and
// GraphConjugacyProblem classes.
//
// @db & @au: 00/10/07 Added GA for solving conjugacy problem for OR groups  
//


#include "ConjugacyProblem.h"
#include "SMWord.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "GraphConjugacyProblem.h"
#include "MSCGConjugacyProblem.h"


//---------------------------------------------------------------------------//
//------------------------- ConjugacyProblem --------------------------------//
//---------------------------------------------------------------------------//


ConjugacyProblem::ConjugacyProblem(const SMWord& w1, const SMWord& w2)
  : Supervisor( true ),
    u( w1 ),
    v( w2 ),
    scMethodFailed (false),
    fpConjugacyWrapper( *this ),
    mscConjugacyWrapper( *this ),
    GAConjugacy( *this ),
    geneticLinkHasBeenSent( false )
{ }


void ConjugacyProblem::takeControl( )
{
  if( fpConjugacyWrapper.exists() &&
      fpConjugacyWrapper->answer() != dontknow ) {
    adminTerminate();
  }

  if( !scMethodFailed ) {

    if( mscConjugacyWrapper.exists() 
	&& mscConjugacyWrapper->answer() != dontknow ) {

      if( !u.getParent().gic.haveMSC() 
	  && mscConjugacyWrapper->answer() == no ) {
	LogMessage msg( *this, u, v );
	msg <<
	  "The small cancellation method has failed; it says `no', "
	  "and the presentation of " << Name( u.getParent() ) <<
	  " is not known to the system to be small cancellation.";
	msg.send();
	scMethodFailed = true;
      }
      else 
	adminTerminate();
    }

  }

  if( GAConjugacy.exists() ) {

    if( !geneticLinkHasBeenSent ) {
      geneticLinkHasBeenSent = true;
      
      LogMessage msg( *this , u ,v );
      msg << Link("Click here to obtain details of the computation.",
		  "GACPforORG", GAConjugacy->getFileName(), true);
      msg.send();
    }
  
    if( GAConjugacy->answer() != dontknow ) {
      
      LogMessage msg( *this, u, v );
      
      if( GAConjugacy->answer() == no ) 
	msg << "Words " << Name(u) << " and " << Name(v) << " are not conjugate.";
      else 
	msg << "Words " << Name(u) << " and " << Name(v) << " are conjugate.";
      
      msg.send();
      adminTerminate();
    }
  }

}


void ConjugacyProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Conjugacy problem"),
		  Text("Are") + Name( u ) + "and" + Name( v )
		  + "conjugate in" + Name( Parent( u ) ) + "?",
		  helpID("ConjugacyProblem", u.getParent()),
		  "Conjugate ?"
		  );
  
  pv.startItemGroup();

  pv.add(Text("Try graph method"), fpConjugacyWrapper.arcSlotID(), 100);

  pv.add(Text("Try small cancellation method"), mscConjugacyWrapper.arcSlotID(), 60);

  pv.startItemGroup( CheckinType( Parent( u ) ) ==
		     SMFPGroup::ONE_RELATOR || 
		     CheckinType( Parent( u ) ) ==
		     SMFPGroup::ONE_RELATOR_WITH_TORSION
		     );

  pv.add(Text("Try genetic algorithm"), GAConjugacy.arcSlotID(), 60);

  pv.done();
}



//---------------------------------------------------------------------------//
//------------------------ FPConjugacyWrapper -------------------------------//
//---------------------------------------------------------------------------//

FPConjugacyARCer::FPConjugacyARCer( ComputationManager& boss )
  : ARCer( boss ), theGroup( 0 ), theAnswer( dontknow )
{  
}

void FPConjugacyARCer::setArguments( const class SMFPGroup& G,
				     const Word& u, const Word& w )
{ 
  theGroup = &G;
  firstWord = u;
  secondWord = w;
}

void FPConjugacyARCer::runComputation( )
{
  FPGroup G = theGroup->getFPGroup();
  GraphConjugacyProblem theGCP( G, firstWord, secondWord );
  SubgroupGraph normalClosure( theGroup->gcm().normalClosure->getSubgroupGraph() );
  Generator g(1);
  int numberOfGenerators = G.numberOfGenerators();

  theGCP.startComputation();

  while( !theGCP.done() ) {

    if( theGCP.theNewGraphIsNeeded() ) {

      if( normalClosure.isComplete() ) {
	// @dp
	theAnswer = no;
	return;
      }
      else { 
	// normalClosure.continueComputation();
	normalClosure.joinConjugate(g);
	int o = ord(g);
	if ( o > 0 )
	  o = -o;
	else
	  o = 1 - o;
	if ( o > numberOfGenerators ) o = 1;
	g = Generator(o);
	
	cerr << "The trivial word word acceptor has "
	     << normalClosure.vertexCount() << " vertices.";
	
	theGCP.continueComputation( normalClosure );
      }

    } 
    else {
      theGCP.continueComputation();
    }

  }
  
  // GraphConjugacyProblem finished with positive answer: 
  // the elements are conjugated.
  theAnswer = yes;
  theConjugator = theGCP.getConjugator();
}

void FPConjugacyARCer::writeResults( ostream& ostr )
{
  ostr < theAnswer;
  if( theAnswer == yes ) 
    ostr < theConjugator;
}

void FPConjugacyARCer::readResults( istream& istr )
{
  istr > theAnswer;
  if( theAnswer == yes )
    istr > theConjugator;
}


FPConjugacyWrapper::FPConjugacyWrapper(ConjugacyProblem& CP)
  : ComputationManager(false), 
    arcer( *this ), problem( CP ), theAnswer( dontknow )
{
  arcer.setArguments( CP.firstWord().getParent(),
		      CP.firstWord().getWord(),
		      CP.secondWord().getWord()
		      );
  adminStart();
}

void FPConjugacyWrapper::takeControl( )
{
  if ( freeARCs() > 0 ) {

    if( arcer.takeControl() ) {

      const SMWord& u = problem.firstWord();
      const SMWord& v = problem.secondWord();
      LogMessage msg( problem, u, v );
      msg << Name( u ) << " and " << Name( v ) << " are conjugate: " 
	  << "X " << Name( u ) << " x = " << Name( v ) << ", where x = ";
      u.getParent().getFPGroup().printWord( msg, arcer.conjugator() );
      msg << ".";
      msg.send();
      
      theAnswer = yes;
      adminTerminate();
      return;
    }
    usedOneARC();
  }
}

//---------------------------------------------------------------------------//
//------------------------ MSCConjugacyWrapper ------------------------------//
//---------------------------------------------------------------------------//

MSCConjugacyARCer::MSCConjugacyARCer( ComputationManager& boss )
  : ARCer( boss ), theAnswer( dontknow )
{
}

void MSCConjugacyARCer::setArguments( const FPGroup& G, const Word& u, const Word& v )
{
  theGroup = G;
  firstWord = u;
  secondWord = v;
}

void MSCConjugacyARCer::runComputation( )
{
  MSCGroup G( theGroup );
  MSCGConjugacyProblem theMSCCP( G, firstWord, secondWord );

  theMSCCP.startComputation( );
  while( !theMSCCP.continueComputation() ) {
    // do nothing: everything is done by `continueComputation'
  }

  theAnswer = theMSCCP.answer();
  if( theAnswer == yes ) 
    theConjugator = theMSCCP.conjugator();
}

void MSCConjugacyARCer::writeResults( ostream& ostr )
{
  ostr < theAnswer;
  if( theAnswer == yes ) 
    ostr < theConjugator;
}

void MSCConjugacyARCer::readResults( istream& istr )
{
  istr > theAnswer;
  if( theAnswer == yes )
    istr > theConjugator;
}


MSCConjugacyWrapper::MSCConjugacyWrapper(ConjugacyProblem& CP)
  : ComputationManager( ), 
    arcer( *this ), problem( CP ), theAnswer( dontknow )
{
  arcer.setArguments( CP.firstWord().getParent().getFPGroup(),
		      CP.firstWord().getWord(), 
		      CP.secondWord().getWord() 
		      );

  adminStart();
}

void MSCConjugacyWrapper::takeControl( )
{
  if ( freeARCs() > 0 ) {

    if( arcer.takeControl() ) {

      theAnswer = arcer.answer();
      const SMWord& u = problem.firstWord();
      const SMWord& v = problem.secondWord();

      if ( theAnswer == yes ) {
	LogMessage msg( problem, u, v );
	msg << Name( u ) << " and " << Name( v ) << " are conjugate: " 
	    << "X " << Name( u ) << " x = " << Name( v ) << ", where x = ";
	u.getParent().getFPGroup().printWord( msg, arcer.conjugator() );
	msg << ".";
	msg.send();
      } 
      else if ( u.getParent().gic.haveMSC() ) {
	LogMessage msg( problem, u, v );
	msg << Name( u ) << " and " << Name( v ) << " are not conjugate.";
	msg.send();
      } 

      adminTerminate();
    }

    usedOneARC();
  }

}



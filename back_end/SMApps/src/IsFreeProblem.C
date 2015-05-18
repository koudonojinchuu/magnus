// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class IsFreeProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#include "IsFreeProblem.h"
#include "OutMessages.h"
#include "viewStructure.h"


// ----------------------- ORIsFreeProblemARCer ---------------------------- //


void ORIsFreeProblemARCer::runComputation( )
{
  retValue = theORProblems->isFree();
}


void ORIsFreeProblemARCer::setArguments(ORProblems* anORProblems )
{
  theORProblems = anORProblems;
}


void ORIsFreeProblemARCer::writeResults( ostream& out ) 
{
  out < retValue;
}

void ORIsFreeProblemARCer::readResults( istream& in )
{
  in > retValue;
}


// ------------------------- ORIsFreeProblem ------------------------------- //


ORIsFreeProblem::ORIsFreeProblem( class SMFPGroup& G )
  : ComputationManager( true ),
    theGroup( G ), theORProblems( NULL ), arcer( *this )
{ 
  if( G.gic.isOneRelatorWithTorsion() )
    resultIsFast();
}


ORIsFreeProblem::~ORIsFreeProblem( )
{
  delete theORProblems;
  theORProblems = NULL;
}


void ORIsFreeProblem::takeControl( )
{
  if( fastResult() ) {
    
    LogMessage msg( *this, theGroup );
    msg << " One relator group with torsion is never free.";
    msg.send();

    adminTerminate();
    return;
  }

  if ( freeARCs() > 0 ) {
    if ( theORProblems == NULL ) {
      theORProblems =
	new ORProblems( theGroup.getFPGroup().numberOfGenerators(),
			theGroup.gic.getOneRelator() );
      arcer.setArguments(theORProblems);
    }
    else 
      if( arcer.takeControl() ) {
	
        bool answer = arcer.getRetValue();
	LogMessage msg( *this, theGroup );
	
	msg << Name( theGroup ) << " is ";
	if( !answer )
	  msg << "not ";
	msg << "free.";
	msg.send();
 	
	delete theORProblems;
	theORProblems = NULL;
	
	adminTerminate();
	return;
      }
      else
	usedOneARC();
  }
}


void ORIsFreeProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Freeness problem"),
		  Text("Is ") + Name( theGroup ) + Text(" free ?"),
		  helpID("ORIsFreeProblem", theGroup),
		  Text(Name(theGroup)) + "free ?"
		  );
  
  pv.startItemGroup();

  pv.add( Text("For the use of this problem"),
	  ThisARCSlotID(),
	  50
	  );
  
  pv.done();
}


// --------------------------- IsFreeChecker ------------------------------- //


IsFreeChecker::IsFreeChecker( class SMFPGroup& group)
  : theGroup( group ),
    G( group.getFPGroup() ),
    gic( group.gic ),
    gcm( group.gcm() ),
    triedAbelianization( false )
{ }


Trichotomy IsFreeChecker::isFree( )
{
  if( gic.isFree() != dontknow )
    return gic.isFree();
  
  int numberOfGenerators = G.numberOfGenerators();
  
  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Use the abelian quotient                                          //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  if ( ! triedAbelianization && gic.haveCyclicDecomposition( ) ) {
	 
    triedAbelianization = true;
    
    if ( gic.isAbelian() == yes ) {
      explanation =
	FEData::Text("this follows from the canonical decomposition of")
	+ FEData::Name(theGroup);
	
      return gic.getCyclicDecomposition().isFree();
    }
    else
      if ( gic.isNilpotent() == yes ) {
	explanation = Chars("this follows from an inspection of the abelianization of the group");
	return gic.getCyclicDecomposition().isFree();
      }
      else
	if ( !gic.getCyclicDecomposition().isFree() ) {
	  explanation = Chars("this follows from an inspection of the abelianization of the group");
	  return no;
	}
  }
  
  return dontknow;
}


// -------------------------- IsFreeProblem -------------------------------- //


IsFreeProblem::IsFreeProblem(class SMFPGroup& G)
  : Supervisor( G.gic.isFree() == dontknow ),
    theGroup( G ),
    theChecker( G ),
    abelianInvariants( *this, G.gcm().abelianInvariants )
{
  if( theGroup.gic.isAbelian() == yes || theGroup.gic.isNilpotent() == yes )
    if ( theGroup.gic.haveCyclicDecomposition() )
      resultIsFast();
}


void IsFreeProblem::takeControl( )
{
  Trichotomy answer = dontknow;
  if ( ( answer = theChecker.isFree() ) != dontknow ) {
    Chars explanation = theChecker.getExplanation();
    LogMessage msg( *this, theGroup );
    msg << Name( theGroup ) << " is ";
    if ( answer == no ) msg << "not ";

    Chars text;
    if( theGroup.gic.isAbelian() == yes )
      text = "free abelian";
    else
      if( theGroup.gic.isNilpotent() == yes )
	text = "free nilpotent";
      else
	text = "free";
    msg << text;
    
    if( explanation.length() > 0 )
      msg << ": " << explanation;
    
    msg << ".";
    msg.send();
    adminTerminate();
  }
}


void IsFreeProblem::start( ) { }


void IsFreeProblem::terminate( ) { }


void IsFreeProblem::viewStructure(ostream& ostr) const
{
  Chars text = Text("Is") + Name( theGroup );
  
  if( theGroup.gic.isAbelian() == yes )
    text = text + " free abelian ?";
  else
    if( theGroup.gic.isNilpotent() == yes )
      text = text + " free nilpotent ?";
    else
      text = text + " free ?";
  
  
  ProblemView pv( ostr, oid(), Text("Freeness problem"), Text(text),
		  helpID("IsFreeProblem", theGroup),
		  Text(Name(theGroup)) + "free ?");

  
  pv.startItemGroup();

  pv.startItemGroup( CheckinType( theGroup ) == SMFPGroup::ABELIAN
		     );
  
  pv.add( Text("Compute abelian invariants of") + Name( theGroup ),
	  abelianInvariants.arcSlotID(),
	  15
	  );


  pv.startItemGroup( CheckinType( theGroup ) != SMFPGroup::ABELIAN
		     );
  
  pv.add( Text("Is abelianization of") + Name( theGroup )
	  + Text("free ?"),
	  abelianInvariants.arcSlotID(),
	  15
	  );

  
  pv.done();
}

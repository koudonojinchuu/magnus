// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class OrderProblem
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#include "GroupFastChecks.h"
#include "IsFiniteProblem.h"
#include "OutMessages.h"
#include "viewStructure.h"


// ------------------------- IsFiniteProblem ------------------------------- //
 

IsFiniteProblem::IsFiniteProblem(SMFPGroup& G)
  : Supervisor( true ),
    theGroup( G ),
    abelianRank( *this, theGroup.gcm().abelianRank ),
    kbSupervisor( *this, theGroup.gcm().kbSupervisor ),
    agSupervisor( *this, theGroup.gcm().agSupervisor ),
    theToddCoxeter( *this, theGroup.gcm().theToddCoxeter),
    ghToddCoxeter( *this, theGroup.gcm().ghToddCoxeter)
  
{
  if( theGroup.gic.isFinite() != dontknow
      || theGroup.gic.haveCyclicDecomposition() ) {
    resultIsFast();
    LogMessage(theGroup.gic.getFiniteMessage()).send();
    return;
  }

  ///////////////////////////////////////////////////////////////////////
  //                                                                   //
  // Fast checks                                                       //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  FPGroup FP = theGroup.getFPGroup();
  GroupFastChecks checker(FP);

  if( checker.numOfRelsLessNumOfGens() ) {
    theGroup.gic.putIsFinite(false, "the number of relators is less than"
			     " the number of generators");
    resultIsFast();
    LogMessage(*this, theGroup.gic.getFiniteMessage(), 2).send();
    return;
  }


  Generator g;
  if( checker.existsGenWithExpSumZeroInEveryRelator(g) ) {
    
    //@njz
    //    ostrstream msg;
    std::ostrstream msg;
    //
    msg << "the exponent sum of generator ";
    FP.printWord(msg, Word(g));
    msg << " is equal to 0 in every relator of the group" << ends;

    Chars explanation = msg.str();
    theGroup.gic.putIsFinite( false, explanation);
    resultIsFast();
    LogMessage(*this, theGroup.gic.getFiniteMessage(), 2).send();
    return;
  }
  

  // @am Why ??
  /*
  if ( theGroup.gic.haveOrder() ) 
    {
      ostrstream msg;
      msg << "according to Todd-Coxeter algorithm";
      Chars explanation = msg.str();
       
      theGroup.gic.putIsFinite( theGroup.gic.getOrder() != 0, explanation);
      resultIsFast();
      return;
    }
    */

}


void IsFiniteProblem::takeControl( )
{
  if ( theGroup.gic.isFinite() != dontknow ) {

    if( !fastResult() ) 
      LogMessage(*this, theGroup.gic.getFiniteMessage(), 2).send();

    adminTerminate();
  }
  else
    {
      if(theGroup.gic.haveCyclicDecomposition() ) {
	
	AbelianGroup& A = (AbelianGroup&)
	  (theGroup.gic.getCyclicDecomposition());

	Chars explanation 
	  = Text("this follows from the canonical decomposition of")
	  + Name(theGroup);

	if ( theGroup.gic.isAbelian() != yes ) {
	  explanation += " abelianized";
	  if( A.order() == 0 )
	    theGroup.gic.putIsFinite(false, explanation);
	}
	else 
	  theGroup.gic.putIsFinite(A.order() != 0, explanation);

	LogMessage(*this, theGroup.gic.getFiniteMessage(), 2).send();
      }
      
      if ( theGroup.gic.haveOrder() ) 
	{
	  //@njz
	  //	  ostrstream msg;
	  std::ostrstream msg;
	  //
	  msg << "according to the Todd-Coxeter algorithm";
	  
	  Chars explanation = msg.str();
	  theGroup.gic.putIsFinite( theGroup.gic.getOrder() != 0, explanation);
	  
	  LogMessage(*this, theGroup.gic.getFiniteMessage(), 2).send();
	  adminTerminate();
	}
    }

  
}


void IsFiniteProblem::start( ) { }


void IsFiniteProblem::terminate( ) { }


void IsFiniteProblem::viewStructure(ostream& ostr) const
{
  Chars abelianText;
  if( theGroup.gic.isAbelian() == yes )
    abelianText = Text("Compute the torsion-free rank of") + Name( theGroup );
  else
    abelianText =
      Text("Compute the torsion-free rank of the abelianization of")
      + Name( theGroup );
  

  ProblemView pv( ostr, oid(), Text("Finiteness problem"),
		  Text("Is") + Name( theGroup ) + Text("finite ?"),
		  helpID("IsFiniteProblem", theGroup),
		  Text(Name(theGroup)) + "finite ?"
		  );

    
  pv.startItemGroup();
  
  pv.add( Text(abelianText),
	  abelianRank.arcSlotID(),
	  15
	  );

  
  pv.startItemGroup( CheckinType( theGroup ) == SMFPGroup::FP
		     );
  
  pv.add( Text("Enumerate cosets")
	  + Name( theGroup ),
	  theToddCoxeter.arcSlotID(),
	  100
	  ); 
  
  pv.add( Text("GH Todd-Coxeter")
	  + Name( theGroup ),
	  ghToddCoxeter.arcSlotID(),
	  100
	  ); 

  pv.add( Text("Seek a rewriting system for") + Name( theGroup ),
	  kbSupervisor.arcSlotID(),
	  50
	  );
  
  pv.add( Text("Seek a ShortLex automatic structure for") + Name( theGroup ),
	  agSupervisor.arcSlotID(),
	  50
	  );

  
  pv.done();
}


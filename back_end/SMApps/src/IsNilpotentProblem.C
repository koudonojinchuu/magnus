// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class IsNilpotentProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#include "IsNilpotentProblem.h"
#include "SMFPGroup.h"
#include "OutMessages.h"
#include "viewStructure.h"


// -------------------------- IsNilpotentProblem --------------------------- //


IsNilpotentProblem::IsNilpotentProblem(SMFPGroup& G, int nilpClass)
  : Supervisor( true/*! G.gic.haveFastWordProblem()*/ ),
    theGroup( G ),
    theClass(nilpClass),
    theChecker( G, nilpClass+1 ),
    theArcer( *this ),
    normalClosure( *this, G.gcm().normalClosure ),
    //    abelianInvariants( *this, G.gcm().abelianInvariants ),
    kbSupervisor( *this, G.gcm().kbSupervisor ),
    agSupervisor( *this, G.gcm().agSupervisor ),
    nilpotentQuotients(*this, G.gcm().nilpotentQuotients),
    nilpotentWPInQuotients(*this)
{
  nilpotentWPInQuotients->initialize(theClass+1,&theGroup,theClass+1);
  if ( ! displayInFE() ) adminStart();
  theArcer.setArguments(&theChecker);
}


void IsNilpotentProblem::takeControl( )
{
  Trichotomy theAnswer = dontknow;
  Chars explanation;

  if ( ! displayInFE() || freeARCs() > 0 ) {
    if (nilpotentWPInQuotients->state() == ComputationManager::TERMINATED)
      if (nilpotentWPInQuotients->isTrivial()==no){
	int retClass;
	theAnswer = nilpotentWPInQuotients->isTrivial(retClass);
	//@njz
	//	ostrstream msgTmp;
	std::ostrstream msgTmp;
	//
	msgTmp << ": because there are not trivial commutators in lower central quotient of class " 
	       << retClass << ends;
	explanation = msgTmp.str();
      }    
    if (theAnswer != dontknow){
 	LogMessage msg( *this, theGroup );
	msg << Name( theGroup ) << " is ";
	if ( theAnswer == no ) msg << "not ";
	msg << "nilpotent of class " << theClass << explanation;
	msg.send();
	if( theAnswer == yes ) theGroup.gic.putIsNilpotent(theClass);
	adminTerminate();     
	return;     
    }
    if( theArcer.takeControl() ) 
      if( (theAnswer = theArcer.getRetValue()) != dontknow ) {
	LogMessage msg( *this, theGroup );
	msg << Name( theGroup ) << " is ";
	if ( theAnswer == no ) msg << "not ";
	msg << "nilpotent of class " << theClass;
	
	explanation = theChecker.getExplanation();
	if( explanation.length() > 0 )
	  msg << " : " << explanation;
	
	msg << ".";
	msg.send();
	if( theAnswer == yes ) theGroup.gic.putIsNilpotent(theClass);
	adminTerminate();
	return;
      }

    usedOneARC();
  }
}


void IsNilpotentProblem::viewStructure(ostream& ostr) const
{
  char classStr[10];
  sprintf(classStr,"%u",theClass);

  ProblemView pv( ostr, oid(), Text("Is nilpotent problem"),
		  Text("Is") + Name( theGroup ) + "nilpotent of class"
		  + classStr + "?",
		  helpID("IsNilpotentProblem", theGroup),
		  Text(Name(theGroup)) + "nilpotent ?"
		  );
  

  pv.startItemGroup( CheckinType(theGroup) == SMFPGroup::FP ||
		     CheckinType(theGroup) == SMFPGroup::SMALL_CANCELLATION ||
		     CheckinType(theGroup) == SMFPGroup::ONE_RELATOR ||
		     CheckinType(theGroup) == SMFPGroup::NILPOTENT
		     );
  
  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 50);

  /*  @am we don't need abelianization here
      pv.add(Text("Compute abelian invariants of") + Name(theGroup)
	 + "abelianized",
	 abelianInvariants.arcSlotID(),
	 15
	 );
	 */
  pv.startItemGroup( CheckinType(theGroup) == SMFPGroup::FP ||
		     CheckinType(theGroup) == SMFPGroup::SMALL_CANCELLATION ||
		     CheckinType(theGroup) == SMFPGroup::ONE_RELATOR 
		     );
  pv.add(Text("Enumerate normal closure of relators"),
	 normalClosure.arcSlotID(), 50);
  
  pv.add(Text("Seek a rewriting system for") + Name(theGroup),
	 kbSupervisor.arcSlotID(),
	 50
	 );
    
  pv.add(Text("Seek a ShortLex automatic structure for")
	 + Name(theGroup),
	 agSupervisor.arcSlotID(),
	 50
	 );

  pv.startItemGroup(CheckinType( theGroup ) == SMFPGroup::FP
		     );

  int upperBound, currentClass = 2;
  if (theGroup.gic.isNilpotent()==yes)
    upperBound = theGroup.gic.getNilpotentcyClass()-1;
  else
    upperBound = 10;
  
  while (currentClass <= upperBound && 
	 theGroup.gic.haveNilpotentQuotInited( currentClass ) )
    currentClass++;
  
  if (!(currentClass > upperBound) )
    pv.add(Text("Compute lower central quotients for") + Name( theGroup ),
	   nilpotentQuotients.arcSlotID(),
	   50
	   );
  pv.add(Text("Solve using lower central quotients of")
	 + Name( theGroup ),
	 nilpotentWPInQuotients.arcSlotID(),
	 50
	 );
  pv.done();
}

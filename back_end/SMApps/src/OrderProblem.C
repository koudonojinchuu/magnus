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


#include "OrderProblem.h"
#include "OutMessages.h"
#include "viewStructure.h"

// -------------------------- OrderProblem --------------------------------- //


OrderProblem::OrderProblem(SMFPGroup& G)
  : Supervisor( ! G.gic.haveMSC() ),
    theGroup( G ),
    kbSupervisor( *this, theGroup.gcm().kbSupervisor ),
    agSupervisor( *this, theGroup.gcm().agSupervisor ),
    computeBasis( *this, theGroup.gcm().computeBasis ),
    theToddCoxeter( *this, theGroup.gcm().theToddCoxeter),
    ghToddCoxeter( *this, theGroup.gcm().ghToddCoxeter)
{
  if ( theGroup.gic.haveOrder() ) resultIsFast();
}


void OrderProblem::takeControl( )
{
  
  // An OrderProblem merely allocates resources to parts of the GIC which
  // may discover the order as a byproduct of what they are doing.
  // Thus we have only to check whether our allocations have paid off.
  if (freeARCs() > 0){
      usedOneARC();
  }

  if ( theGroup.gic.haveOrder() ) {
    
    if( fastResult() )
      LogMessage(theGroup.gic.getOrderMessage()).send();
    else 
      LogMessage(*this, theGroup.gic.getOrderMessage(), 2).send();
    
    adminTerminate();
    return;
  }
  
}




void OrderProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Order problem"),
		  Text("Find the order of") + Name( theGroup ),
		  helpID("OrderProblem", theGroup),
		  Text("Order") + Name(theGroup)
		  );
/*@db  
  pv.startItemGroup();
  
  
  pv.add(Text("Determine whether the abelianization is infinite"),
	 abelianRank.arcSlotID(),
	 15
	 );
  
  pv.add(Text("Compute abelian invariants of") + Name( theGroup ),
	 abelianInvariants.arcSlotID(),
	 15
	 );
*/  


  pv.startItemGroup( CheckinType( theGroup ) == SMFPGroup::FP ||
		     CheckinType( theGroup ) == SMFPGroup::NILPOTENT 
		     );
   pv.add(Text("Enumerate cosets")
	  + Name( theGroup ),
	  theToddCoxeter.arcSlotID(),
	  100
	  );   
   
   pv.add(Text("GH Todd-Coxeter")
	  + Name( theGroup ),
	  ghToddCoxeter.arcSlotID(),
	  100
	  );   

   pv.startItemGroup( CheckinType( theGroup ) == SMFPGroup::NILPOTENT );
   
   pv.add(Text("Compute a basis of ")
	  + Name( theGroup ),
	  computeBasis.arcSlotID(),
	  100
	  );
 
   pv.startItemGroup( CheckinType( theGroup ) == SMFPGroup::FP );
   
   pv.add(Text("Seek a rewriting system for") + Name( theGroup ),
	  kbSupervisor.arcSlotID(),
	  50
	  );
   
   
   pv.add(Text("Seek a ShortLex automatic structure for") + Name( theGroup ),
	  agSupervisor.arcSlotID(),
	  50
	  );
   
   pv.done();

}


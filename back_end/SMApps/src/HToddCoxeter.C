// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class HToddCoxeterARCer HToddCoxeter,
//           HSGIndexToddCoxeter

//
// Principal Author: Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//


#include "HToddCoxeter.h"
#include "OutMessages.h"
#include "SMFPGroup.h"
#include "SMSubgroup.h"
#include "CommutatorIterator.h"
#include "viewStructure.h"

// ------------------- HToddCoxeterARCer --------------------- //


void HToddCoxeterARCer::runComputation( )
{
  tc.start();
  
  //  Timer t(delay()*1000);
  while ( !tc.finished() ) {;}

  if ( tc.getState() == SUCCESS )
    success = true;
  
  theIndex = tc. getIndex();

}

void HToddCoxeterARCer::writeResults( ostream& out ) 
{
  out < success;
  out < theIndex;
}

void HToddCoxeterARCer::readResults( istream& in )
{
  in > success;
  in > theIndex;
}

// -------------------------- HToddCoxeter ---------------------------- //


HToddCoxeter::HToddCoxeter(GCM& boss)
  : theBoss( boss ),
    theGroup( boss.getSMFPGroup() )
{
  if (theGroup.gic.isNilpotent()==yes){
    CommutatorIterator I(theGroup.getFPGroup().numberOfGenerators(), 
			 theGroup.gic.getNilpotentcyClass()+1);
    SetOf<Word> newRelators = theGroup.getFPGroup().getRelators();
    for( I.reset(); !I.done(); I.next() ){
      newRelators |= I.value();
    }
    FPGroup g(theGroup.getFPGroup().namesOfGenerators(),
	      newRelators);
    arcer = new HToddCoxeterARCer(*this,g);
  }
  else
    arcer = new HToddCoxeterARCer(*this,theGroup.getFPGroup());

  adminStart();
}


void HToddCoxeter::takeControl( )
{
  if ( freeARCs() > 0 )

    if( arcer->takeControl() ) {
      if ( arcer->isSuccessful() ){
	Chars explanation;
	explanation = "according to the GH Todd-Coxeter algorithm";
	theGroup.gic.putHaveOrder( arcer->getIndex(),
				   explanation );
      } else
	LogMessage("GH Todd-Coxeter terminated because it has exhausted the memory limits.").send();

      adminTerminate();
    }
    else
      usedOneARC();
}


// -------------------------- HSGIndexToddCoxeter ------------------------- //

HSGIndexToddCoxeter::HSGIndexToddCoxeter(SCM& scm)
  : theSCM( scm ),
    theSubgroup( scm.getSubgroup() ),
    arcer(*this,scm.getSubgroup().getParent().getFPGroup(),
	  scm.getSubgroup().getSubgroup().generators())
  
{
  adminStart();
}


void HSGIndexToddCoxeter::takeControl( )
{
  if ( freeARCs() > 0 )
    
   if( arcer.takeControl() ) {
      if ( arcer.isSuccessful() ){
	Chars explanation;
	explanation = "according to the GH Todd-Coxeter algorithm";
	theSubgroup.sic.putIndex( arcer.getIndex() );
      } else
	LogMessage("GH Todd-Coxeter terminated because it has exhausted the memory limits.").send();
      
      adminTerminate();
    }
    else
      usedOneARC();
}

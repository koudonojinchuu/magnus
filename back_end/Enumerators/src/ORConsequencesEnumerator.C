// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class ORConsequencesEnumerator
//
// Principal Author: 
//
// Status: in progress
//
// Revision History:
//


#include "ORConsequencesEnumerator.h"
#include "SMFPGroup.h"
#include "Subgroup.h"
#include "OneRelatorGroup.h"
#include "SetOfWordsChecker.h"
#include "OutMessages.h"
#include "viewStructure.h"

//--------------------------------------------------------------------------//
//--------------------- ORConsequencesEnumeratorARCer ----------------------//
//--------------------------------------------------------------------------//



void ORConsequencesEnumeratorARCer::writeResults( ostream& out ) 
{
  out < theEnumerator < counter;
}

void ORConsequencesEnumeratorARCer::readResults( istream& in )
{
  in > theEnumerator > counter;
}

void ORConsequencesEnumeratorARCer::enumerate()
{
  
  while ( counter <= numberOfAll ) {

    // returns if terminate was called in parent process
    if (submitSignalRecieved()){
      return;
    }
    
    file << counter << ": ";
    theGroup.printWord( file, theEnumerator.word() );
    file << endl << endl;
    theEnumerator.advance();
    counter++;
  } 
  
}

//--------------------------------------------------------------------------//
//----------------------- ORConsequencesEnumerator -------------------------//
//--------------------------------------------------------------------------//


ORConsequencesEnumerator::ORConsequencesEnumerator(SMFPGroup& s)
  : EnumeratorProblem< Word >( s ),
    arcer(*this, theData,OneRelatorGroup( s.getFPGroup().namesOfGenerators(),
				  s.gic.getOneRelator()
				  ) ),
    theSMFPGroup(s),
    started( false )
{
  
}


void ORConsequencesEnumerator::takeControl( )
{
  if ( freeARCs() > 0 ) {
    if (!started){
      LogMessage msg( *this, theSMFPGroup );
      
      msg << Link( Chars("Click here to see all consequences of the"
			 " relator of ") +
		   Text( Name(theSMFPGroup) ) ,
		   "ConsequencesEnumerator", arcer.getFileName(), 
		   true
		   ) 
	  << ".";
      msg.send();
      started = true;
      
    }
    if (arcer.takeControl()){
      
      adminSuspend();
      return;	
      
    }
 
    usedOneARC();
  }
}


void ORConsequencesEnumerator::viewStructure(ostream& ostr) const
{
  EnumertatorProblemView pv( oid(), "Consequences enumerator",
			     Text("Enumerate all consequences of the relator of the group")
			     + Name(theSMFPGroup),
			     helpID( "ConsequencesEnumerator", theSMFPGroup ), 
			     Text( "ConseqEnum" )
			     );
  
  pv.addParameter(EditInteger("numberOfAll","Number of relators",1,100,0,1000000));

  pv.addProblem( Subproblem("Sp1",ThisARCSlotID(),
			    Text("For the use of this enumerator"),50));
  
  submit(ostr, pv);
}

// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: 

//
// Principal Author: Alexei D. Miasnikov
//
// Status: in progress
//
// Revision History:
//


#include "MakeRandomPresentation.h"
#include "OutMessages.h"
#include "GCM.h"
#include "SMFPGroup.h"
#include "viewStructure.h"
#include "NormalRandomWord.h"
#include "FPGroup.h"

// ------------------- RandomPresentationARCer --------------------- //


void RandomPresentationARCer::runComputation( )
{
  // @am generate a random presentation
  UniformRandom ur;

  int nOfGens = ur.rand(2,maxGens);
  int nOfRels = ur.rand(2,maxRels);

  VectorOf<Chars> genNames(nOfGens);
  SetOf<Word> relts(nOfRels);
  
  for (int i=0;i<nOfGens;i++)
    genNames[i] = Chars("x") + Chars(i+1);

  for (int i=0;i<nOfRels;i++){
    NormalRandomWord nw(nOfGens, averageRels, ur.rand(0,averageRels/5),
			ur.rand(10,100000),ur.rand(10,100000));
    relts |= nw.word();
  }
  
  theGroup = FPGroup(genNames,relts);
}

void RandomPresentationARCer::writeResults( ostream& out ) 
{
  out < theGroup;
}

void RandomPresentationARCer::readResults( istream& in )
{
  in > theGroup;
}

// -------------------------- MakeRandomPresentation ----------------------- //

MakeRandomPresentation::MakeRandomPresentation(/*SMFPGroup& G*/)
  : Supervisor( true ),
    arcer( *this ),
    arcerStarted( false ),
    nOfPresentations( 1 )
  //theGroup( G )
{
  
}

void MakeRandomPresentation::takeControl( )
{
  if( freeARCs() ) {

    if ( !arcerStarted ){

      //      arcer.setArguments( getPermutations(), theTestWords[checkedWord] );
      arcerStarted = true;
    }
    
    if( arcer.takeControl() ) {
      
      SMFPGroup* smo
	= new SMFPGroup( arcer.getRetValue(),
			 Text("Random presentation"),
			 SMFPGroup::FP
			 );
      ListOf<OID> dependencies;
      CheckinMessage( *smo, "", dependencies ).send();
      
      if ((--nOfPresentations) == 0){
	adminTerminate();
	return;
      }
    }
    
    usedOneARC();
  } 

}

void MakeRandomPresentation::start(){
  nOfPresentations = getParameters().getValue("N");
  int maxG = getParameters().getValue("maxG");
  int maxR = getParameters().getValue("maxR");
  int averR = getParameters().getValue("averR");
  arcer.setArguments(maxG,maxR,averR);
}

void MakeRandomPresentation::viewStructure(ostream& ostr) const
{
   EnumertatorProblemView pv(oid(),"Make a random presentation",
			     Text("Make a random presentation" ) ,
			     "MakeRandomPresentation",
			     Text("Rand pres")
			     );
  
  pv.addParameter(EditInteger("N","Number of presentations ",0,1,1,10));
  pv.addParameter(EditInteger("maxG","Maximal number of generators ",0,2,1,100));
  pv.addParameter(EditInteger("maxR","Maximal number of relators ",0,2,1,100));
  pv.addParameter(EditInteger("averR","Average length of relators ",0,10,1,100));
 
  pv.addProblem( Subproblem("Sp1",ThisARCSlotID(),
			    Text("Generate random presentation"),100));
  
  
  submit(ostr, pv);
}

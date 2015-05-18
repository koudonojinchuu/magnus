// Copyright (C) 2001 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes ACEArcer, ACECM, ACEProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//

#include "ACE.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "BlackBox.h"


// -------------------------- ACEArcer ---------------------- //


ACEArcer::ACEArcer( ComputationManager& boss )
  : ARCer( boss )
  //ace(MagnusHome::magnusHome()+ "/back_end/black_boxes/ace/ace")
{ } 

void ACEArcer::setArguments( FPGroup g, VectorOf<Word> sg )
{
  theGroup  =  g;
  theSubgroup = sg;
}

void ACEArcer::runComputation( ) 
{
  /*
  if (!ace.start())
    error("void ACEArcer::runComputation( ) : couldn't start the black box.");

  ostream& out = ace.toCommand();
  //ofstream out("/tmp/tc.in", ios::out | ios::trunc );
  */

  File out;
  //@njz
  //  ostrstream cmd;
  std::ostrstream cmd;
  //
  cmd << MagnusHome::magnusHome()+ "/back_end/black_boxes/ace/ace < "
      << out.getFileName() << " > " << in.getFileName() << ends << flush;

  out << "gr: ";
  int numOfGens = theGroup.numberOfGenerators();
  for( int i = 0; i < numOfGens-1; ++i)
    out << theGroup.nameOfGenerator(i) << ", ";
  if( numOfGens-1 >= 0 )
    out << theGroup.nameOfGenerator(numOfGens-1) << ";" << endl;
  
  out << "rel: ";
  SetOf<Word> rels = theGroup.getRelators();
  int numOfRels = rels.cardinality();
  SetIterator<Word> I(rels);
  for( int i = 0; i < numOfRels-1; ++i, I.next()) {
    theGroup.printWord(out, I.value());
    out << ", ";
  }
  if( numOfRels > 0 ) {
    theGroup.printWord(out, I.value());
    out << ";" << endl;
  }
  
  out << "gen: ";
  numOfGens = theSubgroup.length();
  for( int i = 0; i < numOfGens-1; ++i) {
    theGroup.printWord(out, theSubgroup[i]);
    out << ", ";
  }
  if( numOfGens-1 >= 0 )
    theGroup.printWord(out, theSubgroup[numOfGens-1]);
  out << ";" << endl;
  
  out << "time: " << iTime << ";" << endl;
  out << "workspace: " << iWS << ";" << endl;
  if( rDiff == Chars("hard") )
    out << "hard;" << endl;
  else
    out << "easy;" << endl;

  out << "mess:25000;" << endl << "end;" << endl << flush;

  ofstream out2("/tmp/tc.in", ios::out | ios::trunc );
  out2 << cExtra << endl;
  
  system(cmd.str());

}

void ACEArcer::writeResults( ostream& out ) { }

void ACEArcer::readResults( istream& in ) { }


// ---------------------------- ACECM ------------------------ //


ACECM::ACECM( ACEProblem& boss )
  : arcer( *this ),  
    linkHasBeenSent(false), 
    bFinished( false ),
    theSubgroup( boss.getSubgroup() )

{
  arcer.setArguments( boss.getGroup().getFPGroup(), 
		      theSubgroup.getSubgroup().generators() );
  adminStart();

  LogMessage msg( *this, boss.getGroup(), theSubgroup );
  msg << "Running ACE 3.000 ...";
  msg.send();
}

void ACECM::terminate( )  { }

void ACECM::start( ) { }

void ACECM::takeControl( )
{
  if ( freeARCs() > 0 ) {
    
    if( !linkHasBeenSent ) {
      
      linkHasBeenSent = true;
      
      LogMessage msg( *this, theSubgroup );
      msg << Link("Click here to see the ouput of ACE.",
		  "ACE", arcer.getFileName(), true);
      
      //@db tmp
      //msg << getComputationFileName();
      
      msg.send();
    }

    bool bArcer = arcer.takeControl();

    /*
    if( arcer.getBB().hasInput() ) {

      //@db tmp
      LogMessage msg( *this, theSubgroup );
      msg << "hello!";
      msg.send();

      istream& in = arcer.getBB().fromCommand();
      char theLine[1000];
      while( !in.eof() ) {
	in.getline(theLine,1000);
	file << theLine << endl;
      }
    }
    */
    if( bArcer ) {
      bFinished = true;
      adminTerminate();
      return;
    }
    else
      usedOneARC();
    
  }
}


// -------------------- ACEProblem --------------------------- //


ACEProblem::ACEProblem( const class SMSubgroup& sg )
  : Supervisor( true ), 
    theGroup( sg.getParent() ), 
    theSubgroup( sg ),
    ace( *this )
{ }

ACEProblem::ACEProblem( class SMFPGroup& g )
  : Supervisor( true ), 
    theGroup( g ), 
    theSubgroup( (SMSubgroup&)g ), // @am temporary
    ace( *this )
{ }

void ACEProblem::start( ) 
{
  // Set parameters for the arcer
  
  int iTime = getParameters().getValue("Time");
  ace->getArcer().setTime( iTime );
  int iWS = getParameters().getValue("Workspace");
  ace->getArcer().setWorkspace( iWS );
  Chars rDiff = getParameters().getValue("Difficulty");
  ace->getArcer().setDifficulty( rDiff );
  Chars cExtra = getParameters().getValue("Extra");
  ace->getArcer().setExtra( cExtra );
}

void ACEProblem::takeControl( )
{
  if ( ace->isFinished() ) {
    adminTerminate();
    return;
  }
}


void ACEProblem::viewStructure(ostream& ostr) const
{
  /*
  ProblemView pv( ostr, oid(), "Run Todd-Coxeter",
		  Text("Run Todd-Coxeter for") + Name( theSubgroup ),
		  helpID("ACE", theSubgroup.getParent()),
		  "ACE"
		  );
  */

  EnumertatorProblemView pv( oid(), "Run Todd-Coxeter",
			     Text("Run Todd-Coxeter for")+Name( theSubgroup ),
			     helpID("ACE", theSubgroup.getParent()),
			     "ACE"
			     );

  RadioButton rHard("Hard","Hard",1);
  RadioButton rEasy("Easy","Easy",1);
  RadioButtonGroup rDiff("Difficulty");
  rDiff.add(rHard);
  rDiff.add(rEasy);
  pv.addParameter(rDiff);

  EditInteger iWS("Workspace","Workspace",0,0,0,1000000);
  pv.addParameter(iWS);

  EditInteger iTime("Time","Time",0,-1,-1);
  pv.addParameter(iTime);

  EditText ePars("Extra","More ACE Parameters",1);
  pv.addParameter(ePars);

  pv.addProblem( Subproblem("ACE", ace.arcSlotID(),
			    Text("For the use of the computation"), 100));
  submit(ostr, pv);
}



#include "SMListWordProblem.h"
#include "SMEnumerator.h"
#include "OutMessages.h"
#include "SMFPGroup.h"
#include "viewStructure.h"
#include "SetOfWordsChecker.h"
#include "FreeGroup.h"
#include "SMWord.h"
#include "SMList.h"
#include "SMListIterator.h"

#include "GAWP.h"


// -------------------------- SMListGeneticWPArcer ---------------------- //


SMListGeneticWPArcer::SMListGeneticWPArcer( ComputationManager& boss )
  : ARCer( boss ), theList( NULL ), theIterator( NULL )
{ } 

void SMListGeneticWPArcer::setArguments( SMList<Word>* l )
{
  if ( theIterator )
    delete theIterator;
  theIterator  =  new SMListIterator<Word>(*l);
  theList = l;
}

void SMListGeneticWPArcer::runComputation( ) 
{
  GHNConfig c;
  GAWP2 gawp(theList->getGroup().getFPGroup(),c);
  
  int bLength = theList->lic().numberOfElements();
  for ( int counter = 0; counter < bLength; counter ++ ){
    if ( theList->lic().getTrivialStatus(counter) == dontknow ){
      theList->lic().setTrivialStatus( counter,
				       gawp.isTrivial(theIterator->getData()),
				       GIC::GENETIC);
    }
    theIterator->nextCell();
  }
}

void  SMListGeneticWPArcer::writeResults( ostream& out )
{
}

void SMListGeneticWPArcer::readResults( istream& in )
{
}


// ---------------------------- SMListGeneticWPCM ------------------------ //


SMListGeneticWPCM::SMListGeneticWPCM( SMListSupervisor& sms )
  : arcer( *this )
{
 arcer.setArguments( &(sms.getList()) );
 adminStart();
}

void SMListGeneticWPCM::terminate( )
{
}

void SMListGeneticWPCM::start( ) { }


void SMListGeneticWPCM::takeControl( )
{
  if ( freeARCs() > 0 ) {
    if( arcer.takeControl() ) {
      adminTerminate();
      return;
    }
    else
      usedOneARC();
  }
}


// ---------------------- SMListWPCheckARCer ----------------- //

void SMListWPCheckARCer::setArguments( SMList<Word>* l,
				      const SMFPGroup* g)
{
  if ( theIterator )
    delete theIterator;
  theIterator  =  new SMListIterator<Word>(*l);
  theGroup = g;
  theList = l;
}
 
void SMListWPCheckARCer::runComputation( )
{
  int bLength = theList->lic().numberOfElements();
  for ( int counter = 0; counter < bLength; counter ++ ){
    if ( theList->lic().getTrivialStatus(counter) == dontknow ){
      // we don't have free groups in set checker ?????
      // so I'm checking it separately
      if ( theGroup->getCheckinType() == SMFPGroup::FREE ){
	FreeGroup F = theGroup->getFreePreimage();
	theList->lic().setTrivialStatus( counter, 
					 F.wordProblem(theIterator->getData()),
					 GIC::FREE );
      } else {
	// for others call standart checker
	SetOfWordsChecker theChecker( theIterator->getData(), *theGroup );
	Trichotomy isTrivial = theChecker.isTrivial();
	theList->lic().setTrivialStatus( counter,isTrivial,
					 theChecker.getAlgorithm() );
      }
    }
    theIterator->nextCell();
  }
  
}


// ----------------------- SMListWPCheckComputation ----------------- //


SMListWPCheckComputation::SMListWPCheckComputation(SMListSupervisor& sms)
  : theList( sms.getList() ),
    arcer( *this ),
    started( false )
{ 
  adminStart();
}

SMListWPCheckComputation::~SMListWPCheckComputation()
{

}

void SMListWPCheckComputation::takeControl( )
{
  if (freeARCs() > 0){
    if ( ! started ){
      arcer.setArguments( &theList, &(theList.getGroup()) );
      started = true;
    }
    
    if ( arcer.takeControl() ){
      //      arcer.runComputation();
      started = false;
      if ( theList.lic().IsTrivialChecked() ) {
	adminTerminate();
	return;
      }
    }else
      usedOneARC();
  }
}

// ---------------------- SMListWPPrinResultArcer ----------------- //

void SMListWPPrinResultArcer::setArguments( SMList<Word>* l )
{
  theList = l;
}

void SMListWPPrinResultArcer::runComputation( )
{
  int bLength = theList->lic().numberOfElements();
  char* printStatus = new char[bLength];
  for ( int i = 0; i<bLength; i++)
    printStatus[i] = 0;
  int tCounter = 1;
  int ntCounter = 1;
  bool allPrinted = false;
  FPGroup fpGroup = theList->getGroup().getFPGroup();
  
  trivialData.setWriteMode();
  
  while ( !allPrinted ){
    allPrinted = true;
    SMListIterator<Word>  I( *theList );
    for ( int counter = 0; counter < bLength; counter ++, I.nextCell() ){
      GIC::AlgorithmID theAl = GIC::NONE;
      Chars explanation;

      Trichotomy result( dontknow );
      if ( ( result = 
	     theList->lic().getTrivialStatus( counter, theAl, explanation ) )
	   != dontknow ) {
	
	if ( printStatus[counter] == 0 ){
	  if ( result == yes ) {
	    trivialData  << EnumWriteWord( I.getData( ), fpGroup )
			 << Chars("\n") << explanation 
			 << Chars("\n\n");
	    tCounter++;
	    printStatus[counter] = 1;
	  } else {
	    fileOfNonTrivial << ntCounter << ": " 
			     << EnumWriteWord( I.getData( ), fpGroup )
			     << endl << explanation 
			     << endl << endl;
	    printStatus[counter] = 1;
	    ntCounter++;
	  }
	}
      } else
	allPrinted = false;
    }
  }  
  trivialData.closeCurrentMode(); 
  delete [] printStatus;
}



// --------------------- SMListExtractTrivialWords ----------------- //


SMListExtractTrivialWords::SMListExtractTrivialWords(SMList<Word>& sml)
  : SMListSupervisor( sml ),
    abelianInvariants( *this, sml.getGroup().gcm().abelianInvariants ),
    checker( *this ),
    genetic( *this ),
    normalClosure( *this, sml.getGroup().gcm().normalClosure ),
    kbSupervisor( *this, sml.getGroup().gcm().kbSupervisor ),
    agSupervisor( *this, sml.getGroup().gcm().agSupervisor ),
    arcer(*this )
{
  arcer.setArguments( &sml );
}

void SMListExtractTrivialWords::start( )
{
  LogMessage msg( *this, theList );
  msg << Link(Chars( "Click here to see trivial words in ")+
	      Text( Name( theList ) ),
	      "WEnumerator",
	      arcer.getTrivialData().getDataFileName(),true );
  msg.send();

  LogMessage msg1( *this, theList );
  msg1 << Link(Chars( "Click here to see non-trivial words in ")+
	      Text( Name( theList ) ),
	      "WEnumerator",
	      arcer.getFileOfNonTrivial(),true );
  msg1.send();
}

void SMListExtractTrivialWords::takeControl( )
{
  
  if (freeARCs() > 0){
    if ( arcer.takeControl() ){
      SMList<Word>* smo = 
	new  SMList<Word>(theList.getGroup(),
			  arcer.getTrivialData(),
			  Text("Trivial words from") + Name(theList) 
			  ); 
      
      ListOf<OID> dependencies( smo->getGroup() );
      CheckinMessage( *smo, "", dependencies ).send();
      
      LogMessage msg( *smo );
      msg << Link( Chars("Click here to see the contents of ") + 
		   Text( Name( *smo) ),
		   "WEnumerator",
		   smo->getData().getDataFileName() 
		   );
      msg << ".";
      msg.send();
      adminTerminate();
      return;
    } else 
      usedOneARC();
  }
}


void SMListExtractTrivialWords::viewStructure(ostream& ostr) const
{
  EnumertatorProblemView pv(oid(),"Extract trivial words from a list",
			    Text("Extract trivial words from ") +
			    Name( theList ) ,
			    helpID("SMListExtractTrivialWords",
				   theList.getGroup()),
			    Text("trivial words")
			    );
  
  //  RadioButton rRB1("ToFile","Extract to a file");
  //  RadioButton rRB2("ToList","Extract to another list");
  
  //  RadioButtonGroup extractRBG("ExtractTo");
  
  //  extractRBG.add(rRB1);
  //  extractRBG.add(rRB2);
  
  //  pv.addParameter(extractRBG);


  if (theList.getGroup().getCheckinType() == SMFPGroup::FP ) {
    
    if ( ! theList.getGroup().gic.haveCyclicDecomposition() ) 
      pv.addProblem(Subproblem("SpA",abelianInvariants.arcSlotID(),
			       Text("Compute abelian invariants of") + 
			       Name( Parent( theList ) ),
			       15)
		    );
    
    
    if ( ! theList.getGroup().gic.haveFastWordProblem() ) {
      
      pv.addProblem( Subproblem("Sp1",normalClosure.arcSlotID(),
				Text("Enumerate normal closure of relators"),
				50));
      
      pv.addProblem( Subproblem("Sp2",kbSupervisor.arcSlotID(),
				Text("Seek a rewriting system for") +
				Name( theList.getGroup() ),
			      50));
      
      pv.addProblem( Subproblem("Sp3",agSupervisor.arcSlotID(),
				Text("Seek a ShortLex automatic structure for") +
				Name( theList.getGroup() ),
				50));
      
      pv.addProblem( Subproblem("Sp4",genetic.arcSlotID(),
				Text("Run genetic algorithm"),
				50));

     }
  }
  
  if (theList.getGroup().getCheckinType() == SMFPGroup::FP == SMFPGroup::FP ||
      theList.getGroup().getCheckinType() == SMFPGroup::FP == SMFPGroup::FREE ){
    pv.addProblem( Subproblem("Sp5",checker.arcSlotID(),
			      Text("Extract trivial words"),40));
  }

 pv.addProblem( Subproblem("Sp6",ThisARCSlotID(),
			    Text("For the use of this problem"),50));
  
   submit(ostr, pv);

}


// -------------------------- SMListWordIsCentralARCer --------------------- //
 

SMListWordIsCentralARCer::SMListWordIsCentralARCer( ComputationManager& boss )
  : ARCer( boss ), theList( NULL ), theIterator( NULL ), 
  tCounter( 1 ), ntCounter( 1 )
{ } 

void SMListWordIsCentralARCer::setArguments( SMList<Word>* l )
{
  if ( theIterator )
    delete theIterator;
  theIterator  =  new SMListIterator<Word>(*l);
  theList = l;
}

void SMListWordIsCentralARCer::runComputation( ) 
{

  int bLength = theList->lic().numberOfElements();
  centralData.setWriteMode();

  for ( int counter = 0; counter < bLength; counter ++ ){
    if ( theList->lic().getIsCentralStatus(counter) == dontknow ){
      
      // Make all commutators and check are they trivial or not 
      SetOf<Word> comms;
      
      FPGroup fpGroup = theList->getGroup().getFPGroup(); 
      int numOfGens = fpGroup.numberOfGenerators();
      Word w = theIterator->getData();
      
      for( int i = 0; i < numOfGens; ++i ) {
	Word gen = Word(Generator(i+1));
	comms |= (w.inverse() * gen.inverse() * w * gen).freelyReduce();
      }
      
      SetOfWordsChecker theChecker( comms, theList->getGroup() );
      
      Trichotomy isTrivial = theChecker.isTrivial();
      
      theList->lic().setIsCentralStatus( counter, isTrivial,
					 theChecker.getAlgorithm() );
      if ( isTrivial == yes ) {
	centralData   << EnumWriteWord( w,fpGroup )
		      << Chars("\n") << theChecker.getExplanation() 
		      << Chars("\n\n\n");
	tCounter++;
      } 
      if ( isTrivial == no ) {
	fileOfNonCentral << ntCounter << ": " 
			 << EnumWriteWord( w,fpGroup )
			 << endl << theChecker.getExplanation() 
			 << endl << endl <<endl;
	ntCounter++;
      }
    }
    theIterator->nextCell();
  }

  centralData.closeCurrentMode(); 
}

// ----------------------- SMListWordIsCentralComputation ----------------- //


SMListWordIsCentralComputation::
SMListWordIsCentralComputation( SMListExtractCentralWords& sms )
  : theList( sms.getList() ),
    arcer( *this ),
    started( false )
{ 
  adminStart();
}

SMListWordIsCentralComputation::~SMListWordIsCentralComputation()
{

}

void SMListWordIsCentralComputation::takeControl( )
{
  if (freeARCs() > 0){
    if ( ! started ){
      arcer.setArguments( &theList );
      started = true;
    }
    
    if ( arcer.takeControl() ){
      //    arcer.runComputation();
      started = false;
      if ( theList.lic().IsCentralChecked() ) {
	theList.lic().putIsCentralFiles( arcer.getCentralData().getDataFileName(),
					 arcer.getFileOfNonCentral() );
	adminTerminate();
	return;
      }
    }else
      usedOneARC();
  }
}

// --------------------- SMListExtractCentralWords ----------------- //


SMListExtractCentralWords::SMListExtractCentralWords(SMList<Word>& sml)
  : Supervisor( true ),
    theList( sml ),
    abelianInvariants( *this, sml.getGroup().gcm().abelianInvariants ),
    checker( *this ),
    normalClosure( *this, sml.getGroup().gcm().normalClosure ),
    kbSupervisor( *this, sml.getGroup().gcm().kbSupervisor ),
    agSupervisor( *this, sml.getGroup().gcm().agSupervisor )
{ 
  if ( sml.lic().IsCentralChecked() )
    resultIsFast();
}

void SMListExtractCentralWords::start( )
{

  if ( !fastResult() ) {
    LogMessage msg( *this, theList );
    msg << Link(Chars( "Click here to see central words in ")+
		Text( Name( theList ) ),
		"WEnumerator",
		checker->getCentralData().getDataFileName(),true );
    msg.send();
    
    LogMessage msg1( *this, theList );
    msg1 << Link(Chars( "Click here to see non-central words in ")+
		 Text( Name( theList ) ),
		 "WEnumerator",
		 checker->getFileOfNonCentral(),true );
    msg1.send();
  }
}

void SMListExtractCentralWords::takeControl( )
{
  if ( fastResult() ) {

    LogMessage msg( *this, theList );
    msg << Link(Chars( "Click here to see central words in ")+
		Text( Name( theList ) ),
		"WEnumerator",
		theList.lic().getListOfCentral() );
    msg.send();
    
    LogMessage msg1( *this, theList );
    msg1 << Link(Chars( "Click here to see non-central words in ")+
		 Text( Name( theList ) ),
		 "WEnumerator",
		theList.lic().getListOfNonCentral() );
    msg1.send();   

  } else {
    if ( theList.lic().IsCentralChecked() ) {
      SMList<Word>* smo = 
	new  SMList<Word>(theList.getGroup(),
			  checker->getCentralData(),
			  Text("Central words from") + Name(theList) 
			  ); 
      
      ListOf<OID> dependencies( smo->getGroup() );
      CheckinMessage( *smo, "", dependencies ).send();
      
      LogMessage msg( *smo );
      msg << Link( Chars("Click here to see the contents of ") + 
		   Text( Name( *smo) ),
		   "WEnumerator",
		   smo->getData().getDataFileName() 
		   );
      msg << ".";
      msg.send();
      adminTerminate();
      return;
    }
  }
}

void SMListExtractCentralWords::viewStructure(ostream& ostr) const
{
  EnumertatorProblemView pv(oid(),"Extract central words from a list",
			    Text("Extract central words from ") +
			    Name( theList ) ,
			    helpID("SMListExtractCentralWords",
				   theList.getGroup()),
			    Text("cent words")
			    );
  if (theList.getGroup().getCheckinType() == SMFPGroup::FP ) {
    
    if ( ! theList.getGroup().gic.haveCyclicDecomposition() ) 
      pv.addProblem(Subproblem("SpA",abelianInvariants.arcSlotID(),
			       Text("Compute abelian invariants of") + 
			       Name( Parent( theList ) ),
			       15)
		    );
    
    
    if ( ! theList.getGroup().gic.haveFastWordProblem() ) {
      
      pv.addProblem( Subproblem("Sp2",normalClosure.arcSlotID(),
				Text("Enumerate normal closure of relators"),
				50));
      
      pv.addProblem( Subproblem("Sp2",kbSupervisor.arcSlotID(),
				Text("Seek a rewriting system for") +
				Name( theList.getGroup() ),
				50));
      
      pv.addProblem( Subproblem("Sp2",agSupervisor.arcSlotID(),
				Text("Seek a ShortLex automatic structure for") +
				Name( theList.getGroup() ),
				50));
    }
    
  
  }
  if ( ! theList.lic().IsCentralChecked() )
    pv.addProblem( Subproblem("Sp1",checker.arcSlotID(),
			      Text("Extract central words"),40));
  
  submit(ostr, pv);

}

#include "SMListSubgroupProblems.h"
#include "OutMessages.h"
#include "SMFPGroup.h"
#include "viewStructure.h"
#include "SetOfWordsChecker.h"
#include "SMList.h"
#include "SMListIterator.h"

// -------------------------- SMListSGTrivialARCer ---------------------- //
 

SMListSGTrivialARCer::SMListSGTrivialARCer( ComputationManager& boss )
  : ARCer( boss ), theList( NULL ), theIterator( NULL ), 
    tCounter( 1 ), ntCounter( 1 )
{ } 

void SMListSGTrivialARCer::setArguments( SMList<Subgroup>* l )
{
  if ( theIterator )
    delete theIterator;
  theIterator  =  new SMListIterator<Subgroup>(*l);
  theList = l;
}

void SMListSGTrivialARCer::runComputation( ) 
{

  int bLength = theList->lic().numberOfElements();
  trivialData.setWriteMode();
  
  for ( int counter = 0; counter < bLength; counter ++ ){
    if ( theList->lic().getTrivialStatus(counter) == dontknow ){
      
      VectorOf<Word> gens = theIterator->getData().generators();
      FPGroup fpGroup = theList->getGroup().getFPGroup();
      
      SetOfWordsChecker theChecker( gens,
				    theList->getGroup() );
      
      Trichotomy isTrivial = theChecker.isTrivial();
      
      theList->lic().setTrivialStatus( counter, isTrivial,
				       theChecker.getAlgorithm() );
      if ( isTrivial == yes ) {
	trivialData   << EnumWriteVectorOfWords( gens,
						 fpGroup )
		      << theChecker.getExplanation() 
		      << Chars("\n\n\n");
	tCounter++;
      } 
      if ( isTrivial == no ) {
	fileOfNonTrivial << ntCounter << ": " 
			 << EnumWriteVectorOfWords( gens,
						    fpGroup )
			 << theChecker.getExplanation() 
			 << endl << endl <<endl;
	ntCounter++;
      }
    }
    theIterator->nextCell();
  }
  trivialData.closeCurrentMode(); 

}

// ----------------------- SMListSGTrivialComputation ----------------- //


SMListSGTrivialComputation::
SMListSGTrivialComputation( SMListExtractTrivialSubgroups& sms )
  : theList( sms.getList() ),
    arcer( *this ),
    started( false )
{ 
  adminStart();
}

SMListSGTrivialComputation::~SMListSGTrivialComputation()
{

}

void SMListSGTrivialComputation::takeControl( )
{
  if (freeARCs() > 0){
    if ( ! started ){
      arcer.setArguments( &theList );
      started = true;
    }
    
    if ( arcer.takeControl() ){
      //    arcer.runComputation();
      started = false;
      if ( theList.lic().IsTrivialChecked() ) {
	theList.lic().putIsTrivialFiles( arcer.getTrivialData().getDataFileName(),
					 arcer.getFileOfNonTrivial() );
	adminTerminate();
	return;
      }
    }else
      usedOneARC();
  }
}


// --------------------- SMListExtractTrivialSubgroups ----------------- //


SMListExtractTrivialSubgroups::SMListExtractTrivialSubgroups(SMList<Subgroup>& sml)
  : Supervisor( true ),
    theList( sml ),
    abelianInvariants( *this, sml.getGroup().gcm().abelianInvariants ),
    checker( *this ),
    normalClosure( *this, sml.getGroup().gcm().normalClosure ),
    kbSupervisor( *this, sml.getGroup().gcm().kbSupervisor ),
    agSupervisor( *this, sml.getGroup().gcm().agSupervisor )
{ 
 if ( sml.lic().IsTrivialChecked() )
    resultIsFast();
}

void SMListExtractTrivialSubgroups::start( )
{
  if ( !fastResult() ) {
    LogMessage msg( *this, theList );
    msg << Link(Chars( "Click here to see trivial subgroups in ")+
		Text( Name( theList ) ),
		"SGEnumerator",
		checker->getTrivialData().getDataFileName(),true );
    msg.send();
    
    LogMessage msg1( *this, theList );
    msg1 << Link(Chars( "Click here to see non-trivial subgroups in ")+
		 Text( Name( theList ) ),
		 "SGEnumerator",
		 checker->getFileOfNonTrivial(),true );
    msg1.send();
  }
}

void SMListExtractTrivialSubgroups::takeControl( )
{
  if ( fastResult() ) {
    LogMessage msg( *this, theList );
    msg << Link(Chars( "Click here to see trivial subgroups in ")+
		Text( Name( theList ) ),
		"SGEnumerator",
		theList.lic().getListOfTrivial() );
    msg.send();
    
    LogMessage msg1( *this, theList );
    msg1 << Link(Chars( "Click here to see non-trivial subgroups in ")+
		 Text( Name( theList ) ),
		 "SGEnumerator",
		 theList.lic().getListOfNonTrivial() );
    msg1.send();   

  } else {
    if ( theList.lic().IsTrivialChecked() ) {
      SMList<Subgroup>* smo = 
	new  SMList<Subgroup>(theList.getGroup(),
			  checker->getTrivialData(),
			  Text("Trivial subgroups from") + Name(theList) 
			  ); 
      
      ListOf<OID> dependencies( smo->getGroup() );
      CheckinMessage( *smo, "", dependencies ).send();
      
      LogMessage msg( *smo );
      msg << Link( Chars("Click here to see the contents of ") + 
		   Text( Name( *smo) ),
		   "SGEnumerator",
		   smo->getData().getDataFileName() 
		   );
      msg << ".";
      msg.send();
      adminTerminate();
      return;
    }
  }
}

void SMListExtractTrivialSubgroups::viewStructure(ostream& ostr) const
{
  EnumertatorProblemView pv(oid(),"Extract trivial subgroups from a list",
			    Text("Extract trivial subgroups from ") +
			    Name( theList ) ,
			    helpID("SMListExtractTrivialSubgroups",
				   theList.getGroup()),
			    Text("trivial sgs")
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
  
  if ( !theList.lic().IsTrivialChecked() ) 
    pv.addProblem( Subproblem("Sp1",checker.arcSlotID(),
			      Text("Extract trivial subgroups"),40));
    
  submit(ostr, pv);

}


// -------------------------- SMListSGAbelianARCer ---------------------- //
 

SMListSGAbelianARCer::SMListSGAbelianARCer( ComputationManager& boss )
  : ARCer( boss ), theList( NULL ), theIterator( NULL ), 
  tCounter( 1 ), ntCounter( 1 )
{ } 

void SMListSGAbelianARCer::setArguments( SMList<Subgroup>* l )
{
  if ( theIterator )
    delete theIterator;
  theIterator  =  new SMListIterator<Subgroup>(*l);
  theList = l;
}

void SMListSGAbelianARCer::runComputation( ) 
{

  int bLength = theList->lic().numberOfElements();
  abelianData.setWriteMode();

  for ( int counter = 0; counter < bLength; counter ++ ){
    if ( theList->lic().getAbelianStatus(counter) == dontknow ){
      
      // Make all commutators and check are they trivial or not 
      SetOf<Word> comms;
      VectorOf<Word> gens = theIterator->getData().generators();
      int gensLen = gens.length();
      FPGroup fpGroup = theList->getGroup().getFPGroup(); 
      
      for( int i = 0; i < gensLen-1; ++i ) 
	for( int j = i+1; j < gensLen; ++j ) {
	  Word w1 = gens[i];
	  Word w2 = gens[j];
	  comms |= (w1.inverse() * w2.inverse() * w1 * w2).freelyReduce();
	}


      SetOfWordsChecker theChecker( comms, theList->getGroup() );

      Trichotomy isTrivial = theChecker.isTrivial();
      
      theList->lic().setAbelianStatus( counter, isTrivial,
				       theChecker.getAlgorithm() );
      if ( isTrivial == yes ) {
	abelianData   << EnumWriteVectorOfWords( gens,
						 fpGroup )
		      << theChecker.getExplanation() 
		      << Chars("\n\n\n");
	tCounter++;
      } 
      if ( isTrivial == no ) {
	fileOfNonAbelian << ntCounter << ": " 
			 << EnumWriteVectorOfWords( gens,
						    fpGroup )
			 << theChecker.getExplanation() 
			 << endl << endl <<endl;
	ntCounter++;
      }
    }
    theIterator->nextCell();
  }
  abelianData.closeCurrentMode(); 
}

// ----------------------- SMListSGAbelianComputation ----------------- //


SMListSGAbelianComputation::
SMListSGAbelianComputation( SMListExtractAbelianSubgroups& sms )
  : theList( sms.getList() ),
    arcer( *this ),
    started( false )
{ 
  adminStart();
}

SMListSGAbelianComputation::~SMListSGAbelianComputation()
{

}

void SMListSGAbelianComputation::takeControl( )
{
  if (freeARCs() > 0){
    if ( ! started ){
      arcer.setArguments( &theList );
      started = true;
    }
    
    if ( arcer.takeControl() ){
      //    arcer.runComputation();
      started = false;
      if ( theList.lic().IsAbelianChecked() ) {
	theList.lic().putAbelianFiles( arcer.getAbelianData().getDataFileName(),
				       arcer.getFileOfNonAbelian() );
	adminTerminate();
	return;
      }
    }else
      usedOneARC();
  }
}

// --------------------- SMListExtractAbelianSubgroups ----------------- //


SMListExtractAbelianSubgroups::SMListExtractAbelianSubgroups(SMList<Subgroup>& sml)
  : Supervisor( true ),
    theList( sml ),
    abelianInvariants( *this, sml.getGroup().gcm().abelianInvariants ),
    checker( *this ),
    normalClosure( *this, sml.getGroup().gcm().normalClosure ),
    kbSupervisor( *this, sml.getGroup().gcm().kbSupervisor ),
    agSupervisor( *this, sml.getGroup().gcm().agSupervisor )
{ 
  if ( sml.lic().IsAbelianChecked() )
    resultIsFast();
}

void SMListExtractAbelianSubgroups::start( )
{

  if ( !fastResult() ) {
    LogMessage msg( *this, theList );
    msg << Link(Chars( "Click here to see abelian subgroups in ")+
		Text( Name( theList ) ),
		"SGEnumerator",
		checker->getAbelianData().getDataFileName(),true );
    msg.send();
    
    LogMessage msg1( *this, theList );
    msg1 << Link(Chars( "Click here to see non-abelian subgroup in ")+
		 Text( Name( theList ) ),
		 "SGEnumerator",
		 checker->getFileOfNonAbelian(),true );
    msg1.send();
  }
}

void SMListExtractAbelianSubgroups::takeControl( )
{
  if ( fastResult() ) {

    LogMessage msg( *this, theList );
    msg << Link(Chars( "Click here to see abelian subgroups in ")+
		Text( Name( theList ) ),
		"SGEnumerator",
		theList.lic().getListOfAbelian() );
    msg.send();
    
    LogMessage msg1( *this, theList );
    msg1 << Link(Chars( "Click here to see non-abelian subgroups in ")+
		 Text( Name( theList ) ),
		 "SGEnumerator",
		theList.lic().getListOfNonAbelian() );
    msg1.send();   

  } else {
    if ( theList.lic().IsAbelianChecked() ) {
      SMList<Subgroup>* smo = 
	new  SMList<Subgroup>(theList.getGroup(),
			  checker->getAbelianData(),
			  Text("Abelian subgroups from") + Name(theList) 
			  ); 
      
      ListOf<OID> dependencies( smo->getGroup() );
      CheckinMessage( *smo, "", dependencies ).send();
      
      LogMessage msg( *smo );
      msg << Link( Chars("Click here to see the contents of ") + 
		   Text( Name( *smo) ),
		   "SGEnumerator",
		   smo->getData().getDataFileName() 
		   );
      msg << ".";
      msg.send();
      adminTerminate();
      return;
    }
  }
}

void SMListExtractAbelianSubgroups::viewStructure(ostream& ostr) const
{
  EnumertatorProblemView pv(oid(),"Extract abelian subgroups from a list",
			    Text("Extract abelian subgroups from ") +
			    Name( theList ) ,
			    helpID("SMListExtractAbelianSubgroups",
				   theList.getGroup()),
			    Text("abel sgs")
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
  if ( ! theList.lic().IsAbelianChecked() )
    pv.addProblem( Subproblem("Sp1",checker.arcSlotID(),
			      Text("Extract abelian subgroups"),40));
  
  submit(ostr, pv);

}



// -------------------------- SMListSGCentralARCer ---------------------- //
 

SMListSGCentralARCer::SMListSGCentralARCer( ComputationManager& boss )
  : ARCer( boss ), theList( NULL ), theIterator( NULL ), 
  tCounter( 1 ), ntCounter( 1 )
{ } 

void SMListSGCentralARCer::setArguments( SMList<Subgroup>* l )
{
  if ( theIterator )
    delete theIterator;
  theIterator  =  new SMListIterator<Subgroup>(*l);
  theList = l;
}

void SMListSGCentralARCer::runComputation( ) 
{

  centralData.setWriteMode();
  int bLength = theList->lic().numberOfElements();
  
  for ( int counter = 0; counter < bLength; counter ++ ){
    if ( theList->lic().getIsCentralStatus(counter) == dontknow ){
      
      // Make all commutators and check are they trivial or not 
      SetOf<Word> comms;
      VectorOf<Word> gens = theIterator->getData().generators();
      int gensLen = gens.length();
      FPGroup fpGroup = theList->getGroup().getFPGroup(); 
      int numOfGens = fpGroup.numberOfGenerators();

      for( int i = 0; i < gensLen; ++i ) 
	for( int j = 0; j < numOfGens; ++j ) {
	  Word w1 = gens[i];
	  Word w2 = Word(Generator(j+1));
	  comms |= (w1.inverse() * w2.inverse() * w1 * w2).freelyReduce();
	}
      
      SetOfWordsChecker theChecker( comms, theList->getGroup() );
      
      Trichotomy isTrivial = theChecker.isTrivial();
      
      theList->lic().setIsCentralStatus( counter, isTrivial,
					 theChecker.getAlgorithm() );
      if ( isTrivial == yes ) {
	centralData   << EnumWriteVectorOfWords( gens,
						 fpGroup )
		      << theChecker.getExplanation() 
		      << Chars("\n\n\n");
	tCounter++;
      } 
      if ( isTrivial == no ) {
	fileOfNonCentral << ntCounter << ": " 
			 << EnumWriteVectorOfWords( gens,
						    fpGroup )
			 << theChecker.getExplanation() 
			 << endl << endl <<endl;
	ntCounter++;
      }
    }
    theIterator->nextCell();
  }
  centralData.closeCurrentMode(); 
}

// ----------------------- SMListSGCentralComputation ----------------- //


SMListSGCentralComputation::
SMListSGCentralComputation( SMListExtractCentralSubgroups& sms )
  : theList( sms.getList() ),
    arcer( *this ),
    started( false )
{ 
  adminStart();
}

SMListSGCentralComputation::~SMListSGCentralComputation()
{

}

void SMListSGCentralComputation::takeControl( )
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

// --------------------- SMListExtractCentralSubgroups ----------------- //


SMListExtractCentralSubgroups::SMListExtractCentralSubgroups(SMList<Subgroup>& sml)
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

void SMListExtractCentralSubgroups::start( )
{

  if ( !fastResult() ) {
    LogMessage msg( *this, theList );
    msg << Link(Chars( "Click here to see central subgroups in ")+
		Text( Name( theList ) ),
		"SGEnumerator",
		checker->getCentralData().getDataFileName(),true );
    msg.send();
    
    LogMessage msg1( *this, theList );
    msg1 << Link(Chars( "Click here to see non-central subgroup in ")+
		 Text( Name( theList ) ),
		 "SGEnumerator",
		 checker->getFileOfNonCentral(),true );
    msg1.send();
  }
}

void SMListExtractCentralSubgroups::takeControl( )
{
  if ( fastResult() ) {

    LogMessage msg( *this, theList );
    msg << Link(Chars( "Click here to see central subgroups in ")+
		Text( Name( theList ) ),
		"SGEnumerator",
		theList.lic().getListOfCentral() );
    msg.send();
    
    LogMessage msg1( *this, theList );
    msg1 << Link(Chars( "Click here to see non-central subgroups in ")+
		 Text( Name( theList ) ),
		 "SGEnumerator",
		theList.lic().getListOfNonCentral() );
    msg1.send();   

  } else {
    if ( theList.lic().IsCentralChecked() ) {
      SMList<Subgroup>* smo = 
	new  SMList<Subgroup>(theList.getGroup(),
			  checker->getCentralData(),
			  Text("Central subgroups from") + Name(theList) 
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

void SMListExtractCentralSubgroups::viewStructure(ostream& ostr) const
{
  EnumertatorProblemView pv(oid(),"Extract central subgroups from a list",
			    Text("Extract central subgroups from ") +
			    Name( theList ) ,
			    helpID("SMListExtractCentralSubgroups",
				   theList.getGroup()),
			    Text("cent sgs")
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
			      Text("Extract central subgroups"),40));
  
  submit(ostr, pv);

}

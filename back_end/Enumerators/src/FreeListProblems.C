#include "FreeListProblems.h"
#include "OutMessages.h"
#include "SMFPGroup.h"
#include "viewStructure.h"
#include "FreeGroup.h"
#include "SMWord.h"
#include "SMList.h"
#include "SMListIterator.h"


// ---------------------- SMListProperPowerInFreeARCer ----------------- //

void SMListProperPowerInFreeARCer::setArguments( const SMList<Word>& l )
{
  theIterator  =  new SMListIterator<Word>( l );
  theGroup = l.getGroup().getFPGroup();
}
 
void SMListProperPowerInFreeARCer::runComputation( )
{
  int counter = 1;
  theData.setWriteMode();
  for ( theIterator->reset();!theIterator->EOL();theIterator->nextCell() ){
    Word w = theIterator->getData().cyclicallyReduce();
    if ( w.isProperPower() ) {
      theData << EnumWriteWord( w,theGroup) << Chars("\n\n");
      counter++;
    }
  }
  theData.closeCurrentMode();  
}

// --------------------- SMListExtractProperPowerInFree ----------------- //


SMListExtractProperPowerInFree::SMListExtractProperPowerInFree(SMList<Word>& sml)
  : SMListSupervisor( sml ),
    theList( sml ),
    arcer( *this )
{ 
  arcer.setArguments( sml );
}

void SMListExtractProperPowerInFree::start( )
{
  LogMessage msg( *this, theList );
  msg << Link( Chars("Click here to see proper powers from ") + 
	       Text( Name( theList ) ),
	       "WEnumerator",
	       arcer.getData().getDataFileName() ,
	       true
	       );
  msg << ".";
  msg.send();	
}

void SMListExtractProperPowerInFree::takeControl( )
{
  if ( freeARCs() > 0 ) {
    if ( arcer.takeControl() ){
      SMList<Word>* smo = 
	new  SMList<Word>(theList.getGroup(),
			  arcer.getData(),
			  Text("Proper powers from") + Name(theList) 
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

void SMListExtractProperPowerInFree::viewStructure(ostream& ostr) const
{
  EnumertatorProblemView pv(oid(),"Extract proper powers from a list",
  			    Text("Extract proper powers from ") +
  			    Name( theList ) ,
  			    helpID("SMListExtractProperPowerInFree",
  				   theList.getGroup()),
  			    Text("proper powers")
  			    );
  
  // RadioButton rRB1("ToFile","Extract to a file");
  //  RadioButton rRB2("ToList","Extract to another list");
  
  //  RadioButtonGroup extractRBG("ExtractTo");
  
  //  extractRBG.add(rRB1);
  //  extractRBG.add(rRB2);
  
  //  pv.addParameter(extractRBG);
  pv.addProblem( Subproblem("Sp1",ThisARCSlotID(),
  			    Text("For the use of enumerator"),40));
  
  submit(ostr, pv);
}



// ---------------------- SMListCommutatorsInFreeARCer ----------------- //

void SMListCommutatorsInFreeARCer::setArguments( const SMList<Word>& l )
{
  theIterator  =  new SMListIterator<Word>( l );
  theGroup = l.getGroup().getFreePreimage();
}
 
void SMListCommutatorsInFreeARCer::runComputation( )
{
  int counter = 1;
  theData.setWriteMode();
  for ( theIterator->reset();!theIterator->EOL();theIterator->nextCell() ){
    Word w = theIterator->getData().cyclicallyReduce();
    Word u,v;
    if (  theGroup.isCommutator( w, u, v ) ) {
      theData << EnumWriteWord( w,theGroup) << Chars("\n\n");
      counter++;
    }
  }
  theData.closeCurrentMode();
}




// --------------------- SMListExtractCommutatorsInFree ----------------- //


SMListExtractCommutatorsInFree::SMListExtractCommutatorsInFree(SMList<Word>& sml)
  : SMListSupervisor( sml ),
    theList( sml ),
    arcer( *this )
{ 
  arcer.setArguments( sml );
}

void SMListExtractCommutatorsInFree::start( )
{
  LogMessage msg( *this, theList );
  msg << Link( Chars("Click here to see commutators from ") + 
	       Text( Name( theList ) ),
	       "WEnumerator",
	       arcer.getData().getDataFileName() ,
	       true
	       );
  msg << ".";
  msg.send();	
}

void SMListExtractCommutatorsInFree::takeControl( )
{
  if ( freeARCs() > 0 ) {
    if ( arcer.takeControl() ){
      SMList<Word>* smo = 
	new  SMList<Word>(theList.getGroup(),
			  arcer.getData(),
			  Text("Commutators from") + Name(theList) 
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

void SMListExtractCommutatorsInFree::viewStructure(ostream& ostr) const
{
  EnumertatorProblemView pv(oid(),"Extract commutators from a list",
			    Text("Extract commutators from ") +
			    Name( theList ) ,
			    helpID("SMListExtractCommutatorsInFree",
				   theList.getGroup()),
			    Text("commut")
			    );
  
  //  RadioButton rRB1("ToFile","Extract to a file");
  //  RadioButton rRB2("ToList","Extract to another list");
  
  //  RadioButtonGroup extractRBG("ExtractTo");
  
  //  extractRBG.add(rRB1);
  //  extractRBG.add(rRB2);
  
  //  pv.addParameter(extractRBG);
  pv.addProblem( Subproblem("Sp1",ThisARCSlotID(),
			    Text("For the use of enumerator"),40));
  
  submit(ostr, pv);

}


// --------------------- SGListExtractNormalARCer ---------------------- //


void SGListExtractNormalARCer::setArguments( const SMList<Subgroup>& l)
{
  theIterator  =  new SMListIterator<Subgroup>(l);
  
  theGroup = l.getGroup().getFreePreimage();
}


void SGListExtractNormalARCer::runComputation( )
{
#if SAFETY > 0
  if( ! theIterator )
    error( "void SGListExtractNormalARCer::runComputation( ) : "
	   "fileIterator must be initialized first");
#endif

  int counter = 1;
  theData.setWriteMode();
  for (theIterator->reset();!theIterator->EOL();theIterator->nextCell()){
    SGofFreeGroup s( theGroup, theIterator->getData().generators() );
    if (  s.isNormal( ) ){
      VectorOf<Word> v = s.generators();
      theData  << EnumWriteVectorOfWords( v, theGroup ) << Chars("\n\n");
      counter ++;
    }
  }
  theData.closeCurrentMode();  
}


void SGListExtractNormalARCer::writeResults( ostream& out ) 
{
  out < theData;
}


void SGListExtractNormalARCer::readResults( istream& in )
{
  in > theData;
}


// ----------------------------- SGListExtractNormal -------------------- //


SGListExtractNormal::SGListExtractNormal(SMList<Subgroup>& sml)
  : Supervisor( true ),
    theList( sml ),
    arcer(*this),
    started( false )
{ 
  arcer.setArguments(sml);
}

void SGListExtractNormal::start( ) {              

  LogMessage msg( *this, theList );
  
  msg << Link(Chars( "Click here to see normal subgroups in ") +
	      Text( Name( theList ) ),
	      "SGEnumerator",
	      arcer.getData().getDataFileName(),true );
  
  msg.send();   
}

void SGListExtractNormal::takeControl( )
{
  if (freeARCs() > 0){
    if (arcer.takeControl()){
      SMList<Subgroup>* smo = 
	new  SMList<Subgroup>(theList.getGroup(),
			      arcer.getData(),
			      Text("Normal subgroups in ") + Name(theList)
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
    else
      usedOneARC();
  }
}



void SGListExtractNormal::viewStructure(ostream& ostr) const
{
  EnumertatorProblemView pv(oid(),"Extract normal subgroups from a list",
			    Text("Extract normal subgroups from ") +
			    Name( theList ) ,
			    helpID("SGListExtractNormal",
				   theList.getGroup()),
			    Text("norm sgs")
			    );
  
  //  pv.addParameter(EditInteger("minL","Minimal length ",0,1,1,30000));
  // pv.addParameter(EditInteger("maxL","Maximal length ",0,30000,1,30000));
  
  
  pv.addProblem( Subproblem("Sp1",ThisARCSlotID(),
			    Text("For the use of the problem"),40));
  
  //  RadioButton rRB1("ToFile","Extract to a file");
  // RadioButton rRB2("ToList","Extract to another list");
  //
  //  RadioButtonGroup extractRBG("ExtractTo");
  
  //  extractRBG.add(rRB1);
  //  extractRBG.add(rRB2);

  //  pv.addParameter(extractRBG);
 
  submit(ostr, pv);

}

// --------------------- SGListExtractMalnormalARCer ---------------------- //


void SGListExtractMalnormalARCer::setArguments( const SMList<Subgroup>& l)
{
  theIterator  =  new SMListIterator<Subgroup>(l);
  theGroup = l.getGroup().getFreePreimage();
}


void SGListExtractMalnormalARCer::runComputation( )
{
#if SAFETY > 0
  if( ! theIterator )
    error( "void SGListExtractMalnormalARCer::runComputation( ) : "
	   "fileIterator must be initialized first");
#endif
  int counter = 1;
  theData.setWriteMode();
 
  for (theIterator->reset();!theIterator->EOL();theIterator->nextCell()){
    SGofFreeGroup s( theGroup, theIterator->getData().generators() );
    Word conjugator;
    if (  s.isMalnormal( conjugator ) ){
      VectorOf<Word> v = s.generators();
      theData <<EnumWriteVectorOfWords( v, theGroup) << Chars("\n\n");
      counter ++;
    }
  }
  theData.closeCurrentMode(); 
}


void SGListExtractMalnormalARCer::writeResults( ostream& out ) 
{
  out < theData;
}


void SGListExtractMalnormalARCer::readResults( istream& in )
{
  in > theData;
}


// ----------------------------- SGListExtractMalnormal -------------------- //


SGListExtractMalnormal::SGListExtractMalnormal(SMList<Subgroup>& sml)
  : Supervisor( true ),
    theList( sml ),
    arcer(*this),
    started( false )
{ 
  arcer.setArguments(sml);
}

void SGListExtractMalnormal::start( ) {              
  LogMessage msg( *this, theList );
  
  msg << Link(Chars( "Click here to see malnormal subgroups in ") +
	      Text( Name( theList ) ),
	      "SGEnumerator",
	      arcer.getData().getDataFileName(),true );
  
  msg.send();   
}

void SGListExtractMalnormal::takeControl( )
{
  if (freeARCs() > 0){
    if (arcer.takeControl()){
      SMList<Subgroup>* smo = 
	new  SMList<Subgroup>(theList.getGroup(),
			  arcer.getData(),
			  Text("Malnormal subgroups from") + Name(theList) 
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
    else
      usedOneARC();
  }
}



void SGListExtractMalnormal::viewStructure(ostream& ostr) const
{
  EnumertatorProblemView pv(oid(),"Extract malnormal subgroups from a list",
			    Text("Extract malnormal subgroups from ") +
			    Name( theList ) ,
			    helpID("SGListExtractMalnormal",
				   theList.getGroup()),
			    Text("maln sgs")
			    );
  
  //  pv.addParameter(EditInteger("minL","Minimal length ",0,1,1,30000));
  // pv.addParameter(EditInteger("maxL","Maximal length ",0,30000,1,30000));
  
  
  pv.addProblem( Subproblem("Sp1",ThisARCSlotID(),
			    Text("For the use of the problem"),40));
  
  //  RadioButton rRB1("ToFile","Extract to a file");
  //  RadioButton rRB2("ToList","Extract to another list");
    
  //  RadioButtonGroup extractRBG("ExtractTo");
  
  //  extractRBG.add(rRB1);
  //    extractRBG.add(rRB2);
  
  //  pv.addParameter(extractRBG);
 
  submit(ostr, pv);

}

// --------------------- SGListExtractOfRankARCer -------------------- //


void SGListExtractOfRankARCer::setArguments( const SMList<Subgroup>& l)
{
  theIterator  =  new SMListIterator<Subgroup>(l);
  theGroup = &(l.getGroup());
}


void SGListExtractOfRankARCer::runComputation( )
{
#if SAFETY > 0
  if( ! theIterator )
    error( "void SGListExtractOfRankARCer::runComputation( ) : "
	   "fileIterator must be initialized first.");
#endif
  int counter = 1;
  theData.setWriteMode();

  for (theIterator->reset();!theIterator->EOL();theIterator->nextCell()){
    int rank;
    VectorOf<Word> v =  theIterator->getData().generators();

    if (theGroup->getCheckinType() == SMFPGroup::FREE )
      rank = indexInFreeGroup( v );
    else
      error("void SGListExtractOfRankARCer::runComputation( ) : "
	    "defined only for free groups.");

    if ( rank >= minRank && rank <= maxRank ){
      theData << EnumWriteVectorOfWords( v, theGroup->getFPGroup() ) 
	      << Chars("\nRank :") << Chars(rank)
	      << Chars("\n\n");
      counter ++;
    }
  }
  theData.closeCurrentMode(); 
  
}

int SGListExtractOfRankARCer::indexInFreeGroup( const VectorOf<Word>& g )const
{
  SGofFreeGroup s( theGroup->getFreePreimage(), g );
  return s.rank();
}

void SGListExtractOfRankARCer::writeResults( ostream& out ) 
{
  out < theData;
}


void SGListExtractOfRankARCer::readResults( istream& in )
{
  in > theData;
}


// ----------------------------- SGListExtractOfRank  -------------------- //


SGListExtractOfRank::SGListExtractOfRank(SMList<Subgroup>& sml)
  : Supervisor( true ),
    theList( sml ),
    arcer(*this),
    started( false )
{ 
  arcer.setArguments(sml);
}

void SGListExtractOfRank::start( ) {              

  int min = getParameters().getValue("minRank");
  int max = getParameters().getValue("maxRank");

  arcer.setMinimalRank(min);
  arcer.setMaximalRank(max);
  LogMessage msg( *this, theList );
  
  msg << Link(Chars( "Click here to see subgroups of rank from ") +
	      Chars(min) +Chars(" to ") +
	      Chars(max) + Chars(" in ") +
	      Text( Name( theList ) ),
	      "SGEnumerator",
	      arcer.getData().getDataFileName(),true );
  
  msg.send();        
}

void SGListExtractOfRank::takeControl( )
{
  if (freeARCs() > 0){
    int min = getParameters().getValue("minRank");
    int max = getParameters().getValue("maxRank");
    
    if (arcer.takeControl()){
      SMList<Subgroup>* smo = 
	new  SMList<Subgroup>(theList.getGroup(),
			      arcer.getData(),
			      Text("Subgroups of rank from ") +
			      Text(Chars(min)+Chars(" to ")+Chars(max)+
				   Chars(" in ")) + 
			      Name(theList)
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
    else
      usedOneARC();
  }
}



void SGListExtractOfRank::viewStructure(ostream& ostr) const
{
  EnumertatorProblemView pv(oid(),"Extract subgroups of a given rank from a list",
			    Text("Extract subgroups of a given rank from ") +
			    Name( theList ) ,
			    helpID("SGListExtractOfRank",
				   theList.getGroup()),
			    Text("rank sgs")
			    );
  
  pv.addParameter(EditInteger("minRank","Minimal rank ",0,0,0,30000));
  pv.addParameter(EditInteger("maxRank","Maximal rank ",0,100,0 ,30000));
  
  
  pv.addProblem( Subproblem("Sp1",ThisARCSlotID(),
			    Text("For the use of the problem"),40));
  
  // RadioButton rRB1("ToFile","Extract to a file");
  // RadioButton rRB2("ToList","Extract to another list");
    
  // RadioButtonGroup extractRBG("ExtractTo");
  
  //    extractRBG.add(rRB1);
  //  extractRBG.add(rRB2);
  
  //  pv.addParameter(extractRBG);
 
  submit(ostr, pv);

}

// --------------------- MapListExtractIAautoARCer ---------------------- //


void MapListExtractIAautoARCer::setArguments( const SMList<Map>& l)
{
  theIterator  =  new SMListIterator<Map>(l);
  theGroup = l.getGroup().getFreePreimage();
}


void MapListExtractIAautoARCer::runComputation( )
{
#if SAFETY > 0
  if( ! theIterator )
    error( "void MapListExtractIAautoARCer::runComputation( ) : "
	   "fileIterator must be initialized first");
#endif

  int counter = 1;
  theData.setWriteMode();
  
  for (theIterator->reset();!theIterator->EOL();theIterator->nextCell()){
    
    if (theGroup.isIAAutomorphism(theIterator->getData().generatingImages())){
      theData <<EnumWriteMap( theIterator->getData() ) << Chars("\n\n");
      counter ++;
    }
  }
  theData.closeCurrentMode();  
}


void MapListExtractIAautoARCer::writeResults( ostream& out ) 
{
  out < theData;
}


void MapListExtractIAautoARCer::readResults( istream& in )
{
  in > theData;
}


// ----------------------------- MapListExtractIAauto -------------------- //


MapListExtractIAauto::MapListExtractIAauto(SMList<Map>& sml)
  : Supervisor( true ),
    theList( sml ),
    arcer(*this),
    started( false )
{ 
  arcer.setArguments(sml);
}

void MapListExtractIAauto::start( ) {              

  LogMessage msg( *this, theList );
  
  msg << Link(Chars( "Click here to see IA-automorphisms in ") +
	      Text( Name( theList ) ),
	      "RandomAutomorphisms",
	      arcer.getData().getDataFileName(),true );
  
  msg.send();   
  
}

void MapListExtractIAauto::takeControl( )
{
  if (freeARCs() > 0){
    if (arcer.takeControl()){
      SMList<Map>* smo = 
	new  SMList<Map>(theList.getGroup(),
			      arcer.getData(),
			      Text("IA-automorphisms from ") + Name(theList)
			      ); 
      
      ListOf<OID> dependencies( smo->getGroup() );
      CheckinMessage( *smo, "", dependencies ).send();
      
      LogMessage msg( *smo );
      msg << Link( Chars("Click here to see the contents of ") + 
		   Text( Name( *smo) ),
		   "RandomAutomorphisms",
		   smo->getData().getDataFileName() 
		   );
      msg << ".";
      msg.send();	
      
      adminTerminate();
      return;
    }
    else
      usedOneARC();
  }
}



void MapListExtractIAauto::viewStructure(ostream& ostr) const
{
  EnumertatorProblemView pv(oid(),"Extract IA-automorphisms from a list",
			    Text("Extract IA-automorphisms from") +
			    Name( theList ) ,
			    helpID("MapListExtractIAauto",
				   theList.getGroup()),
			    Text("IA-auto")
			    );
  
  //  pv.addParameter(EditInteger("minL","Minimal length ",0,1,1,30000));
  // pv.addParameter(EditInteger("maxL","Maximal length ",0,30000,1,30000));
  
  
  pv.addProblem( Subproblem("Sp1",ThisARCSlotID(),
			    Text("For the use of the problem"),40));
  
  // RadioButton rRB1("ToFile","Extract to a file");
  // RadioButton rRB2("ToList","Extract to another list");
    
  //RadioButtonGroup extractRBG("ExtractTo");
  
  //   extractRBG.add(rRB1);
  //    extractRBG.add(rRB2);

  //  pv.addParameter(extractRBG);
 
  submit(ostr, pv);

}
// --------------------- MapListExtractInnerARCer ---------------------- //


void MapListExtractInnerARCer::setArguments( const SMList<Map>& l)
{
  theIterator  =  new SMListIterator<Map>(l);
  theGroup = l.getGroup().getFreePreimage();
}


void MapListExtractInnerARCer::runComputation( )
{
#if SAFETY > 0
  if( ! theIterator )
    error( "void MapListExtractIAautoARCer::runComputation( ) : "
	   "fileIterator must be initialized first");
#endif

  int counter = 1;
  theData.setWriteMode();
  
  for (theIterator->reset();!theIterator->EOL();theIterator->nextCell()){
    
    if (theGroup.isInnerAutomorphism(theIterator->getData().generatingImages())){
      theData <<EnumWriteMap( theIterator->getData() ) << Chars("\n\n");
      counter ++;
    }
  }
  
  theData.closeCurrentMode(); 
}


void MapListExtractInnerARCer::writeResults( ostream& out ) 
{
  out < theData;
}


void MapListExtractInnerARCer::readResults( istream& in )
{
  in > theData;
}


// ----------------------------- MapListExtractInner -------------------- //


MapListExtractInner::MapListExtractInner(SMList<Map>& sml)
  : Supervisor( true ),
    theList( sml ),
    arcer(*this),
    started( false )
{ 
  arcer.setArguments(sml);
}

void MapListExtractInner::start( ) {              
  LogMessage msg( *this, theList );
  
  msg << Link(Chars( "Click here to see inner automorphisms in ") +
	      Text( Name( theList ) ),
	      "RandomAutomorphisms",
	      arcer.getData().getDataFileName(),true );
  
  msg.send();   
}

void MapListExtractInner::takeControl( )
{
  if (freeARCs() > 0){
    if (arcer.takeControl()){
      SMList<Map>* smo = 
	new  SMList<Map>(theList.getGroup(),
			      arcer.getData(),
			      Text("Inner automorphisms from ") + Name(theList)
			      ); 
      
      ListOf<OID> dependencies( smo->getGroup() );
      CheckinMessage( *smo, "", dependencies ).send();
      
      LogMessage msg( *smo );
      msg << Link( Chars("Click here to see the contents of ") + 
		   Text( Name( *smo) ),
		   "RandomAutomorphisms",
		   smo->getData().getDataFileName() 
		   );
      msg << ".";
      msg.send();	
      
      adminTerminate();
      return;
    }
    else
      usedOneARC();
  }
}



void MapListExtractInner::viewStructure(ostream& ostr) const
{
  EnumertatorProblemView pv(oid(),"Extract inner automorphisms from a list",
			    Text("Extract inner automorphisms from") +
			    Name( theList ) ,
			    helpID("MapListExtractInner",
				   theList.getGroup()),
			    Text("inner auto")
			    );
  
  //  pv.addParameter(EditInteger("minL","Minimal length ",0,1,1,30000));
  // pv.addParameter(EditInteger("maxL","Maximal length ",0,30000,1,30000));
  
  
  pv.addProblem( Subproblem("Sp1",ThisARCSlotID(),
			    Text("For the use of the problem"),40));
  
  //RadioButton rRB1("ToFile","Extract to a file");
  //RadioButton rRB2("ToList","Extract to another list");
    
  // RadioButtonGroup extractRBG("ExtractTo");
  
  //  extractRBG.add(rRB1);
  //  extractRBG.add(rRB2);

  //  pv.addParameter(extractRBG);
 
  submit(ostr, pv);

}

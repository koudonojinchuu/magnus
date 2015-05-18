
// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes
//
// Principal Author: Eugene Paderin
//
// Status: in progress
//
// Revision History:
//

#include "SMSubgroup.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "SGOfNilpotentGroup.h"
#include "NGSubgroupProblems.h"
#include "conversions.h"
// ----------------- SGOfNGinitPreimageARCer ------------------------ //


void SGOfNGinitPreimageARCer::runComputation( )
{
  theSNG->initPreimage(  );
}


void SGOfNGinitPreimageARCer::setArguments( const NilpotentGroup& NG ,
						   const VectorOf<Word>& gens)
{
  theSNG = new SGOfNilpotentGroup( NG, gens );
}


void SGOfNGinitPreimageARCer::writeResults( ostream& out ) 
{
  out < *theSNG;
}

void SGOfNGinitPreimageARCer::readResults( istream& in )
{
  in > *theSNG;
}
const SGOfNilpotentGroup& SGOfNGinitPreimageARCer::getSubgroup(  ) const
{
#if SAFETY > 0
  if( theSNG == 0 )
    error( "const SGOfNilpotentGroup& SGOfNGinitPreimageARCer::getSubgroup(  )"
	   " const : Subgroup was not initialized");
#endif
  return *theSNG;
}

// ------------------- SGOfNGinitPreimageProblem --------------------- //


SGOfNGinitPreimageProblem::SGOfNGinitPreimageProblem(SCM& scm)
  : theSCM( scm ),   
    theGIC(scm.getSubgroup().getParent().gic),
    arcer( *this )
{
  if (scm.getSubgroup().sic.haveSGOfNGPreimageInitialized())
    resultIsFast();
  if (theGIC.haveNilpotentGroupInited()){
    arcer.setArguments( theGIC.getNilpotentGroupInited() ,
			theSCM.getSubgroup().getSubgroup().generators());
  }else{
    VectorOf<Word> rel(theSCM.getSubgroup().getParent().
		       getFPGroup().getRelators().cardinality());
    SetIterator<Word> I(theSCM.getSubgroup().getParent().
			getFPGroup().getRelators());
    int k = 0;
    for( I.reset(); !I.done(); I.next() ){
      rel[k] = I.value();
      k++;
    }
    NilpotentGroup ng(theSCM.getSubgroup().getParent().
		      getFPGroup().namesOfGenerators(),
		      theGIC.getNilpotentcyClass(),rel);
    arcer.setArguments( ng,theSCM.getSubgroup().getSubgroup().generators());    
  }
  adminStart();
}


void SGOfNGinitPreimageProblem::takeControl( )
{
  if (freeARCs()>0){
    if (fastResult()){
      adminTerminate();
      return;
    }
    if(arcer.takeControl()){
      theSCM.getSubgroup().sic.
	putHaveSGOfNGPreimageInitialized(arcer.getSubgroup());
      adminTerminate();
      return;
    }
    usedOneARC();
  }
}

// ----------------- SGOfNGinitializeARCer ------------------------ //


void SGOfNGinitializeARCer::runComputation( )
{
  theSNG->initBasis(  );
}


void SGOfNGinitializeARCer::setArguments( const NilpotentGroup& NG ,
						   const VectorOf<Word>& gens)
{
  theSNG = new SGOfNilpotentGroup( NG, gens );
}


void SGOfNGinitializeARCer::writeResults( ostream& out ) 
{
  out < *theSNG;
}

void SGOfNGinitializeARCer::readResults( istream& in )
{
  in > *theSNG;
}
const SGOfNilpotentGroup& SGOfNGinitializeARCer::getSubgroup(  ) const
{
#if SAFETY > 0
  if( theSNG == 0 )
    error( "const SGOfNilpotentGroup& SGOfNGinitializeARCer::getSubgroup(  )"
	   " const : Subgroup was not initialized");
#endif
  return *theSNG;
}

// ------------------- SGOfNGinitializeProblem --------------------- //


SGOfNGinitializeProblem::SGOfNGinitializeProblem(SCM& scm)
  : theSCM( scm ),   
    theGIC(scm.getSubgroup().getParent().gic),
    haveParentInited(false),
    arcer( *this )
{
  if (scm.getSubgroup().sic.haveSGOfNGinitialized())
    resultIsFast();
  if (theGIC.haveNilpotentGroupInited()){
    haveParentInited = true;
    arcer.setArguments( theGIC.getNilpotentGroupInited() ,
			theSCM.getSubgroup().getSubgroup().generators());
  }else{
    VectorOf<Word> rel(theSCM.getSubgroup().getParent().
		       getFPGroup().getRelators().cardinality());
    SetIterator<Word> I(theSCM.getSubgroup().getParent().
			getFPGroup().getRelators());
    int k = 0;
    for( I.reset(); !I.done(); I.next() ){
      rel[k] = I.value();
      k++;
    }
    NilpotentGroup ng(theSCM.getSubgroup().getParent().
		      getFPGroup().namesOfGenerators(),
		      theGIC.getNilpotentcyClass(),rel);
    arcer.setArguments( ng,theSCM.getSubgroup().getSubgroup().generators());    
  }
  adminStart();
}


void SGOfNGinitializeProblem::takeControl( )
{
  if (freeARCs()>0){
    if (fastResult()){
      adminTerminate();
      return;
    }
    if(arcer.takeControl()){
      theSCM.getSubgroup().sic.putHaveSGOfNGinitialized(arcer.getSubgroup());
      if (!haveParentInited)
	theGIC.putHaveNilpotentGroupInited(arcer.getSubgroup().parentGroup(),
					  Chars());
      adminTerminate();
      return;
    }
    usedOneARC();
  }
}
//--------------------------------------------------------------------//
//-------------------- SGOfNGcomputeBasisProblem ---------------------//
//--------------------------------------------------------------------//


SGOfNGcomputeBasisProblem::SGOfNGcomputeBasisProblem(SMSubgroup& sg)
  : Supervisor( true ),
    theSubgroup( sg ),
    initializeSGOfNG( *this , sg.scm().initializeSGOfNG),
    computeBasis(*this,sg.getParent().gcm().computeBasis )
{
  if( theSubgroup.sic.haveSGOfNGinitialized() ) resultIsFast();
}


void SGOfNGcomputeBasisProblem::takeControl( )
{
  if ( theSubgroup.sic.haveSGOfNGinitialized()){
    File file;
    LogMessage msg( *this, theSubgroup);
    theSubgroup.sic.getSGOfNilpotentGroup().printBasis(file);
    file << end;
    Chars basisMsg;
    //    if (theSubgroup.getParent().gic.isFreeNilpotent() == yes)
    //      basisMsg = "Click here to view the Malcev basis for ";
    //else
      basisMsg = "Click here to view the basis for ";
     
    msg << Link( Text(basisMsg) + Name(theSubgroup) + ".",
		 "NGSubgroupBasis",
		 file.getFileName());
    msg.send();
    adminTerminate();
    return;
  }
}
void SGOfNGcomputeBasisProblem::start( ) { }


void SGOfNGcomputeBasisProblem::terminate( ) { }

void SGOfNGcomputeBasisProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Compute a basis"),
		  Text("Compute a basis for") + Name(theSubgroup),
		  helpID("SGOfNGcomputeBasisProblem", 
			 theSubgroup.getParent()),
		  "basis"
		  );
  pv.startItemGroup();
  if (!theSubgroup.getParent().gic.haveNilpotentGroupInited())
    pv.add(Text("Compute a basis of ")
	  + Name(theSubgroup.getParent()),
	   computeBasis.arcSlotID(), 100);
  pv.add(Text("For use of this problem"),initializeSGOfNG.arcSlotID(), 100);
  pv.done();
}

// ------------------- SGOfNGDecomposeWordARCer --------------------- //


void SGOfNGDecomposeWordARCer::runComputation( )
{
  isContain = theSNG->decompose(theWord,theDecomposition);
}


void SGOfNGDecomposeWordARCer::setArguments( const SGOfNilpotentGroup& SNG,
					     const PolyWord& w )
{
  theSNG = new SGOfNilpotentGroup(SNG);
  theWord = w;
}


void SGOfNGDecomposeWordARCer::writeResults( ostream& out ) 
{
  out < isContain < theDecomposition ;
}

void SGOfNGDecomposeWordARCer::readResults( istream& in )
{
  in > isContain > theDecomposition ;
}

// ---------------------- SGOfNGDecomposeWordProblem ---------------------- //


SGOfNGDecomposeWordProblem::SGOfNGDecomposeWordProblem(SMSubgroup& sg,const SMWord& word)
  : Supervisor( true ),
    theSubgroup( sg ),
    theWord( word ),
    theSIC( sg.sic ),
    started(false),
    arcer(*this),
    initializeSGOfNG( *this, sg.scm().initializeSGOfNG ),
    computeBasis( *this, sg.getParent().gcm().computeBasis ),
    ngDecomposeWord( *this, word.wcm().ngDecomposeWord )
{ 

}


void SGOfNGDecomposeWordProblem::takeControl( )
{
  if (freeARCs() > 0){
    if (theSIC.haveSGOfNGPreimageInitialized() 
	&& theWord.wic.haveCollectedForm()){
      if (!started){
	arcer.setArguments(theSIC.getSGOfNilpotentGroup(),
			   theWord.wic.getCollectedForm());
	started = true;
      }
      if (arcer.takeControl()){
	LogMessage msg( *this,theWord );
	if (arcer.contains()){
	  msg << "Decomposition of " << Name( theWord ) << 
	    " in terms of basis: " << 
	    theSIC.getSGOfNilpotentGroup().
	    asDecomposition(arcer.decomposition());
	}
	else {
	  msg << Name ( theWord ) << " is not in " << Name (theSubgroup);
	}
	
	msg.send();
	adminTerminate();
	return;
      }
    }
    usedOneARC();
  }
}


void SGOfNGDecomposeWordProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Decompose word in terms of a basis"),
		  Text("Decompose ") + Name( theWord ) + 
		  Text("in terms of a basis of ")+
		  Name(theSubgroup),
		  helpID("SGOfNGDecomposeWordProblem", theWord.getParent()),
		  Text("decomp")
		  );
  
  pv.startItemGroup();

  if (!theWord.getParent().gic.haveNilpotentGroupInited())
    pv.add( Text("Compute a basis of ")
	    + Name( theWord.getParent() ),
	    computeBasis.arcSlotID(),
	    100
	    );
  
  if (!theWord.wic.haveCollectedForm())
    pv.add( Text("Decompose ")
	    + Name( theWord ) + " in " + Name (theWord.getParent()),
	    ngDecomposeWord.arcSlotID(),
	    100
	    );

  if (!theSIC.haveSGOfNGinitialized())
    pv.add( Text("Compute a basis of ")
	    + Name( theSubgroup ),
	    initializeSGOfNG.arcSlotID(),
	    100
	    );  
  
  pv.add( Text("For the use of this problem"), ThisARCSlotID(), 50 );
  pv.done();
}
// ------------------- SGOfNGWordContainARCer --------------------- //


void SGOfNGWordContainARCer::runComputation( )
{
  SetIterator<Word> I( theTestWords );

  for( I.reset(); !I.done(); I.next() ) {
    if( !theSubgroup->contains(I.value()) ) {
      theAnswer = false;
      return;
    }
  }

  theAnswer = true;
}


void SGOfNGWordContainARCer::setArguments( const SGOfNilpotentGroup& subgroup,
					   const SetOf<Word>& testWords )
{
  theSubgroup = new SGOfNilpotentGroup(subgroup);
  theTestWords = testWords;
}


void SGOfNGWordContainARCer::writeResults( ostream& out ) 
{
  out < theAnswer;
}

void SGOfNGWordContainARCer::readResults( istream& in )
{
  in > theAnswer;
}

SGOfNGWordContain::SGOfNGWordContain( SGOfNGWordContainProblem& boss )
  : 
    theSubgroup( boss.getSubgroup() ),
    theTestWords( boss.getTestWord().getWord() ),
    arcer(*this), arcerStarted(false),
    theAnswer( dontknow )
{ 
  adminStart();
}

Trichotomy SGOfNGWordContain::answer( ) const 
{ 
  return theAnswer; 
}

void SGOfNGWordContain::takeControl( )
{
  if( freeARCs() > 0 ) {

    if( theSubgroup.sic.haveSGOfNGPreimageInitialized() ) {

      if( !arcerStarted ) {
	arcer.setArguments( theSubgroup.sic.getSGOfNilpotentGroup(),
			    theTestWords );
	arcerStarted = true;
      }

      if( arcer.takeControl() ) {
	theAnswer = arcer.answer();
	adminTerminate();
	return;
      }

    }
    usedOneARC();
  }
}

// ---------------------- SGOfNGWordContainProblem ----------------------- //


SGOfNGWordContainProblem::SGOfNGWordContainProblem(SMSubgroup& sg,const SMWord& word)
  : Supervisor( true ),
    theSubgroup( sg ),
    theTestWord( word ),
    initPreimageSGOfNG( *this, sg.scm().initPreimageSGOfNG ),
    sgContain( *this )
{ 
}


void SGOfNGWordContainProblem::takeControl( )
{
  if( sgContain->state() == ComputationManager::TERMINATED ) {

    bool answer = sgContain->answer();

    LogMessage msg( *this, theTestWord );
    msg << Name( theTestWord ) << ( answer ? " is " : " is not " )
	<< " in " << Name( theSubgroup )<< ".";
    msg.send();

    adminTerminate();
  }
}


void SGOfNGWordContainProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Containment problem"),
		  Text("Determine whether") + Name(theTestWord)
		    + Text("lies in") + Name(theSubgroup),
		  helpID("SGOfNGWordContainProblem", 
			 theSubgroup.getParent()),
		  Text(Name(theTestWord)) + Text("is in")
		    + Name(theSubgroup) + Text("?")
		  );
  
  pv.startItemGroup();

  pv.add( Text("For the use of this problem"), 
	  sgContain.arcSlotID(), 
	  50 
	  );

  if( !theSubgroup.sic.haveSGOfNGPreimageInitialized() ) {
    pv.add( Text("Compute a basis of the free preimage of ")+Name(theSubgroup),
	    initPreimageSGOfNG.arcSlotID(),
	    100
	    );  
  }

  pv.done();
}

// ----------------- SGOfNGcontainSubgroupARCer ------------------------ //


void SGOfNGcontainSubgroupARCer::runComputation( )
{
   answer = theSubgroup1->contains( theSubgroup2 );
}


void SGOfNGcontainSubgroupARCer::setArguments( const SGOfNilpotentGroup& SNG1 ,
						const VectorOf<Word>& SNG2)
{
  theSubgroup1 = new SGOfNilpotentGroup( SNG1 );
  theSubgroup2 =  SNG2;
}


void SGOfNGcontainSubgroupARCer::writeResults( ostream& out ) 
{
  out < answer;
}

void SGOfNGcontainSubgroupARCer::readResults( istream& in )
{
  in > answer;
}
bool SGOfNGcontainSubgroupARCer::getAnswer(  ) const
{
#if SAFETY > 0
  if( theSubgroup1 == 0 )
    error( "bool SGOfNGcontainSubgroupARCer::getAnswer(  ) const :"
	   "Subgroups was not initializad");
#endif
  return answer;
}

//---------------------------------------------------------------------------//
//-------------------- SGOfNGcontainSubgroupProblem ------------------------//
//---------------------------------------------------------------------------//

SGOfNGcontainSubgroupProblem::SGOfNGcontainSubgroupProblem(SMSubgroup& sg1,const SMSubgroup& sg2)
  : Supervisor( true ),
    theSubgroup1( sg1 ),
    theSubgroup2( sg2 ),
    initPreimageSGOfNG( *this , sg1.scm().initPreimageSGOfNG),
    started(false),
    arcer(*this)
{
  
}

void SGOfNGcontainSubgroupProblem::takeControl( )
{
  if (freeARCs()>0){
    
    if ( theSubgroup1.sic.haveSGOfNGPreimageInitialized()){
      if (!started){
	arcer.setArguments( theSubgroup1.sic.getSGOfNilpotentGroup(),
			    theSubgroup2.getSubgroup().generators());
	started = true;
      }
      if (arcer.takeControl()){
	LogMessage msg( *this, theSubgroup1,theSubgroup2 );
	msg << "The subgroup " << Name( theSubgroup1 );
	
	if( arcer.getAnswer() ) msg << " contains ";
	else msg << " does not contain ";
	
	msg << Name( theSubgroup2 ) << ".";
	msg.send();

	adminTerminate();
	return;
      }
    }
    usedOneARC();
  }
}
void SGOfNGcontainSubgroupProblem::start( ) { }


void SGOfNGcontainSubgroupProblem::terminate( ) { }

void SGOfNGcontainSubgroupProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Subgroup containment problem"),
		  Text("Determine whether") + Name(theSubgroup1 )
		  + Text("contains") + Name( theSubgroup2 ),
		  helpID("SGOfNGcontainSubgroupProblem", 
			 theSubgroup1.getParent()),
		  Text(Name(theSubgroup2)) + Text("is in") +
		  Name(theSubgroup1) + Text("?")
		  );
  
  
  pv.startItemGroup();
  pv.add( Text("Does") + Name(theSubgroup1 )
	  + Text("contain") + Name( theSubgroup2 ), ThisARCSlotID(), 100 );
  
  if( ! theSubgroup1.sic.haveSGOfNGPreimageInitialized() )
    pv.add( Text("Compute a basis of the free preimage of ")+ 
	    Name(theSubgroup1 ),
	    initPreimageSGOfNG.arcSlotID(), 100 );
  
  pv.done();
}
//---------------------------------------------------------------------------//
//-------------------- SGOfNGequalSubgroupProblem ------------------------//
//---------------------------------------------------------------------------//

SGOfNGequalSubgroupProblem::SGOfNGequalSubgroupProblem(SMSubgroup& sg1, SMSubgroup& sg2)
  : Supervisor( true ),
    theSubgroup1( sg1 ),
    theSubgroup2( sg2 ),
    initPreimageSGOfNG1( *this , sg1.scm().initPreimageSGOfNG),
    started1(false),
    arcer1(*this),
    initPreimageSGOfNG2( *this , sg2.scm().initPreimageSGOfNG),
    started2(false),
    arcer2(*this)
{
  
}


void SGOfNGequalSubgroupProblem::takeControl( )
{
  if (freeARCs()>0){
    Trichotomy s1conts2 = dontknow;
    Trichotomy s2conts1 = dontknow;

    if ( theSubgroup1.sic.haveSGOfNGPreimageInitialized()){
      if (!started1){
	arcer1.setArguments( theSubgroup1.sic.getSGOfNilpotentGroup(),
			    theSubgroup2.getSubgroup().generators());
	started1 = true;
      }
      if (s1conts2 == dontknow)
	if (arcer1.takeControl()){
	  s1conts2 = arcer1.getAnswer();
	}
    }
    if ( theSubgroup2.sic.haveSGOfNGPreimageInitialized()){
      if (!started2){
	arcer2.setArguments( theSubgroup2.sic.getSGOfNilpotentGroup(),
			    theSubgroup1.getSubgroup().generators());
	started2 = true;
      }
      if (s2conts1 == dontknow)
	if (arcer2.takeControl()){
	  s2conts1 = arcer2.getAnswer();
	}
    }	
    Trichotomy areEqual = dontknow;
    if (s1conts2 == no || s2conts1 == no)
      areEqual = no;
    if (s1conts2 == yes && s2conts1 == yes)
      areEqual = yes;
    if (areEqual != dontknow){
      LogMessage msg( *this, theSubgroup1, theSubgroup2 );
      msg << "The subgroup " << Name( theSubgroup1 ) << " is ";
      if( areEqual == no ) msg << "not ";
      msg << "equal to " << Name( theSubgroup2 ) << ".";
      msg.send();
      
      adminTerminate();
      return;
    }
    
    usedOneARC();
  }
}
void SGOfNGequalSubgroupProblem::start( ) { }


void SGOfNGequalSubgroupProblem::terminate( ) { }

void SGOfNGequalSubgroupProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Subgroup equality problem"),
		  Text("Determine whether") + Name(theSubgroup1 )
		  + Text("is equal to") + Name( theSubgroup2 ),
		  helpID("SGOfNGequalSubgroupProblem", 
			 theSubgroup1.getParent()),
		  Text(Name(theSubgroup1)) + Text("=") +
		  Name(theSubgroup2) + Text("?")
		  );
  
  pv.startItemGroup();

  pv.add( Text("Does") + Name(theSubgroup1 )
	  + Text("equal to") + Name( theSubgroup2 ), ThisARCSlotID(), 100 );
  
  if( ! theSubgroup1.sic.haveSGOfNGPreimageInitialized() )
    pv.add( Text("Compute a basis of the free preimage of ")+ 
	    Name(theSubgroup1 ),
	    initPreimageSGOfNG1.arcSlotID(), 100 );
  
  if( ! theSubgroup2.sic.haveSGOfNGPreimageInitialized() )
    pv.add( Text("Compute a basis of the free preimage of ")+ 
	    Name(theSubgroup2 ),
	    initPreimageSGOfNG2.arcSlotID(), 100 );  
  pv.done();
}
// ----------------- SGOfNGindexARCer ------------------------ //


void SGOfNGindexARCer::runComputation( )
{
  index = theSNG->index(  );
}


void SGOfNGindexARCer::setArguments(const SGOfNilpotentGroup& sfng)
{
  theSNG = new SGOfNilpotentGroup( sfng );
}


void SGOfNGindexARCer::writeResults( ostream& out ) 
{
  out < index;
}

void SGOfNGindexARCer::readResults( istream& in )
{
  in > index;
}
int SGOfNGindexARCer::getIndex(  ) const
{
#if SAFETY > 0
  if( theSNG == 0 )
    error( "int SGOfNGindexARCer::getIndex(  ) const :"
	   "Subgroup was not initialized");
#endif
  return index;
}

//---------------------------------------------------------------------------//
//-------------------- SGOfNGindexProblem ----------------------------------//
//---------------------------------------------------------------------------//


SGOfNGindexProblem::SGOfNGindexProblem(SMSubgroup& sg)
  : Supervisor( true ),
    theSubgroup( sg ),
    initPreimageSGOfNG( *this , sg.scm().initPreimageSGOfNG),
    started(false),
    arcer( *this )
{
  if (theSubgroup.sic.haveIndex()) resultIsFast();
}


void SGOfNGindexProblem::takeControl( )
{
  
  if (!fastResult())
    if (freeARCs()>0){
      if ( theSubgroup.sic.haveSGOfNGPreimageInitialized()){
	if (!started){
	  arcer.setArguments( theSubgroup.sic.getSGOfNilpotentGroup());
	  started = true;
	}
	if (arcer.takeControl())
	  theSubgroup.sic.putIndex(arcer.getIndex());
      }
      usedOneARC();
    }
  if (theSubgroup.sic.haveIndex()){
    LogMessage msg( *this, theSubgroup );
    if (!theSubgroup.sic.index())
      msg << "The index of " << Name(theSubgroup) << " in " << 
	Name(theSubgroup.getParent())<<" is infinite.";
    else
      msg << "The index of " <<  Name(theSubgroup ) << " in " <<
	Name(theSubgroup.getParent()) << " is " << theSubgroup.sic.index();
    msg.send();
    adminTerminate();    
  }
}
void SGOfNGindexProblem::start( ) { }


void SGOfNGindexProblem::terminate( ) { }

void SGOfNGindexProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Index of subgroup"),
		  Text("Compute the index of") + Name(theSubgroup) +
		  Text(" in ")+Name(theSubgroup.getParent()),
		  helpID("SGOfNGindexProblem", 
			 theSubgroup.getParent()),
		  "Index"
		  );
  pv.startItemGroup();
  pv.add(Text("Compute the index"),ThisARCSlotID(), 100);
  if( ! theSubgroup.sic.haveSGOfNGPreimageInitialized() )
    pv.add( Text("Compute a basis of the free preimage of ")+ 
	    Name(theSubgroup ),
	    initPreimageSGOfNG.arcSlotID(), 100 );

  pv.done();
}

//---------------------------------------------------------------------------//
//-------------------- SGOfNGhirschNumberProblem ----------------------------//
//---------------------------------------------------------------------------//


SGOfNGhirschNumberProblem::SGOfNGhirschNumberProblem(SMSubgroup& sg)
  : Supervisor( true ),
    theSubgroup( sg ),
    initializeSGOfNG( *this , sg.scm().initializeSGOfNG),
    started(false)
{
  if (theSubgroup.sic.haveSGOfNGinitialized()) resultIsFast();
}


void SGOfNGhirschNumberProblem::takeControl( )
{
  
  if (theSubgroup.sic.haveSGOfNGinitialized()){
    LogMessage msg( *this, theSubgroup );
    msg << Text("The Hirsch number of the subgroup ") << Name(theSubgroup) 
	<<" is "<<theSubgroup.sic.getSGOfNilpotentGroup().theHirschNumber()<<".";
    msg.send();
    adminTerminate();    
  }
}
void SGOfNGhirschNumberProblem::start( ) { }


void SGOfNGhirschNumberProblem::terminate( ) { }

void SGOfNGhirschNumberProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Compute the Hirsch number"),
		  Text("Compute the Hirsch number of") + Name(theSubgroup),
		  helpID("SGOfNGhirschNumberProblem",theSubgroup.getParent()),
		  Text("Hirsch num.") + Name(theSubgroup )
		  );
  pv.startItemGroup();
 
  if( ! theSubgroup.sic.haveSGOfNGinitialized() )
    pv.add( Text("Compute a basis of ")+ Name(theSubgroup ),
	    initializeSGOfNG.arcSlotID(), 100 );

  pv.done();
}

// ----------------- SGOfNGisNormalARCer ------------------------ //


void SGOfNGisNormalARCer::runComputation( )
{
  theAnswer = theSNG->isNormal();
}


void SGOfNGisNormalARCer::setArguments(const SGOfNilpotentGroup& sfng)
{
  theSNG = new SGOfNilpotentGroup( sfng );
}


void SGOfNGisNormalARCer::writeResults( ostream& out ) 
{
  out < theAnswer;
}

void SGOfNGisNormalARCer::readResults( istream& in )
{
  in > theAnswer;
}
bool SGOfNGisNormalARCer::answer(  ) const
{
#if SAFETY > 0
  if( theSNG == 0 )
    error( "int SGOfNGindexARCer::getIndex(  ) const :"
	   "Subgroup was not initialized");
#endif
  return theAnswer;
}

//---------------------------------------------------------------------------//
//-------------------- SGOfNGisNormalProblem --------------------------------//
//---------------------------------------------------------------------------//


SGOfNGisNormalProblem::SGOfNGisNormalProblem(SMSubgroup& sg)
  : Supervisor( true ),
    theSubgroup( sg ),
    initPreimageSGOfNG(*this,sg.scm().initPreimageSGOfNG ),
    started(false),
    arcer( *this )
{
  if (theSubgroup.sic.isNormal()!=dontknow) resultIsFast();
}


void SGOfNGisNormalProblem::takeControl( )
{
  
  if (!fastResult())
    if (freeARCs()>0){
      if ( theSubgroup.sic.haveSGOfNGPreimageInitialized ()){
	if (!started){
	  arcer.setArguments( theSubgroup.sic.getSGOfNilpotentGroup());
	  started = true;
	}
	if (arcer.takeControl())
	  theSubgroup.sic.putIsNormal(arcer.answer());
      }
      usedOneARC();
    }
  if (theSubgroup.sic.isNormal()!=dontknow){
    LogMessage msg( *this, theSubgroup );
    msg << Text( Name(theSubgroup) ) + "is ";
    if(!theSubgroup.sic.isNormal()) msg << "not ";
    msg << "normal in " << Name( theSubgroup.getParent() ) << ".";
    msg.send();
    adminTerminate();    
  }
}
void SGOfNGisNormalProblem::start( ) { }


void SGOfNGisNormalProblem::terminate( ) { }

void SGOfNGisNormalProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Is") + Name(theSubgroup) 
		  + "normal in" + Name( Parent(theSubgroup) ) + "?",
		  Text("Is") + Name(theSubgroup) 
		  + "normal in" + Name( Parent(theSubgroup) ) + "?",
		  helpID("SGOfNGisNormalProblem", 
			 theSubgroup.getParent()),
		  "Is normal?"
		  );
  
  pv.startItemGroup();
  pv.add( Text("Compute is normal"), ThisARCSlotID(), 100 );

  if( ! theSubgroup.sic.haveSGOfNGPreimageInitialized () )
    pv.add( Text("Compute a basis of the free preimage of ")+ 
	    Name( theSubgroup ), 
	    initPreimageSGOfNG.arcSlotID(), 100 );

  pv.done();
}
// ----------------- SGOfNGcomputeNClassARCer ------------------------ //


void SGOfNGcomputeNClassARCer::runComputation( )
{
  theClass = theSNG->subgroupClass(  );
}


void SGOfNGcomputeNClassARCer::setArguments( const NilpotentGroup& NG ,
						   const VectorOf<Word>& gens)
{
  theSNG = new SGOfNilpotentGroup( NG, gens );
}


void SGOfNGcomputeNClassARCer::writeResults( ostream& out ) 
{
  out < theClass;
}

void SGOfNGcomputeNClassARCer::readResults( istream& in )
{
  in > theClass;
}
int SGOfNGcomputeNClassARCer::getClass(  ) const
{
#if SAFETY > 0
  if( theSNG == 0 )
    error( "int SGOfNGcomputeNClassARCer::class(  ) const :"
	   "Subgroup was not initializad");
#endif
  return theClass;
}
// ---------------------- SGOfNGcomputeNClassProblem ----------------------- //


SGOfNGcomputeNClassProblem::SGOfNGcomputeNClassProblem(SMSubgroup& sg)
  : Supervisor( true ),
    theSubgroup( sg ),
    theGIC( sg.getParent().gic ),
    computeBasis( *this, sg.getParent().gcm().computeBasis ),
    arcer(*this),
    started(false)
{ 
}


void SGOfNGcomputeNClassProblem::takeControl( )
{
  if (freeARCs() > 0){
    if (theGIC.haveNilpotentGroupInited()){
      if (!started){
	arcer.setArguments(theGIC.getNilpotentGroupInited(),
			   theSubgroup.getSubgroup().generators());
	started = true;
      }
      //arcer.runComputation();
      if (arcer.takeControl()){
	LogMessage msg( *this, theSubgroup);
	msg << Text("The class of the subgroup ") << Name(theSubgroup)  << " is " 
	    << arcer.getClass() << ".";
	msg.send();
	adminTerminate();
	return;
      }
    }
    usedOneARC();
  }
}


void SGOfNGcomputeNClassProblem::viewStructure(ostream& ostr) const
{
 ProblemView pv( ostr, oid(), Text("Compute the class"),
		  Text("Compute the class of")
		  + Name( theSubgroup ),
		  helpID("SGOfNGcomputeNClassProblem",theSubgroup.getParent()),
		  Text("class")
		  );
  
  pv.startItemGroup();

  if (!theGIC.haveNilpotentGroupInited())
    pv.add( Text("Compute a basis of")
	    + Name( theSubgroup.getParent() ),
	    computeBasis.arcSlotID(),
	    100
	    );
  
  pv.add( Text("Compute the class of")
	  + Name( theSubgroup ),
	  ThisARCSlotID(),
	  100
	  );
  pv.done();
}
// ----------------- SGOfNGPresentationARCer ------------------------ //

void SGOfNGPresentationARCer::runComputation( )
{
  *thePresentation = theSubgroup->makePresentation( );  
}


void SGOfNGPresentationARCer::
setArguments(const SGOfNilpotentGroup& presentation )
{
   theSubgroup = new SGOfNilpotentGroup(presentation);
   thePresentation = new PresentationForSNG(presentation.preimageBasis(), 
					    presentation.parentGroup().relators(), 
					    presentation.parentGroup().collector());
}

const PresentationForSNG& SGOfNGPresentationARCer::getPresentation() const
{
 #if SAFETY > 0
  if( theSubgroup == 0 )
    error( "const PresentationForSNG& SGOfNGPresentationARCer::getPresentation() const:"
	   "You have not initialized the presentation");
#endif
  return *thePresentation;
}

void SGOfNGPresentationARCer::writeResults( ostream& out ) 
{
  out < *thePresentation;
}

void SGOfNGPresentationARCer::readResults( istream& in )
{
  in > *thePresentation;
}

//---------------------------------------------------------------------------//
//-------------------- SGOfNGbuildPresentationProblem --------------------------//
//---------------------------------------------------------------------------//


SGOfNGbuildPresentationProblem::SGOfNGbuildPresentationProblem( SMSubgroup& subgroup)
  : Supervisor( true ),
    theSubgroup( subgroup ),
    arcer( *this),
    started(false),
    initPreimageSGOfNG(*this,subgroup.scm().initPreimageSGOfNG )
{
}


void SGOfNGbuildPresentationProblem::takeControl( )
{
  if (freeARCs() > 0){
    if (theSubgroup.sic.haveSGOfNGPreimageInitialized ()){
      if (!started){
	arcer.setArguments(theSubgroup.sic.getSGOfNilpotentGroup());    
	started = true;
      }
      if (arcer.takeControl() ){
	File file;
	arcer.getPresentation().print(file);
	LogMessage msg( *this, theSubgroup);
	msg << Link(Text("Click here to view the presentation of ") +
		    Name(theSubgroup) + "." ,
		    "FNGStructureConstants", file.getFileName());
	msg.send();
	adminTerminate();
	return;
      }
    }
    usedOneARC();
  }
}  


void SGOfNGbuildPresentationProblem::start( ) { }


void SGOfNGbuildPresentationProblem::terminate( ) { }

void SGOfNGbuildPresentationProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Compute a presentation"),
		  Text("Compute a presentation of")
		  + Name( theSubgroup ),
		  helpID("SGOfNGbuildPresentationProblem", theSubgroup.getParent()),
		  Text("Present. of") + Name( theSubgroup )
		  );

  pv.startItemGroup();


  if( ! theSubgroup.sic.haveSGOfNGPreimageInitialized() )
    pv.add( Text("Compute a basis of the free preimage of ")+ 
	    Name(theSubgroup ),
	    initPreimageSGOfNG.arcSlotID(), 100 );  

  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 100);
  pv.done();
}

// ----------------- SGOfNGormalClosureARCer ------------------------ //

void SGOfNGnormalClosureARCer::runComputation( )
{
  nClosure = theSubgroup->normalClosureBasis( );  
}


void SGOfNGnormalClosureARCer::
setArguments(const SGOfNilpotentGroup& presentation )
{
   theSubgroup = new SGOfNilpotentGroup(presentation);
}

const VectorOf<Word>& SGOfNGnormalClosureARCer::normalClosure() const
{
 #if SAFETY > 0
  if( theSubgroup == 0 )
    error( "const VectorOf<Word>& SGOfNGormalClosureARCer::normalClosure() const:"
	   "You have not initialized the presentation");
#endif
  return nClosure;
}

void SGOfNGnormalClosureARCer::writeResults( ostream& out ) 
{
  out < nClosure;
}

void SGOfNGnormalClosureARCer::readResults( istream& in )
{
  in > nClosure;
}

//---------------------------------------------------------------------------//
//-------------------- SGOfNGnormalClosureProblem --------------------------//
//---------------------------------------------------------------------------//


SGOfNGnormalClosureProblem::SGOfNGnormalClosureProblem( SMSubgroup& subgroup)
  : Supervisor( true ),
    theSubgroup( subgroup ),
    started(false),
    initializeSGOfNG(*this,subgroup.scm().initializeSGOfNG ),
    arcer( *this)
{
}


void SGOfNGnormalClosureProblem::takeControl( )
{
  if (freeARCs() > 0){
    if (theSubgroup.sic.haveSGOfNGinitialized() ||
	theSubgroup.getParent().gic.isFreeNilpotent() == yes){
      if (!started){
	if (theSubgroup.sic.haveSGOfNGinitialized())
	  arcer.setArguments(theSubgroup.sic.getSGOfNilpotentGroup());
	else
	  arcer.setArguments
	    (SGOfNilpotentGroup(theSubgroup.getParent().gic.
				getNilpotentGroupInited(),
				theSubgroup.getSubgroup().generators())
	     );
	     
	started = true;
      }
      if (arcer.takeControl() ){
	File file;
	file << "Generators of the subgroup are:" << endl << endl;
	const VectorOf<Word>& gens = arcer.normalClosure();
	for(int i = 0; i < gens.length(); i++) {
	  if (gens[i]!=Word()){
	    theSubgroup.getParent().getFPGroup().printWord( file,gens[i] );
	    file << endl;
	  }
	}
	file << end;
	
	SGofFreeGroup F(theSubgroup.getParent().getFreePreimage(),
			arcer.normalClosure() );
	
	SMObject* smo = 
	  new SMSubgroup( theSubgroup.getParent(), F,
			  Text("The normal closure of") + Name( theSubgroup ),
			  Text("ncl(") + Name( theSubgroup ) +")" 
			  ); 
	
	ListOf<OID> dependencies( theSubgroup.getParent() );
	CheckinMessage( *smo, "", dependencies ).send();
	
	LogMessage msg( *this, *smo);
	msg << Link( Text("Click here to view the generators of ") 
		     + Name(*smo) + ".",
		     "NGNCgenerators",
		     file.getFileName());
	msg.send();
	adminTerminate();
	return;
      }
    }
 
    usedOneARC();
  }
}


void SGOfNGnormalClosureProblem::start( ) { }


void SGOfNGnormalClosureProblem::terminate( ) { }

void SGOfNGnormalClosureProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Find the normal closure"),
		  Text("Find the normal closure of")
		  + Name( theSubgroup ),
		  helpID("SGOfNGnormalClosureProblem", theSubgroup.getParent()),
		  Text("normal closure of") + Name( theSubgroup )
		  );

  pv.startItemGroup();
  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 100);
  
  pv.startItemGroup( CheckinType( Parent( theSubgroup ) ) == SMFPGroup::NILPOTENT);
  
  if (!theSubgroup.sic.haveSGOfNGinitialized())
    pv.add(Text("Compute a basis of ")
	   + Name(theSubgroup),
	   initializeSGOfNG.arcSlotID(), 100); 
  pv.done();
}
// ----------------- SGOfNGormalClosureGensARCer ------------------------ //

void SGOfNGnormalClosureGensARCer::runComputation( )
{
  nClosure = theSubgroup->normalClosureGens( );  
}


void SGOfNGnormalClosureGensARCer::
setArguments(const SGOfNilpotentGroup& presentation )
{
   theSubgroup = new SGOfNilpotentGroup(presentation);
}

const VectorOf<Word>& SGOfNGnormalClosureGensARCer::normalClosure() const
{
 #if SAFETY > 0
  if( theSubgroup == 0 )
    error( "const VectorOf<Word>& SGOfNGormalClosureGensARCer::normalClosure() const:"
	   "You have not initialized the presentation");
#endif
  return nClosure;
}

void SGOfNGnormalClosureGensARCer::writeResults( ostream& out ) 
{
  out < nClosure;
}

void SGOfNGnormalClosureGensARCer::readResults( istream& in )
{
  in > nClosure;
}

//---------------------------------------------------------------------------//
//-------------------- SGOfNGnormalClosureGensProblem ------------------------//
//---------------------------------------------------------------------------//


SGOfNGnormalClosureGensProblem::SGOfNGnormalClosureGensProblem( SMSubgroup& subgroup)
  : Supervisor( true ),
    theSubgroup( subgroup ),
    arcer( *this)
{
  NilpotentGroup ng(theSubgroup.getParent().
		    getFPGroup().namesOfGenerators(),
		    theSubgroup.getParent().gic.getNilpotentcyClass(),
		    makeVectorOf(theSubgroup.getParent().
				 getFPGroup().getRelators()));
  
  SGOfNilpotentGroup sng( ng,theSubgroup.getSubgroup().generators());  
  arcer.setArguments(sng);
}


void SGOfNGnormalClosureGensProblem::takeControl( )
{
  if (freeARCs() > 0){
    if (arcer.takeControl() ){
      VectorOf<Word> gens = arcer.normalClosure();
      
      File file;
      file << "Generators of the subgroup are:" << endl << endl;
      for(int i = 0; i < gens.length(); i++) {
	if (gens[i]!=Word()){
	  theSubgroup.getParent().getFPGroup().printWord( file,gens[i] );
	  file << endl;
	}
      }
      file << end;
      
      SGofFreeGroup F(theSubgroup.getParent().getFreePreimage(),gens );
      
      SMObject* smo = 
	new SMSubgroup( theSubgroup.getParent(), F,
			Text("The normal closure of") + Name( theSubgroup ),
			Text("ncl(") + Name( theSubgroup ) +")" 
			); 
      
      ListOf<OID> dependencies( theSubgroup.getParent() );
      CheckinMessage( *smo, "", dependencies ).send();
      
      LogMessage msg( *this, *smo);
      msg << Link( Text("Click here to view the generators of ") 
		   + Name(*smo) + ".",
		   "NGNCgenerators",
		   file.getFileName());
      msg.send();
      adminTerminate();
      return;
    }
    
    
    usedOneARC();
  }
}


void SGOfNGnormalClosureGensProblem::start( ) { }


void SGOfNGnormalClosureGensProblem::terminate( ) { }

void SGOfNGnormalClosureGensProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Find the normal closure"),
		  Text("Find the normal closure of")
		  + Name( theSubgroup ),
		  helpID("SGOfNGnormalClosureGensProblem", 
			 theSubgroup.getParent()),
		  Text("norm clos")
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of this problem"), ThisARCSlotID(), 100);

  pv.done();
}

// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class ToddCoxeterARCer ToddCoxeter,
//           SGIndexToddCoxeter, SchreierTransversal, PermutationRepProblem,
//           WordRepresentativeProblem, WordRepresentativeProblem 

//
// Principal Author: Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//


#include "conversions.h"
#include "ToddCoxeter.h"
#include "CommutatorIterator.h"
#include "OutMessages.h"
#include "GCM.h"
#include "SMFPGroup.h"
#include "SMSubgroup.h"
#include "viewStructure.h"
#include "SubgroupContainmentProblem.h"


// ------------------- ToddCoxeterARCer --------------------- //

void ToddCoxeterARCer::runComputation( )
{
  retValue = enumerator.enumerate();
  permutationRepresentation = 
    new  PermutationRepresentation(enumerator.permutationRepresentation());
}

void ToddCoxeterARCer::writeResults( ostream& out ) 
{
  out < retValue;
  if (retValue !=0)  out < *permutationRepresentation;
}

void ToddCoxeterARCer::readResults( istream& in )
{
  in > retValue;
  if (retValue !=0) {
    permutationRepresentation = new PermutationRepresentation();
    in > *permutationRepresentation;
  }
}

const PermutationRepresentation& ToddCoxeterARCer::getTransversal()const
{
  if (permutationRepresentation == NULL)
    error("VectorOf<Word> getTransversal()const : transversal was not computed yet.");
  return *permutationRepresentation;
}

// -------------------------- ToddCoxeter ---------------------------- //

ToddCoxeter::ToddCoxeter(GCM& gcm)
  : theGCM( gcm ),
    theGroup( gcm.getSMFPGroup() )
{
  if (theGroup.gic.isNilpotent()==yes) {
    CommutatorIterator I(theGroup.getFPGroup().numberOfGenerators(),
			 theGroup.gic.getNilpotentcyClass()+1);
    SetOf<Word> newRelators = theGroup.getFPGroup().getRelators();
    for( I.reset(); !I.done(); I.next() ) newRelators |= I.value();
    FPGroup g(theGroup.getFPGroup().namesOfGenerators(), newRelators);
    arcer = new ToddCoxeterARCer(*this, g);
  }
  else if (theGroup.gic.isAbelian()==yes) {
    CommutatorIterator I(theGroup.getFPGroup().numberOfGenerators(), 2);
    SetOf<Word> newRelators = theGroup.getFPGroup().getRelators();
    for( I.reset(); !I.done(); I.next() ) newRelators |= I.value();
    FPGroup g(theGroup.getFPGroup().namesOfGenerators(), newRelators);
    arcer = new ToddCoxeterARCer(*this, g);
  }
  else 
    arcer = new ToddCoxeterARCer(*this, theGroup.getFPGroup());
  adminStart();
}

void ToddCoxeter::takeControl( )
{
  if ( freeARCs() > 0 )
    if( arcer->takeControl() ) {
      Chars explanation;
      // @am this is just for answer to be reasonable
      // because Todd-Coxeter checks for order of abelian
      // quotient first
      if (arcer->getRetValue())
	explanation = "according to the Todd-Coxeter algorithm";
      else
	explanation = Text("this follows from the canonical decomposition of")
	  + Name(theGroup);
      theGroup.gic.putHaveOrder( arcer->getRetValue(), explanation );
      if ( arcer->getRetValue() !=0 )
	theGroup.gic.putHaveSchreierTransvl(arcer->getTransversal());
      adminTerminate();
    }
    else
      usedOneARC();
}

// -------------------------- SGIndexToddCoxeter ------------------------- //

SGIndexToddCoxeter::SGIndexToddCoxeter(SCM& scm)
  : theSCM( scm ),
    theSubgroup( scm.getSubgroup() )
{
  if( theSubgroup.getParent().gic.isAbelian()==yes ) {
    CommutatorIterator I( theSubgroup.getParent().getFPGroup().
			  numberOfGenerators(), 2);
    SetOf<Word> newRelators =
      theSubgroup.getParent().getFPGroup().getRelators();
    for( I.reset(); !I.done(); I.next() ) newRelators |= I.value();
    FPGroup newGroup( theSubgroup.getParent().getFPGroup().namesOfGenerators(),
		      newRelators);
    arcer = new ToddCoxeterARCer( *this, newGroup, 
				  theSubgroup.getSubgroup().generators() );
  }
  else arcer = new ToddCoxeterARCer( *this, 
				     theSubgroup.getParent().getFPGroup(),
				     theSubgroup.getSubgroup().generators() );
  adminStart();
}


void SGIndexToddCoxeter::takeControl( )
{
  if ( freeARCs() > 0 )
    if( arcer->takeControl() ) {
      theSubgroup.sic.putIndex( arcer->getRetValue());
      if ( arcer->getRetValue() !=0 )
	theSubgroup.sic.putHaveSchreierTransvl(arcer->getTransversal());
      adminTerminate();
    }
    else
      usedOneARC();
}
// -------------------------- SchreierTransversal ------------------------- //

SchreierTransversal::SchreierTransversal(SMFPGroup& g)
  : Supervisor( true ),
    theObject( g ),
    theGroup( g ),
    theToddCoxeter(*this,g.gcm().theToddCoxeter)
{
  if ( haveSchreierTransvl() ) resultIsFast();
}

SchreierTransversal::SchreierTransversal(SMSubgroup& s)
  : Supervisor( true ),
    theObject( s ),
    theGroup( s.getParent() ),
    theToddCoxeter(*this,s.scm().sgToddCoxeter)
{
  if ( haveSchreierTransvl() ) resultIsFast();
}

void SchreierTransversal::takeControl( )
{
  if (isInfinite()) {
    LogMessage msg(*this,theObject);
    if (Chars(theObject.typeID())==Chars("SMFPGroup"))
      msg << Name(theObject) << " is infinite";
    else 
      msg << Name(theObject) << " has infinite index in " << Name(theGroup); 
    msg << ". Can't compute Schreier transversal.";
    msg.send();
    adminTerminate();
    return;    
  }
  if (haveSchreierTransvl() ) {
    File file;
    VectorOf<Word> transversal = getRepresentatives();
    FPGroup g = theGroup.getFPGroup();
    for (int i=0; i<transversal.length(); i++) {
      file << "s" << i+1 << " = ";
      g.printWord(file, transversal[i]);
      file << endl;
    }
    LogMessage msg(*this,theObject);
    if (Chars(theObject.typeID())==Chars("SMFPGroup"))
      msg << Link( Chars( "Click here to see Shreier representatives for ") 
		   + Text( Name(theObject) ) + Text("."), 
		   "SchreierTransversal", file.getFileName() );
    else
      msg << Link( Chars( "Click here to see Shreier transversal of ")
		   + Text( Name(theObject) ) + Text("."),
		   "SchreierTransversal", file.getFileName() );      
    msg.send();
    adminTerminate();
    return;
  }
}

bool SchreierTransversal::haveSchreierTransvl() const 
{
  if (Chars(theObject.typeID())==Chars("SMFPGroup"))
    return ((SMFPGroup& )theObject).gic.haveSchreierTransvl();
  else
    return ((SMSubgroup& )theObject).sic.haveSchreierTransvl();
}
  
const VectorOf<Word>& SchreierTransversal::getRepresentatives() const
{
  if (Chars(theObject.typeID())==Chars("SMFPGroup"))
    return ((SMFPGroup& )theObject).gic.getSchreierTransvl().getRepresentatives();
  else
    return ((SMSubgroup& )theObject).sic.getSchreierTransvl().getRepresentatives();    
}

bool SchreierTransversal::isInfinite() const
{
  if (Chars(theObject.typeID())==Chars("SMFPGroup"))
     return ((SMFPGroup& )theObject).gic.isFinite()==no;
  else 
    return ((SMSubgroup& )theObject).sic.index()==0;
}

void SchreierTransversal::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Schreier representatives"),
		  Text("Find Schreier representatives for") 
		  + Name( theObject ),
		  helpID("SchreierTransversal",theGroup),
		  Text("schr. repr.") + Name(theObject) );
  pv.startItemGroup( CheckinType( theGroup ) == SMFPGroup::FP ||
		     CheckinType( theGroup ) == SMFPGroup::ABELIAN ||
		     CheckinType( theGroup ) == SMFPGroup::NILPOTENT );
  pv.add(Text("Enumerate cosets") + Name( theObject ),
	 theToddCoxeter.arcSlotID(), 100 );   
  pv.done();
}


// ---------------------- PermutationRepProblem ----------------------- //

PermutationRepProblem::PermutationRepProblem(SMFPGroup& g)
  : Supervisor( true ),
    theObject( g ),
    theGroup( g ),
    theToddCoxeter(*this, g.gcm().theToddCoxeter)
{
  if ( haveSchreierTransvl() ) resultIsFast();
}

PermutationRepProblem::PermutationRepProblem(SMSubgroup& s)
  : Supervisor( true ),
    theObject( s ),
    theGroup( s.getParent() ),
    theToddCoxeter(*this, s.scm().sgToddCoxeter)
{
  if ( haveSchreierTransvl() ) resultIsFast();
}

void PermutationRepProblem::takeControl( )
{
  if (isInfinite()) {
    LogMessage msg(*this,theObject);
    if (Chars(theObject.typeID())==Chars("SMFPGroup"))
      msg << Name(theObject) << " is infinite";
    else 
      msg << Name(theObject) << " has infinite index in " << Name(theGroup); 
    msg << ". Can't compute Schreier transversal.";
    msg.send();
    adminTerminate();
    return;    
  }
  if (haveSchreierTransvl() ) {
    File file;
    FPGroup g = theGroup.getFPGroup();
    getPermutations().printOn(g.namesOfGenerators(), file);
    LogMessage msg(*this, theObject);
    if (Chars(theObject.typeID())==Chars("SMFPGroup"))
      msg << Link(Chars( "Click here to see a permutation representation of ")
		  + Text( Name(theObject) ) + Text("."),
		  "PermutationRepresentation", file.getFileName() );
    else
      msg << Link( Chars( "Click here to see the presentation of ")
		   + Text( Name(theGroup) ) 
		   + Text(" by permutations on cosets of ") 
		   + Text( Name(theObject) ) + Text("."),
		   "PermutationRepresentation", file.getFileName() );     
    msg.send();
    adminTerminate();
    return;
  }
}

bool PermutationRepProblem::haveSchreierTransvl()const
{
  if (Chars(theObject.typeID())==Chars("SMFPGroup"))
    return ((SMFPGroup& )theObject).gic.haveSchreierTransvl();
  else
    return ((SMSubgroup& )theObject).sic.haveSchreierTransvl();
}
  
const PermutationRepresentation& PermutationRepProblem::getPermutations()const
{
  if (Chars(theObject.typeID())==Chars("SMFPGroup"))
    return ((SMFPGroup& )theObject).gic.getSchreierTransvl();
  else
    return ((SMSubgroup& )theObject).sic.getSchreierTransvl();    
}

bool PermutationRepProblem::isInfinite()const 
{
  if (Chars(theObject.typeID())==Chars("SMFPGroup"))
    return ((SMFPGroup& )theObject).gic.isFinite()==no;
  else
    return ((SMSubgroup& )theObject).sic.index()==0;
}

void PermutationRepProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Permutation representation"),
		  Text("Find a permutation representation of") + 
		  Name( theObject ),
		  helpID("PermutationRepProblem",theGroup),
		  Text("perm.") + Name(theObject) );
  pv.startItemGroup( CheckinType( theGroup ) == SMFPGroup::FP ||
		     CheckinType( theGroup ) == SMFPGroup::ABELIAN || 
		     CheckinType( theGroup ) == SMFPGroup::NILPOTENT );
  pv.add(Text("Enumerate cosets") + Name( theObject ), 
	 theToddCoxeter.arcSlotID(), 100);   
  pv.done();
}


// ------------------- WordRepresentativeARCer --------------------- //

void WordRepresentativeARCer::setArguments(const PermutationRepresentation& pr,
					   const Word& w)
{
  theWord = w;
  permutationRepresentation =  new  PermutationRepresentation(pr);
}

void WordRepresentativeARCer::runComputation( )
{
  retValue = permutationRepresentation->representativeOf(theWord);
}

void WordRepresentativeARCer::writeResults( ostream& out ) 
{
  out < retValue;
}

void WordRepresentativeARCer::readResults( istream& in )
{
  in > retValue;
}

// -------------------------- WordRepresentative -------------------------- //

WordRepresentative::WordRepresentative(class WordRepresentativeProblem& boss)
  : theParent( boss.getParent() ),
    theGroup( boss.getGroup() ),
    theTestWords(1),
    theWordsReps(1),
    theWordsRepsFound(1),
    checkedWord(0),
    arcer( *this ),
    arcerStarted( false )
{
  theTestWords[0] = boss.getWord().getWord();
  adminStart();
}

WordRepresentative::WordRepresentative(class SubgroupContainmentProblem& boss)
  : theParent( boss.getSubgroup() ),
    theGroup( boss.getTestWord().getParent() ),
    theTestWords(1),
    theWordsReps(1),
    theWordsRepsFound(1),
    checkedWord(0),
    arcer( *this ),
    arcerStarted( false )
{
  theTestWords[0] = boss.getTestWord().getWord();
  adminStart();
}

WordRepresentative::WordRepresentative(class IsSGNormal& boss)
  : theParent( boss.getSubgroup() ),
    theGroup( boss.getSubgroup().getParent() ),
    theTestWords( makeVectorOf(boss.getTestWords()) ),
    theWordsReps( boss.getTestWords().cardinality() ),
    theWordsRepsFound( boss.getTestWords().cardinality() ),
    checkedWord(0),
    arcer( *this ),
    arcerStarted( false )
{
  for( int i = 0; i < theWordsRepsFound.length(); ++i )
    theWordsRepsFound[i] = false;
  adminStart();
}

void WordRepresentative::takeControl( )
{
  if( isInfinite() ) {
    for( int j = 0; j < theWordsRepsFound.length(); ++j )
      theWordsRepsFound[j] = false;
    adminTerminate();
    return;    
  }
  if( freeARCs() > 0 && haveSchreierTransvl() ) {
    if ( !arcerStarted ){
      if( checkedWord == theTestWords.length() ) {
	adminTerminate();
	return;
      }
      arcer.setArguments( getPermutations(), theTestWords[checkedWord] );
      arcerStarted = true;
    }
    if( arcer.takeControl() ) {
      theWordsRepsFound[checkedWord] = yes;
      theWordsReps[checkedWord] = arcer.getRetValue();
      ++checkedWord;
      arcerStarted = false;
    }
    usedOneARC();
  } 
}

bool WordRepresentative::haveSchreierTransvl( ) const 
{
  if( Chars(theParent.typeID()) == Chars("SMFPGroup") )
    return ((SMFPGroup& )theParent).gic.haveSchreierTransvl();
  else
    return ((SMSubgroup& )theParent).sic.haveSchreierTransvl();
}

const PermutationRepresentation& WordRepresentative::getPermutations( ) const
{
  if( Chars(theParent.typeID()) == Chars("SMFPGroup") )
    return ((SMFPGroup& )theParent).gic.getSchreierTransvl();
  else
    return ((SMSubgroup& )theParent).sic.getSchreierTransvl();    
}

bool WordRepresentative::isInfinite( ) const
{
  if( Chars(theParent.typeID()) == Chars("SMFPGroup") )
    return ((SMFPGroup& )theParent).gic.isFinite()==no;
  else
    return ((SMSubgroup& )theParent).sic.index()==0;
}

// ---------------------- WordRepresentativeProblem ----------------------- //

WordRepresentativeProblem::WordRepresentativeProblem(SMWord& w)
  : Supervisor( true ),
    theParent( w.getParent() ),
    theGroup( w.getParent()),
    theWord( w ),
    theToddCoxeter( *this, w.getParent().gcm().theToddCoxeter ),
    theWordRepCM( *this )
{
  if( isInfinite() ) resultIsFast();
}

WordRepresentativeProblem::WordRepresentativeProblem(SMSubgroup& s,SMWord& w)
  : Supervisor( true ),
    theParent( s ),
    theGroup( w.getParent() ),
    theWord( w ),
    theToddCoxeter( *this, s.scm().sgToddCoxeter ),
    theWordRepCM( *this ) 
{
  if( isInfinite() ) resultIsFast(); 
}

void WordRepresentativeProblem::takeControl( )
{
  if( isInfinite() ){
    LogMessage msg( *this, theParent, theWord );
    msg << Name(theParent) << " is infinite. Can't find a reresentative for " 
	<< Name( theWord ) << ".";
    msg.send();
    adminTerminate();
    return;    
  }
  if( theWordRepCM->state() == ComputationManager::TERMINATED ) {
    LogMessage msg( *this, theParent, theWord );
    msg << "Schreier representative of " << Name( theWord );
    if( Chars(theParent.typeID()) == Chars("SMFPGroup") )
      msg << " modulo 1 is ";
    else
      msg << " modulo "<<  Name( theParent ) << " is ";
    theGroup.getFPGroup().printWord( msg, theWordRepCM->getWordsReps()[0] );
    msg << ".";
    msg.send();
    adminTerminate();
    return;	
  } 
}


bool WordRepresentativeProblem::haveSchreierTransvl()const {
  if (Chars(theParent.typeID())==Chars("SMFPGroup"))
    return ((SMFPGroup& )theParent).gic.haveSchreierTransvl();
  else
    return ((SMSubgroup& )theParent).sic.haveSchreierTransvl();
}
  
/*
const PermutationRepresentation& 
WordRepresentativeProblem::getPermutations()const{
  if (Chars(theParent.typeID())==Chars("SMFPGroup"))
    return ((SMFPGroup& )theParent).gic.getSchreierTransvl();
  else
    return ((SMSubgroup& )theParent).sic.getSchreierTransvl();    
}
*/

bool WordRepresentativeProblem::isInfinite()const{
  if (Chars(theParent.typeID())==Chars("SMFPGroup"))
    return ((SMFPGroup& )theParent).gic.isFinite()==no;
  else 
    return ((SMSubgroup& )theParent).sic.index()==0;
}

void WordRepresentativeProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Schreier representative of a word"),
		  Text("Find Schreier representative of ") + Name( theWord ),
		  helpID("WordRepresentativeProblem",theGroup),
		  Text("repr.") + Name(theWord) );
  pv.startItemGroup( CheckinType( theGroup ) == SMFPGroup::FP ||
		     CheckinType( theGroup ) == SMFPGroup::ABELIAN ||
		     CheckinType( theGroup ) == SMFPGroup::NILPOTENT );
  if( !haveSchreierTransvl() )
    pv.add( Text("Enumerate cosets of") + Name( theParent ),
	    theToddCoxeter.arcSlotID(), 100 );   
  pv.add( Text("Find a representative of ") + Name(theWord),
	  theWordRepCM.arcSlotID(), 50 );
  pv.done();
}

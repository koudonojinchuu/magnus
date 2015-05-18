// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes NilpotentWPARCer, NilpotentWPInQuotients,
//            NilpotentWP, NGcomputeBasisARCer, NGcomputeBasis,
//            NilpotentQuotients, NGcomputeBasisProblem, NGPresentationARCer,
//            NGdecomposeWordARCer, NGdecomposeWord 

// Principal Author: Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//


#include "NilpotentQuotients.h"
#include "CommutatorIterator.h"
#include "SMFPGroup.h"
#include "SMWord.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "conversions.h"

// ---------------------- NilpotentWPARCer --------------------------- //
void NilpotentWPARCer::setGroup(const NilpotentGroup& NG)
{
   theGroup = new NilpotentGroup(NG);
}
void NilpotentWPARCer::setArguments(const int& length,const NilpotentGroup& NG)
{
  theCommutatorsLength = length;
  isCommutatorsChecks = true;
  if (theGroup)
    delete theGroup;
  setGroup(NG);

  int genLen = NG.numberOfGenerators();
  generators = VectorOf<Word>(genLen);
  for( int i = 0; i < genLen; ++i )
    generators[i] = Word(Generator(i+1));
}
void NilpotentWPARCer::setArguments(const int& length,const NilpotentGroup& NG,
				    const VectorOf<Word>& gen)
{
  theCommutatorsLength = length;
  generators = gen;
  isCommutatorsChecks = true;
  if (theGroup)
    delete theGroup;
  setGroup(NG);
}

void NilpotentWPARCer::setArguments(const SetOf<Word>& setW, const NilpotentGroup& NG)
{
  int card = setW.cardinality();
  theWords = VectorOf<Word>(card);
  isCommutatorsChecks = false;

  SetIterator<Word> I(setW);
  for( int i = 0 ; !I.done(); I.next(), ++i )
    theWords[i] = Word(I.value());

 if (theGroup)
    delete theGroup;
  setGroup(NG);
}
void NilpotentWPARCer::setArguments(const VectorOf<Word>& vecW, const NilpotentGroup& NG)
{
  theWords = vecW;
  isCommutatorsChecks = false;

  if (theGroup)
    delete theGroup;
  setGroup(NG);
}
void NilpotentWPARCer::runComputation( )
{
#if SAFETY > 0
  if( theGroup == 0 )
    error( " NilpotentWPARCer::runComputation() : "
	   "The word problem has not been started yet");
#endif
  if (isCommutatorsChecks)
    commutatorsCheck();
  else
    wordsCheck(); 
}

void NilpotentWPARCer::commutatorsCheck( ) 
{
  result = true;
  
  CommutatorIterator I(generators, theCommutatorsLength);
  for( I.reset(); !I.done(); I.next() ){
    // @am if wordProblem == dontknow it will be error.
    // but i suppose it have to be only yes or no, so i leave it.
    retWord = I.value();
    commutator = I.components();
    if (!theGroup->wordProblem(I.value())){
      result = false;
      break;
    }
  }
}      
    
void NilpotentWPARCer::wordsCheck( ) 
{
  int wordNum = theWords.length();
  result = true;
  for (int i=0;i<wordNum;i++){
    retWord = theWords[i];
    if (!theGroup->wordProblem(theWords[i])){
      result = false;
      break;
    }
  }
}      

void NilpotentWPARCer::writeResults( ostream& out ) 
{
  out < result;
  out < retWord;
  out < commutator;
}


void NilpotentWPARCer::readResults( istream& in )
{
  in > result;
  in > retWord;
  in > commutator;
}


bool NilpotentWPARCer::getResult() const
{
#if SAFETY > 0
  if( theGroup == 0 )
    error( "bool NilpotentWPARCer::getResult() const : "
	   "The word problem has not been computed yet");
#endif
  return result;
}

Word NilpotentWPARCer::getWord() const
{
#if SAFETY > 0
  if( theGroup == 0 )
    error( "bool NilpotentWPARCer::getResult() const : "
	   "The word problem has not been computed yet");
#endif
  return retWord;
}
VectorOf<int> NilpotentWPARCer::getCommutator() const
{
#if SAFETY > 0
  if( theGroup == 0 )
    error( "bool NilpotentWPARCer::getResult() const : "
	   "The word problem has not been computed yet");
#endif
  if (!isCommutatorsChecks)
   error( "bool NilpotentWPARCer::getCommutator() const : "
	   "Commutators were not checked");
    
  return commutator;
}
//---------------------------------------------------------------------------//
//---------------------------- NilpotentWP ----------------------------------//
//---------------------------------------------------------------------------//


NilpotentWP::NilpotentWP( Supervisor& sup )
  : arcer( *this ),
    theGroup(0),
    isStarted (false),
    is_trivial( dontknow )
{ 
  //  adminStart();
}
void NilpotentWP::initialize(const int& length, const SMFPGroup* group)
{
  theGroup = group;
  theCommutatorsLength = length;
  isCommutatorsCheck = true;
  theClass = theGroup->gic.getNilpotentcyClass();
  adminStart();
}
void NilpotentWP::initialize(const int& length, const SMFPGroup* group,
			     const VectorOf<Word>& gen)
{
  theGroup = group;
  theCommutatorsLength = length;
  generators = gen;
  isCommutatorsCheck = true;
  theClass = theGroup->gic.getNilpotentcyClass();
  adminStart();
}
void NilpotentWP::initialize(const SetOf<Word>& w, const SMFPGroup* group)
{
  theWords = w;
  isCommutatorsCheck = false;
  theGroup = group;
  theClass = theGroup->gic.getNilpotentcyClass();
  adminStart();
}
void NilpotentWP::initialize(const VectorOf<Word>& vecW, const SMFPGroup* group)
{
  theWords = makeSetOf(vecW);
  isCommutatorsCheck = false;
  theGroup = group;
  theClass = theGroup->gic.getNilpotentcyClass();
  adminStart();
}
void NilpotentWP::start()
{
#if SAFETY > 0
  if( theGroup == 0 )
    error( "void NilpotentWP::start() : "
	   "The computation manager was not initialized yet");
#endif
}

void NilpotentWP::takeControl( )
{
  if ( freeARCs() > 0 ) {
    if ( ! isStarted && theGroup->gic.haveNilpotentQuotInited
	 (theGroup->gic.getNilpotentcyClass() ) ) {
      if (isCommutatorsCheck) // If we want to check enumerated commutators
	if (generators.length()==0)// Commutate the generators of the group
	  arcer.setArguments( theCommutatorsLength, 
			      theGroup->gic.getNilpotentQuotInited
			      (theGroup->gic.getNilpotentcyClass()));
	else // Commutate the generators, given as a parameter
	  arcer.setArguments( theCommutatorsLength, 
			      theGroup->gic.getNilpotentQuotInited
			      (theGroup->gic.getNilpotentcyClass() ),
			      generators);
      else // Check the set of words
	arcer.setArguments( theWords, theGroup->gic.getNilpotentQuotInited
			    (theGroup->gic.getNilpotentcyClass() ));
      isStarted = true;
    }
    
    if ( isStarted ) 
      if (arcer.takeControl()){
	is_trivial = arcer.getResult();
	retWord = arcer.getWord();
	if (isCommutatorsCheck)
	  commutator = arcer.getCommutator();
	adminTerminate();
	return;
      }
    
    usedOneARC();
  }
}

Trichotomy NilpotentWP::isTrivial() const
{
  return is_trivial;
}

Word NilpotentWP::getWord() const
{
  return retWord;
}
VectorOf<int> NilpotentWP::getCommutator() const
{
  return commutator;
}

//---------------------------------------------------------------------------//
//---------------------------- NilpotentWPInQuotients -----------------------//
//---------------------------------------------------------------------------//


NilpotentWPInQuotients::NilpotentWPInQuotients(Supervisor& sup)
  : 
    currentClass(2),
    arcer( *this ),
    isStarted (false),
    is_trivial( dontknow ),
    theGroup(0)
{

}

void NilpotentWPInQuotients::initialize(const SetOf<Word>& w,const SMFPGroup* group,
					int startClass)
{
  theWords = w;
  theGroup = group;
  isCommutatorsCheck = false;
  currentClass = startClass;
  if (theGroup->gic.isNilpotent()==yes || theGroup->gic.isFreeNilpotent()==yes )
    upperBound = theGroup->gic.getNilpotentcyClass()-1;
  else
    upperBound = 10;
  adminStart();
}
void NilpotentWPInQuotients::initialize(const VectorOf<Word>& vecW,const SMFPGroup* group,
					int startClass)
{
  theWords = makeSetOf(vecW);
  theGroup = group;
  isCommutatorsCheck = false;
  currentClass = startClass;
  if (theGroup->gic.isNilpotent()==yes || theGroup->gic.isFreeNilpotent()==yes)
    upperBound = theGroup->gic.getNilpotentcyClass()-1;
  else
    upperBound = 10;
  adminStart();
}
void NilpotentWPInQuotients::initialize(const int& length, const SMFPGroup* group,
					const VectorOf<Word>& gen,
					int startClass)
{
  theGroup = group;
  theCommutatorsLength = length;
  generators = gen;
  currentClass = startClass;
  isCommutatorsCheck = true;
  if (theGroup->gic.isNilpotent()==yes || theGroup->gic.isFreeNilpotent()==yes)
    upperBound = theGroup->gic.getNilpotentcyClass()-1;
  else
    upperBound = 10;
  adminStart();
}
void NilpotentWPInQuotients::initialize(const int& length, const SMFPGroup* group,int startClass)
{
  theGroup = group;
  theCommutatorsLength = length;
  currentClass = startClass;
  isCommutatorsCheck = true;
  if (theGroup->gic.isNilpotent()==yes || theGroup->gic.isFreeNilpotent()==yes)
    upperBound = theGroup->gic.getNilpotentcyClass()-1;
  else
    upperBound = 10;
  adminStart();
}
void NilpotentWPInQuotients::start()
{
#if SAFETY > 0
  if( theGroup == 0 )
    error( "void NilpotentWPInQuotients::start() : "
	   "The computation manager was not initialized yet");
#endif
}

Trichotomy NilpotentWPInQuotients::isTrivial( int& nClass ) const
{
  if (is_trivial != dontknow )
    nClass = currentClass;
  return is_trivial;
}

Word NilpotentWPInQuotients::getWord() const
{
  return retWord;
}

VectorOf<int> NilpotentWPInQuotients::getCommutator() const
{
  return commutator;
}

void NilpotentWPInQuotients::takeControl( )
{
  if ( freeARCs() > 0 ) {
    if (currentClass > upperBound ){
      //      currentClass--;
      adminTerminate();
      return;
    }   
    if ( ! isStarted && theGroup->gic.haveNilpotentQuotInited( currentClass ) ) {
      if (isCommutatorsCheck) // If we want to check enumerated commutators
	if (generators.length() == 0) // Commutate the generators of the group
	  arcer.setArguments( theCommutatorsLength, 
			      theGroup->gic.getNilpotentQuotInited( currentClass ));
	else // Commutate the generators, given as a parameter
	  arcer.setArguments( theCommutatorsLength, 
			      theGroup->gic.getNilpotentQuotInited( currentClass ),
			      generators);
      else // Check the givem set of words
	arcer.setArguments( theWords, 
			    theGroup->gic.getNilpotentQuotInited( currentClass ));
      isStarted = true;
    }
    
    if ( isStarted ) 
      if (arcer.takeControl()){
	if (!arcer.getResult()){
	  is_trivial = false;
	  retWord = arcer.getWord();
	  if (isCommutatorsCheck)
	    commutator = arcer.getCommutator();
	  adminTerminate();
	  return;
	}
	currentClass++;
	isStarted = false;
    }
    usedOneARC();
  }
}
// ---------------------- NGcomputeBasisARCer --------------------------- //


void NGcomputeBasisARCer::setArguments(const FPGroup& group,const int& nilpClass)
{
  if (init)
    delete init;
  // This ARCer can be called for several times, so we have to delete
  // init, if it exists.
  
  init = new NilpotentGroup(group.namesOfGenerators(),
			    nilpClass,
			    makeVectorOf(group.getRelators()));
}


void NGcomputeBasisARCer::runComputation( )
{
#if SAFETY > 0
  if( init == 0 )
    error( "void NGcomputeBasisARCer::runComputation( ): "
	   "The quotient has not been initialized");
#endif
  if (!init->isInitialized())
    init->initialize();
  if (putInFile){
    init->printBasis(structFile);
  }
}


void NGcomputeBasisARCer::writeResults( ostream& out ) 
{
  out < *init;
}


void NGcomputeBasisARCer::readResults( istream& in )
{
  in > *init;
}


const NilpotentGroup& NGcomputeBasisARCer::getGroup() const
{
#if SAFETY > 0
  if( init == 0 )
    error( "NilpotentGroup NGcomputeBasisARCer::getGroup() : "
	   "The quotient has not been computed yet");
#endif
  
  return *init;
}
//---------------------------------------------------------------------------//
//------------------------- NGcomputeBasis ----------------------------------//
//---------------------------------------------------------------------------//


NGcomputeBasis::NGcomputeBasis(GCM& gcm, bool internal)
  : ComputationManager( ! internal ),
    theGCM(gcm),
    arcsNumber (0),
    arcer (*this,true)
{
  if (theGCM.getSMFPGroup().gic.getBasisFileName().length()>0)
    resultIsFast();
  else{
    arcer.setArguments(theGCM.getSMFPGroup().getFPGroup(),
		       theGCM.getSMFPGroup().gic.getNilpotentcyClass());
  }
  adminStart();
}


NGcomputeBasis::~NGcomputeBasis( )
{
}


void NGcomputeBasis::start( )
{
}


void NGcomputeBasis::takeControl( )
{
  if (fastResult()){
    adminTerminate();
    return;
  }
  if ( freeARCs() > 0 ) {
    if (arcsNumber == 99){
      LogMessage msg;
      msg << " Many algorithms involving nilpotent groups are particularly"
      	" sensitive to the ordering of the generators. If you find that"
	" Magnus is working very hard, without coming up with any answers,"
	" try relabeling the generators. For example, instead of the presentation";
      msg << "G= 3 <a,b,c; a^2, [b,c]>,";
      msg << "use the presentation";
      msg << "G= 3 <a,b,c; [a,b], c^2 >.";

      msg << "Surprisingly, Magnus computes the Hirsch number of G, using the latter"
	" presentation, almost immediately, but struggles forever with the first"
	" presentation, before coming up with the Hirsch number.";
      msg.send();
    }    
    if (arcer.takeControl()) 
      //arcer.runComputation();
      {
	theGCM.getSMFPGroup().gic.
	  putHaveNilpotentGroupInited( arcer.getGroup(),
				       arcer.getFileName());
	theGCM.getSMFPGroup().gic.putHaveOrder( arcer.getGroup().order(),
						Chars("this follows from an inspection of a basis of ")+ 
						Text(Name(theGCM.getSMFPGroup())));
	adminTerminate();
	return;
      } else{
	arcsNumber++;
	usedOneARC();
      }
  }
}


void NGcomputeBasis::terminate( )
{
  
}



//---------------------------------------------------------------------------//
//------------------------ NilpotentQuotients -------------------------------//
//---------------------------------------------------------------------------//


NilpotentQuotients::NilpotentQuotients(class GCM& gcm, bool internal)
  : ComputationManager( ! internal ),
    theGCM( gcm ),
    theGroup( gcm.getSMFPGroup() ),
    arcer(*this),
    currentClass( 2 )
{
  if (theGroup.gic.isNilpotent()==yes)
    upperBound = theGroup.gic.getNilpotentcyClass()-1;
  else
    upperBound = 10;
  // Checking upper class.
  
  if ( increaseCurrentClass() )
    arcer.setArguments(theGroup.getFPGroup(),
		       currentClass);
  // Looking for computed quotients.

  adminStart();
}


NilpotentQuotients::~NilpotentQuotients( )
{
}


void NilpotentQuotients::takeControl( )
{

  if ( freeARCs() > 0 ) {
    if (currentClass > upperBound){
      adminTerminate();
      return;
    }
    
    if (arcer.takeControl()) {
      theGroup.gic.putHaveNilpotentQuotInited( arcer.getGroup(), currentClass);
      
      LogMessage msg( *this,theGroup );
      msg << "Computed the nilpotent quotient of class " << currentClass;
      msg << " of " << Name( theGroup ) << ".";
      msg.send();     
      
      currentClass++;
      if (!increaseCurrentClass() ){
	adminTerminate();
	return;
      }
      else 
	arcer.setArguments(theGroup.getFPGroup(),
			   currentClass);
      
    } 
    usedOneARC();
  }
}


void NilpotentQuotients::start( )
{

}


void NilpotentQuotients::terminate( ) { }
bool NilpotentQuotients::increaseCurrentClass()
{
  while (currentClass <= upperBound && 
	 theGroup.gic.haveNilpotentQuotInited( currentClass ) )
    currentClass++;     
  return !(currentClass > upperBound);
}


//---------------------------------------------------------------------------//
//--------------------- NGcomputeBasisProblem ----------------------------//
//---------------------------------------------------------------------------//


NGcomputeBasisProblem::NGcomputeBasisProblem(class SMFPGroup& G)
  : Supervisor( true ),
    theGroup( G ),
    computeBasis( *this, G.gcm().computeBasis )
{ 
  if (theGroup.gic.getBasisFileName().length()>0 )
    resultIsFast();  
}


void NGcomputeBasisProblem::start( )
{

}


void NGcomputeBasisProblem::takeControl( )
{
  if (theGroup.gic.haveNilpotentGroupInited() && 
      theGroup.gic.getBasisFileName().length()>0 ){
    LogMessage msg( *this, theGroup);
    if (theGroup.gic.isFreeNilpotent() == yes){
      msg <<  Link(Text("Click here to view basic commutators of " )
		  + Name(theGroup) + Text(".") ,
		  "NilpotentBasis",theGroup.gic.getBasisFileName());
    }
    else{
      Chars basis;
      //      if (theGroup.gic.getNilpotentGroupInited().isMalcevBasis())
      //	basis = "Mal'cev";
      msg << Link(Text("Click here to view the basis of ")
		  + Name(theGroup) + Text(".") ,
		  "NilpotentBasis",theGroup.gic.getBasisFileName());
    }
    msg.send();
    adminTerminate();
  }
}


void NGcomputeBasisProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Compute a basis of the group "),
		  Text("Compute a basis of ")+
		  Name( theGroup ),
		  helpID("NGcomputeBasisProblem", theGroup),
		  "basis"
		  );


  pv.startItemGroup();
  
  pv.add(Text("For the use of this problem"), computeBasis.arcSlotID(), 100);
  
  pv.done();
}
// ----------------- NGPresentationARCer ------------------------ //

void NGPresentationARCer::runComputation( )
{
  *thePresentation = theGroup->makePresentation( );  
}


void NGPresentationARCer::
setArguments(const NilpotentGroup& presentation )
{
   theGroup = new NilpotentGroup(presentation);
   thePresentation = new PresentationForNG(presentation.collector(),
					   presentation.relators());
}

const PresentationForNG& NGPresentationARCer::getPresentation() const
{
 #if SAFETY > 0
  if( theGroup == 0 )
    error( "const PresentationForFNG& NGPresentationARCer::getPresentation() const:"
	   "You have not initialized the presentation");
#endif
  return *thePresentation;
}

void NGPresentationARCer::writeResults( ostream& out ) 
{
  out < *thePresentation;
}

void NGPresentationARCer::readResults( istream& in )
{
  in > *thePresentation;
}


// ------------------- NGdecomposeWordARCer --------------------- //


void NGdecomposeWordARCer::runComputation( )
{
  retWord = theNG->decompose(theWord);
}


void NGdecomposeWordARCer::setArguments( const NilpotentGroup& NG,
					 const Word& w )
{
  theNG = new NilpotentGroup(NG);
  theWord = w;
}


void NGdecomposeWordARCer::writeResults( ostream& out ) 
{
  out < retWord;
}

void NGdecomposeWordARCer::readResults( istream& in )
{
  in > retWord;
}
//---------------------------------------------------------------------------//
//------------------------- NGdecomposeWord ---------------------------------//
//---------------------------------------------------------------------------//


NGdecomposeWord::NGdecomposeWord(WCM& wcm)
  : ComputationManager( false ),
    theWCM(wcm),
    theWord(wcm.getWord()),
    started ( false ),
    arcer (*this)
{
  if (theWCM.getWord().wic.haveCollectedForm())
    resultIsFast();
  adminStart();
}


NGdecomposeWord::~NGdecomposeWord( )
{
}


void NGdecomposeWord::start( )
{
}


void NGdecomposeWord::takeControl( )
{
  if (fastResult()){
    adminTerminate();
    return;
  }
  if ( freeARCs() > 0 ) {
    if (theWord.getParent().gic.haveNilpotentGroupInited()){
      if (!started){
	arcer.setArguments(theWord.getParent().gic.getNilpotentGroupInited(),
			   theWord.getWord());
	started = true;
      }
      if (arcer.takeControl() ){
	theWord.wic.putHaveCollectedForm(arcer.decomposition());
	adminTerminate();
	return;
      }
    }
    usedOneARC();
  }
} 



void NGdecomposeWord::terminate( )
{
  
}


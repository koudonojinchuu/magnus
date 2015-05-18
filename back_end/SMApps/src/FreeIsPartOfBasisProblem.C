// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes
//           FreeIsPartOfBasisProblem, FreeIsPartOfBasis
//           FreeGeneralIsPartOfBasisProblem, FreeGeneralIsPartOfBasis
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//

#include "FreeIsPartOfBasisProblem.h"
#include "OutMessages.h"
#include "viewStructure.h"



// -------------------------- GAIsPartOfBasisArcer ---------------------- //


GAIsPartOfBasisArcer::GAIsPartOfBasisArcer( ComputationManager& boss )
  : ARCer( boss )
{ } 

void GAIsPartOfBasisArcer::setArguments( FreeGroup f,Word w )
{
  theGroup  =  f;
  theWord = w;
}

void GAIsPartOfBasisArcer::runComputation( ) 
{
  ACConfig c( 50, 1000000, 1, 
	      85, 95, 1, 
	      0, 1,
	      75, 5,15,5,0,0
	      );


 GAIsPartOfBasis ispart(theGroup,c,theWord);
 
 ispart.isPartOfBasis( compFile,resultFile );

 theAuto = ispart.getAutomorphism();

}

void  GAIsPartOfBasisArcer::writeResults( ostream& out )
{
  out < theAuto;
}

void GAIsPartOfBasisArcer::readResults( istream& in )
{
  in > theAuto;
}


// ---------------------------- GAIsPartOfBasisCM ------------------------ //


GAIsPartOfBasisCM::GAIsPartOfBasisCM( FreeIsPartOfBasisProblem& boss )
  : arcer( *this ), answer( dontknow ), theWord(boss.getWord()),  linkHasBeenSent(false)

{
 arcer.setArguments( boss.getWord().getParent().getFreePreimage(),
		     boss.getWord().getWord());
 adminStart();
}

void GAIsPartOfBasisCM::terminate( )
{
}

void GAIsPartOfBasisCM::start( ) { }


void GAIsPartOfBasisCM::takeControl( )
{
  if ( freeARCs() > 0 ) {
    
    if( !linkHasBeenSent ) {
      
      linkHasBeenSent = true;
      
      LogMessage msg( *this, theWord );
      msg << Link("Click here to see details of the genetic algorithm.",
		  "FreeIsPartOfBasis", 
		  getComputationFileName(), true);
      msg.send();
      
    }

    if( arcer.takeControl() ) {
      answer = yes;
      adminTerminate();
      return;
    }
    else
      usedOneARC();
  }
}

// -------------------- FreeIsPartOfBasisProblem --------------------------- //


FreeIsPartOfBasisProblem::FreeIsPartOfBasisProblem(const SMWord& w)
    : Supervisor( true ), theWord( w ), freeIsPartOfBasis( *this ),
      link1HasBeenSent( false ),
      gaIsPartOfBasisCM( *this )
{ }


void FreeIsPartOfBasisProblem::takeControl( )
{
  Trichotomy answer = freeIsPartOfBasis->answer();
  Trichotomy answerGA = gaIsPartOfBasisCM->getAnswer();

  if ( freeIsPartOfBasis.exists() && answer != dontknow ) {
    
    LogMessage msg( *this, theWord );
    if( answer == no )
      msg << Name(theWord) << " is not part of any free basis of "
	  << Name(theWord.getParent()) << ".";
    else {
      msg << Name(theWord) << " is part of the free basis (";

      VectorOf<Word> V = freeIsPartOfBasis->getAutomorphism( )
	.generatingImages();

      int VLen = V.length();
      FreeGroup F  = theWord.getParent().getFreePreimage();
      for( int i = 0; i < VLen; ++i ){
	msg << " ";
	F.printWord( msg, V[i] );
	if( i < VLen - 1)
	  msg << ",";
	else
	  msg << " ): according to the Whitehead algorithm.";
      }
    }
    
    msg.send();
    
    adminTerminate();
    return;
  }

  if ( answerGA != dontknow ){
     LogMessage msg( *this, theWord );
     msg << Name(theWord) << " is part of the free basis (";

     FreeGroup F = theWord.getParent().getFreePreimage();

     VectorOf<Word> invA = 
       F.inverseAutomorphism(gaIsPartOfBasisCM->getAutomorphism());

     int VLen = invA.length();
     for( int i = 0; i < VLen; ++i ){
       msg << " ";
       Word gw( Generator(i+1));
       gw = gw.replaceGenerators(invA);
       F.printWord( msg, gw );
       if( i < VLen - 1)
	  msg << ",";
       else
	 msg << " ): according to the genetic algorithm.";
     }
     
     msg.send();

     LogMessage msgl( *this, theWord );
     msgl << Link("Click here to see the result of the genetic algorithm.",
		  "FreeIsPartOfBasis", 
		  gaIsPartOfBasisCM->getResultFileName());
     msgl.send();
     
     adminTerminate();
     return;
  }
  
}


void FreeIsPartOfBasisProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Is part of free basis problem"),
		  Text("Is") + Name( theWord )
		  + "is part of a free basis in"
		  + Name( Parent( theWord ) ) + "?",
		  helpID("FreeIsPartOfBasisProblem", theWord.getParent()),
		  "Part of basis ?"
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of the Whitehead algorithm"), freeIsPartOfBasis.arcSlotID(), 100);
  pv.add(Text("For the use of the genetic algorithm"), gaIsPartOfBasisCM.arcSlotID(), 100);

  pv.done();
}


// ------------------------ FreeIsPartOfBasis ------------------------------ //


FreeIsPartOfBasis::FreeIsPartOfBasis(FreeIsPartOfBasisProblem& problemObject)
    : theWord( problemObject.getWord() ),
      theAnswer( dontknow ), theGroup( theWord.getParent().getFreePreimage() ),
      theAutomorphism( theGroup, theGroup ),
      GW( new GeneralWhitehead( theGroup, true ) ),
      theFileName( GW->getFileName() ),
      linkHasBeenSent( false )
{
  GW->startComputation( theWord.getWord() );
  adminStart();
}


FreeIsPartOfBasis::~FreeIsPartOfBasis( )
{
  delete GW;
}


Map FreeIsPartOfBasis::getAutomorphism( ) const
{
#if SAFETY > 0
  if ( theAnswer != yes )
	 error("Called FreeIsPartOfBasis::getAutomorphism "
	       "when theAnswer != yes");
#endif
  return theAutomorphism;
}


void FreeIsPartOfBasis::takeControl( )
{
 if ( freeARCs() > 0 ) {

   if( !linkHasBeenSent ) {
     
     linkHasBeenSent = true;
     
     LogMessage msg( *this, theWord );
     msg << Link("Click here to obtain details of the computation.",
		 "FreeIsPartOfBasis", getFileName(), true);
     msg.send();
   }
   
    
    if ( GW->continueComputation() ) {
      
      if( GW->extendsToFreeBasis() )
	theAnswer = yes;
      else
	theAnswer = no;
      
      if ( theAnswer == yes )
	theAutomorphism = GW->getAutomorphism();

      delete GW;
      GW = 0;

      adminTerminate();
      return;
    }
    
    usedOneARC();
  }
}


// ------------------ FreeGeneralIsPartOfBasisProblem ---------------------- //


FreeGeneralIsPartOfBasisProblem::FreeGeneralIsPartOfBasisProblem
  (const SMSetOfWords& S)
    : Supervisor( true ), theSet( S ), freeIsPartOfBasis( *this ),
      linkHasBeenSent( false )
{ }


void FreeGeneralIsPartOfBasisProblem::takeControl( )
{
  if( !linkHasBeenSent ) {

    linkHasBeenSent = true;

    LogMessage msg( *this, theSet );
    msg << Link("Click here to obtain details of the computation.",
		"FreeIsPartOfBasis", freeIsPartOfBasis->getFileName(), true);
    msg.send();
   
  }
  
  Trichotomy answer = freeIsPartOfBasis->answer();

  if ( freeIsPartOfBasis.exists() && answer != dontknow ) {
    
    LogMessage msg( *this, theSet );
    if( answer == no )
      msg << Name(theSet) << " is not part of any free basis of "
	  << Name(theSet.getParent()) << ".";
    else {
      msg << Name(theSet) << " is part of the free basis (";

      VectorOf<Word> V = freeIsPartOfBasis->getAutomorphism( )
	.generatingImages();

      int VLen = V.length();
      FreeGroup F  = theSet.getParent().getFreePreimage();
      for( int i = 0; i < VLen; ++i ){
	msg << " ";
	F.printWord( msg, V[i] );
	if( i < VLen - 1)
	  msg << ",";
	else
	  msg << " ).";
      }
    }
    
    msg.send();
    adminTerminate();
  }
}


void FreeGeneralIsPartOfBasisProblem::viewStructure(ostream& ostr) const
{
  ProblemView pv( ostr, oid(), Text("Is part of free basis problem"),
		  Text("Is") + Name( theSet )
		  + "is part of a free basis in"
		  + Name( Parent( theSet ) ) + "?",
		  helpID("FreeGeneralIsPartOfBasisProblem", theSet.getParent()),
		  "Part of basis ?"
		  );

  pv.startItemGroup();

  pv.add(Text("For the use of this problem"), freeIsPartOfBasis.arcSlotID(), 100);

  pv.done();
}


// ----------------------- FreeGeneralIsPartOfBasis ------------------------ //


FreeGeneralIsPartOfBasis::FreeGeneralIsPartOfBasis
  (FreeGeneralIsPartOfBasisProblem& problemObject)
    : theSet( problemObject.getSetOfWords() ),
      theAnswer( dontknow ), theGroup( theSet.getParent().getFreePreimage() ),
      theAutomorphism( theGroup, theGroup ),
      GW( new GeneralWhitehead( theGroup, true ) ),
      theFileName( GW->getFileName() )
{
  GW->startComputation( theSet.getWords() );
  adminStart();
}


FreeGeneralIsPartOfBasis::~FreeGeneralIsPartOfBasis( )
{
  delete GW;
}


Map FreeGeneralIsPartOfBasis::getAutomorphism( ) const
{
#if SAFETY > 0
  if ( theAnswer != yes )
	 error("Called FreeGeneralIsPartOfBasis::getAutomorphism "
	       "when theAnswer != yes");
#endif
  return theAutomorphism;
}


void FreeGeneralIsPartOfBasis::takeControl( )
{
  if ( freeARCs() > 0 ) {
    
    if ( GW->continueComputation() ) {
      
      if( GW->extendsToFreeBasis() )
	theAnswer = yes;
      else
	theAnswer = no;
      
      if ( theAnswer == yes )
	theAutomorphism = GW->getAutomorphism();

      delete GW;
      GW = 0;

      adminTerminate();
      return;
    }
    
    usedOneARC();
  }
}

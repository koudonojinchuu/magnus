// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes GeneticWPArcer, GeneticWPCM
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#include "GeneticProblems.h"
#include "OutMessages.h"
#include "viewStructure.h"
#include "GAWP.h"


// -------------------------- GeneticWPArcer ------------------------------- //


GeneticWPArcer::GeneticWPArcer( ComputationManager& boss )
  : ARCer( boss ), retValue(dontknow), file( 0 )
{ } 

void GeneticWPArcer::setArguments( const FPGroup& group, 
				   const SetOf<Word>& words, 
				   DetailType dt )
{
  theGroup = group;
  theWords = words;
  dtype = dt;
  if( dtype != NO_DETAILS ) 
    file = new File;
}

Chars GeneticWPArcer::getFileName() 
{
#if SAFETY > 0
  if( file == 0 )
    error("Chars GeneticWPArcer::getFileName( ) : "
	  "You chose not to keep details in a file.");
#endif

  return file->getFileName(); 
}

void GeneticWPArcer::runComputation( ) 
{
  GHNConfig c;
  GAWP2 gawp(theGroup,c);
  if( dtype == WORD ) {
    *file << "The algorithm tries to reduce length of the word genetically. "
      "The fitness value below is the lowest length produced on the "
      "current generation. " << endl << endl << ready;
  } else if ( dtype == SET_OF_WORDS ) {
    *file << "The algorithm tries to reduce length of the words genetically. "
      "The fitness value below is the lowest length produced on the "
      "current generation. " << endl << endl << ready;
  }  else if ( dtype == COMMUTATORS ) {
    *file << "The algorithm tries to reduce length of the commutators "
      "genetically. "
      "The fitness value below is the lowest length produced on the "
      "current generation. " << endl << endl << ready;
  } 
  
  SetIterator<Word> I(theWords);
  retValue = yes;
  for(; !I.done(); I.next() ) {

    if ( dtype != NO_DETAILS && dtype != WORD ) {
      *file << endl << "Solving the word problem for w = ";
      theGroup.printWord(*file, I.value());
      *file << endl << endl << ready;
    } 
    
    retValue = gawp.isTrivial(I.value(), file);
    if( retValue != yes ) break;
  }
}

void  GeneticWPArcer::writeResults( ostream& out )
{
  out < retValue;
}

void GeneticWPArcer::readResults( istream& in )
{
  in > retValue;
}


// ---------------------------- GeneticWPCM -------------------------------- //


GeneticWPCM::GeneticWPCM( Supervisor& boss )
: theBoss( boss ),
  tAreTrivial( dontknow ),
  bStarted( false ),
  bInited( false ),
  arcer( *this )
{
  adminStart();
}

void GeneticWPCM::terminate( )
{
  //arcer.terminate(); 
}

void GeneticWPCM::start( ) { }

void GeneticWPCM::init( const FPGroup& group, const SetOf<Word> words,
			DetailType dt )
{
  theGroup = group;
  theWords = words;
  dtype = dt;
  bInited = true;
}

void GeneticWPCM::takeControl( )
{
  if ( freeARCs() > 0 ) {
    if( !bStarted ) {

    #if SAFETY > 0
      if( !bInited )
	error("void GeneticWPCM::takeControl( ) : "
	      "The computation manager was not initialized.");
    #endif  
    
      arcer.setArguments( theGroup, theWords, dtype );
      bStarted = true;
      if( dtype != NO_DETAILS ) {
	LogMessage msg( *this, theBoss );
	msg << Link("Click here to see details of the genetic algorithm 1.",
		    "GeneticWP", getFileName(), true);
	msg.send(); 
      }
    }
    if( arcer.takeControl() ) {
      tAreTrivial = arcer.getRetValue();
      adminTerminate();
      return;
    }
    else
      usedOneARC();
  }
}

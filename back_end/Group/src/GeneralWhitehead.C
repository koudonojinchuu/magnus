// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the GeneralWhitehead class
//
// Principal Authors: Dmitry Bormotov
//
// Status: 
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


//#define debug_GeneralWhitehead


#include "GeneralWhitehead.h"


// -------------------------  GeneralWhitehead ---------------------------- //


GeneralWhitehead::GeneralWhitehead( const FreeGroup& F, bool keepDetails )
  : theGroup( F ),
    bStart( false ),
    bDone( false ),
    numberOfGenerators( F.numberOfGenerators() ), 
    images( F.numberOfGenerators() ), 
    automorphism( F, F ),
    timer( 0 ),
    numOfAuto(0),
    file( 0 ),
    hasOneWordOnly( false )
{
  if( keepDetails )
    file = new File;
}


GeneralWhitehead::~GeneralWhitehead( ) 
{
  delete [] numOfAuto;
  numOfAuto = 0;
  delete file;
}


void GeneralWhitehead::startComputation( const Word& w )
{
  hasOneWordOnly = true;
  VectorOf<Word> V(1);
  V[0] = w;
  startComputation(V);
}


void GeneralWhitehead::startComputation( const VectorOf<Word>& V )
{
#if SAFETY > 0   
  if ( bStart )
    error("void WhiteheadAutomorphisms::startComputation( ) : "
	  "the computation has been already started.");
#endif
  
  bStart = true;
  bDone = false;

  // Updating the file

  if( file ) {
    file->setColor(titleColor);
    
    if( hasOneWordOnly )
      *file << "Original word:";
    else
      *file << "Original set of words:";

    *file << endl << "  ";
    file->setColor(mainColor);
    
    if( hasOneWordOnly )
      theGroup.printWord(*file, V[0]);
    else
      theGroup.printVectorOfWords(*file, V);
    
    *file << endl << endl << ready;
  }

  // make an identical automorphism

  for( int i = 0; i < numberOfGenerators; ++i )
    images[i] = Generator(i+1);
  automorphism = Map(theGroup, theGroup, images);

  theVector = V;
  theVectorLen = theVector.length();
  theVectorCommonLength = commonLength(theVector);
    
  if( theVectorLen == 0 ) {
    setResult(true);
    return;
  }

  for( int i = 0; i < theVectorLen; ++i )
    if( Word(theVector[i]).length() == 0 ) {
      setResult(false);
      return;
    }

  if( theVectorLen > numberOfGenerators ) {
    setResult(false);
    return;
  }


  SetOf<Word> S;
  for( int i = 0; i < theVectorLen; ++i )
    S |= Word(theVector[i]);

  if( S.cardinality() < theVectorLen ) {
    setResult(false);
    return;
  }

  
  bool allOnes = true;
  for( int i = 0; i < theVectorLen; ++i )
    if( Word(theVector[i]).length() > 1 ) {
      allOnes = false;
      break;
    }
      
  if( allOnes ) {
    prepareAutomorphism(automorphism);
    setResult(true);
    return;
  }

  // Fast checks:
  // 1. Is an exponent sum for all generators in some word is equal to 0,
  //    this set doesn't extend to a free basis.

  for( int i = 0; i < theVectorLen; ++i ) {
     
    Word W = theVector[i];
    int WLen = W.length();
    
    bool allSumAreZero = true;
    for( int j = 0; j < WLen; ++j )
      if( W.exponentSum( W[j] ) != 0 ) {
	allSumAreZero = false;
	break;
      }
    
    if( allSumAreZero ) {
      setResult(false);
      return;
    }
  }
  
#ifdef debug_GeneralWhitehead
  
  numOfShortenings = 0;
  
#endif

  numOfAuto = new int[numberOfGenerators];
  itIsInterrupted = false;
}  


int GeneralWhitehead::commonLength( const VectorOf<Word>& V )
{
  int VLen = V.length();
  int result = 0;
  for( int i = 0; i < VLen; ++i )
    result += Word(V[i]).length();
  return result;
}

 
bool GeneralWhitehead::continueComputation( ) 
{
#if SAFETY > 0
  if ( !bStart )
    error("void WhiteheadAutomorphisms::continueComputation( ) : "
	  "tried to continue computation before it's started");
#endif
  
  if( bDone )
    return bDone;
  
  bool wasShortened;
  timer.reset(timerValue);
    
  while( true ) { // cycle by common length of the origin set

    int n;
    if( itIsInterrupted )
      n = saveN;
    else
      n = 1;
    
    while( n <= numberOfGenerators ) { // cycle by all generators

    #ifdef debug_GeneralWhitehead
    
      cout << "a = " << n << endl;
      
    #endif
    
      // do the deal with the set 
      
      Generator a = n;
      Generator A = -n;

      if( !itIsInterrupted ) {
	memset(numOfAuto, 0, numberOfGenerators * sizeof(int));
	for( int i = 0; i < numberOfGenerators; ++i )
	  images[i] = Generator(i+1);
      }
      
      wasShortened = false;
      itIsInterrupted = false;

      while( true ) {

	if( timer.expired() ) {
	  saveN = n;
	  itIsInterrupted = true;
	  return bDone;
	}
	
        // apply Whitehead's automorphism

	VectorOf<Word> theNewVector(theVectorLen);
	for( int i = 0; i < theVectorLen; ++i ) { 

	  Word W = Word(theVector[i]);
	  int WLen = W.length();
	  Word result;
	  
	  for( int j = 0; j < WLen; ++j )
	    result *= makeWhiteheadAutomorpism
	      ( W[j], numOfAuto[abs(ord(W[j])) - 1], a );
	  result = result.freelyReduce();
	  theNewVector[i] = result;
	}
	
	int theNewVectorCommonLength = commonLength(theNewVector);
	if( theNewVectorCommonLength < theVectorCommonLength ) {

	  theVector = theNewVector;
	  theVectorCommonLength = theNewVectorCommonLength;
	  automorphism = automorphism | Map(theGroup, theGroup, images);
	  wasShortened = true;

	  if( file ) { // Updating the file
	    file->setColor(titleColor);
	    *file << "Applying Whitehead automorphism:" << endl << "  ";
	    file->setColor(mainColor);
	    *file << Map(theGroup, theGroup, images) << endl << endl;
	    file->setColor(titleColor);
	    *file << "Result:" << endl << "  ";
	    file->setColor(mainColor);
	    
	    if( hasOneWordOnly )
	      theGroup.printWord(*file, theVector[0]);
	    else
	      theGroup.printVectorOfWords(*file, theVector);
	    
	    *file << endl << endl << ready;
	  }

	  break;
	}
	
	// prepare the next Whitehead's automorphism
	
	int current = numberOfGenerators-1;
	
	while( current >= 0 ) {
	  if( ++numOfAuto[current] > 3) {
	    numOfAuto[current] = 0;
	    images[current] = Generator(current+1);
	    --current;
	  }
	  else {

	    if( numOfAuto[current] == 1 ) {
	      if( Generator(current+1) == A )
		continue;
	    }
	    else
	      if( numOfAuto[current] == 2 )
		if( Generator(current+1) == a )
		  continue;

	    images[current] = makeWhiteheadAutomorpism(Generator(current+1),
						       numOfAuto[current], a);
	    break;
	  }
	}
	
	if( current < 0 )
	  break;
      }
      
      if( wasShortened )
	break;
      
      // advance n
      
      if( n > 0 )
	n = -n;
      else
	n = 1 - n;
    }
    
    if( !wasShortened ) {
      finishComputation(false);
      return bDone;
    }

  #ifdef debug_GeneralWhitehead
    
    cout << "The vector " << theVector <<  " was shortened, " 
         << ++numOfShortenings << " times." << endl;
    cout << "Images: " << images << endl;
    cout << "Automorphism: " << automorphism << endl;
    
  #endif
    
    for( int i = 0; i < theVectorLen; ++i )
      if( Word(theVector[i]).length() == 0 ) {
	finishComputation(false);
	return bDone;
      }

    bool allOnes = true;
    for( int i = 0; i < theVectorLen; ++i )
      if( Word(theVector[i]).length() > 1 ) {
	allOnes = false;
	break;
      }

    if( allOnes ){

      for( int i = 0; i < theVectorLen-1; ++i )
	for( int j = i+1; j < theVectorLen; ++j )
	  if( Word(theVector[i]) == Word(theVector[j]) ) {
	    finishComputation(false);
	    return bDone;
	  }
      
      prepareAutomorphism(automorphism);
      finishComputation(true);
      return bDone;
    }
  }
}


void GeneralWhitehead::prepareAutomorphism( Map& automorphism ) const
{
  VectorOf<Word> images(numberOfGenerators);
  Generator g;
  VectorOf<Word> V = theVector;
  
#ifdef debug_GeneralWhitehead
  cout << "Prepare automorphism:" << endl;
#endif

  for( int i = 0; i < theVectorLen; ++i ) {
    if( (g = (Word(V[i]))[0]) != Generator(i+1) ) {

      for( int j = 0; j < numberOfGenerators; ++j )
	images[j] = Generator(j+1);

      images[i] = g;
      if( ord(g) > 0 )
	images[abs(ord(g))-1] = Generator(i+1);
      else
	images[abs(ord(g))-1] = Generator(-i-1);

      Map newMap(theGroup, theGroup, images);
      automorphism = automorphism | newMap;
      for( int j = 0; j < theVectorLen; ++j )
	V[j] = newMap.imageOf(V[j]);

    #ifdef debug_GeneralWhitehead
      cout << "V: " << V << endl;
      cout << "Images: " << images << endl;
      cout << "Automorphism: " << automorphism << endl;
    #endif
    }
  }

  automorphism = theGroup.inverseAutomorphism( automorphism );

#ifdef debug_GeneralWhitehead
      cout << "The final automorphism: " << automorphism << endl;
#endif
}


Word GeneralWhitehead::makeWhiteheadAutomorpism( Generator generator,
						 int typeOfAuto,
						 Generator a ) const
{
  Generator A = -ord(a);
  Generator absGen = abs(ord(generator));
  Word result;

  switch( typeOfAuto ) {
  case 0: 
    result = absGen;
    break;
  case 1: 
    result = absGen * a;
    break;
  case 2: 
    result = A * absGen;
    break;
  case 3: 
    result = A * absGen * a;
    break;

#if SAFETY > 0
  default:
    error("Unknown type of Whitehead's automorphism in "
	  "Word WhiteheadAutomorpisms::makeWhiteheadAutomorpism"
	  "(Generator, int, Generator) const;");
#endif
  }
  
  if( ord(generator) > 0 )
    return result;
  else
    return result.inverse();
}


void GeneralWhitehead::setResult( bool DoesExtend )
{
  doesExtend = DoesExtend;
  bDone = true;
  if( file ) {
    *file << end; // finish output to the file
    delete file;
    file = 0;
  }
}

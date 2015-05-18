// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the AbelianInfinitenessProblem class
//
// Principal Authors: Dmitry Bormotov
//
// Status: 
//
// Revision History:
//


#include "Set.h"
#include "AbelianInfinitenessProblem.h"


// ---------------------  AbelianInfinitenessProblem ------------------------//


void AbelianInfinitenessProblem::startComputation( )
{
#if SAFETY > 0   
  if ( bStart )
    error("void AbelianInfinitenessProblem::startComputation( ) : "
	  "the computation has been already started.");
#endif
  
  bStart = true;
  bDone = false;

  width = theGroup.numberOfGenerators();
  SetOf<Word> relators = theGroup.getRelators();
  height = relators.cardinality();

  if( width == 0 ) {
    itIsInfinite = false;
    finishComputation();
    return;
  }
  
  if( ( height = relators.cardinality() ) < width ) {
    itIsInfinite = true;
    finishComputation();
    return;
  }
      
  SetIterator<Word> I( relators );
  int i = 0, j;
  matrix = new Matrix<Rational>(height,width);
  
  // matrix[i][j] = exponent of j^{th} generator in i^{th} relation.
  while ( !I.done() ) {
    Word w = I.value();
    for ( j = 0; j < width; j++ ) {
      Generator g(j+1);
      (*matrix)[i][j] = w.exponentSum( g );
    }
    i++;
    I.next();
  }

  GT = new GaussTransformation<Rational>(*matrix);
  GT->startComputation();
}  


void AbelianInfinitenessProblem::finishComputation( )
{
  if( width > 0 && height >= width ) {
    delete GT;
    delete matrix;
  }
  
  bDone = true;
//  bStart = false;
}

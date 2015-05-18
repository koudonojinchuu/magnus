// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the HomomorphismBuilder class
//
// Principal Authors: Dmitry Bormotov
//
// Status: in development
//
// Revision History:
//

#include "Set.h"
#include "RandomMatrix.h"
#include "GaussTransformation.h"
#include "HomomorphismBuilder.h"


// ------------------------- HomomorphismBuilder ----------------------------//


template <class R>
void HomomorphismBuilder<R>::continueComputation( )
{
#if SAFETY > 0
  if ( !bStart )
    error("void HomomorphismBuilder<R>::continueComputation( ) : "
	  "tried to continue computation before it's started");
  if ( bDone )
    error("void HomomorphismBuilder<R>::continueComputation( ) : "
	  "tried to continue computation after it's done");
#endif

#ifdef DEBUG 
  iterCount++;
  if (iterCount % 100 == 0)
    cout << iterCount << endl;
#endif
  
  RandomMatrix<R> RM(matrixSize);
  int numOfGens = group.numberOfGenerators();
  for( int i = 0; i < numOfGens; i++ ) {
    bool bSuccess;
    do {
      Matrix<R> M = RM.getRandomMatrix();
      homomorphism[i] = M;
      Inverse<R> Inv( M );
      Inv.startComputation();
      while( !Inv.done() ) Inv.continueComputation();
      bSuccess = Inv.isInvertible();
      if( bSuccess )
	invertedMatrices[i] = Inv.getInverseMatrix();
    } while( !bSuccess );
  }

  SetOf<Word> relators = group.getRelators();
  if( relators.cardinality() == 0 ) {
    finishComputation();
    return;
  }

  SetIterator<Word> I(relators);

  for( ; !I.done(); I.next() ) {
    Matrix<R> result;
    Word relator = I.value();
    int relatorLen = relator.length();

    int num = ord( relator[0] );
    if( num > 0 )
      result = homomorphism[num-1];
    else
      result = invertedMatrices[-num-1];
      
    for( int i = 1; i < relatorLen; i++ ) {
      int num = ord( relator[i] );
      if( num > 0 )
	result *= homomorphism[num-1];
      else
	result *= invertedMatrices[-num-1];
    }
    
    if( !result.isIdentity() ) return;
  }

  finishComputation();
}


template class HomomorphismBuilder<Integer>;
template class HomomorphismBuilder<Rational>;

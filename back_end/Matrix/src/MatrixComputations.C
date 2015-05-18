// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementations of class SquareMatrix. 
//
// Principal Author:  Alexey Myasnikov
//
// Status: In development
//
// Usage:
//
// Discussion:
//
// Revision History:
//
// Next Implementation Steps:
//


#include "MatrixComputations.h"

template <class R>
bool MatrixComputations<R>::isIdentity() const
{
  for (int i=0;i<size();i++)
    for (int j=0;j<size();j++){
      if (j==i && theMatrix[i][j]!=R(1)) 
               return false;
     if (j!=i && theMatrix[i][j]!=R(0)) 
               return false;
     } 
  return true;
}

template <class R>
R MatrixComputations<R>::getDeterminant()
{
  
  GaussTransformation<R> *GT= new GaussTransformation<R>(theMatrix);
  GT->startComputation();
  GT->runUntilDiagHasZero();
  //  cout << GT->getMatrix();
  if( (GT->isSingular()) == YES )
    determinant = R(0);
  else {
    determinant = R(1);
    for ( int i = 0; i < size(); i++ )
      determinant *= GT->getMatrix()[i][i];
  }
  detKnown = true;
  delete GT;
  return determinant; 
}
//------------------------ Inverse Matrix Methods --------------------//
template <class R>
void MatrixComputations<R>::invertMatrix()
{
  int theSize = size();
  Matrix<R> tmpMatrix(theSize,theSize*2);
  for( int i = 0; i < theSize; i++ ) {
    for( int j = 0; j < theSize; j++ ) {
      tmpMatrix[i][j] = theMatrix[i][j];
      tmpMatrix[i][theSize+j] = R(0);
    }
    tmpMatrix[i][theSize+i] = R(1);
  }
  GaussTransformation<R> GT(tmpMatrix);
  GT.startComputation();
  GT.runWhileDiagHasSingles();
  if (GT.isInvertible() == no){
       isInvertible = no;
       return;
  }  
  if (!GT.done()) GT.run();
   if( (GT.isInvertible()) != no ) {
    int RealSize = theSize * 2;
    tmpMatrix = GT.getMatrix();
    for(int i = 0; i < theSize - 1; i++ ) 
      for( int j = i + 1; j < theSize; j++ )
	if( tmpMatrix[i][j] != R(0) ) {
	  R quotient = tmpMatrix[i][j] / tmpMatrix[j][j];
	  for(int k = j; k < RealSize; k++ )
	    tmpMatrix[i][k] -= quotient * tmpMatrix[j][k];
	}

    abolishCoefficients(tmpMatrix);
    inverseMatrix = new Matrix<R>(theSize);
    for(int i = 0; i < theSize; i++ )
      for( int j = 0; j < theSize; j++ )
	(*inverseMatrix)[i][j] = tmpMatrix[i][theSize+j];
  }
  isInvertible = yes;
  return;
}

template <> void MatrixComputations<Integer>::abolishCoefficients(Matrix<Integer>& matrix )
{
  int realSize = size() * 2;
  for( int i = 0; i < size(); i++ )
    if( sign(matrix[i][i]) < 0 )
      for( int j = i; j < realSize; j++ )
	matrix[i][j] = - matrix[i][j];
}

template <> void MatrixComputations<Rational>::abolishCoefficients(Matrix<Rational>& matrix )
{
  int realSize = size() * 2;
  for( int i = 0; i < size(); i++ )
    if( matrix[i][i] != Rational(1) ) {
      Rational koef = matrix[i][i];
      for( int j = i; j < realSize; j++ )
	matrix[i][j] /= koef;
    }
}

template <> void MatrixComputations<double>::abolishCoefficients(Matrix<double>& matrix )
{
  int realSize = size() * 2;
  for( int i = 0; i < size(); i++ )
    if( matrix[i][i] != double(1) ) {
      double koef = matrix[i][i];
      for( int j = i; j < realSize; j++ )
	matrix[i][j] /= koef;
    }
}


template class MatrixComputations<Integer>;
template class MatrixComputations<Rational>;
template class MatrixComputations<double>;



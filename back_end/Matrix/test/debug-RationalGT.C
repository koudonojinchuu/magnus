// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Informal tests of class GaussTransformation.
//
// Principal Authors: Dmitry Bormotov
//


#include "Matrix.h"
#include "GaussTransformation.h"


int main( )
{

/*
  const int size = 2;
  Matrix<Integer> M(size);
  
  for( int i = 0; i < size; i++ )
    for( int j = 0; j < size; j++ )
      M[i][j]  = i * size + j; 

  cout << "M = " << M << endl;

  GaussTransformation GT(M.theMatrix, M.size, M.size);
  GT.startComputation();
  while( !GT.done() ) GT.continueComputation();
  GT.printMatrix("M' = ");
*/

/*
  const int size = 3;
  Matrix<Integer> M(size);
  
  for( int i = 0; i < size; i++ )
    for( int j = 0; j < size; j++ )
      M[i][j]  = i * size + j + 1; 

  cout << "M = " << M << endl;
*/

  Matrix<Rational> M;
  cout << "Enter a matrix: " << endl;

  Chars errMsg = cin >> M;
  if( errMsg.length() > 0 ) {
    cout << errMsg << endl;
    exit(-1);
  }
 
  cout << "You entered: " << M << endl;

  Determinant<Rational> D(M);

  D.startComputation();

  while( !D.done() ) D.continueComputation();
  cout << "determinant = " << D.getDeterminant() << endl;

/*
  Matrix<Rational> M;
  cout << "Enter a matrix: " << endl;

  Chars errMsg = cin >> M;
  if( errMsg.length() > 0 ) {
    cout << errMsg << endl;
    exit(-1);
  }
 
  cout << "You entered: " << M << endl;

  Inverse<Rational> I(M);

  I.startComputation();
  while( !I.done() ) I.continueComputation();

  if( I.isInvertible() ) {
    cout << "Inverse matrix = " << I.getInverseMatrix() << endl;
  }
  else
    cout << "This matrix isn't invertible." << endl;
 */


}









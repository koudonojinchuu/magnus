// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Informal tests of class RandomMatrix.
//
// Principal Authors: Dmitry Bormotov
//


#include "RandomMatrix.h"
#include "GaussTransformation.h"


int main( )
{
/*
  srand(1335);
  RandomMatrix<Integer> RM(2);
  for( int i = 0; i < 10; i++ ) {
    Matrix<Integer> M = RM.getRandomMatrix();
    cout << "M = " << M << endl;
    Determinant<Integer> D(M);
    D.startComputation();
    while (!D.done()) D.continueComputation();
    cout << "determinant = " << D.getDeterminant() << endl;
  }
*/

  srand(1335);
  RandomMatrix<Rational> RM(2);
  for( int i = 0; i < 10; i++ ) {
    Matrix<Rational> M = RM.getRandomMatrix();
    cout << "M = " << M << endl;
    Determinant<Rational> D(M);
    D.startComputation();
    while (!D.done()) D.continueComputation();
    cout << "determinant = " << D.getDeterminant() << endl;
  }
}









// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: 
//
// Principal Authors: Dmitry Bormotov
//

#include "RandomMatrix.h"
#include "GaussTransformation.h"

void foo( int& ref )
{
  cout << ref << endl;
  ++ref;
  cout << ref << endl;
}
  

int main( )
{
/*
  srand(1335);

  const int size = 2;
  const int count = 5;
  RandomMatrix<Integer> RM(size);
  VectorOf< Matrix<Integer> > V(count);
  VectorOf< Matrix<Integer> > R(count);

  for( int i = 0; i < count; i++ ) {
    bool bSuccess;
    do {
      V[i] = RM.getRandomMatrix();
      Matrix<Integer> M = V[i];
      Inverse<Integer> Inv(M);
      Inv.startComputation();
      while( !Inv.done() ) Inv.continueComputation();
      bSuccess = Inv.isInvertible();
      if( bSuccess )
	R[i] = Inv.getInverseMatrix();
    } while( !bSuccess );
  }

  for( i = 0; i < count; i++ )
    cout << "V[" << i << "] = " << V[i] << endl;

  for( i = 0; i < count; i++ )
    cout << "R[" << i << "] = " << R[i] << endl;
*/

  int A[2];
  A[0] = 0;
  A[1] = 1;
  int& ref = A[0];
  cout << ref << endl;
  foo(ref);
  cout << ref << endl;
}
  








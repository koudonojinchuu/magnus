// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Informal tests of classes Matrix
//
// Principal Authors: Dmitry Bormotov
//


#include "Integer.h"
#include "WordParser.h"
#include "Matrix.h"


void main( )
{
  const int size = 2;

  Matrix<Integer> M0(size), M1(size), M2(size), M(size);

  for( int i = 0; i < size; i++ )
    for( int j = 0; j < size; j++ ) {
      M0[i][j] = 0;
      M1[i][j] = 1;
      M2[i][j] = 2;
      M[i][j]  = i * size + j; 
    }

  cout << "M0 = " << M0 << endl;
  cout << "M1 = " << M1 << endl;
  cout << "M2 = " << M2 << endl;
  cout << "M  = " << M  << endl;

  cout << "M1 == M2 " << ( M1 == M2 ? "YES" : "NO" ) << endl;
  cout << "M1 + M1 == M2 " << ( M1 + M1 == M2 ? "YES" : "NO" ) << endl;

  cout << "M + M1 = " << M + M1 << endl;
  cout << "M - M2 = " << M - M2 << endl;
  cout << "M * M2 = " << M * M2 << endl;

  Matrix<Integer> X(size);
  X = M;
  cout << "X = " << X << endl;

  X += M1;
  cout << "X += M1 : " << X << endl;
  X -= M2;
  cout << "X -= M2 : " << X << endl;
  X *= X;
  cout << "X *= X  : " << X << endl;
  cout << "- X = " << -X << endl;

  cout << "Enter a matrix: " << endl;

  Chars errMsg = cin >> X;
  if( errMsg.length() > 0 ) {
    cout << errMsg << endl;
    exit(-1);
  }
 
  cout << "You entered: " << X << endl;

}





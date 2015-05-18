// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Informal tests of class Matrix<Rational>
//
// Principal Authors: Dmitry Bormotov
//


#include "Rational.h"
#include "WordParser.h"
#include "Matrix.h"


void main( )
{
  const int size = 2;
  Matrix<Rational> M1_2(size), M1_3(size), M1_4(size), M(size);

  for( int i = 0; i < size; i++ )
    for( int j = 0; j < size; j++ ) {
      M1_2[i][j] = Rational(1,2);
      M1_3[i][j] = Rational(1,3);
      M1_4[i][j] = Rational(1,4);
      M[i][j]    = Rational(i * size + j + 1,5); 
    }

  cout << "M1_2 = " << M1_2 << endl;
  cout << "M1_3 = " << M1_3 << endl;
  cout << "M1_4 = " << M1_4 << endl;
  cout << "M  = " << M  << endl;

  cout << "M1_3 == M1_4 " << ( M1_3 == M1_4 ? "YES" : "NO" ) << endl;
  cout << "M1_4 + M1_4 == M1_2 " << ( M1_4 + M1_4 == M1_2 ? "YES" : "NO" )
       << endl;

  cout << "M + M1_3 = " << M + M1_3 << endl;
  cout << "M - M1_4 = " << M - M1_4 << endl;
  cout << "M * M1_4 = " << M * M1_4 << endl;

  Matrix<Rational> X(size);
  X = M;
  cout << "X = " << X << endl;

  X += M1_3;
  cout << "X += M1_3 : " << X << endl;
  X -= M1_4;
  cout << "X -= M1_4 : " << X << endl;
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





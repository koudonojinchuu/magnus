// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Informal tests of class Polynomial
//
// Principal Authors: Dmitry Bormotov
//


#include "global.h"
#include "Integer.h"
#include "Polynomial.h"

void main( )
{

/*
  Monomial<Integer> M;
  cout << "Enter a monomial: " << endl;

  Chars errMsg = cin >> M;
  if( errMsg.length() > 0 ) {
    cout << errMsg << endl;
    exit(-1);
  }
 
  cout << "You entered: " << M << endl;
*/

/*
  Monomial<Rational> M;
  cout << "Enter a monomial: " << endl;

  Chars errMsg = cin >> M;
  if( errMsg.length() > 0 ) {
    cout << errMsg << endl;
    exit(-1);
  }
 
  cout << "You entered: " << M << endl;
*/

/*
  Monomial<Integer> M("5 x1 x2");
  cout << "M = " << M << endl;
*/

  Polynomial<Integer> P;
  cout << "Enter a polynomial: " << endl;

  Chars errMsg = cin >> P;
  if( errMsg.length() > 0 ) {
    cout << errMsg << endl;
    exit(-1);
  }
 
  cout << "You entered: " << P << endl;
  
}


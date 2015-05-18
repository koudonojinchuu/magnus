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
  Polynomial<Integer> P;
  cout << "Enter polynomial P: " << endl;

  Chars errMsg = cin >> P;
  if( errMsg.length() > 0 ) {
    cout << errMsg << endl;
    exit(-1);
  }
 
  cout << "You entered P: " << P << endl;

  Polynomial<Integer> Q;
  cout << "Enter polynomial Q: " << endl;

  errMsg = cin >> Q;
  if( errMsg.length() > 0 ) {
    cout << errMsg << endl;
    exit(-1);
  }
 
  cout << "You entered Q: " << Q << endl;

  cout << "-P = " << -P << endl;

  cout << "P + Q: " << P + Q << endl;

  cout << "P - Q: " << P - Q << endl;

  cout << "P * Q: " << P * Q << endl;
  
  cout << "P == Q: " << (P == Q ? "YES" : "NO") << endl;

  cout << "Listing monomials of P: " << endl;

  PolynomialIterator<Integer> I(P);
  for( ; !I.done(); I.next() )
    cout << I.getValue() << endl;
  cout << endl;
  I.reset();
  if( !I.done() ) {
    int n = I.getValue().getNumberOfVariables();
    cout << "The first monomial of P:" << I.getValue() << endl
	 << "Coefficient: " << I.getValue().getCoefficient() << endl
	 << "Number of variables: " << n << endl
	 << "Powers: ";
    for( int i = 0; i < n; ++i)
      cout << I.getValue().getPowerOfVariable(i) << " ";
  }
  cout << endl;
}


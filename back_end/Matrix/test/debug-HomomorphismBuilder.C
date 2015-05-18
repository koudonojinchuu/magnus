// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Informal tests of class HomomorphismBuilder.
//
// Principal Authors: Dmitry Bormotov
//


#include "HomomorphismBuilder.h"


int main( )
{
  srand(1335);
  FPGroup F;
  cout << "Enter a FPGroup represenatation, i.e. <a,b,c| a^7, b = c> : "
       << endl;
  Chars errMsg = cin >> F;
  if( errMsg.length() > 0 ) {
    cout << errMsg << endl;
    exit(-1);
  }
  cout << endl;
    
  cout << "You typed the FPGroup: " << F << endl << endl;

  const int size = 2;
  HomomorphismBuilder<Integer> HB(F, size);
  HB.startComputation();
  while( !HB.done() ) HB.continueComputation();
  VectorOf< Matrix<Integer> > hom = HB.getHomomorphism();

  cout << "homomorphism : " << endl;
  for( int i = 0; i < F.numberOfGenerators(); i++ )
    cout <<  hom[i] << endl;

/*
  srand(1335);
  FPGroup F;
  cout << "Enter a FPGroup represenatation, i.e. <a,b,c| a^7, b = c> : "
       << endl;
  Chars errMsg = cin >> F;
  if( errMsg.length() > 0 ) {
    cout << errMsg << endl;
    exit(-1);
  }
  cout << endl;
    
  cout << "You typed the FPGroup: " << F << endl << endl;

  const int size = 2;
  HomomorphismBuilder<Rational> HB(F, size);
  HB.startComputation();
  while( !HB.done() ) HB.continueComputation();
  VectorOf< Matrix<Rational> >  hom = HB.getHomomorphism();

  cout << "homomorphism : " << endl;
  for( int i = 0; i < F.numberOfGenerators(); i++ )
    cout <<  hom[i] << endl;
*/
}









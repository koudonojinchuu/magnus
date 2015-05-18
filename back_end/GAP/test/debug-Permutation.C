// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Informal test of Permutation class
//
// Principal Author: Dmitry Bormotov
//
// Status:
//
// Revision /History:
//


#include "Permutation.h"

void main ( )
{
  //Permutation p;
  //cout << p << p.inGAP();

  //cout << p << p << p.inGAP() << p.inGAP() << p.inGAP() 
  //cin >> p >> p >> p.inGAP() >> p.inGAP() >> p >> p.inGAP() >> p;

  cout << "Enter a permutation: ";
  Permutation p;
  cin >> p;
  cout << endl << "You entered: " << p << endl;

  cout << "Starting GAP and computing inverse ... " << endl;
  Permutation pI = p.inverse();
  cout << "The inverse: " << pI << endl;
}







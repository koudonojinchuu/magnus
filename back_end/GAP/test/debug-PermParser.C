// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Informal test of PermutationParser class
//
// Principal Author: Dmitry Bormotov
//
// Status:
//
// Revision /History:
//


#include "PermutationParser.h"

void main ( )
{
  cout << "Enter permutation: ";
  PermutationParser P(cin);
  Chars errMesg;
  VectorOf<int> v = P.parsePermutation(errMesg);
  if( errMesg.length() == 0 )
    cout << "Your permutation: " << v << endl;
  else
    cout << errMesg << endl;
}







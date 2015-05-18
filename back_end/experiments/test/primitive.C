// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: 
//
//     The program counts the number of primitive elements in a free
//     group.
//
// Principal Author: Dmitry Bormotov
//
// Status: ready
//
// Revision History:
//


#include "GeneralWhitehead.h"

int main ( )
{
  int ng;
  cout << "Enter the number of generators of a free group: ";
  cin >> ng;
  cout << endl;

  FreeGroup F(ng);
  int n = 1;         // element's number
  int curLen = 1;    // the current length of the elements considered
  int sum = 0;       // the number of primitive elements
  int numOfElts = 0; // the number of elements of a current length in
                     // a free group

  cout << "Free Group F = " << F << endl;
  
  while( true ) {

    // get next element of a free group

    Word w = F.getN_thElement(n++);
    int wLen = w.length();

    // if all the elements of current length were enumerated, print a
    // report

    if( wLen > curLen) {
      cout << "Length: " << curLen << "   "
	   << "Number of elements: " << numOfElts << "   "
	   << "Number of primitive elements: " << sum << endl;;
      curLen = wLen;
      sum = 0;
      numOfElts = 0;
    }

    ++numOfElts;
    
    // check whether an element is primitive
    
    GeneralWhitehead GW(F);
    GW.startComputation(w);
    while( !GW.continueComputation() );
    if( GW.extendsToFreeBasis() ) ++sum;
  }
}


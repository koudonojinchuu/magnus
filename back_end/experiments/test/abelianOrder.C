// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: this program computes orders of all abelian groups
//           satisfying the following pattern:
// 
//   < a,b; a^2 b^3, a^3 b^2 >,
//   < a,b,c; a^2 b^3 c^4, a^3 b^4 c^2, a^4 b^2 c^3 >, etc.
//
// Principal Author: Dmitry Bormotov
//
// Status: ready to run
//
// Revision History:
//


#include "List.h"
#include "Map.h"
#include "AbelianGroup.h"


int main ( )
{
  
  int numOfGens = 2, numOfGroups;
  char c;

  do {

    cout << "Enter the number of groups to compute: ";
    cin >> numOfGroups;
    cout << endl;
    int num = 0;
  

  // on each step construct an abelian group satisfying the pattern
  // above and compute it's order
  
    for( ; num < numOfGroups; ++numOfGens, ++num ) {
    
      // make an array of powers in the form: 2,3,4,5,etc
    
      ListOf<int> powers;
      ListIterator< ListOf<int> > I(powers);
    
      int curPower = 2;
      for( int i = 0; i < numOfGens; ++i )
	powers.append(curPower++);
    

      // prepare the set of relators
    
      SetOf<Word> relators;
      for( int i = 0; i < numOfGens; ++i ) {
      
	// construct next relator
      
	Word relator;
	int g;

	for( g = 1, I.reset(); !I.done(); I.next(), ++g )
	  for( int j = I.value(); j > 0; --j )
	    relator *= Word(Generator(g));
      
	relators |= relator;
      
	// prepare the next permutation of 'powers'
      
	powers.append(powers[0]);
	powers.removeElementOfIndex(0);
      }
    
      FPGroup G(numOfGens, relators);
      AbelianGroup A(G);
      A.computeCyclicDecomposition();
    
      cout << "Abelian group: " << endl << G << endl
	   << "The order: " << A.order() << endl << endl;  
    }
    
    cout << "Continue (y/n) ?";
    cin >> c;
    cout << endl;
    
  } while( c == 'y' || c == 'Y' );

}


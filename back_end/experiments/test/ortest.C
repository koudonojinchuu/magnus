// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: 
//
//      The program finds the shortest trivial word in an one-relator
//      group. The algorithm is horrobly exponential, so if you give
//      it a group with more then two generators, don't expect results
//      in reasonable time.
//
// Principal Author: Dmitry Bormotov
//
// Status:
//
// Revision History:
//


#include "OneRelatorGroup.h"
#include "FreeGroup.h"

int main ( )
{
  FPGroup G;
  cout << "Enter an OR group: ";
  cin >> G;
  cout << endl;
  OneRelatorGroup O(G);

  Word r = O.relator();
  int rLen = r.length();
  int ng = O.numberOfGenerators();
  int i = 0;
  int oldLen = 0;

  FreeGroup F(ng);

  while( true ) {

    // Enumerate all elements of an ambient free group of the
    // one-relator group which are shorter than the original relation
    // and solve the word problem.

    Word w  = F.getN_thElement(++i);
    if( w.length() >= rLen ) {
      cout << "There's no trivial word which is shorter than the original "
	   << "relation." << endl;
      break;
    }
    
    if( O.wordProblem(w) == yes ) {
      cout << "The shortest word is  ";
      G.printWord(cout, w);
      cout << endl;
      break;
    }
  }
}


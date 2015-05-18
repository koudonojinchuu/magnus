// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Debugging and performance tests for Set class.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
//


#include "Set.h"
#include "Word.h"


main( ) {
/*
  Word w1(3); w1[0] = 3; w1[1] = 5; w1[2] = 7;

  SetOf<Word> S(8);

  S.addElement(w1);
  S.addElement(w1);
  S.printOn();

//  cout << "S has cardinality 1 =  " << S.cardinality() << endl;

  Word w2(w1); w2[2] = -7;
  Word w3(2);

  S.addElement(w3);
  S.printOn();

//  cout << "S has cardinality 2 =  " << S.cardinality() << endl;
  S.printRep();

  {
	 for( SetIterator I(S); I.more(); ) {
		Word w = I.value();
		cout << "S contains an elt of length " << w.length() << endl;
	 }
  }

  cout << endl << endl;

  SetIterator I(S);
  while ( I.more() ) {
	 Word w = I.value();
	 cout << "S contains an elt of length " << w.length() << endl;
  }

//  S.printStats();
//  S.printRep();

  for(int j = 1; j < 40; j++) {
	 w2[2] = j;
	 S.addElement(w2);
  }
  S.printRep();
  S.printOn();
*/


  SetOf<Word> S;
  srandom(2112);
  for (int k = 0; k < 9999; k++) {
	 int len = 4 + abs( (int)(random() % 31) );
	 Generator *tmp = new Generator[len];
	 for (int j = 0; j < len; j++) {
		if ( (int)(random() % 2) == 0 )
		  tmp[j] = 1 + abs((int)(random() % 12));
		else
		  tmp[j] = -1 - abs((int)(random() % 12));
	 }
	 Word w(tmp, len);
	 delete tmp;
	 S.addElement(w);
	 if ( (k % 400) == 0 ) S.printStats();
  }
  S.printStats();
}

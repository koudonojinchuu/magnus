// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: An experiment which looks for a counter-example to the
//           Hanna Neumann conjecture: if A and B are finitely generated
//           subgroups of a free group of finite rank, and I is the
//           intersection of A and B, then rk(I)-1 <= (rk(A)-1)(rk(B)-1).
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
//
// Next implementation steps:


#include <iostream.h>

#include "SubgroupGraph.h"


SetOf<Word> randomSet( )
// Quick-n-dirty way to make a random set of words on two group generators.
// The size of the set is uniformly random between 100 and 500.
// The lengths of the words are uniformly random between 10 and 75.
{
  int size = 100 + abs((int)(rand() % 401));
  SetOf<Word> result(size);
  while ( size-- ) {
	 int len = 10 + abs( (int)(rand() % 66) );
	 VectorOf<Generator> v(len);
	 int i = 0;
	 while ( i < len ) {
		int gen = rand();
		if ( gen & 128 )
		  gen = 1 + ( (gen & 64) ? 1 : 0 );
		else
		  gen = -1 - ( (gen & 64) ? 1 : 0 );
		if ( i == 0 || v[i - 1] != -gen )
		  v[i++] = gen;
	 }
//	 Word w(v);
//	 result |= w.freelyReduce();
	 result |= Word(v); // No need to freely reduce.
  }
  return result;
}


main( )
{
  cout << "\nThis experiment looks for a counter-example to the\n"
		 << "Hanna Neumann conjecture: if A and B are finitely generated\n"
		 << "subgroups of a free group of finite rank, and I is the\n"
		 << "intersection of A and B, then rk(I)-1 <= (rk(A)-1)(rk(B)-1).\n\n"
		 << "The generators of A and B are chosen randomly, the number being\n"
		 << "uniformly random between 100 and 500. The word lengths of the\n"
		 << "generators are uniformly random between 10 and 75.\n"
		 << "The ambient free group has rank 2.\n"
       << "This prints a count of the number of examples it has tried,\n"
		 << "every time it does 10.\n\n";


  cout << "\nHow's about a random seed: ";
  long seed;
  cin >> seed;
  cout << "seed = " << seed << endl;
  srand(seed);

  long count = 0;

  while ( 1 ) {

	 SetOf<Word> AGens = randomSet();
	 SetOf<Word> BGens = randomSet();

	 SubgroupGraph SGGA(2, AGens);
	 SubgroupGraph SGGB(2, BGens);
	 SubgroupGraph SGGI = SGGA.intersection(SGGB);

    #ifdef DEBUG
	 // These will print output if anything is bad:
	 SGGA.consistentData();
	 SGGB.consistentData();
	 SGGI.consistentData();
    #endif

	 if ( (SGGI.rank() - 1) > (SGGA.rank() - 1) * (SGGB.rank() - 1) ) {
		cout << "\n\n**** Have a counter-example! ****\n\n";
		cout << AGens << BGens << endl << endl;
      exit;
	 }

	 if ( (++count % 10) == 0 ) cout << count << endl;
  }
}

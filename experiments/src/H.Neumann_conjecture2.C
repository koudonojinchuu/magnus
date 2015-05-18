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
// The size of the set is uniformly random between 4 and 100.
// The lengths of the words are uniformly random between 4 and 50.
{
  int size = 4 + abs((int)(rand() % 97));
  SetOf<Word> result(size);
  while ( size-- ) {
	 int len = 4 + abs( (int)(rand() % 47) );
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
		 << "uniformly random between 4 and 100. The word lengths of the\n"
		 << "generators are uniformly random between 4 and 50.\n"
		 << "The ambient free group has rank 2.\n"
       << "This prints the ranks of various relevant subgroups.\n\n";


  cout << "\nHow's about a random seed: ";
  long seed;
  cin >> seed;
  srand(seed);

  long count = 0;

  int juju = 3;

  while ( juju-- ) {

	 SetOf<Word> HGens = randomSet();
	 SubgroupGraph SGGH(2, HGens);
//	 SGGH.consistentData();
	 int rkH = SGGH.rank();
	 cout << "rk(H) = " << rkH; cout.flush();

	 SGGH.MHallComplete( );
//	 SGGH.consistentData();
	 int rkS = SGGH.rank();
	 long j = SGGH.vertexCount();
	 cout << "\trk(S) = " << rkS;
	 cout << "\t[F:S] = " << j; cout.flush();

	 if ( j == 1 ) {
		cout << endl;
		continue;
	 }

	 SetOf<Word> KGens = randomSet();
	 SubgroupGraph SGGK(2, KGens);
//	 SGGH.consistentData();
	 int rkK = SGGK.rank();
	 cout << "\trk(K) = " << rkK; cout.flush();

	 SubgroupGraph SGGI = SGGH.intersection(SGGK);
//	 SGGI.consistentData();
	 int rkI = SGGI.rank();
	 cout << "\tS^K = " << rkI << endl;

//@rn can comp intersection of H with I, and H with K & check if ==
	 

//	 if ( (++count % 10) == 0 ) cout << count << endl;
  }
}

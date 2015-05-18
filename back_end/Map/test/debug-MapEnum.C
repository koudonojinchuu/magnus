// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Tests of classes MapEnum, IntTuples
//
// Principal Author: Roger Needham
//
// Status: In development
//
// Further Implementation Steps:
//
// Revision History:
//


#include "MapEnum.h"
#include "FPGroup.h"



void printVec(const int* vec, int len)
{
  while ( len-- ) cout << *vec++ << ' ';
  cout << endl;
}



void debug_IntTuples( )
{
  cout << "\n\nHere are the 1-tuples: " << endl;
  IntTuples it1(1);
  for( int i = 0; i < 10; ++i )
	 printVec( it1.nextTuple(), 1 );

  cout << "\n\nHere are the 2-tuples: " << endl;
  IntTuples it2(2);
  for( i = 0; i < 50; ++i )
	 printVec( it2.nextTuple(), 2 );

  cout << "\n\nHere are the 3-tuples, starting at radius 400: " << endl;
  IntTuples it3(3, 400);
  for( i = 0; i < 100; ++i )
	 printVec( it3.nextTuple(), 3 );

  cout << "\n\nHere are the 4-tuples, starting at radius 40000: " << endl;
  IntTuples it4(4, 40000);
  for( i = 0; i < 100; ++i )
	 printVec( it4.nextTuple(), 4 );

  cout << endl << endl << endl;
}



void debug_kthWord(int k, int n)
{

}



main( )
{
  debug_IntTuples();

  
  FPGroup domain;
  cout << "\n\nType a presentation for the domain:\n" << endl;

  Chars errMesg = cin >> domain;

  if ( errMesg.length() > 0 ) { cout << errMesg << endl << endl; exit; }


  FPGroup range;
  cout << "\n\nType a presentation for the range:\n" << endl;

  errMesg = cin >> range;

  if ( errMesg.length() > 0 ) { cout << errMesg << endl << endl; exit; }


  int radius;
  cout << "Starting radius: ";
  cin >> ws >> radius;


  MapEnum ME(domain, range, radius);

  while ( 1 ) cout << ME.nextMap() << endl << endl;
}

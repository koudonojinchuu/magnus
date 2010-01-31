#include <iostream>
#include "RandomPrimitiveElement.h"
#include "FreeGroup.h"
#include "WhiteheadAutomorphisms.h"

main( )
{
  FreeGroup F;

  cout << "Enter a free group:" << endl << flush;

  Chars errMesg = cin >> F;

  if ( errMesg.length() > 0 ) {
	 cout << errMesg << endl << endl;
	 exit;
  }
  else cout << "\n\nYou typed:\n" << F << endl << endl;

  cout << "Enter a seed (1000-2000):" << endl << flush;
  int seed;
  cin >> seed;
  cout << "Enter an average number of Whitehead automorphisms to apply:" << endl << flush;
  int avgGenNum;
  cin >> avgGenNum;
  cout << "How many tests you want to run:" << endl << flush;
  int numOfTests;
  cin >> numOfTests;
  
  RandomPrimitiveElement rp(F,seed);
  
  for ( int i = 0; i < numOfTests; i++ ) {
    Word elt = rp.getElement( avgGenNum );
    cout << elt << endl;

    WhiteheadAutomorphisms WA( F );
    WA.startComputation( elt );
    while ( !WA.continueComputation() );
    if ( WA.isPrimitive() ) cout << "PRIMITIVE." << endl << flush;
    else cout << "NOT PRIMITIVE!!!!!!!!!!!!!!!?" << endl << flush;

  }

  return 1;
}

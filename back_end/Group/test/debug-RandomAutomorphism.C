#include <iostream.h>
#include "RandomAutomorphism.h"
#include "FreeGroup.h"


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

  RandomAutomorphism ra(F,1995);

  for ( int i = 0; i < 20; i++ ) {
    cout << ra.getGeneratingVector( 10 ) << endl;
  }

  return 1;
}

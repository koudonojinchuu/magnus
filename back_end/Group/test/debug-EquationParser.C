#include <iostream>
#include "EquationParser.h"
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

  cout << "Enter an equation:" << endl << flush;

  EquationParser P( cin );
  VectorOf<Chars> newNames;
  Word E = P.parseEquation( F.namesOfGenerators(), newNames, errMesg );

  if ( errMesg.length() > 0 ) {
	 cout << errMesg << endl << endl;
	 exit;
  }
  else {
	 FreeGroup G( newNames );
	 cout << "\n\nYou typed:\n";
	 G.printWord( cout, E );
	 cout << endl << "Group: " << G << endl;
	 cout << endl << endl;
  }
}

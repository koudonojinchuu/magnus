#include "global.h"
#include "Map.h"
#include "AbelianInfinitenessProblem.h"


int main ( )
{
  cout << "\n\nEnter a group presentation, e.g.,\n";
  cout << "  <x,y;x^2=y^3>\n" << endl;

  FPGroup G;
  Chars errMsg = cin >> G;

  if ( errMsg.length() > 0 ) {
    cout << errMsg << endl << endl;
    return 1;
  }

  cout << "\n\nYou entered:\n";
  cout << G << endl << endl;

  cout << "Checking of infiniteness..." << endl << endl;
  AbelianInfinitenessProblem AIP(G);
  AIP.startComputation();
  while ( !AIP.continueComputation() ) ;

  cout << "The group is ";
  if( AIP.isInfinite() )
    cout << "in";
  cout << "finite." << endl;
}


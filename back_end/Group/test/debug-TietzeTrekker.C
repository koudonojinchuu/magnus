// Principal Author: Roger Needham

#include <iostream.h>

#include "TietzeTrekker.h"

main ( ) {

  cout << "\n\nEnter a group presentation, e.g.,\n";
  cout << "  <x,y;x^2=y^3>\n" << endl;

  FPGroup G;

  Chars errMesg = cin >> G;

  if ( errMesg.length() > 0 ) {
	 cout << errMesg << endl << endl;
	 exit;
  }

  cout << "\n\nYou entered:\n";
  cout << G << endl << endl;

  cout << "Invoking TietzeTrekker..." << endl;

  TietzeTrekker TT(G);
}

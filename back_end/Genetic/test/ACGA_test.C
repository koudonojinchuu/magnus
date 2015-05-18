// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Informal test.
//
// Principal Author: Alexei Miasnikov
//
// Status:
//
// Revision History:
//


#include <values.h>
#include "global.h"
#include "Map.h"
#include "ACConfig.h"
#include "RandomNumbers.h"
#include "Roulette.h"
#include "ACGA.h"

int main ( )
{

  FreeGroup f( 12 );

  Chars errMsg;
  ACConfig c;
  //cout << c << endl;

  ifstream in("ac.in");
  
  if ( !in ) {
    cout << " Error: Configuration file \"ac.in\" couldn't be found." << endl;
    exit( 0 );
  }
    

  in >> c;
  cout << c << endl;

  Chars best_file_name (tempnam("./", "acga_"));
  ofstream best_out(best_file_name);
  cout << "Best fitness details in " << best_file_name << "." << endl;

  FPGroup G;
  cout << "Enter a finitely presented group: ";
  errMsg = cin >> G;
  if( errMsg.length() > 0 ) {
    cout << errMsg;
    exit(0);
  }
  
  cout << G << endl;
  
  ACGA acga(G,c);
  acga.transform( cout, best_out );
}

// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Informal test.
//
// Principal Author: Dmitry Bormotov
//
// Status:
//
// Revision History:
//


#include <values.h>
#include "global.h"
#include "Map.h"
#include "GHNConfig.h"
#include "RandomNumbers.h"
#include "Roulette.h"
#include "GAWP.h"

int main ( )
{
  Chars errMsg;
  GHNConfig c;
  //cout << c << endl;

  ifstream in("../wp.in");
  in >> c;
  cout << c << endl;

  FPGroup G;
  cout << "Enter a finitely presented group: ";
  errMsg = cin >> G;
  if( errMsg.length() > 0 ) {
    cout << errMsg;
    exit(0);
  }
  cout << endl;
  
  cout << "Enter a word: ";
  Word w;
  w = G.readWord(cin,errMsg);
  if( errMsg.length() > 0 ) {
    cout << errMsg;
    exit(0);
  }
  cout << endl;

  GAWP2 gawp(G,c);
  gawp.isTrivial(w, &cout);
}

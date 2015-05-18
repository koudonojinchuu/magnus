// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:
//
// Principal Author: Stephane Collart
//
// Status:
//
// Revision History:
//

#include "Map.h"

//#include "FreeGroup.h"

//#include <iostream.h>


main ( ) {

/*

int i;

FreeGroup F;

Map f(F,F);

cout << f;

F = FreeGroup(3);

f = Map(F,F);

cout << endl << f;

VectorOf<Word> vw(3);

f.setGeneratingImages(vw);

cout << endl << endl << f;

cout << endl << endl << f.composeAfter(f);

f = composition(f,f);

cout << endl << endl << f;

vw[0] = Generator(2); vw[1] = Generator(3); vw[2] = Generator(1);

Map g(F,F,vw);

cout << endl << endl << g;

cout << endl << endl << composition(g,g);

for (i = 1; i<=3; i++)
	g(Generator(i)) = g(Generator(i)) * g(Generator(i % 3 + 1));

cout << endl << endl << g;

cout << endl << endl;

for (i = 1; i<=3; i++) cout << endl << Generator(i) << " |-> "
	<< g(Generator(i));

cout << endl << endl << (g | g);

cout << endl << g.extendsToHom();

*/


  cerr << "\n\nEnter the domain, e.g.,\n";
  cerr << "  <x,y;x^2=y^3>\n" << endl;
  
  FPGroup G;
  
  Chars errMesg = cin >> G;
  
  if ( errMesg.length() > 0 ) {
    cerr << errMesg << endl << endl;
    exit;
  }
  
  cerr << "\n\nYou entered:\n";
  cerr << G << endl << endl;

  cerr << "\n\nEnter the range\n";
  cerr << "  <a,b;[a,b]>\n" << endl;
  
  FPGroup H;
  
  errMesg = cin >> H;
  
  if ( errMesg.length() > 0 ) {
    cerr << errMesg << endl << endl;
    exit;
  }
  
  cerr << "\n\nYou entered:\n";
  cerr << H << endl << endl;


  cerr << "\n\nEnter a map, e.g.,\n";
  cerr << "  { x -> a^2, y -> b }\n" << endl;
  
  Map m(G,H);
  
  errMesg = cin >> m;
  
  if ( errMesg.length() > 0 ) {
    cerr << errMesg << endl << endl;
    exit;
  }
  
  cerr << "\n\nYou entered:\n";
  cerr << m << endl << endl;

}

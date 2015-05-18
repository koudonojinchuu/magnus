// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:
//
// Principal Author: Roger Needham
//
// Status:
//
// Revision History:
//

#include "Word.h"


main ( ) {


VectorOf<Generator> vg(3);

vg[0]=Generator(1);
vg[1]=Generator(2);
vg[2]=Generator(3);

cout << endl << vg;

Word w(vg);

Elt e = w;

cout << endl << &w;

cout << endl << &e;

cout << endl << &((Word&)e);

cout << endl << ((Word&)e).length();

cout << endl << e;

cout << endl << ((Word&)e);

VectorOf<Elt> ve(3);

ve[0] = w;
ve[1] = w;
ve[2] = w;

Word ww = w.replaceGenerators(ve);

cout << endl << "ww = ";

cout << ww;


}

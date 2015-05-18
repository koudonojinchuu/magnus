// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Informal tests of class BTree
//
// Principal Authors: Dmitry Bormotov
//


#include "global.h"
#include "BTree.h"


void main( )
{
  const numOfElt = 17;
  const char Elt[numOfElt+1] = "CAKRMGOTDXSLJZNPW";

  BTree<Char> T;
  for( int i = 0; i < numOfElt; ++i )
    T.insert(Elt[i]);
//  T.insert('Y');
  T.Delete('Z');
  T.printAll();
  

/*
  BTreeIterator<Char> I(T);
  for( ; !I.done(); I.next() )
    cout << I.value() << endl;
*/    
}

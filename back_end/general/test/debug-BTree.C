// Copyright (C) 1997 The New York Group Theory Cooperative
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
  /*
  const numOfElt = 17;
  const char Elt[numOfElt+1] = "CAKRMGOTDXSLJZNPW";

  BTree<int,char> T;
  for( int i = 0; i < numOfElt; ++i )
    T.insert(Elt[i], Elt[i]);

  T.remove(Elt[4]);
  T.printAll();
  cout << endl;

  int key = Elt[2];
  char* value = T.search(key);
  cout << "(" << key << "," << *value << ")" << endl;
  */

  /*
  BTree<int,int> T(3);
 
  T.insert(5, 50);
  T.printAll();
  cout << endl;
  
  T.insert(4, 40);
  T.printAll();
  cout << endl;
 
  T.insert(3, 30);
  T.printAll();
  cout << endl;
 
  T.insert(2, 20);
  T.printAll();
  cout << endl;
 
  T.insert(1, 10);
  T.printAll();
  cout << endl;
  
  T.remove(2);
  
  T.printAll();
  cout << endl;

  for( int key = 1; key <= 5; ++key ) {
    int* pValue = T.search(key);
    if( pValue ) 
      cout << "(" << key << "," << *pValue << ")" << endl;
    else
      cout << "The key " << key << " is not found."<< endl;
  }
  
  cout << endl;
  */

  BTree<int,int> T(7);
  int size = 10000;

  /*
  for( int i = 1; i <= size; ++i )
    T.insert(i,i);
  */

  for( int i = size; i > 0; --i )
    if( (i % 4) == 0 ) 
      T.insert(i,i);
  
  for( int i = size; i > 0; --i )
    if( (i % 3) == 0 ) 
      T.insert(i,i);

  for( int i = size; i > 0; --i )
    if( (i % 5) == 0 ) 
      T.insert(i,i);
    
  /*
  T.printAll();
  cout << endl;
  */

  for( int i = 1; i <= size; ++i )
    if( (i % 2) > 0 ) 
      T.remove(i);

  for( int i = 1; i <= size; ++i )
    if( (i % 10) == 0 ) 
      T.insert(i,i);

  for( int i = size; i > 0; --i )
    if( (i % 10) != 0 ) 
      T.remove(i);


  T.printAll();
  cout << endl;

  /*
  T.remove(1);
  T.printAll();
  cout << endl;

  T.remove(2);
  T.printAll();
  cout << endl;
 
  T.remove(3);
  T.printAll();
  cout << endl;

  T.remove(6);
  T.printAll();
  cout << endl;

  T.remove(7);
  T.printAll();
  cout << endl;

  T.remove(5);
  T.printAll();
  cout << endl;

  T.remove(4);
  T.printAll();
  cout << endl;
  */
}



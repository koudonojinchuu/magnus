// Principal Author: Roger Needham


// Quickie test of conversions.h


#include "conversions.h"

main( ) {


  ListOf<int> li;

  cout << endl << li;

  li.append(1);

  cout << endl << li;

  li.append(3);

  cout << endl << li;

  ListOf<int> l2 = li;

  cout << endl;
  li.debugPrint(cout);

  cout << endl;
  l2.debugPrint(cout);

  l2.append(5);
  cout << endl;
  li.debugPrint(cout);
  
  cout << endl;
  l2.debugPrint(cout);
  
  ListIterator< ListOf<int> > it(l2);

  cout << endl;

  while ( !it.done() ) {
    cout << " -> " << it.value();
    it.next();

  }

  VectorOf<int> v = makeVectorOf(l2);
  cout << endl << v << endl;
}

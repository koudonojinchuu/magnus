// Informal tests of class VectorOf<T>

#include <iostream.h>

#include "Vector.h"


struct foo
{
  foo( ) : i(1) { }
  void bar( ) { ++i; }
  int operator == ( const foo& f ) { return i == f.i; }
  inline friend ostream& operator << ( ostream& o, const foo& f )
  { o << f.i; return o; }
  int i;
};


main( )
{
  VectorOf<foo> V1(1);

  cout << "V1 = " << V1 << endl;

  VectorOf<foo> V2 = V1;
  cout << "V2 = " << V2 << endl;

  ((foo&)V2[0]).bar();

  cout << "V1 = " << V1 << endl;
  cout << "V2 = " << V2 << endl;
}

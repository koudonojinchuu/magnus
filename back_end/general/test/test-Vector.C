// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Regression test of the VectorOf class.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
//

#include "Chars.h"
#include "Vector.h"


int badCount = 0;


void validate(const int test, const char* name) {
  if ( test ) {
	 cout << "+ " << name << endl;
  } else {
	 ++badCount;
	 cout << "- " << name << endl;
  }
}

// int foo( Chars c ) { return c.length(); } //@rn for cast test below

// Make sure VectorOf does not require any more of the parameter
// than claimed, by instantiating one with minimal classes.

class testElement {
public:
  testElement( ) { dataMember = 0; }
  // Use compiler's copy constructor and destructor.
  testElement& operator = ( const testElement& tk ) {
	 dataMember = tk.dataMember; // @stc fixed == into =
	 return *this;
  }
  int operator == ( const testElement& tk ) const {
	 return dataMember == tk.dataMember;
  }
  friend ostream& operator << ( ostream& ostr, const testElement& tk ) {
    ostr << tk.dataMember;
  } // @stc uncommented this
private:
  int dataMember;
};


main( ) {

  VectorOf<testElement> try_to_instantiate;

  cout << "Testing VectorOf class\n" << endl;

  VectorOf<Chars> v1;

  VectorOf<Chars> v2(3);
  v2[0] = Chars("Larry");
  v2[1] = Chars("Moe");
  v2[2] = Chars("Curly");
  validate( v1.length() == 0 &&
				v2.length() == 3 &&
				v2[0] == Chars("Larry") &&
				v2[1] == Chars("Moe") &&
				v2[2] == Chars("Curly"),
				"VectorOf<Chars>::VectorOf<Chars>(int len = 0)" );
  validate( 1, "int VectorOf<Chars>::length( ) const" );

//  int dummy = foo(v2[0]); //@rn make sure g++ 2.5.8 can find the cast operator!
//  cerr << dummy;          //@rn make sure it's not optimized away!

  VectorOf<Chars> v3 = v2;
  validate( v2 == v3,
				"int VectorOf<Chars>::operator == (const VectorOf<Chars>&) const" );
  validate( 1,
				"VectorOf<Chars>& VectorOf<Chars>::operator = (const VectorOf<Chars>&)" );
  validate( v2 != v1,
				"int VectorOf<Chars>::operator != (const VectorOf<Chars>&) const" );

  validate( v3.indexOf(Chars("Larry")) == 0 &&
				v3.indexOf(Chars("Moe")) == 1 &&
				v3.indexOf(Chars("Curly")) == 2,
				"int VectorOf<Chars>::indexOf( const Chars& ) const" );

  VectorOf<Chars> v4 = v3;
  v4[2] = Chars("Shemp");
  validate( v4.length() == 3 &&
				v4[0] == Chars("Larry") &&
				v4[1] == Chars("Moe") &&
				v4[2] == Chars("Shemp"),
				"VectorItemRef<Chars> VectorOf<Chars>::operator [ ] ( int ) const" );

  VectorOf<Chars> v5 = v1;
  v5.append(Chars("Moe"));
  v5.prepend(Chars("Larry"));
  v5.append(Chars("Shemp"));

  VectorOf<Chars> v6 = v4;
  v6.shrink(1,1);
  v6.prepend(Chars("Larry"));
  v6.append(Chars("Shemp"));

  int append_prepend_test = ( v5 == v4 && v6 == v4 );
  validate( append_prepend_test,
				"void VectorOf<Chars>::append( const Chars& )" );
  validate( append_prepend_test,
				"void VectorOf<Chars>::prepend( const Chars& )" );

  v5.shrink(2);
  validate( v5.length() == 2 &&
				v5[0] == Chars("Larry") &&
				v5[1] == Chars("Moe"),
				"void VectorOf<Chars>::shrink( int newlength )" );

  v5.shrink(1,0);
  v4.shrink(1,1);
  validate( v5.length() == 0 &&
				v4.length() == 1 &&
				v4[0] == Chars("Moe"),
				"void VectorOf<Chars>::shrink( int start, int newlength )" );


  if ( badCount == 0 ) cout << "\nAll tests pass.\n" << endl;
  else cout << endl << badCount << " tests failed.\n" << endl;
}

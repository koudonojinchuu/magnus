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
#include "VectorPtr.h"


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
	//  testElement( ) { dataMember = 0; } //++ no default constructor

	testElement(int i); // arbitrary constructor

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

  VectorPtrOf<testElement> try_to_instantiate;

  cout << "Testing VectorPtrOf class\n" << endl;

  VectorPtrOf<Chars> v1;

  VectorPtrOf<Chars> v2(3);

	// Chars t = v2[0];  //@dp make sure using of uninitialized element of 
	                     //@dp VectorPtrOf<> causes fatal error.

  v2[0] = Chars("Larry");  

	validate( !(v2[1] == Chars("What is it?")), "op==");

  v2[2] = Chars("Curly");

	validate( v2.indexOf(Chars("Curly")) == 2, "indexOf()");

  v2[1] = Chars("Moe");

  validate( v1.length() == 0 &&
				v2.length() == 3 &&
				v2[0] == Chars("Larry") &&
				v2[1] == Chars("Moe") &&
				v2[2] == Chars("Curly"),
				"VectorPtrOf<Chars>::VectorPtrOf<Chars>(int len = 0)" );
  validate( 1, "int VectorPtrOf<Chars>::length( ) const" );

//int dummy = foo(v2[0]); //@rn make sure g++ 2.5.8 can find the cast operator!
//cerr << dummy;          //@rn make sure it's not optimized away!

  VectorPtrOf<Chars> v3 = v2;
  validate( v2 == v3,
		"int VectorPtrOf<Chars>::operator == (const VectorPtrOf<Chars>&) const" );
  validate( 1,
		"VectorPtrOf<Chars>& VectorPtrOf<Chars>::operator = (const VectorPtrOf<Chars>&)" );
	validate( v2 != v1,
		"int VectorPtrOf<Chars>::operator != (const VectorPtrOf<Chars>&) const" );

  validate( v3.indexOf(Chars("Larry")) == 0 &&
			v3.indexOf(Chars("Moe")) == 1 &&
			v3.indexOf(Chars("Curly")) == 2,
			"int VectorPtrOf<Chars>::indexOf( const Chars& ) const" );

  VectorPtrOf<Chars> v4 = v3;
  v4[2] = Chars("Shemp");
  validate( v4.length() == 3 &&
			v4[0] == Chars("Larry") &&
			v4[1] == Chars("Moe") &&
			v4[2] == Chars("Shemp"),
			"VectorItemRef<Chars> VectorPtrOf<Chars>::operator [ ] ( int ) const" );

  VectorPtrOf<Chars> v5 = v1;
  v5.append(Chars("Moe"));
  v5.prepend(Chars("Larry"));
  v5.append(Chars("Shemp"));

  VectorPtrOf<Chars> v6 = v4;
  v6.shrink(1,1);
  v6.prepend(Chars("Larry"));
  v6.append(Chars("Shemp"));

  int append_prepend_test = ( v5 == v4 && v6 == v4 );
  validate( append_prepend_test,
				"void VectorPtrOf<Chars>::append( const Chars& )" );
  validate( append_prepend_test,
				"void VectorPtrOf<Chars>::prepend( const Chars& )" );

  v5.shrink(2);
  validate( v5.length() == 2 &&
				v5[0] == Chars("Larry") &&
				v5[1] == Chars("Moe"),
				"void VectorPtrOf<Chars>::shrink( int newlength )" );

  v5.shrink(1,0);
  v4.shrink(1,1);
  validate( v5.length() == 0 &&
				v4.length() == 1 &&
				v4[0] == Chars("Moe"),
				"void VectorPtrOf<Chars>::shrink( int start, int newlength )" );


  if ( badCount == 0 ) cout << "\nAll tests pass.\n" << endl;
  else cout << endl << badCount << " tests failed.\n" << endl;
}










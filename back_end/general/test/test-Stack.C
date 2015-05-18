// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Regression test of the StackOf class.
//
// Principal Author: Sergey Lyutikov
//
// Status: Useable.
//
// Revision History:
//

#include "Chars.h"
#include "Stack.h"


int badCount = 0;


void validate(const int test, const char* name) {
  if ( test ) {
	 cout << "+ " << name << endl;
  } else {
	 ++badCount;
	 cout << "- " << name << endl;
  }
}

// Make sure StackOf does not require any more of the parameter
// than claimed, by instantiating one with minimal classes.

class testElement {
public:
  testElement( ) { dataMember = 0; }
  // Use compiler's copy constructor and destructor.
  testElement& operator = ( const testElement& tk ) {
	 dataMember = tk.dataMember; // @stc fixed == into =
	 return *this;
  }
  Bool operator == ( const testElement& tk ) const { 
	return dataMember == tk.dataMember;
  }
  friend ostream& operator << ( ostream& ostr, const testElement& tk ) {
    ostr << tk.dataMember;
  } 
private:
  int dataMember;
};


main()
{

  StackOf<testElement> try_to_instantiate;

  cout << "Testing StackOf class\n" << endl;

  StackOf<Chars> s1;

  s1.push( Chars("Laska") );

  validate(	s1.isntEmpty() &&
		s1.pop() == Chars("Laska") &&
		s1.isEmpty(),  "StackOf<Chars>::StackOf<Chars>( )" );

  StackOf<Chars> s2( Chars("Kotik") );
  s2.push( Chars("Murka") );
  s2.push( Chars("Barsik") );

  validate( 	s2.pop() == Chars("Barsik") &&
		s2.pop() == Chars("Murka") &&
		s2.pop() == Chars("Kotik") &&
		s2.isEmpty(), "StackOf<Chars>::StackOf<Chars>( Chars& )" );

  StackOf<Chars> s3( Chars("Kotik") );
  s3.push( Chars("Murka") );
  s3.push( Chars("Barsik") );

  validate(	s3.isntEmpty() &&
		( s3.popAll(), s3.isEmpty() ), "StackOf<Chars>::popAll( )" );

  StackOf<int> S;

  for ( int i = 0; i < 10; i++ ) S.push(i);

  int count = 10;
  Bool checkIt = TRUE;

  while ( S.isntEmpty() ) {
	count--;
	if ( S.pop() != count ) checkIt = FALSE;
  }

  validate(	count == 0 && checkIt, "StackOf<T>::push( T& )" );
  validate(	count == 0 && checkIt, "StackOf<T>::pop( )" );

  if ( badCount == 0 ) cout << "\nAll tests pass.\n" << endl;
  else cout << endl << badCount << " tests failed.\n" << endl;

  return 0;
}




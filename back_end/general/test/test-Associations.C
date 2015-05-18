// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Regression test of the AssociationsOf and AssociationsIterator
//           classes.
//
// Some tests may fail because of a bug somewhere other than indicated.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
//

#include "Chars.h"
#include "Elt.h"
#include "Word.h"
#include "Associations.h"


int badCount = 0;


void validate(const int test, const char* name) {
  if ( test ) {
	 cout << "+ " << name << endl;
  } else {
	 ++badCount;
	 cout << "- " << name << endl;
  }
}


Word initWord(const Generator* p, int len) {
  VectorOf<Generator> v(len);
  for( int i = 0; i < len; i ++ ) v[i] = *p++;
  return Word(v);
}

// Make sure AssociationsOf does not require any more of the parameters
// than claimed, by instantiating one with minimal classes.

class testKey {
public:
  // Use compiler's copy constructor and destructor.
  int operator == ( const testKey& tk ) const {
	 return dataMember == tk.dataMember;
  }
  friend ostream& operator << ( ostream& ostr, const testKey& tk ) {
	 ostr << tk.dataMember;
  }
private:
  testKey( ) { dataMember = 0; }
  testKey& operator = ( const testKey& tk ) {
	 dataMember == tk.dataMember;
	 return *this;
  }
  int dataMember;
};

class testVal {
public:
  // Use compiler's copy constructor and destructor.
  testVal& operator = ( const testVal& tk ) {
	 dataMember == tk.dataMember;
	 return *this;
  }
private:
  testVal( ) { dataMember = 0; }
  int operator == ( const testVal& tk ) const {
	 return dataMember == tk.dataMember;
  }
  int dataMember;
};


main( ) {

  AssociationsOf<testKey,testVal> try_to_instantiate;

  cout << "Testing AssociationsOf<Key,Val> class\n" << endl;

  AssociationsOf<Chars,Elt> A;
  AssociationsOf<Chars,Elt> B;

  validate( 1, "AssociationsOf<Chars,Elt>( )" );

  Chars c1("elt1");
  const Generator i1[] = { 2, 3, 4, 5, 6 };
  Word w1 = initWord(i1,5);
  Elt e1 = w1;

  B.bind(c1, e1);

  validate( B.bound(c1) && !A.bound(c1),
				"Bool AssociationsOf<Chars,Elt>::bound( const Key& ) const" );

  Chars c2("elt2");
  const Generator i2[] = { 3, -4, 7, 17 }; //@rn added 17
  Word w2 = initWord(i2,3);
  Elt e2 = w2;

  B.bind(c1, e2);
  B.bind(c2, e2);

  validate( B.bound(c1) && B.bound(c2) && B[c1] == e2 && B[c2] == e2,
				"void AssociationsOf<Chars,Elt>::bind( const Key&, const Val& )" );
  validate( 1,
				"Elt AssociationsOf<Chars,Elt>::operator [ ] ( const Key& ) const" );

  validate( B.valueOf(c1) == e2 && B.valueOf(c2) == e2,
				"Elt AssociationsOf<Chars,Elt>::valueOf( const Key& ) const" );

  B.unbind(c1);

  validate( !B.bound(c1) && B.bound(c2) && B[c2] == e2,
				"void AssociationsOf<Chars,Elt>::unbind( const Key& )" );

  B.bind(c1,e1);


  // Test iteration.

  // First, an empty one:

  Bool shouldBeFalse = FALSE;
  AssociationsIterator<Chars,Elt> IA(A);
  while ( !IA.done() ) { shouldBeFalse = TRUE; }

  validate( 1,
				"AssociationsIterator<Chars,Elt>::AssociationsIterator(const AssociationsOf<Chars,Elt>&)" );

  // Now make a not-so-wee one; test iter by checking for duplicate keys:

  AssociationsOf<int,Word> C;

  for( int i = 1; i < 101; i++ )
	 C.bind(i, Word(Generator(i)));

  // Change some vals to keep it honest:
  for( i = 1; i < 101; i += 2 )
	 C.bind(i, Word(Generator(-i)));

  AssociationsIterator<int,Word> IC(C);

  Bool doneTest;
  for (i = 1; i < 101; i++) {
	 if ( (doneTest = IC.done()) == TRUE ) break;
	 IC.next();
  }

  validate( !shouldBeFalse && !doneTest && IC.done(),
				"Bool AssociationsIterator<int,Word>::done( ) const" );

  // Now check for duplicate keys in a stupid way:

  Bool valueTest;
  AssociationsIterator<int,Word> I1(C);
  while( !I1.done() ) {
	 AssociationsIterator<int,Word> I2 = I1;
	 while( I2.next() )
		if ( valueTest = (I1.key() == I2.key()) ) break;
	 I1.next();
  }

  validate( !valueTest,
				"AssociationsIterator<int,Word>& AssociationsIterator<int,Word>::operator = (const AssociationsIterator<int,Word>&) const" );

  validate( !valueTest,
				"Bool AssociationsIterator<int,Word>::next( )" );

  validate( 1,
				"int AssociationsIterator<int,Word>::key( ) const" );

  int count = 0;
  int sum = 0;
  I1.reset();
  while ( !I1.done() ) {
	 ++count;
	 sum += ord( (I1.value())[0] );
	 I1.next();
  }

  validate( count == 100 && sum == 50,
				"Word AssociationsIterator<int,Word>::value( ) const" );

  validate( count == 100 && sum == 50,
				"void AssociationsIterator<int,Word>::reset( )" );

  //@rn still need test for op==

  AssociationsOf<int,Word> D = C;
  AssociationsIterator<int,Word> I3(D);

  shouldBeFalse = (I1 == I3);
  I1.reset();
  Bool shouldBeTrue = (I1 == I3);
  I1.next();
  I3.next();

  validate( !shouldBeFalse && shouldBeTrue && (I1 == I3),
				"Bool AssociationsIterator<int,Word>::operator == ( const AssociationsIterator<int,Word>& ) const" );
  // Ok, it's pretty low-rent.

  sum = 0;
  ListOf<int> theKeys = D.keys();
  ListIterator< ListOf<int> > LI(theKeys);
  while( !LI.done() ) {
	 sum += LI.value();
	 LI.next();
  }

  validate( sum == 5050 && theKeys.length() == 100,
			 "ListOf<int> AssociationsIterator<int,Word>::keys( ) const" );


/* Maybe wanna see 'em?

  while ( !I.done() ) {
	 cout << "key: " << I.key() << endl;
	 cout << "val: " << I.value() << endl << endl;
	 I.next();
  }
*/

  if ( badCount == 0 ) cout << "\nAll tests pass.\n" << endl;
  else cout << endl << badCount << " tests failed.\n" << endl;
}

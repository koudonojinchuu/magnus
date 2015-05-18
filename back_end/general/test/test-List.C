// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Regression test of the ListOf and ListIterator classes.
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
#include "List.h"


int badCount = 0;


void validate(const int test, const char* name) {
  if ( test ) {
	 cout << "+ " << name << endl;
  } else {
	 ++badCount;
	 cout << "- " << name << endl;
  }
}

// Make sure ListOf does not require any more of the parameter
// than claimed, by instantiating one with minimal classes.

class testElement {
public:
  // Use compiler's copy constructor and destructor.
  testElement& operator = ( const testElement& tk ) {
	 dataMember == tk.dataMember;
	 return *this;
  }
  int operator == ( const testElement& tk ) const {
	 return dataMember == tk.dataMember;
  }
  friend ostream& operator << ( ostream& ostr, const testElement& tk ) {
	 ostr << tk.dataMember;
  }
private:
  testElement( ) { dataMember = 0; }
  int dataMember;
};


main( ) {

  ListOf<testElement> try_to_instantiate;

  cout << "Testing ListOf class\n" << endl;

  Chars c1("Calliope");
  Chars c2("Clio");
  Chars c3("Erato");
  Chars c4("Euterpe");
  Chars c5("Melpomene");
  Chars c6("Polymnia");
  Chars c7("Terpsichore");
  Chars c8("Thalia");
  Chars c9("Urania");

  Chars r1("Calliope");
  Chars r2("Clio");
  Chars r3("Erato");
  Chars r4("Euterpe");
  Chars r5("Melpomene");
  Chars r6("Polymnia");
  Chars r7("Terpsichore");
  Chars r8("Thalia");
  Chars r9("Urania");

  ListOf<Chars> L1;

  validate( L1.length() == 0,
				"ListOf<Chars>::ListOf<Chars>()" );

  ListOf<Chars> L4(c4);

  validate( L4.length() == 1 && L4[0] == r4,
				"ListOf<Chars>::ListOf<Chars>(Chars&)" );

  L1.append(c1);
  ListOf<Chars> L2(L1);

  validate( L2.length() == 1 && L2[0] == r1,
				"ListOf<Chars>::ListOf<Chars>(ListOf<Chars>&)" );

  ListOf<Chars> L3 = L2;

  validate( L3.length() == 1 && L3[0] == r1,
				"ListOf<Chars>& ListOf<Chars>::operator = (ListOf<Chars>&)" );

  validate( L2 == L3,
				"Bool ListOf<Chars>::operator == (const ListOf<Chars>&) const" );

  L3.append(c2);

  validate( L2 != L3 && L2 != L4,
				"Bool ListOf<Chars>::operator != (const ListOf<Chars>&) const" );

  validate( L3[0] == r1 && L3[1] == r2,
				"Chars& ListOf<Chars>::operator [ ] (int i) const" );

  ListOf<Chars> L20 = L3;

  validate( L20.equalTo(L3),
				"Bool ListOf<Chars>::equalTo(const ListOf<Chars>&) const" );

  validate( L3.element(0) == r1 && L3.element(1) == r2,
				"Chars& ListOf<Chars>::element(int i) const" );

  validate( L3.length() == 2,
				"int ListOf<Chars>::length() const" );

  validate( L3.contains(c1) && L3.contains(c2) && !L3.contains(c3),
				"Bool ListOf<Chars>::contains(const Chars&) const" );

  validate( L3.indexOf(c1) == 0 && L3.indexOf(c2) == 1 && L3.indexOf(c3) == -1,
				"int ListOf<Chars>::indexOf(const T&) const" );

  validate( L2.prefixOf(L3) && L3.prefixOf(L3),
				"Bool ListOf<Chars>::prefixOf(const ListOf<Chars>&) const" );

  validate( L2.properPrefixOf(L3) && !L3.properPrefixOf(L3),
				"Bool ListOf<Chars>::properPrefixOf(const ListOf<Chars>&) const" );

  ListOf<Chars> L5(c2);

  validate( L3.agreement(L3) == 2 && L3.agreement(L2) == 1 &&
				L2.agreement(L4) == 0 && L5.agreement(L3, 1) == 1,
				"int ListOf<Chars>::agreement(const ListOf<Chars>&,int start=0) const" );

  L3.append(c3); // Now L3 = (c1 c2 c3)

  ListOf<Chars> L6 = L3.sublist(0,2);
  ListOf<Chars> L7 = L3.sublist(1,3);
  ListOf<Chars> L8 = L3.sublist(0,3);
  ListOf<Chars> L9 = L3.sublist(1,1);

  validate( L6.length() == 2 && L6[0] == r1 && L6[1] == r2 &&
				L7.length() == 2 && L7[0] == r2 && L7[1] == r3 &&
				L8 == L3 && L9.length() == 0,
				"ListOf<Chars>& ListOf<Chars>::sublist(int,int) const" );

  ListOf<Chars> L10 = concatenate(L6, c3);
  ListOf<Chars> L11 = concatenate(c1, L7);

  validate( L10 == L3 && L11 == L3,
				"ListOf<Chars>& concatenate(const ListOf<Chars>&, const ListOf<Chars>&)" );

  ListOf<Chars> L12 = L9.reverse();
  ListOf<Chars> L13 = L4.reverse();
  ListOf<Chars> L14 = L8.reverse();

  validate( L12.length() == 0 &&
				L13.length() == 1 && L13[0] == r4 &&
				L14.length() == 3 && L14[0] == r3 && L14[1] == r2 && L14[2] == r1,
				"ListOf<Chars>& ListOf<Chars>::reverse() const" );

  L6.append(c3);
  L12.append(c6);

  validate( L6 == L3 && L12.length() == 1 && L12[0] == r6,
				"void ListOf<Chars>::append(const Chars&)" );

  L7.prepend(c1);
  ListOf<Chars> L15;
  L15.prepend(c7);

  validate( L7 == L3 && L15.length() == 1 && L15[0] == r7,
				"void ListOf<Chars>::prepend(const Chars&)" );

  ListOf<Chars> L16(c1);
  L16.append(c3);
  L16.insert(c2, 1);

  validate( L16 == L3,
				"void ListOf<Chars>::insert(const Chars&, int)" );

  L16.removeElement(c8);
  int remTest = (L16 == L3);
  L16.removeElement(c2);
  remTest = ( remTest && L16.length() == 2 && L16[0] == r1 && L16[1] == r3 );
  L16.removeElement(c2);
  L16.removeElement(c1);
  remTest = ( remTest && L16.length() == 1 && L16[0] == r3 );
  L16.removeElement(c3);
  L16.removeElement(c3);

  validate( remTest && L16.length() == 0,
				"void ListOf<Chars>::removeElement(const Chars&)" );

  L16 = L3;
  L16.removeElementOfIndex(1);
  remTest = ( L16.length() == 2 && L16[0] == r1 && L16[1] == r3 );
  L16.removeElementOfIndex(0);
  remTest = ( remTest && L16.length() == 1 && L16[0] == r3 );
  L16.removeElementOfIndex(0);

  validate( remTest && L16.length() == 0,
				"void ListOf<Chars>::removeElementOfIndex(int)" );

  ListOf<Chars> L17 = L3;
  L17.append(c4);
  L17.splice(L4,1,3);
  int spliceTest = ( L17.length() == 3 &&
							L17[0] == r1 && L17[1] == r4 && L17[2] == r4 );
  ListOf<Chars> L18(c1);
  L18.splice(L3.sublist(1,3),1,1);
  ListOf<Chars> L19(c3);
  L19.splice(L3.sublist(0,2),0,0);

  validate( spliceTest && L18 == L3 && L19 == L3,
				"void ListOf<Chars>::splice(const ListOf<Chars>&, int, int)" );


  // More or less canonical test of iterators:

  ListOf<Chars> L21(c1);
  L21.append(c2);
  L21.append(c3);
  L21.append(c4);
  L21.append(c5);
  L21.append(c6);
  L21.append(c7);
  L21.append(c8);
  L21.append(c9);

  Bool doneTest;
  ListIterator< ListOf<Chars> > I(L21);
  for (int k = 0; k < L21.length(); k++) {
	 if ( (doneTest = I.done()) == TRUE ) break;
	 I.next();
  }

  validate( 1,
				"ListIterator< ListOf<Chars> >::ListIterator< ListOf<Chars> >(const ListOf<Chars>&)" );
  validate( !doneTest && I.done(),
				"Bool ListIterator< ListOf<Chars> >::done() const" );

  // Now check for duplicate entries in a stupid way (none in L21):

  Bool valueTest;
  ListIterator< ListOf<Chars> > I1(L21);
  while( !I1.done() ) {
	 ListIterator< ListOf<Chars> > I2 = I1;
	 while( I2.next() )
		if ( valueTest = (I1.value() == I2.value()) ) break;
	 I1.next();
  }

  validate( !valueTest,
				"ListIterator< ListOf<Chars> >& ListIterator< ListOf<Chars> >::operator = (const ListIterator< ListOf<Chars> >&) const" );

  int count = 0;
  I1.reset();
  while( !I1.done() ) {
    ListIterator< ListOf<Chars> > I2(I1);
    do { if ( I1.value() == I2.value() ) ++count; } while( I2.next() );
	 I1.next();
  }

  int finalTest = ( count == L21.length() );

  validate( finalTest, "ListIterator< ListOf<Chars> >::ListIterator< ListOf<Chars> >(const ListIterator< ListOf<Chars> >&)" );
  validate( finalTest, "Chars ListIterator< ListOf<Chars> >::value() const" );
  validate( finalTest, "Bool ListIterator< ListOf<Chars> >::next()" );
  validate( finalTest, "void ListIterator< ListOf<Chars> >::reset()" );

  ListIterator< ListOf<Chars> > I3(L21);

  Bool shouldBeFalse = (I1 == I3);
  I1.reset();
  Bool shouldBeTrue = (I1 == I3);
  I1.next();
  I3.next();

  validate( !shouldBeFalse && shouldBeTrue && (I1 == I3),
				"Bool ListIterator< ListOf<Chars> >::operator == ( const ListIterator< ListOf<Chars> >& ) const" );
  // Ok, it's pretty low-rent.


  count = L21.length();
  I3.reset();
  while ( !I3.done() ) {
	 --count;
	 L21.removeElementOfIndex( count );
	 I3.next();
  }

  validate( count == 0 && L21.length() == 0,
				"Altered list while iterating over it." );


  if ( badCount == 0 ) cout << "\nAll tests pass.\n" << endl;
  else cout << endl << badCount << " tests failed.\n" << endl;
}

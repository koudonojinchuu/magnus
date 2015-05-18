// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Regression test of the DListOf and DListIterator classes.
//
// Some tests may fail because of a bug somewhere other than indicated.
//
// Principal Author: Sergey Lyutikov, Roger Needham
//
// Status: Useable.
//
// Revision History:
//

#include "Chars.h"
#include "DList.h"


int badCount = 0;


void validate(const int test, const char* name) {
  if ( test ) {
	 cout << "+ " << name << endl;
  } else {
	 ++badCount;
	 cout << "- " << name << endl;
  }
}

// Make sure DListOf does not require any more of the parameter
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

  DListOf<testElement> try_to_instantiate;

  cout << "Testing DListOf class\n" << endl;

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

  DListOf<Chars> L1;

  validate( L1.length() == 0,
				"DListOf<Chars>::DListOf<Chars>()" );

  DListOf<Chars> L4(c4);

  validate( L4.length() == 1 && L4[0] == r4,
				"DListOf<Chars>::DListOf<Chars>(Chars&)" );

  L1.append(c1);
  DListOf<Chars> L2(L1);

  validate( L2.length() == 1 && L2[0] == r1,
				"DListOf<Chars>::DListOf<Chars>(DListOf<Chars>&)" );

  DListOf<Chars> L3 = L2;

  validate( L3.length() == 1 && L3[0] == r1,
				"DListOf<Chars>& DListOf<Chars>::operator = (DListOf<Chars>&)" );

  validate( L2 == L3,
				"Bool DListOf<Chars>::operator == (const DListOf<Chars>&) const" );

  L3.append(c2);

  validate( L2 != L3 && L2 != L4,
				"Bool DListOf<Chars>::operator != (const DListOf<Chars>&) const" );

  validate( L3[0] == r1 && L3[1] == r2,
				"Chars& DListOf<Chars>::operator [ ] (int i) const" );

  DListOf<Chars> L20 = L3;

  validate( L20.equalTo(L3),
				"Bool DListOf<Chars>::equalTo(const DListOf<Chars>&) const" );

  validate( L3.element(0) == r1 && L3.element(1) == r2,
				"Chars& DListOf<Chars>::element(int i) const" );

  validate( L3.length() == 2,
				"int DListOf<Chars>::length() const" );

  validate( L3.contains(c1) && L3.contains(c2) && !L3.contains(c3),
				"Bool DListOf<Chars>::contains(const Chars&) const" );

  validate( L3.indexOf(c1) == 0 && L3.indexOf(c2) == 1 && L3.indexOf(c3) == -1,
				"int DListOf<Chars>::indexOf(const T&) const" );

  validate( L2.prefixOf(L3) && L3.prefixOf(L3),
				"Bool DListOf<Chars>::prefixOf(const DListOf<Chars>&) const" );

  validate( L2.properPrefixOf(L3) && !L3.properPrefixOf(L3),
				"Bool DListOf<Chars>::properPrefixOf(const DListOf<Chars>&) const" );

  DListOf<Chars> L5(c2);

  validate( L3.agreement(L3) == 2 && L3.agreement(L2) == 1 &&
				L2.agreement(L4) == 0 && L5.agreement(L3, 1) == 1,
				"int DListOf<Chars>::agreement(const DListOf<Chars>&,int start=0) const" );

  L3.append(c3); // Now L3 = (c1 c2 c3)

  DListOf<Chars> L6 = L3.sublist(0,2);
  DListOf<Chars> L7 = L3.sublist(1,3);
  DListOf<Chars> L8 = L3.sublist(0,3);
  DListOf<Chars> L9 = L3.sublist(1,1);

  validate( L6.length() == 2 && L6[0] == r1 && L6[1] == r2 &&
				L7.length() == 2 && L7[0] == r2 && L7[1] == r3 &&
				L8 == L3 && L9.length() == 0,
				"DListOf<Chars>& DListOf<Chars>::sublist(int,int) const" );

  DListOf<Chars> L10 = concatenate(L6, c3);
  DListOf<Chars> L11 = concatenate(c1, L7);

  validate( L10 == L3 && L11 == L3,
				"DListOf<Chars>& concatenate(const DListOf<Chars>&, const DListOf<Chars>&)" );

  DListOf<Chars> L12 = L9.reverse();
  DListOf<Chars> L13 = L4.reverse();
  DListOf<Chars> L14 = L8.reverse();

  validate( L12.length() == 0 &&
				L13.length() == 1 && L13[0] == r4 &&
				L14.length() == 3 && L14[0] == r3 && L14[1] == r2 && L14[2] == r1,
				"DListOf<Chars>& DListOf<Chars>::reverse() const" );

  L6.append(c3);
  L12.append(c6);

  validate( L6 == L3 && L12.length() == 1 && L12[0] == r6,
				"void DListOf<Chars>::append(const Chars&)" );

  L7.prepend(c1);
  DListOf<Chars> L15;
  L15.prepend(c7);

  validate( L7 == L3 && L15.length() == 1 && L15[0] == r7,
				"void DListOf<Chars>::prepend(const Chars&)" );

  DListOf<Chars> L16(c1);
  L16.append(c3);
  L16.insert(c2, 1);

  validate( L16 == L3,
				"void DListOf<Chars>::insert(const Chars&, int)" );

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
				"void DListOf<Chars>::removeElement(const Chars&)" );

  L16 = L3;
  L16.removeElementOfIndex(1);
  remTest = ( L16.length() == 2 && L16[0] == r1 && L16[1] == r3 );
  L16.removeElementOfIndex(0);
  remTest = ( remTest && L16.length() == 1 && L16[0] == r3 );
  L16.removeElementOfIndex(0);

  validate( remTest && L16.length() == 0,
				"void DListOf<Chars>::removeElementOfIndex(int)" );

  DListOf<Chars> L17 = L3;
  L17.append(c4);
  L17.splice(L4,1,3);
  int spliceTest = ( L17.length() == 3 &&
							L17[0] == r1 && L17[1] == r4 && L17[2] == r4 );
  DListOf<Chars> L18(c1);
  L18.splice(L3.sublist(1,3),1,1);
  DListOf<Chars> L19(c3);
  L19.splice(L3.sublist(0,2),0,0);

  validate( spliceTest && L18 == L3 && L19 == L3,
				"void DListOf<Chars>::splice(const DListOf<Chars>&, int, int)" );


  // More or less canonical test of iterators:

  DListOf<Chars> L21(c1);
  L21.append(c2);
  L21.append(c3);
  L21.append(c4);
  L21.append(c5);
  L21.append(c6);
  L21.append(c7);
  L21.append(c8);
  L21.append(c9);

  Bool doneTest;
  DListIterator< DListOf<Chars> > I(L21);
  for (int k = 0; k < L21.length(); k++) {
	 if ( (doneTest = I.done()) == TRUE ) break;
	 I.next();
  }

  validate( 1,
				"DListIterator< DListOf<Chars> >::DListIterator< DListOf<Chars> >(const DListOf<Chars>&)" );
  validate( !doneTest && I.done(),
				"Bool DListIterator< DListOf<Chars> >::done() const" );

  // Now check for duplicate entries in a stupid way (none in L21):

  Bool valueTest;
  DListIterator< DListOf<Chars> > I1(L21);
  while( !I1.done() ) {
	 DListIterator< DListOf<Chars> > I2 = I1;
	 while( I2.next() )
		if ( valueTest = (I1.value() == I2.value()) ) break;
	 I1.next();
  }

  validate( !valueTest,
		"DListIterator< DListOf<Chars> >& DListIterator< DListOf<Chars> >::operator = (const DListIterator< DListOf<Chars> >&) const" );

  int count = 0;
  I1.reset();
  while( !I1.done() ) {
    DListIterator< DListOf<Chars> > I2(I1);
    do { if ( I1.value() == I2.value() ) ++count; } while( I2.next() );
	 I1.next();
  }

  DListIterator< DListOf<Chars> > I4( L14 );
  DListIterator< DListOf<Chars> > I5( L8 );

  I5.setToEnd();

  int checkIt = TRUE;

  int finalTest = ( count == L21.length() );

  count = 0;

  while( I4.valid() || I5.valid() ) {
	if ( I4.value() != I5.value() ) {
		checkIt = FALSE;
		break;
	}
        count++;
	I4.next();
	I5.previous();
  }
  
  validate( checkIt && count == L14.length(), "Chars DListIterator< DListOf<Chars> >::previous()" );

  validate( checkIt && count == L8.length(), "Chars DListIterator< DListOf<Chars> >::setToEnd()" );

  DListOf<Chars> L22;
  L22.append( c1 );
  L22.append( c2 );
  L22.append( c3 );
  L22.append( c4 );
  L22.append( c5 );

  DListOf<Chars> L23;
  L23.append( c7 );
  L23.append( c8 );
  L23.append( c9 );

  L22.insert( L23, 5 );

  DListOf<Chars> L24 = L22.reverse();

  DListIterator< DListOf<Chars> > I6( L22 );
  DListIterator< DListOf<Chars> > I7( L24 );

  I6.setToEnd();
  count = 0;
  while( I6.valid() || I7.valid() ) {
	if ( I6.value() != I7.value() ) {
		checkIt = FALSE;
		break;
	}
	count++;
	I7.next();
	I6.previous();
  }

  validate( 	L22.length() == 8 &&
		count == 8 &&
		checkIt, "DListOf<Chars>::insert( const DListOf&, int )" );

  validate( finalTest, "DListIterator< DListOf<Chars> >::DListIterator< DListOf<Chars> >(const DListIterator< DListOf<Chars> >&)" );
  validate( finalTest, "Chars DListIterator< DListOf<Chars> >::value() const" );
  validate( finalTest, "Bool DListIterator< DListOf<Chars> >::next()" );
  validate( finalTest, "void DListIterator< DListOf<Chars> >::reset()" );

  DListIterator< DListOf<Chars> > I3(L21);

  Bool shouldBeFalse = (I1 == I3);
  I1.reset();
  Bool shouldBeTrue = (I1 == I3);
  I1.next();
  I3.next();

  validate( !shouldBeFalse && shouldBeTrue && (I1 == I3),
				"Bool DListIterator< DListOf<Chars> >::operator == ( const DListIterator< DListOf<Chars> >& ) const" );
  // Ok, it's pretty low-rent.


  count = L21.length();
  I3.setToEnd();
  while ( !I3.done() ) {
	 --count;
	 L21.removeElementOfIndex( count );
	 I3.previous();
  }

  validate( count == 0 && L21.length() == 0,
				"Altered list while iterating over it." );


  if ( badCount == 0 ) cout << "\nAll tests pass.\n" << endl;
  else cout << endl << badCount << " tests failed.\n" << endl;
}










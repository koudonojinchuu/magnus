// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Partial regression tests of SetOf and SetIterator classes.
//           Because of the inherent `randomness' of hash tables,
//           we can't hope to test everything in reasonable time.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#include "Set.h"
#include "Word.h"


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
  for( int i = 0; i < len; i++ ) v[i] = *p++;
  return Word(v);
}


// Make sure SetOf does not require any more of the parameter
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
  int hash( ) const { return 0; }
//  friend ostream& operator << ( ostream& ostr, const testElement& tk ) {
//	 ostr << tk.dataMember;
//  }
private:
  testElement( ) { dataMember = 0; }
  int dataMember;
};

//template class SetData<testElement>;

main( ) {

  SetOf<testElement> try_to_instantiate;

  // Many array initializers end in 17(s) to work around a mind-boggling
  // bug in g++ 2.6.0: initializers must have length at least 4!?

  cout << "Testing SetOf class\n" << endl;

  const Generator i1[] = { 1, 2, 3, 17 };
  Word w1 = initWord(i1, 3);

  SetOf<Word> S1(4);
  validate( S1.cardinality() == 0, "SetOf<Word>::SetOf<Word>(int)" );

  SetOf<Word> T1(w1);

  validate( T1.cardinality() == 1 && T1.contains(w1),
				"SetOf<Word>::SetOf<Word>( const Word&)" );

  S1.adjoinElement(w1);
  SetOf<Word> S2 = S1;

  Word w2 = w1; w2[0] = -1;
  Word w3 = w1; w3[0] = -2;

  S2.adjoinElement(w2);
  S1.adjoinElement(w3);

//  cout << "S1 = " << S1 << endl;
//  cout << "S2 = " << S2 << endl;

  validate( S1.cardinality() == 2 && S2.cardinality() == 2,
				"int SetOf<Word>::cardinality() const" );
  
  validate( S2.contains(w1) && S2.contains(w2) && !S2.contains(w3) &&
				S1.contains(w1) && S1.contains(w3) && !S1.contains(w2),
				"Bool SetOf<Word>::contains(const Word&) const" );

  validate( 1, "void SetOf<Word>::adjoinElement(const Word&)" );

  const Generator i4[] = { -2, 2, 3, 17 };
  Word w4 = initWord(i4, 3);

  S1.removeElement(w4);
  S2.removeElement(w4);

  validate( S1.cardinality() == 1 && S1.contains(w1) &&
				S2.cardinality() == 2 && S2.contains(w1) && S2.contains(w2),
				"void SetOf<Word>::removeElement(const Word&)" );

  validate( !S1.contains(S2) && S2.contains(S1),
				"Bool contains(const SetOf<Word>&) const" );

  validate( !S2.properlyContains(S2) && S2.properlyContains(S1),
				"Bool SetOf<Word>::properlyContains(const SetOf<Word>&) const" );

  S1.removeAllElements();

  validate( S1.cardinality() == 0 && !S1.contains(w1),
				"void SetOf<Word>::removeAllElements()" );


  SetOf<Word> S3 = setUnion(S1, S2);

  validate( S3 == S2,
				"Bool SetOf<Word>::operator == (const SetOf<Word>&) const" );
  validate( S2.equalTo(S3),
				"Bool SetOf<Word>::equalTo(const SetOf<Word>&) const" );

  S3.adjoinElement(w3);

  validate( setIntersection(S1, S2) == S1 &&
				setIntersection(S3, S2) == S2,
				"SetOf<Word> setIntersection(const SetOf<Word>&, const SetOf<Word>&)" );

  S3.removeElement(w2);
  SetOf<Word> S4 = setUnion(S2, S3);

  validate( S4.cardinality() == 3 &&
				S4.contains(w1) && S4.contains(w2) && S4.contains(w3),
				"SetOf<Word> setUnion(const SetOf<Word>&, const SetOf<Word>&)" );

  S1 = setMinus(S4, S3);
  S2 = setMinus(S3, S4);

  validate( S1.cardinality() == 1 && S1.contains(w2) && S2.cardinality() == 0,
				"SetOf<Word> setMinus(const SetOf<Word>&, const SetOf<Word>&)" );

  SetOf<Word> U1(w1);
  SetOf<Word> U2(w2);
  U2.adjoinElement(w3);
  w4[0] = -3;
  SetOf<Word> U4(w4);

  U1.adjoinElements(U2);
  U4.adjoinElements(U2);

  validate( U1.cardinality() == 3 &&
				U1.contains(w1) && U1.contains(w2) && U1.contains(w3) &&
				U4.cardinality() == 3 &&
				U4.contains(w2) && U4.contains(w3) && U4.contains(w4),
				"void SetOf<Word>::adjoinElements(const SetOf<Word>&)" );

  SetOf<Word> U3 = setSymmetricDifference(U1, U4);

  validate( U3.cardinality() == 2 && U3.contains(w1) && U3.contains(w4),
				"SetOf<Word> setSymmetricDifference(const SetOf<Word>&, const SetOf<Word>&)" );

  U1.removeElements(U4);

  validate( U1.cardinality() == 1 && U1.contains(w1),
				"void SetOf<Word>::removeElements(const SetOf<Word>&)" );

  U4.shrinkToIntersectionWith(U3);

//  cout << "U3 = " << U3 << endl;
//  cout << "U4 = " << U4 << endl;
//  cout << "\n\n |U4| = " << U4.cardinality() << endl << endl;

  validate( U4.cardinality() == 1 && U4.contains(w4),
				"void SetOf<Word>::shrinkToIntersectionWith(const SetOf<Word>&)" );


  // More or less canonical test of iterators:

  // Make a fairly big random SetOf<Word>, then iterate over it.

  srandom(2112);
  for (int k = 0; k < 999; k++) {
	 int len = 4 + abs( (int)(random() % 31) );
	 Generator *tmp = new Generator[len];
	 for (int j = 0; j < len; j++) {
		if ( (int)(random() % 2) == 0 )
		  tmp[j] = 1 + abs((int)(random() % 12));
		else
		  tmp[j] = -1 - abs((int)(random() % 12));
	 }
	 Word w = initWord(tmp, len);
	 delete tmp;
	 S2.adjoinElement(w);
  }

  Bool doneTest;
  SetIterator<Word> I(S2);
  for (int k = 0; k < S2.cardinality(); k++) {
	 if ( (doneTest = I.done()) == TRUE ) break;
	 I.next();
  }

  validate( 1, "SetIterator<Word>::SetIterator<Word>(const SetOf<Word>&)" );
  validate( !doneTest && I.done(), "Bool SetIterator<Word>::done() const" );

  // Now check for duplicate entries in a stupid way:

  Bool valueTest;
  SetIterator<Word> I1(S2);
  while( !I1.done() ) {
	 SetIterator<Word> I2 = I1;
	 while( I2.next() )
		if ( valueTest = (I1.value() == I2.value()) ) break;
	 I1.next();
  }

  validate( !valueTest,
				"SetIterator<Word>& SetIterator<Word>::operator = (const SetIterator<Word>&)" );

  int count = 0;
  I1.reset();
  while( !I1.done() ) {
    SetIterator<Word> I2(I1);
    do { if ( I1.value() == I2.value() ) ++count; } while( I2.next() );
	 I1.next();
  }

  int finalTest = ( count == S2.cardinality() );

  validate( finalTest, "SetIterator<Word>::SetIterator<Word>(const SetIterator<Word>&)" );
  validate( finalTest, "Word SetIterator<Word>::value() const" );
  validate( finalTest, "void SetIterator<Word>::next()" );
  validate( finalTest, "void SetIterator<Word>::reset()" );

  SetIterator<Word> I3(S2);

  Bool shouldBeFalse = (I1 == I3);
  I1.reset();
  Bool shouldBeTrue = (I1 == I3);
  I1.next();
  I3.next();

  validate( !shouldBeFalse && shouldBeTrue && (I1 == I3),
				"Bool SetIterator<Word>::operator == (const SetIterator<Word>&) const" );
  // Ok, it's pretty low-rent.


  count = S2.cardinality();
  I3.reset();
  S2.removeAllElements();
  while ( !I3.done() ) {    //@rn adjoin random elts for good measure
	 --count;
	 I3.next();
  }

  validate( count == 0,
				"Altered set while iterating over it." );


  if ( badCount == 0 ) cout << "\nAll tests pass.\n" << endl;
  else cout << endl << badCount << " tests failed.\n" << endl;
}

/* Not tested (yet) since they just call tested member functions:

  Bool SetOf<Word>::operator != (const SetOf<Word>&) const

  Bool SetOf<Word>::operator < (const SetOf<Word>&) const

  Bool SetOf<Word>::operator <= (const SetOf<Word>&) const

  Bool SetOf<Word>::operator > (const SetOf<Word>&) const

  Bool SetOf<Word>::operator >= (const SetOf<Word>&) const

  Bool SetOf<Word>::operator >= (const Word&) const

  SetOf<Word> SetOf<Word>::operator & (const SetOf<Word>&) const

  SetOf<Word> SetOf<Word>::operator &= (const SetOf<Word>&)

  SetOf<Word> SetOf<Word>::operator | (const SetOf<Word>&) const

  SetOf<Word> SetOf<Word>::operator |= (const SetOf<Word>&)

  SetOf<Word> SetOf<Word>::operator - (const SetOf<Word>&) const

  SetOf<Word> SetOf<Word>::operator -= (const SetOf<Word>&)

  SetOf<Word> SetOf<Word>::operator ^ (const SetOf<Word>&) const

  SetOf<Word> SetOf<Word>::operator |= (const Word&)

  SetOf<Word> SetOf<Word>::operator -= (const Word&)
*/

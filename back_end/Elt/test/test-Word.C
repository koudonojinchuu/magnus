// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Regression test of the Word class.
//
// Principal Author: Roger Needham
//
// Status: Useable with g++ 2.5.8
//         Array initializers are not implemented in
//         ObjectFuckingCenter 2.0.4.
//
// Revision History:
//

// This does explicit tests of all public methods of the Word class.
// It does not explicitly test private methods, or methods of the
// ancillary classes Generator and Genref, since if the public tests
// pass, it's a safe bet the rest is ok.


#include "Word.h"


int badCount = 0;


int cmpWords(const Word& w, const Generator *p, const int len) {
  if ( w.length() != len ) {
	 cout << "\n# ";
	 for (short i = 0; i < w.length(); i++) cout << w[i] << ' ';
	 cout << "should have length " << len << endl;
	 return 0;
  }
  for (int i = 0; i < len; i++)
	 if ( w[i] != p[i] ) {
		cout << "\n# ";
		for (short i = 0; i < w.length(); i++) cout << w[i] << ' ';
		cout << "should be ";
		for (i = 0; i < len; i++) cout << p[i] << ' ';
		cout << endl;
		return 0;
	 }
  return 1;
}


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


main( ) {

  // Some of these tests may fail because of a bug somewhere other
  // than indicated.

  cout << "Testing Word class\n" << endl;

  // Test public methods of Word:

  const Generator i1[] = { 2, 3, 4, 5, 6 };
  Word w1 = initWord(i1,5);

//  cout << "\nw1 = " << w1 << endl << endl;

  const Generator p1[] = { 2, 3, 4, 5, 6 };
  validate( cmpWords( w1, p1, 5 ), "Word::Word(const VectorOf<Generator>&)");

  const Generator i2[] = { 2, 3, 4, 5, 5 };
  Word w2 = initWord(i2,5);
  const Generator p2[] = { 2, 3, 4, 5, 5 };

  Elt e1 = w1;

//  cout << "\ne1 = " << e1 << endl << endl;

  Word w3(e1);

//  cout << "\nw3 = " << w3 << endl << endl;

  validate( cmpWords( w3, p1, 5 ), "Word::Word( const Elt& e )");

  validate( w3.length() == 5, "int Word::length( ) const");

  w3[2] = -4;
  validate( w3[2] == -4 && w1[2] == 4,
				"Genref Word::operator [] (int)");

  const Generator i4[] = { 1, 2, -1, -2 };
  Word w4 = initWord(i4,4);
  validate( (w4 < w3) && (w2 < w1) && (w1 < w3),
				"Bool Word::operator < (const Word&) const");

  const Generator r1[] = { 2, 3, 4, 5, -6 };
  const Generator r2[] = { 1, 2, 2, 1 };
  const Generator i5[] = { -2, -2, -2, 17 }; //@rn added 17
  Word w5 = initWord(i5,3);
  const Generator r3[] = { 1, 1, 1, 1 };
  validate( cmpWords( w1.nextInShortLex(6), r1, 5 ) &&
				cmpWords( w4.nextInShortLex(2), r2, 4 ) &&
				cmpWords( w5.nextInShortLex(2), r3, 4 ),
				"Word Word::nextInShortLex(Generator&) const");

  const Generator r4[] = { 3, 4, 5, 17 }; //@rn added 17
  validate( cmpWords( w1.subword(1,4), r4, 3 ) &&
				cmpWords( w1.subword(1,1), r4, 0 ),
				"Word Word::subword(const int, const int) const");

  const Generator r5[] = { 2, 3, 4, 5 };
  validate( cmpWords( w1.initialSegment(4), r5, 4 ),
				"Word Word::initialSegment(const int) const");

  const Generator r6[] = { 4, 5, 6, 17 }; //@rn added 17
  validate( cmpWords( w1.terminalSegment(3), r6, 3 ),
				"Word Word::terminalSegment(const int) const");

  validate( cmpWords( w1.findAgreement(w2), r5, 4 ),
				"Word Word::findAgreement(const Word&) const");

  const Generator i19[] = { 2, 3, 4, 5, 8, 7 };
  const Generator r19[] = { -7, -8, 6, 17 }; //@rn added 17
  Word w19 = initWord(i19,6);
  const Generator i20[] = { 3, 4, 5, -9, 8 };
  const Generator r20[] = { 2, -8, 9, 6 };
  Word w20 = initWord(i20,5);
  const Generator i21[] = { 2, 3, -9, 8 };
  const Generator r21[] = { -8, 9, 4, 5, 6 };
  Word w21 = initWord(i21,4);
  const Generator i22[] = { 5, 6, 17, 17 }; //@rn added 17's
  const Generator r22[] = { 2, 3, 4, 17 }; //@rn added 17
  Word w22 = initWord(i22,2);
  const Generator r23[] = { 2, 3, 4, 6 };
  Word w23 = initWord(i22,1);
  Word w24 = initWord(i22,0);
  // w1 == { 2, 3, 4, 5, 6 }
  validate( cmpWords( w1.shortenByRelator(w19), r19, 3 ) &&
				cmpWords( w1.shortenByRelator(w20), r20, 4 ) &&
				cmpWords( w1.shortenByRelator(w21), p1, 5 ) &&
				cmpWords( w1.shortenByRelator(w22), r22, 3 ) &&
				cmpWords( w1.shortenByRelator(w23), r23, 4 ) &&
				cmpWords( w1.shortenByRelator(w24), p1, 5 ),
				"Word Word::shortenByRelator(const Word&) const");

  validate( w2.numberOfOccurrences(5) == 2 &&
				w4.numberOfOccurrences(-2) == 2 &&
				w5.numberOfOccurrences(-2) == 3 &&
				w1.numberOfOccurrences(8) == 0,
				"int Word::numberOfOccurrences(const Generator&) const");

  validate( w2.exponentSum(5) == 2 &&
				w4.exponentSum(-2) == 0 &&
				w5.exponentSum(-2) == 3 &&
				w1.exponentSum(8) == 0,
				"int Word::exponentSum(const Generator&) const");

  validate( w1.maxOccurringGenerator() == 6 &&
				w5.maxOccurringGenerator() == 2,
				"Generator Word::maxOccurringGenerator() const");

  const Generator r7[] = { 1, 3, 4, 5, -1, -5, -4, -3 };
  validate( cmpWords( w4.replaceGeneratorWithWord(2,w1.subword(1,4)), r7, 8) &&
				cmpWords( w4.replaceGeneratorWithWord(3,w5), i4, 4 ),
				"Word Word::replaceGeneratorWithWord(const Generator&, const Word&) const");

  const Generator r8[] = { 1, -2, -2, -2, -2 };
  validate( cmpWords( w4.replaceSubword(1,3,w5), r8, 5 ) &&
				cmpWords( w4.replaceSubword(0,4, w5), i5, 3 ),
				"Word Word::replaceSubword(const int, const int, const Word&) const");

  const Generator i10[] = { 2, 3, 4, -4, 5 };
  const Generator r10[] = { 2, 3, 5, 17 }; //@rn added 17
  const Generator i11[] = { 2, -3, 4, -4, 3 };
  const Generator r11[] = { 2, 99, 17, 17 };               // avoid GNU bug
  const Generator i12[] = { -2, 2, -3, 4, -4, 3 };
  const Generator r12[] = { 99, 99, 17, 17 };              // avoid GNU bug
  Word w10 = initWord(i10,5);
  Word w11 = initWord(i11,5);
  Word w12 = initWord(i12,6);
  validate( cmpWords( w10.freelyReduce(), r10, 3 ) &&
				cmpWords( w11.freelyReduce(), r11, 1 ) &&
				cmpWords( w12.freelyReduce(), r12, 0 ),
				"Word Word::freelyReduce() const");

  const Generator i13[] = { -5, 2, 3, 4, -4, 5, -6, 6 };
  const Generator r13[] = { 2, 3, 17, 17 }; //@rn added 17's
  Word w13 = initWord(i13,8);
  validate( cmpWords( w13.cyclicallyReduce(), r13, 2 ),
				"Word Word::cyclicallyReduce() const");

  const Generator r14[] = { 2, 3, 4, 5, 6 };
  const Generator r15[] = { 6, 2, 3, 4, 5 };
  validate( cmpWords( w1.cyclicallyPermute(5), r14, 5 ) &&
				cmpWords( w1.cyclicallyPermute(4), r15, 5 ) &&
				cmpWords( w1.cyclicallyPermute(-1), r15, 5 ),
				"Word Word::cyclicallyPermute(const int) const");

  const Generator r27[] = { 1, 2, -1, -2, 2, 3, 4, 5, 6 };
  Word w27 = w4 * w1;

  validate( cmpWords( w27, r27, 9 ),
				"Word Word::operator * ( const Word& )" );

  const Generator r28[] = { -2, 2, 3, 4, 5, 6 };
  Word w28 = w4[3] * w1;

  validate( cmpWords( w28, r28, 6 ),
				"Word operator * ( const Generator&, const Word& )" );

  const Generator r29[] = { 1, 2, -1, -2, 2 };
  Word w29 = w4 * w4[1];

  validate( cmpWords( w29, r29, 5 ),
				"Word Word::operator * ( const Generator& )" );

  const Generator r50[] = { -2, 2, 17, 17 };
  Word w50 = w4[3] * w4[1];

  validate( cmpWords( w50, r50, 2 ),
				"Word operator * ( const Generator&, const Generator& )" );

  const Generator r40[] = { 1, 2, -1, -2, 2, -2, 2, 3, 4, 5, 6 };
  w29 *= w28;

  validate( cmpWords( w29, r40, 11 ),
				"Word Word::operator *= ( const Word& )" );

  const Generator r41[] = { 1, 2, -1, -2, 2, 3, 4, 5, 6, -6 };
  w27 *= inv(w1[4]);

  validate( cmpWords( w27, r41, 10 ),
				"Word Word::operator *= ( const Generator& )" );

  Word w51(w50[0]);

  validate( cmpWords( w51, r50, 1 ), "Word::Word( const Generator& )" );


  // Test public methods inherited from Elt:

  const Generator i30[] = { 1, 2, -1, -2 };
  Word w30 = initWord(i30,4);
  validate( w1 == w1 && w30 == w4,
				"Bool Elt::operator == ( const Elt& ) const");

  Elt e27 = ((Elt)w4) * ((Elt)w1);
  Elt e28 = ((Elt)((Word)Generator(w4[3]))) * ((Elt)w1);
  Elt e29 = ((Elt)w4) * ((Elt)((Word)Generator(w4[1])));
  validate( cmpWords( e27, r27, 9 ) &&
				cmpWords( e28, r28, 6 ) &&
				cmpWords( e29, r29, 5 ),
				"Elt Elt::operator * ( const Elt& ) const" );

  e29 *= e28;
  validate( cmpWords( e29, r40, 11 ),
				"Elt Elt::operator *= ( const Elt& )" );

  const Generator r31[] = { 2, 1, -2, -1 };
  Word w31 = w4.inverse();
  validate( cmpWords( w31, r31, 4),
				"Elt Elt::inverse() const");

  const Generator r32[] = { 1, 2, -1, -2, 2, 3, 4, 5, 6, 2, 1, -2, -1 };
  Word w32 = w1.conjugateBy(w31);
  validate( cmpWords( w32, r32, 13),
				"Elt Elt::conjugateBy( const Elt& ) const");

  const Generator r33[] = { -6, -5, -4, -3, -2, 1, 2, -1, -2, 2, 3, 4, 5, 6, 2, 1, -2, -1 };
  Word w33 = commutator( w1, w31 );
  validate( cmpWords( w33, r33, 18),
				"Elt commutator( const Elt&, const Elt& )");

  Word w34 = w30;
  validate( cmpWords( w34, i30, 4 ),
				"Word& Word::operator = (const Word&)");


  if ( badCount == 0 ) cout << "\nAll tests pass.\n" << endl;
  else cout << endl << badCount << " tests failed.\n" << endl;
}

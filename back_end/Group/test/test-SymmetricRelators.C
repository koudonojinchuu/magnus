// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Tests of SymmetricRelatorsIterator class
//
// Principal Author: Dmitry Bormotov
//
// Status: 
//
// Revision History:
//

#include "SymmetricRelators.h"

int errorCount = 0;

void validate(const bool test, const char* title) {
  if ( test ) {
	 cout << "+ " << title << endl;
  } else {
	 ++errorCount;
	 cout << "- " << title << endl;
  }
}

Word initWord(const Generator* p, int len) {
  VectorOf<Generator> v(len);
  for( int i = 0; i < len; i ++ ) v[i] = *p++;
  return Word(v);
}

void main() {    
  cout << "Testing SymmetricRelatorsIterator class.\n" << endl;
 
// Compare the set of symmetrised relators with the set received
// using SymmetricRelatorsIterator.

  const Generator gl1[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1};
  const Generator gl2[] = { 2, 2, 2, 2, 2, 2, 2, 2, 2 };
  const Generator gl3[] = { 1, 2, 3, -1, 2, -3, -3 };
  Word w1 = initWord(gl1, 9);
  Word w2 = initWord(gl2, 9);
  Word w3 = initWord(gl3, 7);
  
  SetOf<Word> S;
  S.adjoinElement(w1);
  S.adjoinElement(w2);
  S.adjoinElement(w3);
  SetOf<Word> symmSet(S);
  symmetrise(symmSet);
  //cout << "symmSet:" << endl << symmSet;

  SetOf<Word> symmRelsSet;
  SymmetricRelatorsIterator I(S); 
 
  int count = 0;
  for ( ; !I.done(); I.next()) {
    symmRelsSet |= I.value();
    ++count;
  }
  //cout << "symmRelsSet:" << endl << symmRelsSet;
  //cout << "count = " << count << endl;

  validate( true, "SymmetricRelatorsIterator::SymmetricRelatorsIterator"
	   "(const SymmetricRelators&)");
  validate( (symmSet.cardinality() == count) && (symmSet == symmRelsSet), 
	   "Bool SymmetricRelatorsIterator::done() const" );

  // Check for duplicate values:

  Bool bSuccess = true;
  symmSet.removeAllElements();
  for( I.reset(); !I.done(); I.next() ) {
    if( symmSet.contains( I.value() ) ) { bSuccess = false; break; }
    symmSet |= I.value();
  }

  validate( bSuccess, "Word SymmetricRelatorsIterator::value() const" );
  validate( bSuccess, "Bool SymmetricRelatorsIterator::next()" );
  validate( bSuccess, "void SymmetricRelatorsIterator::reset()" );

  // The different ways constructing iterators. 

  bSuccess = true;
  I.reset();
  SymmetricRelatorsIterator J1(I);
  SymmetricRelatorsIterator J2(S);
  J1.next();
  if( J1 == J2 ) bSuccess = false;
  J2.next();
  if(!(J1 == J2)) bSuccess = false;
  J2.reset();
  SymmetricRelatorsIterator J3 = J2;
  if(!(J3 == J2)) bSuccess = false;
  J1.next();
  J3.next();
  J3.next();
  if(!(J1 == J3)) bSuccess = false;
  
  validate(bSuccess, "SymmetricRelatorsIterator::SymmetricRelatorsIterator"
	   "(const SymmetricRelatorsIterator&)");
  validate( bSuccess, "SymmetricRelatorsIterator& SymmetricRelatorsIterator::"
	   "operator = (const SymmetricRelatorsIterator&)" );
  validate( bSuccess,"Bool SymmetricRelatorsIterator::operator == (const "
	   "SymmetricRelatorsIterator&) const");

  if ( errorCount == 0 ) cout << "\nAll tests pass.\n" << endl;
  else cout << endl << errorCount << " tests failed.\n" << endl;
}



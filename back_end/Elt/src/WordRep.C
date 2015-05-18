// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of WordRep class.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//

#include "WordRep.h"

Bool WordRep::operator == ( const EltRep& a ) const
{
  const WordRep& w = (WordRep&)a;

  if ( length() != w.length() ) return 0;

  cGeneratorPtrType p1 = cFirst();
  cGeneratorPtrType p2 = w.cFirst();
  for (int i = 0; i < length(); i++)
	 if ( *p1++ != *p2++ ) return 0;
  return 1;
}


int WordRep::hash() const
// Borrowed from Hamish (so blame him).
{
  int h = 0;
  int g;
  int i = 0;
  cGeneratorPtrType p = cFirst();
  while ( i++ < length() ) {
	 h = (h << 4) + *p++;
	 if ((g = h & 0xf0000000) != 0)
		h = (h ^ (g >> 24)) ^ g;
  }
  return h;
}


/*
EltRep* WordRep::operator * ( const EltRep& a ) const {

  // @rn Old version which freely reduces. Can re-use in FreeGroup.
  
  const WordRep& w = (WordRep&)a;
  
  int i = 0;
  int minl = length() < w.length() ? length() : w.length();
  cGeneratorPtrType t = cFirst();
  cGeneratorPtrType e = cLast();
  cGeneratorPtrType b = w.cFirst();
  if ( minl != 0 ) while ( *e == -*b && ++i < minl ) { e--; b++; }
  WordRep* res = new WordRep(length() + w.length() -2*i);
  GeneratorPtrType r = res->first();
  for (int j = 0; j < length() - i; j++) *r++ = *t++;
  for (j = 0; j < w.length() - i; j++) *r++ = *b++;
  
  return res;
}
*/

EltRep* WordRep::operator * ( const EltRep& a ) const {
  
  const WordRep& w = (WordRep&)a;

  int len = length();
  int w_len = w.length();

  WordRep* result = new WordRep(len + w_len);
  cGeneratorPtrType tp = cFirst();
  GeneratorPtrType rp = result->first();
  for( int i = 0; i < len; i++) *rp++ = *tp++;
  tp = w.cFirst();
  for( int i = 0; i < w_len; i++ ) *rp++ = *tp++;

  return result;
}


EltRep* WordRep::inverse() const {
  int len = length();
  WordRep* result = new WordRep(len);
  cGeneratorPtrType tp = cFirst();
  GeneratorPtrType rp = result->last();
  while ( len-- ) *rp-- = -*tp++;
  return result;
}


EltRep* WordRep::rightMultBy( const Generator& x ) const {
  #if SAFETY > 0
    if ( ord(x) == 0 )
		error("Tried to right-multiply Word with Generator of ord 0");
  #endif
  int len = length();
  WordRep* result = new WordRep(len + 1);
  cGeneratorPtrType tp = cFirst();
  GeneratorPtrType rp = result->first();
  while ( len-- ) *rp++ = *tp++;
  *rp++ = ord(x);
  return result;
}


EltRep* WordRep::leftMultBy( const Generator& x ) const {
  #if SAFETY > 0
    if ( ord(x) == 0 )
		error("Tried to left-multiply Word with Generator of ord 0");
  #endif
  int len = length();
  WordRep* result = new WordRep(len + 1);
  cGeneratorPtrType tp = cFirst();
  GeneratorPtrType rp = result->first();
  *rp++ = ord(x);
  while ( len-- ) *rp++ = *tp++;
  return result;
}


EltRep* WordRep::conjugateBy( const EltRep* ep ) const {
  const WordRep& a = (WordRep&)*this;
  const WordRep& b = (WordRep&)*ep;
  WordRep* result = new WordRep( a.length() + 2 * b.length() );
  GeneratorPtrType rp = result->first();
  cGeneratorPtrType tp = b.cLast();
  int i = b.length();
  while ( i-- ) *rp++ = -*tp--;
  tp = a.cFirst();
  i = a.length();
  while ( i-- ) *rp++ = *tp++;
  tp = b.cFirst();
  i = b.length();
  while ( i-- ) *rp++ = *tp++;
  return result;
}


EltRep* WordRep::commutatorWith( const EltRep* ep ) const {
  const WordRep& a = (WordRep&)*this;
  const WordRep& b = (WordRep&)*ep;
  WordRep* result = new WordRep(2 * ( a.length() + b.length() ));
  GeneratorPtrType rp = result->first();
  cGeneratorPtrType tp = a.cLast();
  int i = a.length();
  while ( i-- ) *rp++ = -*tp--;
  tp = b.cLast();
  i = b.length();
  while ( i-- ) *rp++ = -*tp--;
  tp = a.cFirst();
  i = a.length();
  while ( i-- ) *rp++ = *tp++;
  tp = b.cFirst();
  i = b.length();
  while ( i-- ) *rp++ = *tp++;
  return result;
}

  


void WordRep::printOn(ostream& ostr) const {
  EltRep::printOn(ostr);
  // @stc note how information from each level is incrementally added
  ostr << "[";
  for (int i = 0; i < length(); i++) ostr << " " << val(i);
  ostr << " ]";
}


void WordRep::debugPrint(ostream& ostr) const {
  EltRep::debugPrint(ostr);
  ostr << "len : " << length();
  ostr << "; ";
  ostr << " [";
  for (int i = 0; i < length(); i++) ostr << " " << val(i);
  ostr << " ]" << endl;
}


//--------------------------- WordRep -------------------------------//
//------------------------ static members ---------------------------//
 
//
// static data members initialised in Word.C
//

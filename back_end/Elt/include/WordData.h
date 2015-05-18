// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of expanded format WordData class.
//
// Principal Author: Stephane Collart
//
// Status: Useable.
//
// Revision History:
//

#ifndef _WORD_DATA_H_
#define _WORD_DATA_H_

#include "global.h"
#include "Generator.h"
#include "List.h"
#include "Vector.h"


class WordData {

public:
  
  // provide type parameter access to users
  typedef int GeneratorType;
  typedef GeneratorType* GeneratorPtrType; // for pseudo-ptr would be a class
  typedef const GeneratorType* cGeneratorPtrType;
  
  // copy constructor does deep copy
  WordData( const WordData& wd ) :
  len(wd.len), wrd(new GeneratorType[wd.len]) {
	 for (int i = 0; i < len; i++) ref(i) = wd.val(i);
  }
  
  // destructor deletes data
  ~WordData( ) { delete[] wrd; }
  
  // data specific constructor(s)

  WordData( int le ) {
	 #if SAFETY > 0
	   if ( le > MAXLENGTH )
		  error("Tried to init a Word which is too long");
    #endif
	 len = le;
	 wrd = new GeneratorType[le];
  }

  WordData( const Generator& x ) : len(1), wrd(new GeneratorType[1]) {
	 #if SAFETY > 0
      if ( ord(x) == 0 ) error("Tried to init Word with Generator of ord 0");
    #endif
	 *wrd = ord(x);
  }

  WordData( const Generator& x, const Generator& y ) :
    len(2), wrd(new GeneratorType[2]) {
	 #if SAFETY > 0
      if ( ord(x) == 0 || ord(y) == 0 )
		  error("Tried to init Word with Generator of ord 0");
    #endif
	 wrd[0] = ord(x);
	 wrd[1] = ord(y);
  }

  WordData( const VectorOf<Generator>& v ) {
	 #if SAFETY > 0
	   if ( v.length() > MAXLENGTH )
		  error("Tried to init a Word which is too long");
    #endif
	 len = v.length();
	 wrd = new GeneratorType[len];
	 #if SAFETY > 0
      for (int i = 0; i < len; i++)
        if ( (wrd[i] = ord(v[i])) == 0 )
          error("Tried to init Word with Generator of ord 0");
    #else
	   for (int i = 0; i < len; i++) wrd[i] = ord(v[i]);
    #endif
  }

  WordData( const ListOf<Generator>& l ) {
	 #if SAFETY > 0
	   if ( l.length() > MAXLENGTH )
		  error("Tried to init a Word which is too long");
    #endif
	 len = l.length();
	 wrd = new GeneratorType[len];
    ListIterator< ListOf<Generator> > it(l);
	 #if SAFETY > 0
      for (int i = 0; i < len; i++) {
        if ( (wrd[i] = ord(it.value())) == 0 )
          error("Tried to init Word with Generator of ord 0");
		  it.next();
      }
    #else
	   for (int i = 0; i < len; i++) {
        wrd[i] = ord(it.value());
        it.next();
      }
    #endif
  }

  WordData( const GeneratorType *p, int le ) {
	 #if SAFETY > 0
	   if ( le > MAXLENGTH )
		  error("Tried to init a Word which is too long");
    #endif
	 len = le;
	 wrd = new GeneratorType[le];
	 #if SAFETY > 0
      for (int i = 0; i < le; i++)
        if ( (wrd[i] = *p++) == 0 )
          error("Tried to init Word with Generator of ord 0");
    #else
	   for (int i = 0; i < le; i++) wrd[i] = *p++;
    #endif
  }

  
  // handles:
  
  int length( ) const { return len; }
  
  // for reference access
  GeneratorType& ref(int i) {
    #if ( SAFETY > 0 )
	   if ( i < 0 || i >= len ) error("Word index out of bounds "
			"in GeneratorType& WordData::ref(int)");
	 #endif
	 return wrd[i];
  }
  
  // for value access
  GeneratorType val(int i) const {
    #if ( SAFETY > 0 )
	   if ( i < 0 || i >= len ) error("Word index out of bounds "
			"in GeneratorType WordData::val(int)");
	 #endif
	 return wrd[i];
  }
  
  // for pointer and pseudo pointer manipulation
  GeneratorPtrType first( ) { return wrd; }
  cGeneratorPtrType cFirst( ) const { return wrd; }
  GeneratorPtrType last( ) { return wrd + len - 1; }
  cGeneratorPtrType cLast( ) const { return wrd + len - 1; }
  // for pseudo-p would have +, -, ++ and -- defined,
  // and a dereferencer operator*( )


private:

  int len;
  static const int MAXLENGTH = 2000000000; // The biggest int which will fit in len.
  GeneratorType* wrd;


//@@db temporary declaration.

  friend class WordRep;
};

#endif

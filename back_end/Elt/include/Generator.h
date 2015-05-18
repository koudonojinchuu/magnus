// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the Generator class.
//
//	Should be considered part of the implementation of class Word
//	and similar classes. Serves as an interface class for these.
//  A Generator is something with an ordinal and an inverse, and
//  can be compared with other ordinals.
//
// Principal Author: Stephane Collart
//
// Status: Useable.
//
// Questions to consider:
//
// * should Generator have a cast operator to Elt?
//
// Revision History:
//
// * 95/07 @dp added method  int hash() const;
//
// * 01/96 Dmitry B. implemented IPC tools.
//
// * 11/96 EP added new global functions: power, abs, shortLexIndex

#ifndef _GENERATOR_H_
#define _GENERATOR_H_


class Generator {

public:
  
  // copy constructor compiler-supplied
  // destructor compiler-supplied
  // assignment operator compiler-supplied

  Generator( ) : genrep(0) { }
  // A default constructor for making, e.g., arrays of uninitialized
  // Generators. With SAFETY > 0, the Word constructor which takes arrays of
  // Generators checks for uninitialized ones.

  Generator( int i ) : genrep(i) {
	 #if SAFETY > 0
      if ( i == 0 ) error("initialization of Generator with ord 0");
 	 #endif
  }
  // @rn Is it worthwhile to refuse negative args, thus forcing the use
  // of inv?

  // Generator has no non-static members in its public interface
  // in order to have the use of a Genref wherever a Generator is expected
  // (the compiler would not understand g.member() if g were a Genref
  // and member() in Generator).
    
  inline friend int ord( Generator g ) { return g.genrep; }
  // this is the only friend; all other functions in the public interface
  // of Generator are defined in terms of ord()

  int hash() const { return genrep; }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const Generator& g )
  {
    ostr < g.genrep;
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, Generator& g )
  {
    istr > g.genrep;
    return istr;
  }


private:
  
  // data members

  int genrep;

  /*@stc
	// alternative implementation:

	class GenData { protected: int genrep; }
	class Generator : private GenData {
	// ...
	private:
		// only Word or other classes using Generator should see the cast to int
		friend class Word;
		operator int( ) const { return ord(*this); }
	}

	// this would make it possible to implicitly use a generator as an
	// int in Word. Pushing genrep up into GenData is necessary so Word
	// doesn't see it.
  */

};


// functions for manipulating Generators

// the notion of inverse:

inline Generator inv( Generator g ) { return Generator(-ord(g)); }

// comparators:

inline int operator == ( Generator g, Generator h ) { return ord(g) == ord(h); }

inline int operator != ( Generator g, Generator h ) { return ord(g) != ord(h); }

inline int operator < ( Generator g, Generator h ) { return ord(g) < ord(h); }

inline int operator <= ( Generator g, Generator h ) { return ord(g) <= ord(h); }

inline int operator > ( Generator g, Generator h ) { return ord(g) > ord(h); }

inline int operator >= ( Generator g, Generator h ) { return ord(g) >= ord(h); }

// I/O

inline ostream& operator << ( ostream& ostr, Generator g ) {
	ostr << ord(g);
	return ostr;
}

// Generator properties

inline int power(Generator g) {
	return ( ord(g) < 0 ) ? -1 : 1;
}

inline int abs(Generator g) {
	int v = ord(g);
	return ( v < 0) ? -v : v;
}
// The absolute value of generator representation

inline int shortLexIndex(Generator g) {
  int i = ord(g);
	return (i > 0 ) ? 2*(i-1) : -2*i - 1;
}
// Return the generator's index in ShortLex ordering:
//   1  -> 0
//  -1  -> 1
//   2  -> 2
//  -2  -> 3
//  ........

#endif

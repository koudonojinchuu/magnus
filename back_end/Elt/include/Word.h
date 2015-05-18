// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the Word and Genref classes.
//
// Principal Authors: Stephane Collart, Roger Needham
//
// Status: Useable.
//
// Next Implementation Steps:
//
// * Need to classify and order the methods and operators better.
//
// * Need to decide on a more coherent scheme where and how to declare
//   global functions on container classes of words. In particular,
//   functions which modify their arguments in place need a closer
//   look. (are they really nice and convenient to use? - problem
//   of unnecessarily cloning).
//
// Revision History:
//
// 4.6.94 @stc added
//   method replaceGenerators( const VectorOf<Elt>& eltvec) const
// 22.11.94 @stc added
//   int agreementLength( const Word& ) const;
//   SetOf<Word>& closeUnderInverses(SetOf<Word>& S);
//   SetOf<Word>& closeUnderCyclicPermutations(SetOf<Word>& S);
//   inline SetOf<Word>& symmetrise(SetOf<Word>& S);
// 7.12.94 @stc added bool isProperPower from the Omsk randomMSC stuff
// 12/16/94 @rn Added nextFreelyReduced, nextCyclicallyReduced.
// 12/16/94 @rn Added maximalRoot global function. It must be merged with
//              isProperPower.
//
// 12/06/95 Roger added bool allExponentSumsZero( ).
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//

#ifndef _WORD_H_
#define _WORD_H_

#include "Elt.h"
#include "WordRep.h"
#include "Generator.h"
#include "Vector.h"
#include "List.h"
#include "Chars.h"
#include "DerivedObjectOf.h"


class Genref;

class Word : public DerivedObjectOf<Elt,WordRep> {

public:  

///////////////////////////////////////////////////////////////////////
//                                                                   //
// Constructors:                                                     //
//                                                                   //
///////////////////////////////////////////////////////////////////////

  Word( ) : DerivedObjectOf<Elt,WordRep>(new WordRep(0)) { }
  // Default constructor makes empty word.

  // copy constructor supplied by compiler

  Word( const VectorOf<Generator>& v ) :
    DerivedObjectOf<Elt,WordRep>(new WordRep(v))
  { }
  // For initializing a Word from a Vector of Generators.

  Word( const ListOf<Generator>& l ) :
    DerivedObjectOf<Elt,WordRep>(new WordRep(l))
  { }
  // For initializing a Word from a List of Generators.

  Word( const Generator& g ) : DerivedObjectOf<Elt,WordRep>( new WordRep(g) ) { }
  // Cast constructor.

  Word( const Elt& e ) : DerivedObjectOf<Elt,WordRep>( e ) { }
  // Cast constructor.
  
  // destructor supplied by compiler

///////////////////////////////////////////////////////////////////////
//                                                                   //
// Type and representation stuff:                                    //
//                                                                   //
///////////////////////////////////////////////////////////////////////
 
  static Type type( ) { return WordRep::type(); }
 
  // inherited from Elt:
  // Type actualType( ) const { return look()->actualType(); }

///////////////////////////////////////////////////////////////////////
//                                                                   //
// Methods:                                                          //
//                                                                   //
///////////////////////////////////////////////////////////////////////

  int length( ) const { return look()->length(); }

  Generator operator [] ( int i ) const;
  // access to const word just gives a val
  // @stc added the const version of op[] for const words - selected
  // automatically by compiler

  Genref operator [] ( int i );
  // Subscripting for random read/write access. Zero-based indexing.
  // WordData does bounds checking. Return type is actually Genref,
  // to be used as a Generator by the user.
  // @stc removed const qualifier to remove loophole

  Bool operator < ( const Word& ) const;
  // ShortLex order defined by following order on generators:
  // compare them first by magnitude of ord, then g < g^-1.

  Word nextInShortLex(int numberOfGens) const;
  // Return the word, not freely reduced, which comes after
  // *this in the ShortLex order on numberOfGens generators.

  Word nextFreelyReduced(int numberOfGens) const;
  // Return the freely reduced word which comes after
  // *this in the ShortLex order on numberOfGens generators.
  // It is a fatal error to call this with a non-freely reduced word.

  Word nextCyclicallyReduced(int numberOfGens) const;
  // Return the cyclically reduced word which comes after
  // *this in the ShortLex order on numberOfGens generators.
  // It is a fatal error to call this with a non-freely reduced word.

  Word subword(const int i, const int j) const;
  // Return the subword [i, j).

  Word initialSegment(const int i) const {
	 return subword(0, i);
  }

  Word terminalSegment(const int i) const {
	 return subword(length() - i, length());
  }

  Word findAgreement(const Word&) const;
  // Return identical initial segment of the argument and *this.

  int agreementLength( const Word& ) const;
  // give max length of identical initial segment; more efficient
  // than actually constructing segment to determine length

  Word shortenByRelator(const Word&) const;
  // Find a subword w of *this which is an initial segment of the argument
  // of length more than half of the argument.
  // Return *this if there is no such w; otherwise the result of replacing w
  // by the inverse of the shorter half.

  int numberOfOccurrences(const Generator& g) const;
  // Return the number of g's and -g's in *this.

  int exponentSum(const Generator& g) const;
  // Return the exponent sum of g in *this.

  bool allExponentSumsZero( ) const;
  // That is, iff this word is trivial in the derived group of the
  // ambient free group.

  bool isProperPower( ) const;
  // whether *this is a proper power of a non-trivial word.
  // @stc might need one which gives you the smallest proper factor too.
  // @stc apart from the implementation which might make you blanch,
  // there is the Q whether this should reduce first or no: now it does.

  Generator maxOccurringGenerator( ) const;
  // Return the generator in *this with ord of maximal magnitude
  // in the standard order on integers. It is an error for *this to be
  // the trivial word.

  Word replaceGeneratorWithWord(const Generator&, const Word&) const;
  // Return the result of replacing each occurrence of the Generator
  // (or its inverse) with the Word (or its inverse).

  Elt replaceGenerators( const VectorOf<Elt>& eltvec ) const;
  // Return the result of simultaneously replacing generator with ord i+1
  // through eltvec[i], i = 0 .. eltvec.length(); it is an error for 
  // maxOccuringGenerator() to be greater than eltvec.length()

  Word replaceGenerators( const VectorOf<Word>& eltvec ) const;
  // Return the result of simultaneously replacing generator with ord i+1
  // through eltvec[i], i = 0 .. eltvec.length(); it is an error for 
  // maxOccuringGenerator() to be greater than eltvec.length()
  // @stc this method added as temporary palliative to the lack of
  // convertibility between various Vectors
  // @rn Changed return type to Word to work around g++ 2.6.0 bug,
  //     and replaced implementation with more somewhat efficient one.
  //     It does not do free reduction.

  Word replaceSubword(const int i, const int j, const Word& w) const;
  // Return result of replacing *this[i, j) with w.
  // w may have any length.

  Word freelyReduce( ) const;
  // Return a freely reduced copy of *this.

  Word cyclicallyReduce(void) const;
  // Return a cyclically reduced copy of *this.

  Word cyclicallyPermute(const int j) const;
  // Return result where old jth gen becomes new 0th gen.
  // Thus w.cyclicallyPermute(j), j > 0, means `left-shift' j letters.
  // Negative j's yield a `right-shift'.
  // j's of magnitude exceeding length of *this wrap around.

  Word inverse( ) const { return Word(look()->inverse()); }

  // All Word multiplication does pure concatenation without free reduction.

  Word operator * ( const Word& w ) const {
	 return ( *look() * *w.look() );
  }

  Word operator * ( const Generator& x ) {
	 return Word( look()->rightMultBy(x) );
  }

  inline friend Word operator * ( const Generator& x, const Word& w ) {
	 return Word( w.look()->leftMultBy(x) );
  }

  inline friend Word operator * ( const Generator& x, const Generator& y ) {
	 return Word( new WordRep(x, y) );
  }

  Word operator *= ( const Word& w ) {
	return *this = *this * w;
  }

  Word operator *= ( const Generator& x ) {
	return *this = *this * x;
  }

///////////////////////////////////////////////////////////////////////
//                                                                   //
// Static auxiliary functions:                                       //
//                                                                   //
///////////////////////////////////////////////////////////////////////

  static Word wordByLexRank( int numGens, int lexRank );
  // gives the lexRank-th word with numGens number of Generators in
  // the short lexicographic order

  static Word wordByLexRank( VectorOf<int> vi )
  // same for call with standardised single argument signature
	{ return wordByLexRank(vi[0],vi[1]); }

///////////////////////////////////////////////////////////////////////
//                                                                   //
// I/O:                                                              //
//                                                                   //
///////////////////////////////////////////////////////////////////////

  // Inherited from Elt:
  // void printOn(ostream&) const; // pseudo-virtual
  // void debugPrint(ostream&) const; // pseudo-virtual

    // helper classes
 
    class EmptyWord {
        Chars emptyWord;
    public:
        EmptyWord( const Chars& ew ) : emptyWord(ew) { };
        operator Chars( ) const { return emptyWord; }
    };
 
    class ProductJunctor {
        Chars junctor;
    public:
        ProductJunctor( const Chars& j ) : junctor(j) { };
        operator Chars( ) const { return junctor; }
    };


private:

  typedef WordData::GeneratorPtrType GeneratorPtrType;  // Pointer to generator
  typedef WordData::cGeneratorPtrType cGeneratorPtrType;// Same, but const
  typedef WordData::GeneratorType GeneratorType;
  // A signed integral type. 0 is not used, and the inverse of a generator g
  // is -g.

  friend class Genref;
  // so Genref can access the representation to perform assignments.

  // Some hidden constructors:

  Word( int len ) : DerivedObjectOf<Elt,WordRep>( new WordRep(len) ) { }
  // Returns an uninitialized word of specified length.

  Word( const GeneratorType* p, int len ) :
    DerivedObjectOf<Elt,WordRep>( new WordRep(p, len) )
  { }
  // For initializing a word from a raw array of GeneratorType.

  //@rn:@stc arg type ok?:
  Word( EltRep* rep ) : DerivedObjectOf<Elt,WordRep>((WordRep*)rep) { }
  // Special constructor to make an object out of a delegated method
  // which returns a representation.

};


//--------------------------- Genref ------------------------------//


class Genref {
  
public:
  
  // no default constructor because of reference members
  // destructor compiler-supplied
  
  Generator operator = ( const Generator& g ) {
	 return wref.change()->ref(index) = ord(g);
  }

  int operator == ( const Generator& g ) {
	 return wref.look()->val(index) == g;
  }
  // @stc as long as there is no global == which takes two Generator
  // arguments, this has to be explicitely provided (ARM prohibits
  // type conversion to apply method).
 
  operator Generator( ) { return wref.look()->val(index); }

  
private:

friend class Word; //@rn only op[] when possible.

  Genref( Word& w, int i ) : wref(w), index(i) { }
  // Hide this from unauthorized users.

  // data members
  
  Word& wref;
  
  int index;

  // make copy constructor inaccessible
  Genref( const Genref& g ) : wref(g.wref), index(g.index) {
	 error("called hidden Genref copy constructor");
  }
  
  // make assignment operator inaccessible
  Genref operator = ( const Genref& g ) {
	 error("called hidden Genref assignment operator");
  }
};


//----------------------------------- ------------------------------//
//------------------------------ Word ------------------------------//
//----------------------- inline functions -------------------------//


inline Generator Word::operator [] ( int i ) const { return look()->val(i); }

inline Genref Word::operator [] ( int i ) { return Genref( *this, i ); }



#include "Set.h"

 
//-------------- Word: associated global functions ----------------//

istream& operator>>( istream& istr, Word& );

// @rn 12/16/94 This is here temporarily, until we can sort things out.
int maximalRoot(const Word& w);


// syntactic operations on container classes of words:
// @stc some or all of these functions could be made into template
// functions for various container classes, if the container classes
// have sufficiently standardised handles.


SetOf<Word>& closeUnderInverses(SetOf<Word>& S);
// closes S syntactically to contain all syntactic inverses of words
// in S; returns a reference to the new S.
// Does not freely reduce.

 
SetOf<Word>& closeUnderCyclicPermutations(SetOf<Word>& S);
// closes S syntactically to contain all syntactic cyclic permutations
// of words in S; returns a reference to the new S.
// Does not freely reduce.


inline SetOf<Word>& symmetrise(SetOf<Word>& S) {

  return closeUnderCyclicPermutations(closeUnderInverses(S));
}
// closes S syntactically to contain all syntactic inverses and
// syntactic cyclic permutations of words in S; returns a reference
// to the new S.
// Does not freely reduce.


int cancellationLambda( const SetOf<Word>& ss );
// computes the smallest lambda such that no two words of ss have a
// common initial segment of length greater than or equal to
// one lamdba-th of either of their lengths.
// ss is assumed to be symmetrised.
// the function returns 0 if lambda is infinite, otherwise the value of
// lambda; as a special case, lambda == 1 means two words agree over
// at least half of one, ie. the words do not define a small
// cancellation group.
// @stc beware that if the words are compacted words, lambda can easily
// overflow; the function does not check this
// @stc need to add security checks


Trichotomy hasMetricSmallCancellation( const SetOf<Word>& S, const int lambda );
// This method takes an int argument lambda, and checks whether the given
// words satisfy the C'(1/lambda) condition.
// The words are expected to be freely reduced, and the set symmetrised.
// If any two words R1, R2 in symmetrized relators set such that
// R1 = p * r1 and R2 = p * r2 (graphically) satisfy the condition
// |p| < 1/lambda * min{|R1|, |R2|},  where |w| is the length of a word w,
// then the finitely presented group is the metric small cancellation one,
// and the answer is YES; otherwise NO.
// This function is of limited usefulness, but can refute the small
// cancellation property faster for a given lambda.
// @stc need to add security checks

#endif


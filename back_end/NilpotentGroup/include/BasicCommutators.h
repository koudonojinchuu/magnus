
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Principal Authors: Eugene Paderin, Sergei Lyutikov
//
// Description:
//
// A declaration of classes BCEntry, BasicCommutators.
//
//
// Status: Draft
//
// Revision History:
//
// * first built April 31, 1995 (says SL :)
//
// * This is a EP's remake of SL's BasicCommutators class. I had to
//   make some changes for my special needs.
//   BCEntry is taken from SL's BasicCommutators.h file.
//
// * Class BasicCommutators contains an ordered set of basic 
//   commutators (BC) ( see Khukhro? ) corresponding to nilpotent 
//   group of particular number of generators and particular nilpotency 
//   class. Each BC is stored as BCEntry, which keeps an index of left 
//   part, an index of right part of BC and a weight of BC ( see comment
//   below. ) There is a correspondence between the set of BC and the set 
//   of natural numbers (0...(#ofBC-1)) given by a member of BasicCommutators
//   BCEntry* entries.                  ( Note: less BC has less index )
//   
// 
// * March 1997: remake
//

#ifndef _BASIC_COMMUTATOR_H_
#define _BASIC_COMMUTATOR_H_

#include "Vector.h"
#include "Word.h"
#include "Generator.h"
#include "PolyWord.h"


//====================================================================
//====================== class BCEntry ===============================
//====================================================================

//@ep Borrowed from SL

class BCEntry {

  //
  // Basic commutators are defined as follows:

  //   1. All generators of the free nilpotent group G are basic commutators of
  //   weight 1. Commutators of weight 1 are lineary ordered. If g and h
  //   are Generators then the order is defined by the function ord().
  //   2. If u and v are basic commutators of weight i and j respectively
  //   then ( u, v ) is a basic commutator of weight i + j if the following
  //   two conditions are satisfied:
  //     a. u > v
  //     b. If u = (u1,u2), where u1 and u2 are basic commutators, then u2 < v.
  //   The order on basic commutators u and v is defined in the following way:
  //     1. If weight of u is less than weight of v then u < v
  //     2. If weight of u is equal to weight of v, different from 1, than
  //        u = ( u1, u2 ), v = ( v1, v2 ) and order is defined
  //        lexicographically on pairs ( u1, u2 ) and ( v1, v2 ).
  //

  //------------------------------------------------------------------
  //  Constructors
  //------------------------------------------------------------------

public:

  BCEntry( ) { }

  BCEntry(const Generator& g) : weight(1), rPart(ord(g)) { }
  // to make a basic commutator of a generator.

  BCEntry(int i) : weight(1), rPart(i) { }
  // to make a basic commutator of an integer representing a generator.

  BCEntry(int left, int right, int aWeight) :
    weight(aWeight), lPart(left), rPart(right)
  { }
  

  //------------------------------------------------------------------
  //  Operators
  //------------------------------------------------------------------

  BCEntry& operator = ( int i ) {
    weight = 1; 
    rPart = i; 
    return *this;
  }


  bool operator == ( const BCEntry& v ) const;

  bool operator != ( const BCEntry& v ) const {
    return ( !( *this == v ) );
  }

  bool operator < ( const BCEntry& v ) const;

  bool operator > ( const BCEntry& v ) const {
    return ( v < *this );
  }

  bool operator <= ( const BCEntry& v ) const {
    return ( *this == v || *this < v );
  }

  bool operator >= ( const BCEntry& v ) const {
    return ( *this == v || *this > v );
  }

  friend ostream& operator << (ostream& s, const BCEntry& );

  //------------------------------------------------------------------
  // IPC tools:                                                   
  //------------------------------------------------------------------

  friend ostream& operator < ( ostream& ostr, const BCEntry& BC )
  {
    ostr < BC.weight < BC.lPart < BC.rPart;
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, const BCEntry& bc)
  {
    BCEntry& BC = (BCEntry&)bc;
    istr > BC.weight > BC.lPart > BC.rPart;
    return istr;
  }
  
  //------------------------------------------------------------------
  //   Friends:
  //------------------------------------------------------------------

  friend class BasicCommutators;


  //------------------------------------------------------------------
  //   Data members:
  //------------------------------------------------------------------

private:

  int weight;
  int lPart, rPart;

};



//====================================================================
//================ class BasicCommutators ============================
//====================================================================


class BasicCommutators {

public:


  //------------------------------------------------------------------
  //  Constructors and initialization
  //------------------------------------------------------------------

  BasicCommutators(int numgen, int nilclass, bool initialize = true);
  // To construct the set of basic commutators which constitute a Mal'cev
  // basis for free nilpotent group of `numgen' generators and nilpotency
  // class `nilclass'.
  // Flag initialize shows whether the initialization should be
  // performed immediately or delayed

  //copy constructor, destructor and assignment operator provided by compiler


  void initialize() const;    // Logical const!
  // initializes uninitialized object


  //------------------------------------------------------------------
  //  Methods dealing with the whole set of basic commutators
  //  (accessors)
  //------------------------------------------------------------------

  int theHirschNumber() const { return theNumberOfBC; }
  // the Hirsch number (the number of basic commutators)

  int numberOfGenerators() const { return numGens; }
  // the number of generators

  int nilpotencyClass() const { return nilClass; }
  // the nilpotency class

  int numberOfWeight(int i) const { 
    return firstOfWeight.val(i+1) - firstOfWeight.val(i); 
  }
  // The number of basic commutators of given weight

  int theFirstOfWeight(int i) const { 
    return firstOfWeight.val(i);
  }
  // The number of first basic commutator of given weight
  

  bool isInitialized() const { return initialized; }
  // true iff the structure is initialized

  class NGWordForms wordForm() const;   // inline
  // produces an object of helper class


  //------------------------------------------------------------------
  //  Methods dealing with basic commutators
  //------------------------------------------------------------------

  // Here basic commutators are given by indices of corresponding
  // entries in the table


  //--------------- access to BCEntry properties ---------------------

  int weightOf(int i) const { 
    return entries.val(i).weight; 
  }

  int leftIndexOf(int i) const {
    return entries.val(i).lPart;
  }

  int rightIndexOf(int i) const {
    return entries.val(i).rPart;
  }

  virtual Chars commutatorName(int i) const {
    return (Chars)"c" + Chars(i);
  }
  // default name for basic commutator, can be overridden
    
  VectorOf<Chars> commutatorNames() const;
  // names of basic commutators

  //-------------- properties of commutator [g,h] --------------------

  bool commutatorIsBasic(int g, int h) const {
    if( g > numGens && entries.val(g).rPart > h)
      return false;
    return ( g > h );
  }
  // returns true iff the commutator [g,h] is proper basic.

  int findBC(int g, int h) const;
  // returns the index of the commutator [g,h].
  // If the commutator is not basic, generates an error.
  // If [g,h] = 1, returns 0.

  bool generatorsCommute(int g, int h) const {
    return ( entries.val(g).weight + entries.val(h).weight > nilClass );
  }
  // return true iff [g,h] = 1


  //------------- methods for collecting process ----------------------


  PolyWord commuteLetters(const Letter& left, const Letter& right) const;
  // If left.gen < right.gen, returns the word ( left + right ).
  // If left.gen == right.gen, collects them.
  // If left.gen > right.gen, commutes the letters.
  // "Commutes" means: result.firstLetter().gen < left.gen.

  PolyWord findCommutationRelation( Letter Cj, Letter Ci ) const;
  // finds a commutation relation for Ci^-1 Cj Ci
  // Note: only signs of powers are taken into account, not powers themselves

  PolyWord decomposeLetter(Letter C) const;
  // decomposes the given letter [a,b]^p into a PolyWord:
  // a^-1 b^-1 a b  a^-1 b^-1 a b  ...  a^-1 b^-1 a b (p times)


  //------------------------------------------------------------------
  //  I/O and debugging stuff
  //------------------------------------------------------------------


  friend ostream& operator < ( ostream& ostr, const BasicCommutators& BC );
  friend istream& operator > ( istream& istr, const BasicCommutators& BC );
  // IPC tools

  virtual void print( ostream& BClist, const VectorOf<Chars>& genNames ) const;
  // outputs basic commutators to a file

  //------------------------------------------------------------------
  //   Friends:
  //------------------------------------------------------------------

  friend class NGWordForms;

  //------------------------------------------------------------------
  //   Data members:
  //------------------------------------------------------------------


private:

  int nilClass;
  // the nilpotency class

  int numGens;
  // the number of generators

  int theNumberOfBC;
  // the number of basic commutators

  bool initialized;
  // true iff the commutators are built

  VectorOf<int> firstOfWeight;
  // for fast access to basic commutators of certain weight.
  // firstOfWeight[i] is the index of the first basic commutator of weight i.
  // 1-based indices ( well, we don't have any BC's of weight less than 1. )

  VectorOf<BCEntry> entries;
  // ordered array of all basic commutators.
  // 1-based indices.

};


//===========================================================================
//===================== class NGWordForms ===================================
//===========================================================================


// The class contains procedures that perform conversions of various word
// presentations: PolyWord, Word in terms of the group, or basic commutators,
// and so on.

// This is merely a procedure package, controls no data.

class NGWordForms {

public:

  NGWordForms(const BasicCommutators& bc) : BC(bc) { }
  // The only constructor (except copy constructor)


  //@ep I deliberately use no conversion operators to avoid
  //    undesirable implicit conversions


  //-----------------------------------------------------------------
  //  Conversions to the Word
  //-----------------------------------------------------------------

  // A result of the conversion is a Word in terms of the group
  // generators

  Word toWord(int commIndex) const;
  // The argument is a commutator index

  Word toWord(const PolyWord& w) const;
  // The argument is a PolyWord in terms of basic commutators

  Word toGroupWord(const Word& basicWord) const;
  // The argument is a Word given in terms of basic commutators


  //-----------------------------------------------------------------
  //   Conversions to the text string
  //-----------------------------------------------------------------


  //--- to string in terms of basic commutators (c1, c2, c3....) ----


  Chars commutatorStructure(int commIndex) const;
  // If weight(commIndex) == 1, outputs generatorName(commindex).
  // Else outputs "[cj,ci]".

  Chars asCommutatorWord(const PolyWord& w) const;
  // Outputs the given word in terms of basic commutator names:
  //           c1 c2^-3 c4^2

  Chars asCommutatorWord(Letter s) const;
  // Outputs the given letter in terms of basic commutator names:
  //            c2^-3


  //---- to string in terms of group generators ----------------------
  //  Names of the generators are provided


  Chars asBracketedWord(int commIndex, const VectorOf<Chars>& genNames,
			bool embeddingBrackets = true) const;
  // Produces a string representing the given basic commutator commIndex
  // as a word of genNames in commutator form, e.g.:
  //       [x,[x,y],z]
  // The parameter embeddingBrackets defines whether the outermost
  // brackets should present.

  Chars asBracketedWord(const PolyWord& w, const VectorOf<Chars>& genNames) const;
  // Outputs PolyWord in the bracketed form (in terms of group generators)


private:

  const BasicCommutators& BC;

};


inline NGWordForms BasicCommutators::wordForm() const {
  return NGWordForms(*this);
}
// produces an object of helper class


#endif











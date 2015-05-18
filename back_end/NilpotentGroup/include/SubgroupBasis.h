// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of SubgroupBasis class.
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Description:
//
// Special Notes:
//
//
// Revision History:
//

#ifndef _SUBGROUP_BASIS_H_
#define _SUBGROUP_BASIS_H_

#include "NilpotentGroup.h"
#include "QuickAssociations.h"
#include "AbelianGroup.h"

//================================================================
//------------------- SubgroupBasis ------------------------------
//================================================================

// The class is based on QuickAssociations. The key is theWord.leader()

class SubgroupBasis {

public:
  
  //-----------------------------------------------------
  //   Constructors / initializers
  //-----------------------------------------------------

  SubgroupBasis(const VectorOf<Word>& v, const NilpotentGroup& ng);

  // copy constructor and destructor provided by compiler

  void initialize() const;     //Logical const!
  //Initializes theSet and makes it to be full
  // Slow, performs full initialization of the parent group

  //-----------------------------------------------------
  //   Accessors 
  //-----------------------------------------------------

  int cardinality() const { return theSet.cardinality(); }
  // returns number of elements currently in the basis

  const NilpotentGroup& parentGroup() const {
    return theParent;
  }
  //returns reference to the parent group

  const VectorOf<Word>& generators() const {
    return theGenerators;
  }
  //returns the generators

  bool isFull() const { return basisIsFull; }
  // returns true if the set is full

  bool isNormalClosure() const;
  // returns true if the basis is normal closure
  // Slow for the first run

  int theHirschNumber() const;
  // returns the Hirsch number of the basis
  // Fast, the set must be full

  //-----------------------------------------------------
  //   Methods to form the set
  //-----------------------------------------------------

  // assignment operator provided by compiler

  SubgroupBasis normalClosure() const;
  // makes the normal closure of the set
  // Slow


  //-----------------------------------------------------
  //   Word containment problem
  //-----------------------------------------------------


  bool contains(const Word& w) const;
  // returns true iff the word is a member of the set
  // Slow

  bool decomposeWord(const PolyWord& w, PolyWord& decomp) const;
  // if w is an element of the subgroup, computes its decomposition;
  // if not, decomp is empty word
  // returns true iff the word can be decomposed
  // Slow

  //-----------------------------------------------------
  //   Conversions to other presentations
  //-----------------------------------------------------


  VectorOf<Word> asWords() const;
  // returns the basis as a vector of words in terms of
  // the group generators
  
  VectorOf<PolyWord> asGroupWords() const;
  // returns the basis as a vector of PolyWords
  // in terms of the group basis


  //-----------------------------------------------------
  //   IPC I/O
  //-----------------------------------------------------


  friend ostream& operator < (ostream& s, const SubgroupBasis& b);
  // IPC output

  friend istream& operator > (istream& s, const SubgroupBasis& b);
  // IPC intput


  //-----------------------------------------------------
  //   Helper methods
  //-----------------------------------------------------

private:

  //---------- operations in parent group ---------------

  //Everywhere it is assumed that arguments and return value
  //are expressed in terms of the parent basis

  PolyWord parentMultiply(const PolyWord& pw1, const PolyWord& pw2) const;
  // multiplies two words

  PolyWord parentInvert(const PolyWord& pw) const;
  // inverts the word

  PolyWord parentRaiseToPower(const PolyWord& pw, int power) const;
  // computes pw^power

  PolyWord parentCommute(const PolyWord& pw1, const PolyWord& pw2) const;
  // commutes two words

  int leaderOrder(const PolyWord& pw) const;
  // returns the order of the leading letter, 0 means infinity


  PolyWord toParentBasis(const PolyWord& pw) const;
  //The argument is expressed in terms of basic commutators.
  //Result is in terms of parent basis.


  PolyWord toBasicCommutators(const PolyWord& pw) const;
  //The argument is expressed in terms of parent basis.
  //Result is in terms of basic commutators.


  //---------- properties of the word -------------------


  static int power(const PolyWord& pw) { 
    if( pw.isEmpty() ) return 0;
    return pw.firstLetter().power;
  }
  // power

  static int absPower(const PolyWord& pw) { 
    if( pw.isEmpty() ) return 0;
    return abs( pw.firstLetter().power );
  }
  // absolute power :)

  static Generator leader(const PolyWord& pw) {
    if( pw.isEmpty() ) return 1;
    return pw.firstLetter().gen;
  }
  //the leading letter of the collected form

  static int sign(const PolyWord& pw) {
    if( pw.isEmpty() ) return 0;
    return pw.firstLetter().power > 0 ? 1 : 0;
  }
  // The sign of the leader power


  //----------- methods to deal with the set -----------------

  bool reduceWords(PolyWord& mw1, PolyWord& mw2) const;
  // Reduces two words
  // After reduction the leading term of mw1 is in minimal power,
  // the leading term of mw2 is greater.
  // Returns true iff mw1 was changed
  // Slow

  void checkMembership(PolyWord& w) const;
  // checks whether w can be presented as an admissible set of exponents.
  // returns the "remainder"
  // The argument must be collected
  // Slow

  bool addWord(const Word& w);
  // adds w to the set
  // returns true iff the set was changed
  // Slow

  bool addWord(const PolyWord& w);
  // returns true iff the set was changed
  // Slow

  void makeFull();
  //forms the basis
  // Slow

  void makeNormalClosure();
  // makes the set to be normally closed
  // Slow

  //-----------------------------------------------------
  //   Data
  //-----------------------------------------------------

private:

  NilpotentGroup theParent;
  VectorOf<Word> theGenerators;

  QuickAssociationsOf<Generator, PolyWord> theSet;
  bool basisIsFull;
  Trichotomy isNormal;

  int hirschNumber;

};

#endif

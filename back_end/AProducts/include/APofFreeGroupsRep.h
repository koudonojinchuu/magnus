// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the AmalgProductOfFreeGroupsRep class.
//           The name is abbreviated to fit in a library.
//
// Principal Authors: Eugene Paderin, Dmitry Pechkin
//
// Status: draft
//
// Revision History:
//
// Discussion:
//
// Bugs:
//
//
// Special Notes:
//
//

#ifndef _AMPRODUCT_FREE_GROUPS_REP_H_
#define _AMPRODUCT_FREE_GROUPS_REP_H_

#include "FPGroupRep.h"
#include "FreeGroup.h"
#include "VectorPtr.h"
#include "SGofFreeGroup.h"
#include "AP-fixups.h"
#include "Automorphism.h"

enum NumberOfFactor { LEFT_FACTOR = 0, RIGHT_FACTOR = 1, PRODUCT, INTERSECTION };
// This value indicates word membership. LEFT_FACTOR or RIGHT_FACTOR
// mean that the word consists entirely of generators of one factor,
// INTERSECTION means that the word belongs to the amalgamated subgroup
// and thus can be written in generators of any factor, and PRODUCT means
// the other cases.

//-------------------- LocalWord -------------------------------------

// A helper class: the word written in generators of one factor,
// together with the factor number.
//
// To understand the concept of local and global words, see comments in
// AmalgProductOfFreeGroups.h header.

struct LocalWord
{
  // methods

  LocalWord() : theWord(), theFactor(INTERSECTION) {}

  LocalWord(const Word& w, const NumberOfFactor& fac) : theWord(w),
  theFactor(fac) {}

  // copy constructor, operator= and destructor supplied by compiler

  operator Word() const { return theWord; }

  friend LocalWord operator * (const LocalWord& w1, const LocalWord& w2);

  LocalWord& operator *= (const LocalWord& w) {
    return *this = *this * w;
  }

  LocalWord inverse() const {
    return LocalWord(theWord.inverse(), theFactor);
  }

  LocalWord freelyReduce() const {
    return LocalWord(theWord.freelyReduce(), theFactor);
  }

  // data members

  Word theWord;
  NumberOfFactor theFactor;
};


//---------------- Class AmalgProductOfFreeGroupsRep ------------------

struct AmalgProductOfFreeGroupsRep : FPGroupRep {

// Constructors:

  // Copy constructor and operator= provided by compiler (do deep copy).

  AmalgProductOfFreeGroupsRep(const FreeGroup& g1, const FreeGroup& g2,
			      const VectorOf<Word>& gen1, const VectorOf<Word>& gen2 );

  AmalgProductOfFreeGroupsRep(const SGofFreeGroup& sg1,
			      const SGofFreeGroup& sg2);

  // Destructor provided by compiler

// Accessors / Manipulators:

  // Inherited:
  // virtual SetOf<Word>& setRelators( const SetOf<Word>& r );
  // virtual SetOf<Word>& addRelators( const SetOf<Word>& r );
  // virtual SetOf<Word>& removeRelators( const SetOf<Word>& r );

//
// Representation methods:
//

  PureRep* clone( ) const { return new AmalgProductOfFreeGroupsRep(*this); }
  // overrides FGGroupRep::clone()

  static const Type theAmalgProductOfFreeGroupsType;

  static Type type( ) { return theAmalgProductOfFreeGroupsType; }
  // dominates FPGroupRep::type()

  Type actualType( ) const { return type(); }
  // overrides FPGroupRep::actualType();


//
// Methods dealing with the properties of the group:
//

  int order( ) const;
  // Overrides FPGroupRep::order().

  Trichotomy isTrivial( ) const;
  // Overrides FPGroupRep::isTrivial().

  Trichotomy isFinite( ) const;
  // Overrides FPGroupRep::isFinite().

  Trichotomy isInfinite( ) const;
  // Overrides FPGroupRep::isInfinite().

  Trichotomy isAbelian( ) const;
  // Overrides FPGroupRep::isAbelian().

  Trichotomy isFree( ) const;
  // Overrides FPGroupRep::isFree().

  Trichotomy isHyperbolic() const;
  // Determines whether given group is hyperbolic.

//
// I/O:
//

  void printOn(ostream&) const;
  // Overrides FPGroupRep::printOn().

  GroupRep* readFrom(istream&, Chars&) const;
  // Overrides FPGroupRep::readFrom().

  void printRelators(ostream&) const;
  // Overrides FPGroupRep::printRelators().

  void printDecomposition(ostream& ostr, const VectorOf<Word>& deco) const;
  // Prints given vector of words in follow format: words are separated
  // by dot.
  

//
// Methods dealing with group elements:
//

  Elt eval( const Word& w ) const {
#if SAFETY > 0
    if ( ord(w.maxOccurringGenerator()) > theNumberOfGenerators )
      error("tried to evaluate Word with no interpretation in FreeGroup");
#endif
    return reducedFormOf(w);
  }
  // Overrides FGGroupRep::eval().


  Trichotomy wordProblem( const Word& w ) const {
    return ( reducedFormOf(w).length() == 0 ? yes : no );
  }
  // Overrides FGGroupRep::wordProblem().

  NumberOfFactor factorOfFormalWord(const Word& w) const;
  // Determines the group the given formal word belongs to.

  NumberOfFactor factorOfElement(const Word& w) const;
  // Same as above, but also checks whether the given element of
  // the product belongs to the amalgamated subgroup.

  VectorOf<Word> decompose(const Word& w) const;
  // Decomposes the given word to the product of words d_1 d_2 ....
  // such that every d_i belongs to one of the factors and any
  // two successive words belong to distinct factors.

  VectorOf<Word> reducedDecomposition(const Word& w) const;
  // Find the minimal (in the sense of number of components) decomposition
  // of the given word.

  Word reducedFormOf(const Word& w) const {
    return compose(reducedDecomposition(w));
  }
  // As above, but result is presented as a single word.

  VectorOf<Word> normalDecomposition(const Word& w) const;
  // Finds the normal decomposition of the given word: this is
  // the reduced decomposition where all components except the
  // first one are some right Schreier representatives.
  //

  Word normalFormOf(const Word& w) const {
    return compose(normalDecomposition(w));
  }
  // As above, but result is presented as a single word.

  int lengthOf(const Word& w) const { return decompose(w).length(); }
  // Compute the length of word decomposition.

  int numberOfSubstitutions( const Word& w ) const;
  // If the given word represents 1 in the group
  // the function computes the number of uses of a relation
  // a = b to deduce that w = 1, i.e. in re-expressing w as 
  // a product of conjugates of a * b^-1, computes the number
  // of these conjugates.

  //
  // Local coding word <--> global coding word conversions.
  //

  Word localToGlobal(const LocalWord& w) const {
    return localToGlobal(w.theWord, w.theFactor);
  }
  // Convert local coding word into global coding one.

  Word localToGlobal(const Word& theWord, NumberOfFactor theFactor) const;
  // Convert local word presented by word and factor into the global 
  // coding one.

  LocalWord globalToLocal(const Word& w) const;
  // Convert global coding word into local coding one.


  NumberOfFactor factorOfGenerator(const Generator& gen) const {
    if( abs(ord(gen)) <= numerationShift )
      return LEFT_FACTOR;
    else
      return RIGHT_FACTOR;
  }
  // Determine whether gen is a generator of first or second factor.

  LocalWord mapFromSubgroup(const LocalWord& w) const;
  // w is an element of associated subgroup (written in generators
  // of one factor). The function rewrites it in generators of another
  // factor.

  Word mapFromSubgroup(const Word& w) const {
    LocalWord lw = globalToLocal(w);
    return localToGlobal( mapFromSubgroup(lw) );
  }
  // The same as above. There is warning: no checking is done for the word `w'
  // is actually belongs to the associated subgroup or just one of factors.

  bool isElementOfSubgroup(const LocalWord& w) const;
  // Determines whether the given word is an element of associated subgroup.

  LocalWord rightSchreierRepresentativeOf(const LocalWord& w) const;
  // Finds right Schreier Representative of given word.

  void makeSubgroupMapping(const VectorOf<Word>& gen1,
			   const VectorOf<Word>& gen2);
  // Constructs mapping between associated subgroups.
  // Used *only* by constructors.

  void fixGeneratorsNames();
  // Used *only* by constructors.

  virtual void maximalRoot(const Word& w, Word& root, int& power) const;
  // Returns maximal root of given word w and maximal power.
  //@dp this method is not implemented yet.

  bool isProperPower(const Word& w) const;
  // Determines whether w is a proper power.

  bool isProperPowerOfSecond(const Word& u, const Word& w, int& power) const;
  // Determines whether u is a proper power of w.

  bool commute(const Word& u, const Word& w) const;
  // Determines whether [u,w] = 1.

  bool isSubgroupTrivial(const VectorOf<Word>& subgrp) const;
  // Determines whether subgroup generated by vec is trivial.

  bool isSubgroupCyclic(const VectorOf<Word>& subgrp) const;
  // Determines whether subgroup generated by vec is cyclic.

  bool isSubgroupAbelian(const VectorOf<Word>& subgrp) const;
  // Determines whether subgroup generated by vec is abelian.

  void cyclicReduction(const Word& w, Word& result, Word& conjugator) const;
  // Finds cyclic reduction of w such that w = result^conjugator.

  void cyclicDecomposition(const Word& w, VectorOf<Word>& result, 
			   Word& conjugator) const;
  // The same as above, but result is represented as vector of components.

  // Data members:

  // These four vectors consist of two elements concerning two factors
  // of the product.
  VectorPtrOf<FreeGroup> factor;            // [2]
  VectorPtrOf<SGofFreeGroup> assocSubgroup; // [2]
  VectorPtrOf<Map> subgroupMapping;         // [2]
  VectorPtrOf<Automorphism> nielsenBasisToGensOfSubgroup; // [2]
  int rankOfSubgroups;

  int numerationShift; // shift in global numeration of the second factor
  // subgroupMapping[i] maps i-th factor subgroup into another subgroup
  // the word being mapped is written in Nielsen basis of the subgroup,
  // *not* in generators of the group. The result is written in
  // generators of (the opposite) group.

};

#endif




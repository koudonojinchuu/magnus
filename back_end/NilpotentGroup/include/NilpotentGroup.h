// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the NilpotentGroup class.
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Revision History:
//
// Special Notes:
//
// * There are numerous ways to represent the group element:
//   1. As Word in terms of group generators.
//   2. As PolyWord in terms of basic commutators (not collected).
//   3. As PolyWord in terms of basic commutators (collected).
//   4. As PolyWord in terms of the group basis (collected).
//   (For free nilpotent groups, cases 3 and 4 are identical.)
//
//   We always explicitly specify the form we mean.
//
// * Also bear in mind that:
//
//   1. You can transform Word to PolyWord and vice versa:
//        Word w;
//        PolyWord pw = w;
//        w = pw;
//
//   2. You can evaluate PolyWord expressed in terms of basis A
//      if you know how basic words of A are expressed in terms of 
//      basis B. The result is a PolyWord in terms of B:
//        PolyWord pwInA;
//        VectorOf<PolyWord> AinB;
//        PolyWord pwInB = pwInA.evaluate(AinB);
//
//   3. You can write the given PolyWord in readable form using toChars:
//        PolyWord pw;
//        VectorOf<Chars> basisNames;
//        Chars output = pw.toChars(basisNames);
//
//   4. You can collect the PolyWord given in terms of basic commutators:
//        NilpotentGroup ng;
//        PolyWord nonCollectedPW;
//        PolyWord collectedPW = ng.collector().collect(nonCollectedPW);
//


#ifndef _NILPOTENT_GROUP_H_
#define _NILPOTENT_GROUP_H_

#include "NilpotentGroupRep.h"
#include "FGGroup.h"
#include "LCSQuotients.h" 

//=====================================================================
//================== class NilpotentGroup =============================
//=====================================================================


class NilpotentGroup : public DerivedObjectOf<FGGroup, NilpotentGroupRep> {

public:

  //-------------------------------------------------------------------
  // 	Constructors / initializers
  //-------------------------------------------------------------------

  // To construct a group with named generators, given nilpotency class

  //FPNilpotentGroup
  NilpotentGroup(const VectorOf<Chars>& gennames, 
		 int nilclass,
		 const VectorOf<Word>& rels,
		 enum NilpotentCollector::Type colType 
		                           = NilpotentCollector::STANDARD
		 );


  //FreeNilpotentGroup
  NilpotentGroup(const VectorOf<Chars>& gennames, 
		 int nilclass,
		 enum NilpotentCollector::Type collectorType 
		                           = NilpotentCollector::STANDARD
		 );

  // To construct a group with unnamed generators, given nilpotency class

  //FPNilpotentGroup
  NilpotentGroup(int numOfGen,
		 int nilclass,
		 const VectorOf<Word>& rels,
		 enum NilpotentCollector::Type colType 
		                          = NilpotentCollector::STANDARD
		 );

  //FreeNilpotentGroup
  NilpotentGroup(int numOfGen,
		 int nilclass,
		 enum NilpotentCollector::Type collectorType 
		                          = NilpotentCollector::STANDARD
		 );


  void initialize() const {
    look()->initialize();
  }
  // Initializes the group.
  // FP initialization means computing of LCS factors.
  // Free NG is always initialized; nevertheless, you should always invoke
  // initialize(), for the group definition may be changed in the future.
  // pseudo-virtual

  //-------------------------------------------------------------------
  // 	Accessors
  //-------------------------------------------------------------------

  // Inherited from Group:
  static Type type( ) { return NilpotentGroupRep::type(); }
  // dominates FGGroup::type();

  // Inherited from FGGroup:
  //  Type actualType() const; // pseudo-virtual
  //  int numberOfGenerators( ) const;
  //  Chars nameOfGenerator(int i) const;
  //  VectorOf<Chars> namesOfGenerators( ) const;

  bool isInitialized() const {
    return look()->isInitialized();
  }
  // True iff the group is initialized
  // pseudo-virtual

  VectorOf<Word> relators() const {
    return look()->relators();
  }
  // Returns the group relators (empty vector if the group is free nilpotent)
  // pseudo-virtual

  VectorOf<Word> relationSubgroupGenerators() const {
    return look()->relationSubgroupGenerators();
  }
  // Returns the generators of the relation subgroup
  // (empty vector if the group is free nilpotent)
  // pseudo-virtual

  VectorOf<PolyWord> basis() const {
    return look()->basis();
  }
  // The basis of the group, in terms of basic commutators (collected).
  // pseudo-virtual

  VectorOf<Chars> basisNames() const {
    return look()->basisNames();
  }
  // The names of the basis words
  // pseudo-virtual

  const BasicCommutators& commutators() const {
    return look()->commutators();
  }
  //Returns the basic commutators of the group's free preimage

  int nilpotencyClass() const {
    return look()->nilpotencyClass();
  }
  // Returns the nilpotency class

  const NGCollector collector() const {
    return look()->collector();
  }
  // Returns the current collector

  
  //-------------------------------------------------------------------
  // 	Methods dealing with the properties of the group:
  //-------------------------------------------------------------------

  // Inherited from FGGroup, redefined within Rep:
  //  int order( ) const;
  // Trichotomy isTrivial( ) const; // pseudo-virtual
  // Trichotomy isFinite( ) const; // pseudo-virtual
  // Trichotomy isInfinite( ) const; // pseudo-virtual
  // Trichotomy isAbelian( ) const; // pseudo-virtual

  virtual int theHirschNumber() const {
    return look()->theHirschNumber();
  }
  // returns the Hirsch number (the number of basic words
  // of infinite order)
  // pseudo-virtual
  
  
  // The following methods are meaningful only for FP nilpotent groups


  bool isFreeNilpotent( int* freeRank, int* freeClass ) const {
    return look()->isFreeNilpotent(freeRank,freeClass );
  }
  // returns true iff the group is free nilpotent or has free
  // nilpotent presentation.
  // in this case also returns its rank and nilpotency class.
  // pseudo-virtual

  int minimalNilpotencyClass( ) const {
    return look()->minimalNilpotencyClass( );
  }
  // returns the minimal nilpotency class of the group
  // pseudo-virtual

  bool isMalcevBasis() const {
    return look()->isMalcevBasis();
  }
  // returns truey iff the group basis is the Malcev basis
  // (has no words of finite order)
  // pseudo-virtual

  Integer orderOfTheTorsionSubgroup( ) const {
    return look()-> orderOfTheTorsionSubgroup( );
  }
  // returns order of the torsion subgroup
  // pseudo-virtual

  PresentationForNG makePresentation() const {
    return look()->makePresentation();
  }
  // produces FP presentation of the group

  VectorOf<Word> getLCSterm(int i) const {
     return look()->getLCSterm(i);
  }
  // returns the generators of --th term of lower central series

  LCSQuotient getLCSQuotient( int N ) const {
    return look()->getLCSQuotient(N);
  }
  // returns the N_th LCSQuotient 
  // can be used only after calling initialize()


  //-------------------------------------------------------------------
  //  Methods dealing with group elements (inherited):
  //-------------------------------------------------------------------

  // Inherited from FGGroup:
  // Elt makeIdentity( ) const;
  // Bool isSyntacticIdentity(const Elt& e) const;
  // Elt firstElt( ) const;
  // Elt nextElt(const Elt& e) const;
  // Elt multiply(const Elt& e1, const Elt& e2) const;
  // Elt inverseOf(const Elt& e) const;
  // Elt raiseToPower(const Elt& e, int n) const;
  // Elt conjugateBy(const Elt& e1, const Elt& e2) const;
  // Elt commutator(const Elt& e1, const Elt& e2) const;

  //@ep an Elt of this group is a Word, so I left these methods unchanged.
  // Most of them demand more careful design of the whole hierarchy.
  

  // The following methods are inherited and redefined within Rep

  // Elt eval( const Word& w ) const;
  // Trichotomy wordProblem( const Word& w ) const;
  // Trichotomy conjugacyProblem( const Word& u, const Word& v ) const;
  // Trichotomy isTrivialElt( const Elt& e ) const; // pseudo-virtual
  // Trichotomy areEqual(const Elt& e1, const Elt& e2) const;


  // Below are methods specific for this class

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods which deal with morpisms                 //
  //                                                   //
  ///////////////////////////////////////////////////////

  VectorOf<Word> inverseAuto(const VectorOf<Word>& V) const{
    return look()->inverseAuto(V);
  }
  // *** Time consuming algorithm ! ***
  // Returns the inverse  of automorphism, defined by V. 
  // Does not make check is it automorphism or not!!!

  //------------------------------------------------------------
  //  Conversions from one basis to another:
  //------------------------------------------------------------

  PolyWord decompose(const Word& w) const {
    return look()->decompose(w);
  }
  // decomposes the given word in terms of the group basis
  // input: word in terms of group generators
  // output: word in terms of group basis.
  // pseudo-virtual

  PolyWord decompose(const PolyWord& w) const {
    return look()->decompose(w);
  }
  // decomposes the given word in terms of the group basis
  // input: word in terms of basic commutators (not collected)
  // output: word in terms of group basis.

  Word toWord(const PolyWord& pw) const {
    return look()->toWord(pw);
  }
  //input: word in terms of the group basis
  //output: word in terms of the group generators
 

  PolyWord toCommutatorWord(const PolyWord& pw) const {
    return look()->toCommutatorWord(pw);
  }
  //input: word in terms of the group basis
  //output: word in terms of the basic commutators (not collected)

  Chars asDecomposition(const PolyWord& decomposition) const {
    return look()->asDecomposition(decomposition);
  }
  // Input: word in terms of group basis.
  // Output: text in terms of group basis.
  // pseudo-virtual


  //-----------------------------------------------------------
  //  Properties of the group elements:
  //-----------------------------------------------------------

  int weightOf(const Word& w) const {
    return look()->weightOf(w);
  }
  // determines in which term of the lower central series w lies
  // nilClass + 1 if the word is trivial
  
  int weightOf(const PolyWord& w) const {
    return look()->weightOf(w);
  }
  // determines in which term of the lower central series w lies
  // nilClass + 1 if the word is trivial
  //w is given in terms of the group basis

  int orderOfBasic(Generator g) const {
    return look()->orderOfBasic(g);
  }
  //Returns order of the basic word corresponding to the generator g.
  //0 means infinity.

  int orderOf(const Word& w) const {
    return look()->orderOf(w);
  }
  //Returns the order of the word given in terms of group generators

  int orderOf(const PolyWord& w) const {
    return look()->orderOf(w);
  }
  //Returns the order of the word given in terms of group basis
 
  VectorOf<Word> centralizer( const Word& w) const {
    return look()->centralizer(w);
  }

  bool isInCommutatorSubgroup(const Word& w) const {
    return look()->isInCommutatorSubgroup(w);
  }
  // returns true iff w is in the commutator subgroup, i.e. can be
  // represented as a product of commutators

  bool isCentral(const Word& w) const {
    return look()->isCentral(w);
  }
  // returns true iff w lies in the group center
  // Slow, requires initialization

  void maximalRoot(const Word& w, PolyWord& root, int& power ) const {
    look()->maximalRoot(w, root, power);
  }
  // finds maximal root of pw.
  // w is given in terms of the group generators.
  // root is in terms of the group basis.

  void maximalRoot(const PolyWord& pw, PolyWord& root, int& power ) const {
    look()->maximalRoot(pw, root, power);
  }
  // finds maximal root of pw.
  // pw is in terms of the group basis.
  // root is in terms of the group basis.
  // pseudo-virtual

  bool isProperPower(const Word& w) const {
    return look()->isProperPower(w);
  }
  // returns true iff the word is proper power
  // w is given in terms of the group generators.

  bool isProperPower(const PolyWord& pw) const {
    return look()->isProperPower(pw);
  }
  // returns true iff the word is proper power
  // pw is in terms of the group basis.

  
  //-------------------------------------------------------------------
  // I/O:
  //-------------------------------------------------------------------

  // Inherited from FGGroup:

  //  Word readWord(istream& istr, Chars& errMesg) const
  //  void printWord( ostream& ostr, const Word& w ) const
  //@ep temporarily disabled

  // Native:

  void printBasis( ostream& BClist ) const {
    look()->printBasis(BClist);
  }
  // outputs the basis to a file
  // pseudo-virtual


  //-------------------------------------------------------------------
  // Special methods
  //-------------------------------------------------------------------

protected:

  // Special wrapping constructor to wrap new representations (returned
  // by eg. delegated methods) and for base initialisation by derived
  // classes:
 
  NilpotentGroup( NilpotentGroupRep* newrep ) 
    : DerivedObjectOf<FGGroup,NilpotentGroupRep>(newrep) { }
 
  static VectorOf<Chars> defaultNames(int num);
  // Produces vector of words "x1", "x2", ..... 
  // These are default names for group generators.


};


#endif


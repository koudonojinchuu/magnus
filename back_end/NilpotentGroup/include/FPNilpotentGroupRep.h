
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the FPNilpotentGroupRep class.
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Revision History:
//
// Special Notes:
//
//

#ifndef _FP_NILPOTENT_GROUP_REP_H_
#define _FP_NILPOTENT_GROUP_REP_H_

#include "Chars.h"
#include "NilpotentGroupRep.h"
#include "BasicCommutators.h"
#include "LCSQuotients.h"

//=====================================================================
//================== struct FPNilpotentGroupRep =======================
//=====================================================================

struct FPNilpotentGroupRep : public NilpotentGroupRep {

  //-------------------------------------------------------------------
  // 	Constructors / initializers
  //-------------------------------------------------------------------


  // To construct a group with named generators, given nilpotency class
  FPNilpotentGroupRep(const VectorOf<Chars>& gennames, 
		      int nilclass,
		      const VectorOf<Word>& rels,
		      enum NilpotentCollector::Type collectorType 
		      );


  virtual void initialize() const;
  // initializes the group (computes LCS)
  // Slow

  //-------------------------------------------------------------------
  // 	Accessors
  //-------------------------------------------------------------------

  const BasicCommutators& commutators() const {
    return theCollector.commutators();
  }
  //Returns the basic commutators of the group's free preimage

  int nilpotencyClass() const {
    return nilClass;
  }
  // Returns the nilpotency class

  const NGCollector& collector() const {
    return theCollector;
  }

  bool isInitialized() const {
    return LCScomputed;
  }
  // True iff the group is initialized

  VectorOf<Word> relators() const { 
    return theRelators; 
  }

  VectorOf<Word> relationSubgroupGenerators() const;
  // Returns the generators of the relation subgroup
  //Slow, does not require initialization

  VectorOf<PolyWord> basis() const;
  // The basis of the group
  //Fast, but requires initialization

  VectorOf<Chars> basisNames() const;
  // The basis of the group
  //Fast, but requires initialization

  static Type type( );
  // dominates FGGroupRep::type()


  //-------------------------------------------------------------------
  // 	Methods dealing with the properties of the group:
  //-------------------------------------------------------------------

  // All methods within this section are fast but require initialization

  //  Redefine inherited from GroupRep:
  int order( ) const;
  Trichotomy isTrivial( ) const;
  Trichotomy isFinite( ) const;
  Trichotomy isInfinite( ) const;
  Trichotomy isAbelian( ) const;
  Trichotomy isFree( ) const;

  int theHirschNumber() const;
  // returns the Hirsch number (the number of basic commutators of infinite order)
  
  // Native methods

  bool isFreeNilpotent( int* freeRank, int* freeClass ) const;
  // returns true iff the group is free nilpotent.
  // in this case also returns its rank and nilpotency class.

  int minimalNilpotencyClass( ) const;
  // returns the minimal nilpotency class of the group

  bool isMalcevBasis() const;
  // returns true iff the basis found in computeLCSQuotients() 
  // is the Malcev basis

  Integer orderOfTheTorsionSubgroup( ) const;
  // Returns order of the torsion subgroup.

  //class PresentationForNG makePresentation() const;
  // produces FP presentation of the group
  // Inherited from NilpotentGroupRep

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods which deal with morpisms                 //
  //                                                   //
  ///////////////////////////////////////////////////////

  VectorOf<Word> inverseAuto(const VectorOf<Word>& V) const{
      error("Inverse automorphism for FP nilpotent group not emplemented yet");
  }
 
  //-------------------------------------------------------------------
  //  Methods dealing with group elements
  //-------------------------------------------------------------------

  // Inherited from FGGroupRep:

  // virtual Elt makeIdentity( ) const;
  // returns empty Word

  // virtual Bool isSyntacticIdentity( const Elt& e) const;

  // virtual Elt firstElt( ) const;
  // virtual Elt nextElt( const Elt& e ) const;
  // virtual Elt multiply( const Elt& e1, const Elt& e2 ) const;
  // virtual Elt inverseOf( const Elt& e ) const;
  // virtual Elt raiseToPower(const Elt&, int) const;
  // virtual Elt conjugateBy(const Elt&, const Elt&) const;
  // virtual Elt commutator(const Elt&, const Elt&) const;

  //@ep an Elt of this group is a Word, so I left these methods unchanged.
  // Most of them demand more careful design of the whole hierarchy.
  

  // Inherited from NilpotentGroupRep:

  
  Trichotomy conjugacyProblem( const Word& u, const Word& v ) const;
  //@ep to do

  //virtual Trichotomy isTrivialElt( const Elt& e ) const;

  //virtual bool isProperPower(const PolyWord& pw) const;
  // returns true iff the word is proper power


  // The following methods override those from FGGroupRep
  
  Trichotomy wordProblem( const Word& w ) const;

  Trichotomy areEqual(const Elt& e1, const Elt& e2) const;


  //------------------------------------------------------------
  //  Conversions from one basis to another:
  //------------------------------------------------------------

  //virtual PolyWord decompose(const Word& w) const;
  // decomposes the given word in terms of the group basis
  // input: word in terms of group generators
  // output: word in terms of group basis.
  // Inherited

  virtual PolyWord decompose(const PolyWord& w) const;
  // decomposes the given word in terms of the group basis
  // input: word in terms of basic commutators (not collected)
  // output: word in terms of group basis.


  //Word toWord(const PolyWord& w) const;
  // Transforms the given PolyWord to a common Word in terms of
  // the group
  //Inherited

  virtual PolyWord toCommutatorWord(const PolyWord& pw) const;
  //input: word in terms of the group basis
  //output: word in terms of the basic commutators (not collected)

  //Chars asDecomposition(const PolyWord& decomposition) const;
  // Input: word in terms of group basis.
  // Output: text in terms of group basis.
  //Inherited


  //-----------------------------------------------------------
  //  Properties of the group elements:
  //-----------------------------------------------------------

  virtual int weightOf(const Word& w) const;
  // determines in which term of the lower central series w lies
  // nilclass + 1 if the word is trivial

  virtual int weightOf(const PolyWord& pw) const {
    return weightOfDecomposed(pw);
  }
  // determines in which term of the lower central series w lies
  // nilClass + 1 if the word is trivial
  //w is given in terms of the group basis


  virtual int orderOfBasic(Generator g) const;
  //Returns order of the basic word corresponding to the generator g.
  //0 means infinity.

  virtual int orderOf(const Word& w) const;
  //Returns the order of the word given in terms of group generators

  virtual int orderOf(const PolyWord& w) const;
  //Returns the order of the word given in terms of group basis

   VectorOf<Word> centralizer( const Word& w) const;

  // Inherited from NilpotentGroupRep:

  //virtual bool isInCommutatorSubgroup(const Word& w) const;

  //virtual bool isCentral(const Word& w) const;
  // returns true iff w lies in the group center
  // time consuming

  //void maximalRoot(const Word& w, PolyWord& root, int& power ) const;

  virtual void maximalRoot(const PolyWord& pw, PolyWord& root, int& power ) const;
  // finds maximal root of pw.
  // Time consuming.
  // pw is in terms of the group basis.
  // root is in terms of the group basis.
  //@ep NOT IMPLEMENTED

  // bool isProperPower(const PolyWord& pw) const;
  // returns true iff the word is proper power
  // Inherited from NilpotentGroupRep

  virtual LCSQuotient getLCSQuotient( int N ) const; 
  // returns the N_th LCSQuotient 
  // can be used only after calling initialize()

  
  //-------------------------------------------------------------------
  // I/O:
  //-------------------------------------------------------------------

  virtual void printBasis( ostream& BClist ) const;
  // outputs the basis to a file

  virtual void write( ostream& ostr ) const;
  virtual void read( istream& istr );
  //IPC tools

  //-------------------------------------------------------------------
  // Internal methods
  //-------------------------------------------------------------------


  void computeLCSQuotients() const;    // Logical const!

  void computeQuotient( int theWeight );

  PolyWord decomposeInQuotient(PolyWord& rest, int k ) const;

  Word decomposeInQuotientPresentation(const Word& w, int k ) const;

  PolyWord decomposeFromQuotientPresentation(const PolyWord& w, int k ) const;

 const AbelianGroup& abelianization() const;
  //Returns LCS[1]

  virtual int weightOfDecomposed( const PolyWord& pw) const;
  // Returns the weight of the word decomposed in terms of group basis.

  //-------------------------------------------------------------------
  // 	Representation methods
  //-------------------------------------------------------------------
	
  PureRep* clone( ) const { return new FPNilpotentGroupRep(*this); }
  // overrides NilpotentGroupRep::clone()

  static const Type theFPNilpotentGroupType;


  Type actualType( ) const { return type(); }
  // overrides FGGroupRep::actualType();
  
  //-------------------------------------------------------------------
  // Data members:
  //-------------------------------------------------------------------

  // Inherited:
  // int theOrder;
  // int theNumberOfGenerators;
  // VectorOf<Chars> theNamesOfGenerators;

  int nilClass;  
  NGCollector theCollector;

  VectorOf<Word> theRelators;

  VectorOf<LCSQuotient> LCS;
  bool LCScomputed;
  int hirschNumber;

  VectorOf<PolyWord> theBasis;
  VectorOf<Chars> theBasisNames;
  VectorOf<int> theBasisOrders;
  
};

#endif


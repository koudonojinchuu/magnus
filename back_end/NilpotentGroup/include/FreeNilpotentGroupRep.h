
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the FreeNilpotentGroupRep class.
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

#ifndef _FREE_NILPOTENT_GROUP_REP_H_
#define _FREE_NILPOTENT_GROUP_REP_H_

#include "NilpotentGroupRep.h"
#include "Chars.h"
#include "LCSQuotients.h"

//=====================================================================
//=================== class FreeNilpotentGroupRep =====================
//=====================================================================

class FreeNilpotentGroupRep : public NilpotentGroupRep {

  //-------------------------------------------------------------------
  // 	Constructors / initializers
  //-------------------------------------------------------------------

public:

  // To construct a group with named generators, given nilpotency class
  FreeNilpotentGroupRep(const VectorOf<Chars>& gennames, 
		    int nilclass,
		    enum NilpotentCollector::Type collectorType 
		    )
    : NilpotentGroupRep(gennames), nilClass(nilclass),
      theCollector(theNumberOfGenerators, nilclass, collectorType)
  { }


  virtual void initialize() const { }
  //The group is already initialized
    

  //-------------------------------------------------------------------
  // 	Accessors / modifiers
  //-------------------------------------------------------------------

  //All are inherited from NilpotentGroupRep

  virtual bool isInitialized() const { return true; }
  // True iff the group is initialized

  const BasicCommutators& commutators() const {
    return theCollector.commutators();
  }

  int nilpotencyClass() const {
    return nilClass;
  }

  const NGCollector& collector() const {
    return theCollector;
  }

  virtual VectorOf<Word> relators() const {
    return VectorOf<Word>() ;
  }
  // Returns the group relators (empty vector here)

  virtual VectorOf<Word> relationSubgroupGenerators() const {
    return VectorOf<Word>() ;
  }
  // Returns the generators of the relation subgroup
  // (empty vector here )

  virtual VectorOf<PolyWord> basis() const;
  // The basis of the group
  // The basis is c1, c2, c3 ... It is formed every time the method
  // is called, so do not abuse it.
  // The matter of fact is that this method is not very useful here.

  virtual VectorOf<Chars> basisNames() const;
  // The names of basic commutators

  static Type type( ) { return theFreeNilpotentGroupType; }
  // dominates FGGroupRep::type()


  //-------------------------------------------------------------------
  // 	Methods dealing with the properties of the group:
  //-------------------------------------------------------------------

  //  Inherited from GroupRep:

  int order( ) const;
  Trichotomy isTrivial( ) const;
  Trichotomy isFinite( ) const;
  Trichotomy isInfinite( ) const;
  Trichotomy isAbelian( ) const;
  Trichotomy isFree( ) const;

  int theHirschNumber() const { 
    return commutators().theHirschNumber(); 
  }
  // returns the Hirsch number (the number of basic commutators)
  
  // The following methods are meaningless here

  bool isFreeNilpotent( int* freeRank, int* freeClass ) const {
    *freeRank = theNumberOfGenerators;
    *freeClass = nilClass;
    return true;
  }
  // returns true because the group is free nilpotent

  int minimalNilpotencyClass( ) const {
    return nilClass;
  }
  // returns the minimal nilpotency class of the group

  bool isMalcevBasis() const {
    return true;
  }
  // returns true iff the group basis is the Malcev basis

  Integer orderOfTheTorsionSubgroup( ) const {
    return 1;
  }
  // returns order of the torsion subgroup

  //PresentationForNG makePresentation() const;
  // produces FP presentation of the group

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods which deal with morpisms                 //
  //                                                   //
  ///////////////////////////////////////////////////////

  VectorOf<Word> inverseAuto(const VectorOf<Word>& V) const;

  // *** Time consuming algorithm ! ***
  // Returns the inverse  of automorphism, defined by V. 
  // Does not make check is it automorphism or not!!!

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

  //Trichotomy conjugacyProblem( const Word& u, const Word& v ) const;
  //@ep to do

  //virtual Trichotomy isTrivialElt( const Elt& e ) const;

  //virtual bool isProperPower(const PolyWord& pw) const;
  // returns true iff the word is proper power
  //Elt eval( const Word& w ) const;


  // The following methods override those from FGGroupRep
  
  Trichotomy wordProblem( const Word& w ) const;
  
  Trichotomy areEqual(const Elt& e1, const Elt& e2) const;

  VectorOf<Word> centralizer( const Word& w) const;

  //------------------------------------------------------------
  //  Conversions from one basis to another:
  //------------------------------------------------------------

  virtual PolyWord decompose(const PolyWord& w) const {
    return theCollector.collect(w);
  }
  // decomposes the given word in terms of the group basis

  //Word toWord(const PolyWord& w) const;
  // Transforms the given PolyWord to a common Word in terms of
  // the group
  // Inherited

  virtual PolyWord toCommutatorWord(const PolyWord& pw) const {
    return pw;
  }
  //input: word in terms of the group basis
  //output: word in terms of the basic commutators (not collected)
  //For free nilpotent groups, these representations are identical

  //Chars asDecomposition(const PolyWord& decomposition) const;
  // Input: word in terms of group basis.
  // Output: text in terms of group basis.
  //Inherited

  //-----------------------------------------------------------
  //  Properties of the group elements:
  //-----------------------------------------------------------

  virtual int weightOf(const Word& w) const {
    return theCollector.weightOf(w);
  }
  // determines in which term of the lower central series w lies
  // nilclass + 1 if the word is trivial

  virtual int weightOf(const PolyWord& w) const {
    return theCollector.weightOf(w);
  }
  // determines in which term of the lower central series w lies
  // nilclass + 1 if the word is trivial


  virtual int orderOfBasic(Generator g) const {
    return 0;
  }
  //Returns order of the basic word corresponding to the generator g.
  //0 means infinity.

  virtual int orderOf(const Word& w) const {
    return 0;
  }    
  //Returns the order of the word given in terms of group generators

  virtual int orderOf(const PolyWord& w) const {
    return 0;
  }
  //Returns the order of the word given in terms of group basis

  //bool isInCommutatorSubgroup(const Word& w) const;
  // returns true iff w is in the commutator subgroup, i.e. can be
  // represented as a product of commutators
  //Inherited

  //bool isCentral(const Word& w) const;
  // returns true iff w lies in the group center
  // Slow, requires initialization
  //Inherited

  virtual void maximalRoot(const PolyWord& pw, PolyWord& root, int& power ) const;
  // finds maximal root of pw.
  // Time consuming.

  //bool isProperPower(const Word& w) const;
  // returns true iff the word is proper power
  // w is given in terms of the group generators.
  //Inherited

  //bool isProperPower(const PolyWord& pw) const;
  // returns true iff the word is proper power
  // pw is in terms of the group basis.
  //Inherited

  virtual LCSQuotient getLCSQuotient( int N ) const {
    return LCSQuotient();
  }
  // returns the N_th LCSQuotient 
  // can be used only after calling initialize()


  //-------------------------------------------------------------------
  // I/O:
  //-------------------------------------------------------------------

  virtual void printBasis( ostream& BClist ) const {
    commutators().print(BClist, theNamesOfGenerators);
  }
  // outputs the basis to a file

  virtual void write( ostream& ostr ) const;
  virtual void read( istream& istr );  //Logical const!
  //IPC tools


  //-------------------------------------------------------------------
  // Internal methods
  //-------------------------------------------------------------------

protected:

  static void findAbelianRoot(const PolyWord& pw, PolyWord& root,
			      int& power );
  // finds the root of the given PolyWord assuming that all
  // the word generators commute and the word is collected
  
  static int GCDofExponents(const PolyWord& pw);
  // finds GCD of generators exponents
  // the word assumed to be collected

  int weightOfDecomposed( const PolyWord& pw) const;
  // Returns the weight of the word decomposed in terms of group basis.


  //-------------------------------------------------------------------
  // 	Representation methods
  //-------------------------------------------------------------------
	
  PureRep* clone( ) const { return new FreeNilpotentGroupRep(*this); }
  // overrides NilpotentGroupRep::clone()

  static const Type theFreeNilpotentGroupType;

  Type actualType( ) const { return type(); }
  // overrides FGGroupRep::actualType();
  
  //-------------------------------------------------------------------
  // Data members:
  //-------------------------------------------------------------------

private:

  // Inherited:
  // int theOrder;
  // int theNumberOfGenerators;
  // VectorOf<Chars> theNamesOfGenerators;

  int nilClass;  
  NGCollector theCollector;

};

#endif


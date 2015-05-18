
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the NilpotentGroupRep class.
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

#ifndef _NILPOTENT_GROUP_REP_H_
#define _NILPOTENT_GROUP_REP_H_

#include "FGGroupRep.h"
#include "NilpotentCollector.h"
#include "Presentation.h"
#include "LCSQuotients.h"

//=====================================================================
//================== struct NilpotentGroupRep =========================
//=====================================================================


struct NilpotentGroupRep : public FGGroupRep {

  //-------------------------------------------------------------------
  // 	Constructors / initializers
  //-------------------------------------------------------------------


  NilpotentGroupRep(const VectorOf<Chars>& gennames )
  : FGGroupRep(gennames)
  {  }
  //The only explicit constructor to construct the base class

  // copy constructor and destructor provided by compiler

  virtual void initialize() const = 0;
  // initializes the group

  //-------------------------------------------------------------------
  // 	Accessors
  //-------------------------------------------------------------------

  virtual const BasicCommutators& commutators() const = 0;
  //Returns the basic commutators of the group's free preimage

  virtual int nilpotencyClass() const = 0;
  // Returns the nilpotency class

  virtual const NGCollector& collector() const = 0;

  virtual bool isInitialized() const = 0;
  // True iff the group is initialized

  virtual VectorOf<Word> relators() const = 0;
  // Returns the group relators (empty vector if the group is free nilpotent)

  virtual VectorOf<Word> relationSubgroupGenerators() const = 0;
  // Returns the generators of the relation subgroup
  // (empty vector if the group is free nilpotent)

  virtual VectorOf<PolyWord> basis() const = 0;
  // The basis of the group

  virtual VectorOf<Chars> basisNames() const = 0;
  // The names of the basis words

  //-------------------------------------------------------------------
  // 	Methods dealing with the properties of the group:
  //-------------------------------------------------------------------

  //  Inherited from GroupRep:
  //virtual int order( )             const = 0;
  //virtual Trichotomy isTrivial( )  const = 0;
  //virtual Trichotomy isFinite( )   const = 0;
  //virtual Trichotomy isInfinite( ) const = 0;
  //virtual Trichotomy isAbelian( )  const = 0;
  //virtual Trichotomy isFree( )     const = 0;

  virtual int theHirschNumber() const = 0;
  // returns the Hirsch number (the number of basic words
  // of infinite order)
  

  // The following methods are meaningful only for FP nilpotent groups

  virtual bool isFreeNilpotent( int* freeRank, int* freeClass ) const = 0;
  // returns true iff the group is free nilpotent or has free
  // nilpotent presentation.
  // in this case also returns its rank and nilpotency class.

  virtual int minimalNilpotencyClass( ) const = 0;
  // returns the minimal nilpotency class of the group

  virtual bool isMalcevBasis() const = 0;
  // returns true iff the group basis is the Malcev basis

  virtual Integer orderOfTheTorsionSubgroup( ) const = 0;
  // returns order of the torsion subgroup

  class PresentationForNG makePresentation() const;
  // produces FP presentation of the group

  VectorOf<Word> getLCSterm(int i) const;
  // returns the generators of --th term of lower central series

  virtual LCSQuotient getLCSQuotient( int N ) const = 0; 
  // returns the N_th LCSQuotient 
  // can be used only after calling initialize()

  
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


  Elt eval( const Word& w ) const { return w; }

  //@ep an Elt of this group is a Word, so I left these methods unchanged.
  // Most of them demand more careful design of the whole hierarchy.
  

  // The following methods override those from FGGroupRep
  
  //virtual Trichotomy areEqual(const Elt& e1, const Elt& e2) const = 0;
  //overriden in derived classes

  //virtual Trichotomy wordProblem( const Word& w ) const = 0;
  
  Trichotomy conjugacyProblem( const Word& u, const Word& v ) const {
    return DONTKNOW;
  }
  //@ep to do

  virtual Trichotomy isTrivialElt( const Elt& e ) const {
    return wordProblem( (Word&)e );
  }

  
  // Methods specific for this class

  //------------------------------------------------------------
  //  Conversions from one basis to another:
  //------------------------------------------------------------

  virtual PolyWord decompose(const Word& w) const {
    return decompose( PolyWord(w) );
  }
  // decomposes the given word in terms of the group basis
  // input: word in terms of group generators
  // output: word in terms of group basis.

  virtual PolyWord decompose(const PolyWord& w) const = 0;
  // decomposes the given word in terms of the group basis
  // input: word in terms of basic commutators (not collected)
  // output: word in terms of group basis.

  Word toWord(const PolyWord& w) const {
    return commutators().wordForm().toWord( toCommutatorWord(w) );
  }
  // Transforms the given PolyWord to a common Word in terms of
  // the group

  virtual PolyWord toCommutatorWord(const PolyWord& pw) const = 0;
  //input: word in terms of the group basis
  //output: word in terms of the basic commutators

  Chars asDecomposition(const PolyWord& decomposition) const {
    return decomposition.toChars( basisNames() );
  }
  // Input: word in terms of group basis.
  // Output: text in terms of group basis.

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods which deal with morpisms                 //
  //                                                   //
  ///////////////////////////////////////////////////////

  virtual VectorOf<Word> inverseAuto(const VectorOf<Word>& V) const = 0;
  // Returns the inverse  of automorphism, defined by V. 

  //-----------------------------------------------------------
  //  Properties of the group elements:
  //-----------------------------------------------------------

  virtual int weightOf(const Word& w) const = 0;
  // determines in which term of the lower central series w lies
  // nilClass + 1 if the word is trivial

  virtual int weightOf(const PolyWord& w) const = 0;
  // determines in which term of the lower central series w lies
  // nilClass + 1 if the word is trivial
  //w is given in terms of the group basis (collected)

  virtual int orderOfBasic(Generator g) const = 0;
  //Returns order of the basic word corresponding to the generator g.
  //0 means infinity.

  virtual int orderOf(const Word& w) const = 0;
  //Returns the order of the word given in terms of group generators

  virtual int orderOf(const PolyWord& w) const = 0;
  //Returns the order of the word given in terms of group basis

  virtual  VectorOf<Word> centralizer( const Word& w) const =0;

  bool isInCommutatorSubgroup(const Word& w) const {
    return weightOf(w) != 1;
  }
  // returns true iff w is in the commutator subgroup, i.e. can be
  // represented as a product of commutators

  bool isCentral(const Word& w) const;
  // returns true iff w lies in the group center
  // Slow, requires initialization

  void maximalRoot(const Word& w, PolyWord& root, int& power ) const {
    maximalRoot( decompose(w), root, power);
  }
  // finds maximal root of pw.
  // w is given in terms of the group generators.
  // root is in terms of the group basis.

  virtual void maximalRoot(const PolyWord& pw, PolyWord& root, int& power ) const = 0;
  // finds maximal root of pw.
  // pw is in terms of the group basis.
  // root is in terms of the group basis.

  bool isProperPower(const Word& w) const {
    return isProperPower( decompose(w) );
  }
  // returns true iff the word is proper power
  // w is given in terms of the group generators.

  bool isProperPower(const PolyWord& pw) const;
  // returns true iff the word is proper power
  // pw is in terms of the group basis.

  
  //-------------------------------------------------------------------
  // I/O:
  //-------------------------------------------------------------------

  // Inherited from GroupRep:

  virtual GroupRep* readFrom(istream&, Chars&) const { return 0; }
  //@ep temporarily disabled

  virtual void printOn(ostream&) const {}
  //@ep temporarily disabled


  // Native:

  virtual void printBasis( ostream& BClist ) const = 0;
  // outputs the basis to a file
  

  //-------------------------------------------------------------------
  // IPC tools:                                                          
  //-------------------------------------------------------------------
    
  virtual void write( ostream& ostr ) const;

  virtual void read( istream& istr );  //Logical const!


  //-------------------------------------------------------------------
  // 	Representation methods
  //-------------------------------------------------------------------
	
  //PureRep* clone( ) const;
  // No need for clone in derived abstract representation

  static const Type theNilpotentGroupType;

  static Type type( ) { return theNilpotentGroupType; }
  // dominates FGGroupRep::type()

  Type actualType( ) const { return type(); }
  // overrides FGGroupRep::actualType();


  //-------------------------------------------------------------------
  // Internal methods                                                         
  //-------------------------------------------------------------------

protected:

  void mapToClass( PolyWord& pw, int theClass ) const;
  // remove all basic generators of weight more than k
  // pw is given in terms of the basic commutators


  //-------------------------------------------------------------------
  // Data members:
  //-------------------------------------------------------------------

  // Inherited:
  // int theOrder;
  // int theNumberOfGenerators;
  // VectorOf<Chars> theNamesOfGenerators;

  //no native data
};

#endif




// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of SGOfNilpotentGroupRep class
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Description:
//
// Special Notes:
//
// Revision History:
//
 
#ifndef _SG_OF_NILPOTENT_GROUP_REP_H_
#define _SG_OF_NILPOTENT_GROUP_REP_H_

#include "Presentation.h"

//=======================================================================
//================ class SGOfNilpotentGroupRep ==========================
//=======================================================================


class SGOfNilpotentGroupRep : public GenericRep {

public:

  //---------------------------------------------------------------------
  //    Initializers:
  //---------------------------------------------------------------------


  //no constructors for pure abstract class

  virtual void initPreimage( ) const = 0;    // logical const !
  // the initialization of thePreimageBasis

  virtual void initParent( ) const = 0;    // logical const !
  // The initialization of theParentGroup.
  
  virtual void initBasis( ) const = 0;    // logical const !
  // Finding the basis of the subgroup.
  // Performs initPreimage and initParent before initialization
  // After this, the subgroup is completely initialized
  

  //---------------------------------------------------------------------
  //  Accessors/modifiers:
  //---------------------------------------------------------------------


  virtual const class NilpotentGroup& parentGroup() const = 0;
  // returns the parent group


  virtual const class VectorOf<Word>& generators() const = 0;
  // returns the generators


  virtual const class MalcevSet& preimageBasis() const = 0;
  // returns the basis of subgroup's full preimage

  virtual bool preimageIsInitialized( ) const = 0;
  // returns true iff the preimage is initialized

  virtual bool parentIsInitialized( ) const = 0;
  // returns true iff the parent group is initialized

  virtual bool basisIsInitialized( ) const = 0;
  // returns true iff the subgroup basis is computed

  virtual VectorOf<PolyWord> basis() const = 0;
  // returns the basis of the subgroup in terms of the group basis 
  // Fast, requires the basis

  virtual VectorOf<Chars> basisNames() const = 0;
  // returns the names of basis words
  // Fast, requires the basis

  //---------------------------------------------------------------------
  // Methods dealing with subgroup properties:
  //---------------------------------------------------------------------

  virtual int theHirschNumber() const = 0;
  // returns the Hirsch number of the subgroup
  
  int index() const;
  // Returns the index of this subgroup in its parent group
  // 0 if infinite
  // Slow, requires the preimage to be initialized

  //bool isAFreeFactor() const;
  // Returns true iff this subgroup is a free factor of the parent group
  // not implemented

  bool isTrivial() const;
  // Returns true iff this subgroup is trivial
  // fast, but requires theParentGroup to be initialized

  bool isCentral() const;
  // Returns true iff this subgroup is central
  // Slow, requires theParentGroup to be initialized

  
  bool isNormal() const;
  // Returns true iff this subgroup is normal
  // Slow, requires thePreimage to be initialized

  bool isAbelian() const;
  // Returns true iff this subgroup is abelian
  // Slow, requires theParentGroup to be initialized

  int subgroupClass() const;
  // returns the class of subgroup
  // Slow, requires initialization of the parent

  virtual int order() const = 0;
  // returns the order of subgroup (0 means infinite).
  // Fast, requires initialization of the parent group and the basis

  //---------------------------------------------------------------------
  //  Methods producing related structures
  //---------------------------------------------------------------------

  //virtual SGOfNilpotentGroup centraliser() const = 0;
  // returns the centralizer of the subgroup
  

  //virtual SGOfNilpotentGroup normaliser() const = 0;
  // returns the normalizer of the subgroup

  //virtual SGOfNilpotentGroup isolator() const = 0;
  // returns the isolator of the subgroup

  virtual VectorOf<Word> normalClosureBasis() const = 0;
  // returns a basis of a normal closure of the subgroup

  VectorOf<Word> normalClosureGens() const;
  // returns the generators of normal closure of the subgroup

  PresentationForSNG makePresentation() const;
  // produces FP presentation of the subgroup
  // requires the preimage basis to be built


  //---------------------------------------------------------------------
  //  Methods dealing with other subgroups
  //---------------------------------------------------------------------

  VectorOf<Word> join(const class SGOfNilpotentGroup& SGR) const;
  // Returns the join of this subgroup and the argument.
  // no intialization is necessary

  VectorOf<Word> join(const VectorOf<Word>& V) const;
  // Returns the join of this subgroup and the argument.
  // no intialization is necessary
  // words are in terms of the group generators

  //virtual SGOfNilpotentGroup intersection(SGOfNilpotentGroup& SGR) const = 0;
  // Returns the intersection of this subgroup and the argument.

  bool contains(const VectorOf<Word>& V) const;
  // Returns true iff this subgroup contains the subgroup generated by `V'.
  // Slow, requires theFullPreimage initialization

  //virtual bool conjugateTo(const SetOf<Word>& S) const = 0;
  // Returns true iff this subgroup and the argument are conjugate.

  //virtual bool conjugateTo(const SGOfNilpotentGroup& S) const = 0;
  // Returns true iff this subgroup and the argument are conjugate.


  //---------------------------------------------------------------------
  // Methods dealing with group elements:
  //---------------------------------------------------------------------

  bool contains(const Word& w) const;
  // Returns true iff this subgroup contains `w'.
  // Slow, requires theFullPreimage initialization
  // w is in terms of the group generators

  virtual bool decompose(const PolyWord& w, PolyWord& decomp) const = 0;
  // Finds decomposition of the subgroup element w
  // returns true iff w is an element of the subgroup
  // Input: w, in terms of the group basis
  // Output: decomp, in terms of the subgroup basis

  //virtual bool conjugateInSubgroup(const Word& w, Word& conjugator) const = 0;
  // Returns true iff some conjugate of `w' is in the subgroup.
  // If true, `conjugator' is set to the conjugator.

  //virtual int powerInSubgroup( const Word& w ) const = 0;
  // returns `the minimal power' or 0 if there are no powers of the
  // element w in H.


  //---------------------------------------------------------------------
  // I/O:
  //---------------------------------------------------------------------
 
  virtual void printBasis(ostream&) const = 0;

  virtual ostream& writeIPC(ostream& s) const;

  virtual istream& readIPC(istream& s) const;  // Logical const!

  virtual Chars asDecomposition( const PolyWord& p ) const = 0;
  // Produces string presentation of decomposed subgroup element


  //-------------------------------------------------------------------
  // 	Representation methods
  //-------------------------------------------------------------------
	

  static const Type theSGOfNilpotentGroupType;

  static Type type( ) { return theSGOfNilpotentGroupType; }

  virtual Type actualType( ) const { return type(); }

  virtual GenericRep* clone( ) const = 0;

};


#endif


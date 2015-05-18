
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of SGOfFPNilpotentGroupRep class
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
 
#ifndef _SG_OF_FPNG_REP_H_
#define _SG_OF_FPNG_REP_H_

#include "SGOfNilpotentGroupRep.h"
#include "NilpotentGroup.h"
#include "SubgroupBasis.h"

//=======================================================================
//=============== class SGOfFPNilpotentGroupRep =======================
//=======================================================================


class SGOfFPNilpotentGroupRep : public SGOfNilpotentGroupRep {

public:

  //---------------------------------------------------------------------
  //    Constructors / Initializers:
  //---------------------------------------------------------------------

  SGOfFPNilpotentGroupRep(const NilpotentGroup& ng, 
			    const VectorOf<Word>& gens);
  //Constructs a representation for subgroup of free nilpotent group

  virtual void initPreimage( ) const;    // logical const !
  // the initialization of thePreimageBasis
  // Slow

  virtual void initParent( ) const;   // logical const !
  // The initialization of theParentGroup.
  // Slow

  virtual void initBasis( ) const;     // logical const !
  // Finding the basis of the subgroup.
  // Slow


  //---------------------------------------------------------------------
  //  Accessors:
  //---------------------------------------------------------------------


  virtual const class NilpotentGroup& parentGroup() const {
    return theBasisSet.parentGroup();
  }
  // returns the parent group

  virtual const class VectorOf<Word>& generators() const {
    return theBasisSet.generators();
  }
  // returns the generators


  virtual const class MalcevSet& preimageBasis() const;
  // returns the basis of subgroup preimage

  bool preimageIsInitialized( ) const {
    return thePreimage.isMalcevBasis();
  }
  // returns true iff the preimage is initialized

  bool parentIsInitialized( ) const {
    return parentGroup().isInitialized();
  }
  
  bool basisIsInitialized( ) const {
    return theBasisSet.isFull();
  }
  // returns true iff the subgroup basis is computed
    
  VectorOf<PolyWord> basis() const;
  // returns the basis of the subgroup in terms of the group basis
  // Fast, requires the basis

  VectorOf<Chars> basisNames() const;
  // returns the names of basis words
  // Fast, requires the basis


  //---------------------------------------------------------------------
  // Methods dealing with subgroup properties:
  //---------------------------------------------------------------------

  virtual int theHirschNumber() const {
    return theBasisSet.theHirschNumber();
  }
  // returns the Hirsch number of the subgroup
  // fast, the basis must be initialized

  //int index() const;
  // Returns the index of this subgroup in its parent group
  // 0 if infinite
  // Inherited

  //bool isAFreeFactor() const;
  // Returns true iff this subgroup is a free factor of the parent group

  //bool isTrivial() const;
  // Returns true iff this subgroup is trivial
  //Inherited

  //bool isCentral() const;
  // Returns true iff this subgroup is central
  //Inherited

  //bool isNormal() const {
  //  return theBasisSet.isNormalClosure();
  //}
  // Returns true iff this subgroup is normal
  // Slow, the basis must be initialized

  //bool isAbelian() const;
  // Returns true iff this subgroup is abelian
  // Slow, does not require initialization
  //Inherited

  //int subgroupClass() const;
  // returns the class of subgroup
  //Inherited

  virtual int order() const;
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

  virtual VectorOf<Word> normalClosureBasis() const;
  // returns the normal closure of the subgroup
  // Slow, does not require initialization

  //virtual PresentationForSNG makePresentation() const;
  // produces FP presentation of the subgroup
  // Slow, requires the preimage basis to be built
  // Inherited

  //---------------------------------------------------------------------
  //  Methods dealing with other subgroups
  //---------------------------------------------------------------------

  //VectorOf<Word> join(const class SGOfNilpotentGroup& SGR) const;
  // Returns the join of this subgroup and the argument.
  // no intialization is necessary
  // Inherited

  //VectorOf<Word> join(const VectorOf<Word>& V) const;
  // Returns the join of this subgroup and the argument.
  // no intialization is necessary
  // Inherited

  //virtual SGOfNilpotentGroup intersection(SGOfNilpotentGroup& SGR) const = 0;
  // Returns the intersection of this subgroup and the argument.

  //bool contains(const VectorOf<Word>& V) const;
  // Returns true iff this subgroup contains the subgroup generated by `V'.
  // Slow, requires initialization
  // Inherited

  //virtual bool conjugateTo(const SetOf<Word>& S) const = 0;
  // Returns true iff this subgroup and the argument are conjugate.

  //virtual bool conjugateTo(const SGOfNilpotentGroup& S) const = 0;
  // Returns true iff this subgroup and the argument are conjugate.


  //---------------------------------------------------------------------
  // Methods dealing with group elements:
  //---------------------------------------------------------------------

  //bool contains(const Word& w) const;
  // Returns true iff this subgroup contains `w'.
  // Slow, requires initialization
  // Inherited

  bool decompose(const PolyWord& w, PolyWord& decomp) const;
  // Finds decomposition of the subgroup element w
  // returns true iff w is an element of the subgroup
  // Slow, requires initialization
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
 
  virtual void printBasis(ostream&) const;

  virtual ostream& writeIPC(ostream& s) const;

  virtual istream& readIPC(istream& s) const;  // Logical const!

  virtual Chars asDecomposition( const PolyWord& p ) const;
  // Produces string presentation of decomposed subgroup element


  //-------------------------------------------------------------------
  // 	Representation methods
  //-------------------------------------------------------------------
	

  static const Type theSGOfFPNilpotentGroupType;

  static Type type( ) { return theSGOfFPNilpotentGroupType; }

  virtual Type actualType( ) const { return type(); }

protected:

  virtual GenericRep* clone( ) const;

  //-------------------------------------------------------------------
  // 	Data
  //-------------------------------------------------------------------


private:

  MalcevSet thePreimage;
  bool preimageInitialized;

  SubgroupBasis theBasisSet;
  VectorOf<PolyWord> theBasis;
  VectorOf<Chars> theBasisNames;

};


#endif


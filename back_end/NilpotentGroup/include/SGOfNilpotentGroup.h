
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of SGOfNilpotentGroup class
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Description:
//
// Special Notes:
//
//  * There are various ways to represent an element of the subgroup.
//    See comments to the file "NilpotentGroup.h".
//
//    Another form of presentation is added: PolyWord in terms of the
//    subgroup basis.
//
// Revision History:
//
 
#ifndef _SG_OF_NILPOTENT_GROUP_H_
#define _SG_OF_NILPOTENT_GROUP_H_

#include "SGOfNilpotentGroupRep.h"


//=======================================================================
//=================== class SGOfNilpotentGroup ==========================
//=======================================================================


class SGOfNilpotentGroup : public ObjectOf<SGOfNilpotentGroupRep> {

public:

  //---------------------------------------------------------------------
  //   Constructors / Initializers:
  //---------------------------------------------------------------------

  SGOfNilpotentGroup(const NilpotentGroup& ng, const VectorOf<Word>& gens);
  // constructs the subgroup of nilpotent group ng


  void initPreimage( ) const {    // logical const !
    look()->initPreimage( );
  }
  // the initialization of the preimage

  void initParent( ) const {    // logical const !
    look()->initParent( );
  }
  // The initialization of the parent group
  
  void initBasis( ) const {    // logical const !
    look()->initBasis( );
  }
  // Finding the basis of the subgroup.
  // Performs initPreimage and initParent before initialization
  // After this, the subgroup is completely initialized
  

  //---------------------------------------------------------------------
  //  Accessors/modifiers:
  //---------------------------------------------------------------------


  const class NilpotentGroup& parentGroup() const {
    return look()->parentGroup();
  }
  // returns the parent group


  const class VectorOf<Word>& generators() const {
    return look()->generators();
  }
  // returns the generators


  const class MalcevSet& preimageBasis() const {
    return look()->preimageBasis();
  }
  // returns the basis of subgroup's full preimage

  bool preimageIsInitialized( ) const {
    return look()->preimageIsInitialized( );
  }
  // returns true iff the preimage is initialized

  bool parentIsInitialized( ) const {
    return look()->parentIsInitialized( );
  }
  // returns true iff the parent group is initialized

  bool basisIsInitialized( ) const {
    return look()->basisIsInitialized( );
  }
  // returns true iff the subgroup basis is computed

  static Type type( ) { return SGOfNilpotentGroupRep::type(); }

  Type actualType( ) const { return look()->actualType(); }

  VectorOf<PolyWord> basis() const {
    return look()->basis();
  }
  // returns the basis of the subgroup in terms of the group basis
  // Fast, requires the basis

  VectorOf<Chars> basisNames() const {
    return look()->basisNames();
  }
  // returns the names of basis words
  // Fast, requires the basis

  //---------------------------------------------------------------------
  // Methods dealing with subgroup properties:
  //---------------------------------------------------------------------

  int theHirschNumber() const {
    return look()->theHirschNumber();
  }
  // returns the Hirsch number of the subgroup
  // not implemented

  int index() const {
    return look()->index();
  }
  // Returns the index of this subgroup in its parent group
  // 0 if infinite
  // Slow, requires theParentGroup to be initialized

  //bool isAFreeFactor() const;
  // Returns true iff this subgroup is a free factor of the parent group
  // not implemented

  bool isTrivial() const {
    return look()->isTrivial();
  }
  // Returns true iff this subgroup is trivial
  // fast, but requires theParentGroup to be initialized

  bool isCentral() const {
    return look()->isCentral();
  }
  // Returns true iff this subgroup is central
  // **time consuming, requires theParentGroup to be initialized

  bool isNormal() const {
    return look()->isNormal();
  }
  // Returns true iff this subgroup is normal
  // **time consuming, requires thePreimage to be initialized

  bool isAbelian() const {
    return look()->isAbelian();
  }
  // Returns true iff this subgroup is abelian
  // **time consuming, requires theParentGroup to be initialized

  int subgroupClass() const {
    return look()->subgroupClass();
  }
  // returns the class of subgroup
  // **time consuming, requires theParentGroup to be initialized

  int order() const {
    return look()->order();
  }
  // returns the order of subgroup (0 means infinite).
  // Fast, requires initialization of the parent group and the basis

  //---------------------------------------------------------------------
  //  Methods producing related structures
  //---------------------------------------------------------------------

  //SGOfNilpotentGroup centraliser() const {
  // returns the centralizer of the subgroup
  

  //SGOfNilpotentGroup normaliser() const {
  // returns the normalizer of the subgroup

  //SGOfNilpotentGroup isolator() const {
  // returns the isolator of the subgroup

  VectorOf<Word> normalClosureBasis() const {
    return look()->normalClosureBasis();
  }
  // returns a basis of normal closure of the subgroup
  // words are in terms of the group generators

  VectorOf<Word> normalClosureGens() const {
    return look()->normalClosureGens();
  }
  // returns the generators of normal closure of the subgroup
  // words are in terms of the group generators

  PresentationForSNG makePresentation() const {
    return look()->makePresentation();
  }
  // produces FP presentation of the subgroup
  // requires the preimage basis to be built

  //---------------------------------------------------------------------
  //  Methods dealing with other subgroups
  //---------------------------------------------------------------------

  VectorOf<Word> join(const SGOfNilpotentGroup& SGR) const {
    return look()->join(SGR);
  }
  // Returns the join of this subgroup and the argument.
  // no intialization is necessary

  VectorOf<Word> join(const VectorOf<Word>& V) const {
    return look()->join(V);
  }
  // Returns the join of this subgroup and the argument.
  // no intialization is necessary
  // words are in terms of the group generators

  //SGOfNilpotentGroup intersection(SGOfNilpotentGroup& SGR) const {
  // Returns the intersection of this subgroup and the argument.

  bool contains(const VectorOf<Word>& V) const {
    return look()->contains(V);
  }
  // Returns true iff this subgroup contains the subgroup generated by `V'.
  // ** time consuming, requires theFullPreimage initialization

  bool contains(const SGOfNilpotentGroup& SG) const {
    return look()->contains(SG.generators());
  }
  // Returns true iff this subgroup contains `SG'.
  //@ep add validity check and explanation
  // ** time consuming, requires theFullPreimage initialization

  bool equalTo(const SGOfNilpotentGroup& SG) const {
    return contains(SG) && SG.contains(*this);
  }
  //Returns true iff this subgroup and SG are equal.
  //@ep add explanation
  // ** time consuming, requires theFullPreimage initialization

  //bool conjugateTo(const SetOf<Word>& S) const {
  // Returns true iff this subgroup and the argument are conjugate.

  //bool conjugateTo(const SGOfNilpotentGroup& S) const {
  // Returns true iff this subgroup and the argument are conjugate.


  //---------------------------------------------------------------------
  // Methods dealing with group elements:
  //---------------------------------------------------------------------

  bool contains(const Word& w) const {
    return look()->contains(w);
  }
  // Returns true iff this subgroup contains `w'.
  // Slow, requires theFullPreimage initialization
  // w is in terms of the group generators

  bool decompose(const PolyWord& w, PolyWord& decomp) const {
    return look()->decompose(w, decomp);
  }
  // Finds decomposition of the subgroup element w
  // returns true iff w is an element of the subgroup
  // Input: w, in terms of the group basis
  // Output: decomp, in terms of the subgroup basis

  //bool conjugateInSubgroup(const Word& w, Word& conjugator) const {
  // Returns true iff some conjugate of `w' is in the subgroup.
  // If true, `conjugator' is set to the conjugator.

  //int powerInSubgroup( const Word& w ) const {
  // returns `the minimal power' or 0 if there are no powers of the
  // element w in H.


  //---------------------------------------------------------------------
  // I/O:
  //---------------------------------------------------------------------
 
  void printBasis(ostream& s) const {
    look()->printBasis(s);
  }

  ostream& writeIPC(ostream& s) const {
    return look()->writeIPC(s);
  }

  istream& readIPC(istream& s) const {  // Logical const!
    return look()->readIPC(s);
  }

  Chars asDecomposition( const PolyWord& p ) const {
    return look()->asDecomposition(p);
  }
  // Produces string presentation of decomposed subgroup element
  // Input: word in terms of the subgroup basis


  //---------------------------------------------------------------------
  // Internal methods:
  //---------------------------------------------------------------------

private:

  static SGOfNilpotentGroupRep* makeRep(const NilpotentGroup& ng,
					const VectorOf<Word>& gens);
  //Produces a representation depending on group's type


};


//======================================================================
//======================= Global functions =============================
//======================================================================

inline ostream& operator < (ostream& s, const SGOfNilpotentGroup& g) {
  return g.writeIPC(s);
}
// IPC output


inline istream& operator > (istream& s, const SGOfNilpotentGroup& g) {
  return g.readIPC(s);
}
// IPC intput


#endif


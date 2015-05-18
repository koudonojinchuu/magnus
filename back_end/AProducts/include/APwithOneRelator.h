// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the APwithOneRelator class.
//
//  It is implemented ONLY for free groups.
//
// Principal Authors: Eugene Paderin, Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
// Special Notes:
//
//

#ifndef _APRODUCT_WITH_ONE_RELATOR_H_
#define _APRODUCT_WITH_ONE_RELATOR_H_


#include "APofFreeGroups.h"
#include "APwithOneRelatorRep.h"


class APwithOneRelator : public DerivedObjectOf<AmalgProductOfFreeGroups,APwithOneRelatorRep> {

public:

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Constructors                                     //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Copy constructor, operator= and destructor provided by compiler

  APwithOneRelator() :
  DerivedObjectOf<AmalgProductOfFreeGroups, APwithOneRelatorRep>(
    new APwithOneRelatorRep( 0 )
  ) {}
  // Default constructor is provided solely for constructing an AP group
  // from an input stream, e.g. like this:
  //
  //   APwithOneRelator G;
  //   Chars errMesg = cin >> G;
  //   ...
  // Do not use it for purposes other than this.

  APwithOneRelator(const FreeGroup& g1, const FreeGroup& g2,
		   const Word& a, const Word& b ) :
    DerivedObjectOf<AmalgProductOfFreeGroups, APwithOneRelatorRep>(
      new APwithOneRelatorRep(g1, g2, a, b)
    ) {}
  // Construct an amalgamated product of given free groups. Given words
  // generate cyclic associated subgroups.

  APwithOneRelator(const FreeGroup& g1, const FreeGroup& g2,
		   const VectorOf<Word>& gen1,
		   const VectorOf<Word>& gen2  ) 
  : DerivedObjectOf<AmalgProductOfFreeGroups, APwithOneRelatorRep>
    ( new APwithOneRelatorRep(g1, g2, gen1, gen2) ) {}
  // Construct an amalgamated product of given free groups. Given vectors
  // of words generate associated subgroups. Length of vectors must be 1.

  APwithOneRelator(const SGofFreeGroup& sg1, const SGofFreeGroup& sg2)
  : DerivedObjectOf<AmalgProductOfFreeGroups, APwithOneRelatorRep>
    ( new APwithOneRelatorRep(sg1, sg2) ) {}
  // Construct an amalgamated product with given (associated) subgroups
  // which store references to their parent groups.

  APwithOneRelator( const Group& G ) 
  : DerivedObjectOf<AmalgProductOfFreeGroups,APwithOneRelatorRep>(G) { }
  // Cast construtor: to cast an arbitrary group as an AP group.
  // NB. This rewraps the unchanged representation, hence is in general
  // only useful for casting a group known to be an actual AP group.

  // Destructor provided by compiler.

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Accessors / Modifiers                            //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Type actualType( ) const;
  // Overrides pseudo-virtual AmalgProductOfFreeGroups::actualType().

  static Type type( ) { return APwithOneRelatorRep::type(); }
  // Overrides AmalgProductOfFreeGroups::type().

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods dealing with group structure             //
  //                                                   //
  ///////////////////////////////////////////////////////

  // All of them are inherited from AmalgProductOfFreeGroups.

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Methods which deal with group elements           //
  //                                                   //
  ///////////////////////////////////////////////////////

  // Trichotomy conjugacyProblem( const Word& u, const Word& v ) const;
  // Overrides pseudo-virtual AmalgProductOfFreeGroups::conjugacyProblem().

  // VectorOf<int> computeInvariants() const;
  // Overrides pseudo-virtual AmalgProductOfFreeGroups::computeInvariants().

  // void maximalRoot(const Word& w, Word& root, int& power);
  // Overrides pseudo-virtual AmalgProductOfFreeGroups::maximalRoot().

  Trichotomy conjugacyProblem(const Word& u, const Word& w, Word& conjugator) 
    const {
      return look()->conjugacyProblem(u,w,conjugator);
    }
  // Returns `yes' and conjugator of w if u and v represent conjugate 
  // elements (w^conjugator = u).  `no' if not.

  VectorOf<Word> centralizerOf(const Word& w) const {
    return look()->centralizerOf(w);
  }
  // Returns set of words which generate centralizer of given word.

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Private helper stuff:                            //
  //                                                   //
  ///////////////////////////////////////////////////////

protected:

  // Special wrapping constructor to wrap new representations (returned
  // by eg. delegated methods) and for base initialisation by derived
  // classes:

  APwithOneRelator( APwithOneRelatorRep* newrep ) :
    DerivedObjectOf<AmalgProductOfFreeGroups,APwithOneRelatorRep>(newrep) { }


  ///////////////////////////////////////////////////////
  //                                                   //
  //  Debugging stuff                                  //
  //                                                   //
  ///////////////////////////////////////////////////////

private:

#ifdef DEBUG
  //  friend int main(...);
  friend void debugPrint(ostream&, const APwithOneRelator& g);
#endif

};

#endif



// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the APwithOneRelatorRep class.
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

#ifndef _AMPRODUCT_WITH_ONE_RELATOR_REP_H_
#define _AMPRODUCT_WITH_ONE_RELATOR_REP_H_

#include "APofFreeGroups.h"

struct APwithOneRelatorRep : AmalgProductOfFreeGroupsRep {

// Constructors:

  // Copy constructor and operator= provided by compiler (do deep copy).

  APwithOneRelatorRep(const int dummy) 
    : AmalgProductOfFreeGroupsRep( FreeGroup(), FreeGroup(), 
				  VectorOf<Word>(), VectorOf<Word>() )
  { }
  // special `default' constructor. It omits any chechkings.
  // It usally called on constructing an amalgamated product from
  // input stream, e.g like this:
  //  
  //  APwithOneRelator G;
  //  Chars errMesg = cin >> G;
  //  ...
    

  APwithOneRelatorRep(const FreeGroup& g1, const FreeGroup& g2,
		      const Word& gen1, const Word& gen2 );
  
  APwithOneRelatorRep(const FreeGroup& g1, const FreeGroup& g2,
		      const VectorOf<Word>& gen1, const VectorOf<Word>& gen2 )
    :	AmalgProductOfFreeGroupsRep( g1, g2, gen1, gen2 ) 
  {
    #if ( SAFETY > 0 )
     if( gen1.length() != 1 || gen2.length() != 1 )
      error("Constructing APwithOneRelator object with non-cyclic subgroups.");
    #endif
  }

  APwithOneRelatorRep(const SGofFreeGroup& sg1,	const SGofFreeGroup& sg2)
    : AmalgProductOfFreeGroupsRep( sg1, sg2 ) 
  {
    #if ( SAFETY > 0 )
     if( sg1.generators().length() != 1 || sg2.generators().length() != 1 )
      error("Constructing APwithOneRelator object with non-cyclic subgroups.");
    #endif
  }

  // Destructor provided by compiler

// Accessors / Manipulators:

  Word assocWord(int i) const { return assocSubgroup[i].generators()[0]; }
  // Provided that amalgamating condition is a = b, returns a or b.

//
// Representation methods:
//

  PureRep* clone( ) const { return new APwithOneRelatorRep(*this); }
  // Overrides FGGroupRep::clone()

  static const Type theAPwithOneRelatorType;

  static Type type( ) { return theAPwithOneRelatorType; }
  // Dominates AmalgProductOfFreeGroupsRep::type()

  Type actualType( ) const { return type(); }
  // Overrides AmalgProductOfFreeGroupsRep::actualType();


//
// Methods dealing with the properties of the group:
//

  Trichotomy isHyperbolic() const;
  // Overrides AmalgProductOfFreeGroupsRep::isHyperbolic().

//
// I/O:
//

  GroupRep* readFrom(istream&, Chars&) const;
  // Overrides AmalgProductOfFreeGroupsRep::readFrom().

//
// Methods dealing with group elements:
//

  VectorOf<int> computeInvariants() const;

  void maximalRoot(const Word& w, Word& root, int& power) const;

  VectorOf<Word> centralizerOf(const Word& w) const;

  Trichotomy conjugacyProblem( const Word& u, const Word& v ) const {
    Word dummy;
    return conjugacyProblem(u,v,dummy);
  }
  // Overrides AmalgProductOfFreeGroupsRep::().

  Trichotomy conjugacyProblem(const Word& u, const Word& v, Word& conjugator)
    const;
  
private:
  // These methods are used by maximalRoot. Don't use them directly anyway!!
  void maximalRoot_case1(const Word& w, Word& root, int& power) const;
  void maximalRoot_case2(const Word& w, Word& root, int& power) const;
  
  Word translateToAnotherPresentation(const Word& w, const Word& aConjInv, 
				      const Word& bConjInv) const;

  Trichotomy conjugacyProblem_reduced(const Word& u, const Word& v, 
				      Word& conjugator) const;

  Trichotomy conjugacyProblem_case1(const Word& u, const Word& v, 
				    Word& conjugator) const;
  // Conjugacy problem for special case: amalgamated words are cyclically
  // reduced, and every of u, v belongs to a factor

  Trichotomy conjugacyProblem_case2(VectorOf<Word>& uDec, VectorOf<Word>& vDec,
				    Word& conjugator) const;
  // Conjugacy problem for special case: amalgamated words are cyclically
  // reduced, and length of u is greater than 1.

// Data members:

};

#endif




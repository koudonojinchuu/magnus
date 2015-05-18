// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: 
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Usage:
//
//
// Special Notes:
//
// Revision History:
//


#ifndef _HNN_EXTENSION_OF_FREE_GROUP_H_
#define _HNN_EXTENSION_OF_FREE_GROUP_H_


#include "HNNExtension.h"
#include "FreeGroup.h"
#include "SGofFreeGroup.h"
#include "VectorPtr.h"
#include "Automorphism.h"
#include "AP-fixups.h"

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//      Class HNNExtOfFreeGroupRep                                       //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

class HNNExtOfFreeGroupRep: public HNNExtensionRep 
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Constructors:                                                      //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  HNNExtOfFreeGroupRep( const FreeGroup& F, const Chars& nameOfStableLetter,
			const SGofFreeGroup& subgroupA,
			const SGofFreeGroup& subgroupB );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Representation methods:                                             //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  PureRep* clone( ) const { return new HNNExtOfFreeGroupRep(*this); }
  // overrides HNNExtensionRep::clone()

  static const Type theHNNExtOfFreeGroupType;

  static Type type( ) { return theHNNExtOfFreeGroupType; }
  // dominates HNNExtensionRep::type()

  Type actualType( ) const { return type(); }
  // overrides HNNExtensionRep::actualType();

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Accessors:                                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const FGGroup& getBasisGroup( ) const { 
    return theBasisFreeGroup; 
  }
 
  // @dp I don't know how to define accessor to subgroups of the group.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods and operators which deal with the group:                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  virtual int order( ) const { return -1; }
  // overrides FGGroupRep::order()

  //Trichotomy isTrivial( ) const { } // pseudo-virtual
  //Trichotomy isFinite( ) const { } // pseudo-virtual
  //Trichotomy isInfinite( ) const { } // pseudo-virtual
  //Trichotomy isAbelian( ) const { } // pseudo-virtual
  
  Trichotomy isFree( ) const;
  // *** Time consuming algorithm ! ***

   /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods and operators which deal with subgroups:                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods which deal with group elements:                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Overrides pseudo-virtual wordProblem(const Elt).

  Trichotomy isProperPower( const Word& w ) const;
  // It can always determine a result for cyclic subgroups case only.
  // General case is partially supported, it can return `dontknow'.

  Trichotomy maximalRoot( const Word& w, Word& maxRoot, int& maxPower ) const;
  // Returns `yes' if maxPower > 1, and `no' if maxPower == 1.
  // Returns `dontknow' if a result cannot be determined.
  // It can always determine a result for cyclic subgroups case only.
  // General case is partially supported, it can return `dontknow'.
  
  Trichotomy conjugacyProblem( const Word& w, const Word& u ) const {
    Word conjugator;
    return conjugacyProblem( w, u, conjugator );
  }
  // Overrides FGGroupRep::conjugacyProblem().


  Trichotomy conjugacyProblem( const Word& w, const Word& u, Word& conjugator ) const;
  // @dp Not implemented yet.

  bool isProperPowerOfSecond( const Word& w, const Word& u, int& k ) const;
  // Returns `true' iff it exists a number k>1 such that w^k = u,
  // `false' otherwise.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O:                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  GroupRep* readFrom( istream& istr, Chars& errMesg ) const;
  // Overrides FGGroupRep::readFrom().

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void write( ostream& ostr ) const;
  // Overrides HNNExtensionRep::write().
  
  void read( istream& istr );
  // Overrides HNNExtensionRep::read().

protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Protected functions:                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void makeSubgroupsMappings();

  void makeReducedCyclicPresentation( ) const;

  Word mappingFromSubgroup( const NumberOfSubgroup S, const Word& w ) const;

  Trichotomy conjugacyProblem_reduced( const Word& u, const Word& v, 
				       Word& conjugator ) const;

  Trichotomy conjugateInSubgroups( NumberOfSubgroup S, const Word& u, 
				   const Word& v, Word& conjugator, 
				   bool oneIteration ) const;
  // It determines whether the word `u' is conjugated (in HNN-extention) 
  // with an element of the given amalgamated subgroup `S' and last one 
  // is conjugated (in free group) with the word `v'.

  Trichotomy conjugacyProblem_case1( const Word& u, const Word& v, 
				     Word& conjugator ) const;
  // Conjugacy problem for special case: amalgamated words are cyclically
  // reduced, and every of u, v belongs to the basis group.

  Trichotomy conjugacyProblem_case2(VectorOf<Word>& uDec, VectorOf<Word>& vDec, 
				    Word& conjugator ) const;

  // Due the fact that the implementations of subgroups are different, 
  // we hide this through next interfacing members.

  Word getGeneratorOfSubgroup( const NumberOfSubgroup S, const int gen ) const {
    return  theSubgroups[S].generators()[gen];
  }

  int getNumberOfGeneratorsInSubgroup( const NumberOfSubgroup S ) const {
    return  theSubgroups[S].generators().length();
  }

  bool subgroupContains( const NumberOfSubgroup S, const Word& w ) const {
    return  theSubgroups[S].contains( w );
  }

  //Word rewriteInSubgroupBasis( const NumberOfSubgroup S, const Word& w ) const;

  Word rightRepresentative( const NumberOfSubgroup S, const Word& w ) const {
    return theSubgroups[S].rightSchreierRepresentative( w );
  }


  ///////////////////////////////////////////
  //                                       //
  // Helper class SpecialHNNExtOfFreeGroup //
  //                                       //
  ///////////////////////////////////////////

  // This class contains a cyclically reduced presentation of an HNN-extension
  // of a free group with some helper info.

  struct SpecialHNNExtOfFreeGroup {
  public:

    SpecialHNNExtOfFreeGroup() 
      : H(0), toOriginalPresentation( FreeGroup(0), VectorOf<Word>(0) ) { }
    // dumb ctor.

    SpecialHNNExtOfFreeGroup( const Automorphism& mapToOriginalPresentation,
			      const FreeGroup& basisFreeGroup,
			      const Chars& nameOfStableLetter,
			      const SGofFreeGroup& A,
			      const SGofFreeGroup& B,
			      bool  areAmalgSubgroupsConjugateSeparated,
			      const VectorOf<Word>& rootsOfSubgroupsGens,
			      const VectorOf<int>&  powersOfSubgroupsGens
			      );

    SpecialHNNExtOfFreeGroup( const SpecialHNNExtOfFreeGroup& S );
    ~SpecialHNNExtOfFreeGroup() { delete H; }
    SpecialHNNExtOfFreeGroup& operator = ( const SpecialHNNExtOfFreeGroup& S );

    bool doneInit( ) const { return H != 0; }

    const HNNExtOfFreeGroupRep& presentation() const;

    Automorphism         toOriginalPresentation;
    bool                 areAmalgSubgroupsConjugateSeparated;
    VectorOf<Word>       theRootsOfSubgroupsGens;
    VectorOf<int>        thePowersOfSubgroupsGens;   

  private:
    HNNExtOfFreeGroupRep *H;
  };
    

  //////////////////////////////////////
  //                                  //
  // Utility class MaximalRootProblem //
  //                                  //
  //////////////////////////////////////

  class MaximalRootProblem {
  public:
    MaximalRootProblem( const Word& w );
    
    // status query:

    Trichotomy answer() const { return theAnswer; }
    Word root() const { return theRoot; }
    int power() const { return thePower; }
    void solve( const SpecialHNNExtOfFreeGroup& group );

  private:

    void length0( const Word& w );
    void lengthN( const VectorOf<Word> wDeco );
    void setAnswer( const Word& root, int power );

    SpecialHNNExtOfFreeGroup theGroup;
    Word theWord;
    Word theRoot;
    int  thePower;
    Trichotomy theAnswer;
    bool isSolved;
    Word stableLetter;
  };
  
  friend class MaximalRootProblem;

  // Data members:

  FreeGroup theBasisFreeGroup;
  VectorPtrOf<SGofFreeGroup> theSubgroups; // vector of length 2.
  VectorOf< VectorOf<Word> > mapNielsenGensToSubgroupGens;//[2];
  
  // This member contains useful information for solving conjugacy
  // and maximal root problems.
  SpecialHNNExtOfFreeGroup reducedPresentation;
};




///////////////////////////////////////////////////////////////////////////
//                                                                       //
//      Class HNNExtOfFreeGroup                                          //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

class HNNExtOfFreeGroup  
  : public DerivedObjectOf< HNNExtension, HNNExtOfFreeGroupRep>
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Constructors:                                                      //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  HNNExtOfFreeGroup( const FreeGroup& F, 
		     const Chars& nameOfStableLetter,
		     const SGofFreeGroup& subgroupA,
		     const SGofFreeGroup& subgroupB )
  : DerivedObjectOf< HNNExtension, HNNExtOfFreeGroupRep>(
    new HNNExtOfFreeGroupRep(F, nameOfStableLetter, subgroupA, subgroupB )
        ) { }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Accessors:                                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // inherited  FPGroup getFPGroup() const { return look()->getFPGroup(); }

  // const FGGroup& getBasisGroup( ) const;
  // Overrides pseudo-virtual `HNNExtension::getBasisGroup()'.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods and operators which deal with the group:                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  // Overrides pseudo-virtual functions from FGGroup:
  // 
  // int order( ) const; // pseudo-virtual
  // 
  // Trichotomy isFree( ) const { } // pseudo-virtual
  // *** Time consuming algorithm ! ***

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods and operators which deal with subgroups:                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods which deal with group elements:                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy isProperPower( const Word& w ) const {
    return look()->isProperPower( w );
  }
  // It can always determine a result for cyclic subgroups case only.
  // General case is partially supported, it can return `dontknow'.

  // Trichotomy maximalRoot( const Word& w, Word& maxRoot, int& maxPower ) const;
  // overrides pseudo-virtual HNNExtension::maximalRoot().
  // It can determines a result for cyclic subgroups case only.
  // General case is partially supported, it can returns `dontknow'.

  Trichotomy conjugacyProblem( const Word& w, const Word& u, Word& conjugator ) const {
    return look()->conjugacyProblem( w, u, conjugator );
  }

  bool isProperPowerOfSecond( const Word& w, const Word& u, int& k ) const {
    return look()->isProperPowerOfSecond( w, u, k ); 
  }
  // Returns `true' iff it exists a number k>1 such that w^k = u,
  // `false' otherwise.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O:                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const HNNExtOfFreeGroup& G )
  {
    G.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, HNNExtOfFreeGroup& G )
  {
    G.change()->read(istr);
    return istr;
  }

protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Protected functions:                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  HNNExtOfFreeGroup( HNNExtOfFreeGroupRep *newrep ) 
    : DerivedObjectOf<HNNExtension, HNNExtOfFreeGroupRep> ( newrep ) { }
};

#endif

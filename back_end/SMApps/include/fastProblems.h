// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declarations of CM classes for fast problems
//
// Principal Author: Roger Needham, Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 10/11/96 @ep added:
//          class SGOfFreeWhiteheadReduction : public FastComputation
//          class SGOfFreeIsAFreeFactor : public FastComputation
//
//  * 10/09/96 @dp added:
//	class FreeGetNextN_thElement : public FastComputation
//	class TerminalSegmentOfWord : public FastComputation
//      class SegmentOfWord : public FastComputation
//
//  * 10/16/96 @dp added:
//	class APOfFreeNumberOfSubstitutions : public FastComputation
//	class SGOfFreeIsMalnormal : public FastComputation
//	class APOfFree_DoEltsCommute : public FastComputation
//	class APOfFreeIsHyperbolic : public FastComputation
//
//  * 04/06/97 @dp added:
//      class SGOfFreeRank : public FastComputation
//      class WordInNielsenBasisSGOfFree : public FastComputation
//
// Each derivative of class FastComputation has merely a ctor which takes
// the SMObject argument(s), data members to store them, and the
// `takeControl' method. It would involve less typing at least to factor
// out all but the last in a base class parameterized by an int, then
// specialize `takeControl' for values of the int. But of course gcc
// 2.6.3 can't handle template method specialization.


#ifndef _FASTPROBLEMS_H_
#define _FASTPROBLEMS_H_


#include "ComputationManager.h"
#include "Word.h"
#include "SMVectorOfWords.h"
#include "SMEnumerator.h"

//---------------------------------------------------------------------------//
//------------------------- FastComputation ---------------------------------//
//---------------------------------------------------------------------------//


class FastComputation : public ComputationManager
{
public:

  FastComputation( );

  //@njz
  //  void readMessage(class istream &) { }       // overrides SMObject
  void readMessage( istream &) { }       // overrides SMObject
  //

  void viewStructure(ostream& ostr) const;    // overrides SMObject

  void start( ) { }                           // overrides ComputationManager

  void terminate( ) { }                       // overrides ComputationManager
};


//---------------------------------------------------------------------------//
//--------------------- Free Group Computations -----------------------------//
//---------------------------------------------------------------------------//


class CommutatorInFree : public FastComputation
{
public:

  CommutatorInFree(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class FreeInCommutatorSG : public FastComputation
{
public:

  FreeInCommutatorSG(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};

class ProductOfCommutators : public FastComputation
{
public:

  ProductOfCommutators(const class SMWord& w) : 
    theWord( w ),
    theFile( )
    { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
  File theFile;
};

class ProductOfSquares : public FastComputation
{
public:

  ProductOfSquares(const class SMWord& w) : 
    theWord( w ),
    theFile( )
    { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
  File theFile;
};

class FreeIsElementAProperPower : public FastComputation
{
public:

  FreeIsElementAProperPower(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class FreeMaximalRootOfElement : public FastComputation
{
public:

  FreeMaximalRootOfElement(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class FreeCentolizerOfElement : public FastComputation
{
public:

  FreeCentolizerOfElement(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};

class FreeGetN_thElement : public FastComputation
{
public:

  FreeGetN_thElement( class SMFPGroup& G, int n = 0 ) : 
    theGroup( G ), theNumber( n ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
  int theNumber;
};

class FreeGetNextN_thElement : public FastComputation
{
public:

  FreeGetNextN_thElement( class SMWord& w, int n = 0 ) : 
    theWord( w ), theNumber( n ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMWord& theWord;
  int theNumber;
};


class WordProblemInFree : public FastComputation
{
public:

  WordProblemInFree(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class WordsAreEqual : public FastComputation
{
public:

  WordsAreEqual(const class SMWord& w1, const class SMWord& w2)
    : word1( w1 ), word2( w2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& word1;
  const class SMWord& word2;
};


class EndoOnFreeIsMono : public FastComputation
{
public:

  EndoOnFreeIsMono( class SMMap& m ) : map( m ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMMap& map;
};


class EndoOnFreeIsEpi : public FastComputation
{
public:

  EndoOnFreeIsEpi( class SMMap& m ) : map( m ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMMap& map;
};


class EndoOnFreeIsAut : public FastComputation
{
public:

  EndoOnFreeIsAut(const class SMMap& m) : map( m ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMMap& map;
};


class EndoOnFreeIsInner : public FastComputation
{
public:

  EndoOnFreeIsInner(const class SMMap& m) : map( m ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMMap& map;
};

class EndoOnFreeIsIAAut : public FastComputation
{
public:

  EndoOnFreeIsIAAut(const class SMMap& m) : map( m ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMMap& map;
};


class InverseAuto : public FastComputation
{
public:

  InverseAuto(const class SMMap& m) : map( m ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMMap& map;
};

class AutoWhiteheadDecomposition : public FastComputation
{
public:

  AutoWhiteheadDecomposition(const class SMMap& m) : map( m ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMMap& map;
};


class WordInSGOfFree : public FastComputation
{
public:

  WordInSGOfFree(const class SMSubgroup& S, const class SMWord& w)
    : word( w ), subgroup( S )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& word;
  const class SMSubgroup& subgroup;
};


class PowerOfWordInSGOfFree : public FastComputation
{
public:

  PowerOfWordInSGOfFree(const class SMSubgroup& S, const class SMWord& w)
    : word( w ), subgroup( S )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& word;
  const class SMSubgroup& subgroup;
};


class ConjugacyProblemInFree : public FastComputation
{
public:

  ConjugacyProblemInFree(const class SMWord& w1, const class SMWord& w2)
    : word1( w1 ), word2( w2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& word1;
  const class SMWord& word2;
};

class ConjugateOfWordInSGOfFree : public FastComputation
{
public:

  ConjugateOfWordInSGOfFree(const class SMSubgroup& S, const class SMWord& w)
    : word( w ), subgroup( S )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& word;
  const class SMSubgroup& subgroup;
};


class WordInNielsenBasisSGOfFree : public FastComputation
{
public:

  WordInNielsenBasisSGOfFree(const class SMSubgroup& S, const class SMWord& w)
    : word( w ), subgroup( S )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& word;
  const class SMSubgroup& subgroup;
};

class SchreierRepOfWordInSGOfFree : public FastComputation
{
public:

  SchreierRepOfWordInSGOfFree(const class SMSubgroup& S, const class SMWord& w)
    : word( w ), subgroup( S )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& word;
  const class SMSubgroup& subgroup;
};


class SGOfFreeContainment : public FastComputation
{
public:

  SGOfFreeContainment(const class SMSubgroup& S1, const class SMSubgroup& S2)
    : subgroup1( S1 ), subgroup2( S2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup1;
  const class SMSubgroup& subgroup2;
};


class SGOfFreeAreEqual : public FastComputation
{
public:

  SGOfFreeAreEqual(const class SMSubgroup& S1, const class SMSubgroup& S2)
    : subgroup1( S1 ), subgroup2( S2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup1;
  const class SMSubgroup& subgroup2;
};




class SGOfFreeJoin : public FastComputation
{
public:

  SGOfFreeJoin(const class SMSubgroup& S1, const class SMSubgroup& S2)
    : subgroup1( S1 ), subgroup2( S2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup1;
  const class SMSubgroup& subgroup2;
};



class SGOfFreeIntersection : public FastComputation
{
public:

  SGOfFreeIntersection(const class SMSubgroup& S1, const class SMSubgroup& S2)
    : subgroup1( S1 ), subgroup2( S2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup1;
  const class SMSubgroup& subgroup2;
};


class SGOfFreeIsNormal : public FastComputation
{
public:

  SGOfFreeIsNormal( class SMSubgroup& S) : subgroup( S ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMSubgroup& subgroup;
};


class SGOfFreeIsAFreeFactor : public FastComputation
{
public:

  SGOfFreeIsAFreeFactor(const class SMSubgroup& S) : subgroup( S ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup;
};


class SGOfFreeIsMalnormal : public FastComputation
{
public:

  SGOfFreeIsMalnormal(const class SMSubgroup& S) : subgroup( S ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup;
};


class QuadEquationSurfaceForm : public FastComputation
{
public:

  QuadEquationSurfaceForm(const class SMEquation& S) : equation( S ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMEquation& equation;
};


class SGOfFreeWhiteheadReduction : public FastComputation
{
public:

  SGOfFreeWhiteheadReduction(const class SMVectorOfWords& S) : vect( S ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMVectorOfWords& vect;
};


class SGOfFreeNielsenBasis : public FastComputation
{
public:

  SGOfFreeNielsenBasis(const class SMSubgroup& S) : subgroup( S ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup;
};


class SGOfFreeIndex : public FastComputation
{
public:

  SGOfFreeIndex(const class SMSubgroup& S) : subgroup( S ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup;
};

class SGOfFreeRank : public FastComputation
{
public:

  SGOfFreeRank(const class SMSubgroup& S) : subgroup( S ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup;
};


class SGOfFreeNormaliser : public FastComputation
{
public:

  SGOfFreeNormaliser(const class SMSubgroup& S) : subgroup( S ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup;
};


class SGOfFreeHallCompletion : public FastComputation
{
public:

  SGOfFreeHallCompletion(const class SMSubgroup& S) : subgroup( S ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup;
};

class NormalApproximationProblem : public FastComputation
{
public:

  NormalApproximationProblem(const class SMSubgroup& S, int l = 1 ) 
    : subgroup( S ), level( l ) { }

  void takeControl( );

private:

  const class SMSubgroup& subgroup;
  int level;
};


class FreeIsSGTrivial : public FastComputation
{
public:

  FreeIsSGTrivial(const class SMSubgroup& S) : subgroup( S ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup;
};


class FreeIsAutomatic : public FastComputation
{
public:

  FreeIsAutomatic(const class SMFPGroup& G) : group( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMFPGroup& group;
};

class FreeIsHyperbolic : public FastComputation
{
public:

  FreeIsHyperbolic(class SMFPGroup& G) : group( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMFPGroup& group;
};

///---------------------------------------------------------------------------//
//---------------------- Misc Fast Computations -----------------------------//
//---------------------------------------------------------------------------//



class FreelyReduceWord : public FastComputation
{
public:

  FreelyReduceWord(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class CyclicallyReduceWord : public FastComputation
{
public:

  CyclicallyReduceWord(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class FormalInverseOfWord : public FastComputation
{
public:

  FormalInverseOfWord(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class WordLength : public FastComputation
{
public:

  WordLength( const class SMWord& w )
    : theWord ( w ) 
  { }
  
  void takeControl( );              // overrides ComputationManager  
  
private:
  
  const SMWord& theWord;
};


class InitialSegmentOfWord : public FastComputation
{
public:

  InitialSegmentOfWord(const class SMWord& w, int length = 1) : 
    theWord( w ), theLength( length ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
  int theLength;
};

class TerminalSegmentOfWord : public FastComputation
{
public:

  TerminalSegmentOfWord(const class SMWord& w, int length = 1) : 
    theWord( w ), theLength( length ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
  int theLength;
};

class SegmentOfWord : public FastComputation
{
public:

  SegmentOfWord(const class SMWord& w, int start = 1, int length = 1) : 
    // @dp bogus ctor in gcc 2.7.2.1: don't work without default parameters!!!
    theWord( w ), theStart( start ), theLength( length ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
  int theStart;
  int theLength;
};


class FormalProductOfWords : public FastComputation
{
public:

  FormalProductOfWords(const class SMWord& w1, const class SMWord& w2)
    : word1( w1 ), word2( w2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& word1;
  const class SMWord& word2;
};


class ConjugateOfWord : public FastComputation
{
public:

  ConjugateOfWord(const class SMWord& w1, const class SMWord& w2)
    : theWord1( w1 ), theWord2( w2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord1;
  const class SMWord& theWord2;
};


class CommutatorOfWords : public FastComputation
{
public:

  CommutatorOfWords(const class SMWord& w1, const class SMWord& w2)
    : theWord1( w1 ), theWord2( w2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord1;
  const class SMWord& theWord2;
};


class PowerOfMap : public FastComputation
{
public:

  PowerOfMap(const class SMMap& m, int p) : map( m ), power( p ) { }

  PowerOfMap(const class SMMap& m) : map( m ) { }
  //@rn Bogus ctor; see `But no' comment in Menu.h.

  void takeControl( );              // overrides ComputationManager

private:

  const class SMMap& map;
  int power;
};


class ComposeMaps : public FastComputation
{
public:

  ComposeMaps(const class SMMap& m1, const class SMMap& m2)
    : map1( m1 ), map2( m2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMMap& map1;
  const class SMMap& map2;
};


class FreeAreHomsEqual : public FastComputation
{
public:

  FreeAreHomsEqual(const class SMMap& m1, const class SMMap& m2)
    : map1( m1 ), map2( m2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMMap& map1;
  const class SMMap& map2;
};


class ImageUnderMap : public FastComputation
{
public:

  ImageUnderMap(const class SMMap& m, const class SMWord& w)
    : map( m ), word( w )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMMap& map;
  const class SMWord& word;
};


class SGImageUnderMap : public FastComputation
{
public:

  SGImageUnderMap(const class SMMap& m, const class SMSubgroup& S)
    : map( m ), subgroup( S )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMMap& map;
  const class SMSubgroup& subgroup;
};


class ExtendFreeByAut : public FastComputation
{
public:

  ExtendFreeByAut(const class SMMap& m);

  void takeControl( );              // overrides ComputationManager

private:

  const class SMFPGroup& theGroup;
  const class SMMap& theMap;
};


class FPIsMSC : public FastComputation
{
public:

  FPIsMSC(class SMFPGroup& G) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
};


class FastHomology : public FastComputation
{
public:

  // This handles all integral homology computations which are `hard-wired',
  // i.e., do not rely on other, potentially expensive computations, like
  // a rewrite system.
  // Envisaged thus far are one-relator, and abelian groups.

  FastHomology(class SMFPGroup& G) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
};


class SubgroupJoin : public FastComputation
{
public:

  SubgroupJoin(const class SMSubgroup& S1, const class SMSubgroup& S2)
    : subgroup1( S1 ), subgroup2( S2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup1;
  const class SMSubgroup& subgroup2;
};

class SubgroupConjugateBy : public FastComputation
{
public:

  SubgroupConjugateBy(const class SMSubgroup& S, const class SMWord& W)
    : theSubgroup( S ), theWord( W )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& theSubgroup;
  const class SMWord& theWord;
};


//--------------------- Abelian Group Computations --------------------------//


class FastAbelianForm : public FastComputation
{
public:

  FastAbelianForm(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class FastInverseInAbelianForm : public FastComputation
{
public:

  FastInverseInAbelianForm(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class ProductInAbelianForm : public FastComputation
{
public:

  ProductInAbelianForm(const class SMWord& w1, const class SMWord& w2)
    : word1( w1 ), word2( w2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& word1;
  const class SMWord& word2;
};

class AbelianSGJoin : public FastComputation
{
public:

  AbelianSGJoin(const class SMSubgroup& S1, const class SMSubgroup& S2)
    : subgroup1( S1 ), subgroup2( S2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup1;
  const class SMSubgroup& subgroup2;
};

class AbelianIsAutomatic : public FastComputation
{
public:

  AbelianIsAutomatic(class SMFPGroup& G) { }
  AbelianIsAutomatic(class SMSubgroup& G) { }

  void takeControl( );
};

class AbelianIsConfluent : public FastComputation
{
public:

  AbelianIsConfluent(class SMFPGroup& G) { }
  AbelianIsConfluent(class SMSubgroup& G) { }

  void takeControl( );
};

//----------------------- MSC Group Computations ----------------------------//

class MSCOrder : public FastComputation
{
public:

  MSCOrder(class SMFPGroup& G) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
};


class MSCIsTrivial : public FastComputation
{
public:

  MSCIsTrivial(class SMFPGroup& G) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
};


class MSCIsFinite : public FastComputation
{
public:

  MSCIsFinite(class SMFPGroup& G) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
};


class MSCIsAbelian : public FastComputation
{
public:

  MSCIsAbelian(class SMFPGroup& G) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
};


// ------------------ One Relator Group Computations ----------------------- //


class ORIsTrivial : public FastComputation
{
public:

  ORIsTrivial(class SMFPGroup& G) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
};


class ORIsFinite : public FastComputation
{
public:

  ORIsFinite(class SMFPGroup& G) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
};


class ORIsAbelian : public FastComputation
{
public:

  ORIsAbelian(class SMFPGroup& G) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
};


class OROrder : public FastComputation
{
public:

  OROrder(class SMFPGroup& G) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
};


class ORWithTorsionEltFiniteOrder : public FastComputation
{
public:

  ORWithTorsionEltFiniteOrder(class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMWord& theWord;
};


class ORWithTorsionAreEltsEqual : public FastComputation
{
public:

  ORWithTorsionAreEltsEqual(class SMWord& w1, class SMWord& w2) 
    : theWord1( w1 ), theWord2( w2 ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMWord& theWord1;
  class SMWord& theWord2;
};

/*
class ORWithTorsionWordProblem : public FastComputation
{
public:

  ORWithTorsionWordProblem(class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMWord& theWord;
};
*/

class ORWithTorsionExtendedWordProblem : public FastComputation
{
public:

  ORWithTorsionExtendedWordProblem(class SMSubgroup& S, class SMWord& w) 
    : theSubgroup( S ),  theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMSubgroup& theSubgroup;
  class SMWord& theWord;
};

class ORWithTorsionCentralizerOfElt : public FastComputation
{
public:

  ORWithTorsionCentralizerOfElt(class SMWord& w) 
    : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMWord& theWord;
};

class ORWithTorsionConjugacyProblem : public FastComputation
{
public:

  ORWithTorsionConjugacyProblem(class SMWord& w1, class SMWord& w2) 
    : word1( w1 ), word2( w2 ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMWord& word1;
  class SMWord& word2;
};


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Make Menu:                                                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


class MakeCyclicDecomposition : public FastComputation
{
public:

  MakeCyclicDecomposition(class SMFPGroup& G ) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager
    
private:
    
  class SMFPGroup& theGroup;
};


// ------------------------------ Quotients -------------------------------- //


class MakeAbelianQuotient : public FastComputation
{
public:

  MakeAbelianQuotient(class SMFPGroup& G ) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager
    
private:
    
  class SMFPGroup& theGroup;
};


class MakeQuotientFromSubgroup : public FastComputation
{
public:

  MakeQuotientFromSubgroup(class SMSubgroup& S ) : theSubgroup( S ) { }

  void takeControl( );              // overrides ComputationManager
    
private:
    
  class SMSubgroup& theSubgroup;
};

class MakeNilpotentQuotient : public FastComputation
{
public:

  MakeNilpotentQuotient( class SMFPGroup& G, int n = 1 ) : 
    theGroup( G ), theClass( n ) { }
  
  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
  int theClass;
};


class MakeQuotient : public FastComputation
{
public:

  MakeQuotient( class SMFPGroup& G, const SetOf<Word>& S = SetOf<Word>() ) : 
    theGroup( G ), relators( S ) { }
  
  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
  SetOf<Word> relators;
};


// ------------------------------ Products --------------------------------- //


class MakeAPOfFree : public FastComputation
{
public:

  MakeAPOfFree( const SMFPGroup& G1, const SMFPGroup& G2,  
	        const SMSubgroup& S1, const SMSubgroup& S2 )
  : subgroup1( S1 ),
    subgroup2( S2 )
  { }

  void takeControl( );              // overrides ComputationManager
    
private:
    
  const SMSubgroup& subgroup1;
  const SMSubgroup& subgroup2;
};


class APOfFreeReducedForm : public FastComputation
{
public:

  APOfFreeReducedForm(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class APOfFreeNormalForm : public FastComputation
{
public:

  APOfFreeNormalForm(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class APOfFreeCyclicNormalForm : public FastComputation
{
public:

  APOfFreeCyclicNormalForm(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class APOfFreeIsTrivial : public FastComputation
{
public:

  APOfFreeIsTrivial(class SMFPGroup& G) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
};


class APOfFreeIsHyperbolic : public FastComputation
{
public:

  APOfFreeIsHyperbolic(class SMFPGroup& G) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
};


class APOfFreeIsFinite : public FastComputation
{
public:

  APOfFreeIsFinite(class SMFPGroup& G) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
};


class APOfFreeIsAbelian : public FastComputation
{
public:

  APOfFreeIsAbelian(class SMFPGroup& G) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& theGroup;
};


class APOfFreeOrder : public FastComputation
{
public:

  APOfFreeOrder(class SMFPGroup& G) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager
    
private:

  class SMFPGroup& theGroup;
};


class APOfFreeWordProblem : public FastComputation
{
public:

  APOfFreeWordProblem(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class APOfFreeNumberOfSubstitutions : public FastComputation
{
public:

  APOfFreeNumberOfSubstitutions(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class APOfFreeAreEqual : public FastComputation
{
public:

  APOfFreeAreEqual(const class SMWord& w1, const class SMWord& w2)
    : word1( w1 ), word2( w2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& word1;
  const class SMWord& word2;
};


class APOfFree_DoEltsCommute : public FastComputation
{
public:

  APOfFree_DoEltsCommute(const class SMWord& w1, const class SMWord& w2)
    : word1( w1 ), word2( w2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& word1;
  const class SMWord& word2;
};


class APOfFreeIsSGTrivial : public FastComputation
{
public:

  APOfFreeIsSGTrivial(const class SMSubgroup& S) : subgroup( S ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup;
};


class CheckinAPOfFree : public FastComputation
{
public:

  CheckinAPOfFree( ) { }

  void takeControl( );              // overrides ComputationManager
    
//private:
};


class APOfFreeIsSGAbelian : public FastComputation
{
public:

  APOfFreeIsSGAbelian(const class SMSubgroup& S) : theSubgroup( S ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& theSubgroup;
};


class APOfFreeCyclic_CentralizerOfElt : public FastComputation
{
public:

  APOfFreeCyclic_CentralizerOfElt(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};

class APOfFreeCyclic_ConjugacyProblem : public FastComputation
{
public:

  APOfFreeCyclic_ConjugacyProblem( const class SMWord& w1,
				   const class SMWord& w2 )
    : word1( w1 ), word2( w2 )
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& word1;
  const class SMWord& word2;
};


class APOfFreeCyclic_MaximalRoot : public FastComputation
{
public:

  APOfFreeCyclic_MaximalRoot(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class APOfFreeCyclic_IsEltAProperPower : public FastComputation
{
public:

  APOfFreeCyclic_IsEltAProperPower(const class SMWord& w) : theWord( w ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& theWord;
};


class APOfFreeCyclic_IsEltAProperPowerOfSecond : public FastComputation
{
public:

  APOfFreeCyclic_IsEltAProperPowerOfSecond( const class SMWord& w1,
					    const class SMWord& w2 ) 
    : word1( w1 ), word2( w2 ) 
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMWord& word1;
  const class SMWord& word2;
};

//---------------------------------------------------------------------------//
//--------------------- Nilpotent Group Computations ------------------------//
//---------------------------------------------------------------------------//

class FNGAutoIsIAAut : public FastComputation
{
public:

  FNGAutoIsIAAut(const class SMMap& m) : map( m ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMMap& map;
};

class SGOfNGjoinSubgroupProblem : public FastComputation
{
public:

  SGOfNGjoinSubgroupProblem(const class SMSubgroup& s1, const class SMSubgroup& s2) 
    : subgroup1(s1), subgroup2(s2)
  { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMSubgroup& subgroup1;
  const class SMSubgroup& subgroup2;

};

class NGLCStermGensProblem : public FastComputation
{
public:

  NGLCStermGensProblem( class SMFPGroup& g, int num = 1) 
    : group(g),number(num)
  { }

  void takeControl( );              // overrides ComputationManager

private:

  class SMFPGroup& group;
  int number;
};

//------------------------------------------------------//
// ------------------ Products Creation ----------------//
//------------------------------------------------------//

class MakeFreeProduct : public FastComputation
{
public:

  MakeFreeProduct( class SMFPGroup& g1, class SMFPGroup& g2);

  void takeControl( );              // overrides ComputationManager

protected:
  
  void makeHomomorphisms(SMFPGroup* smo);

  class SMFPGroup& group1;
  class SMFPGroup& group2;

  FPGroup fpGroup1;
  FPGroup fpGroup2;

  bool isAbelian;

  class FreeProduct* fp;
};

class MakeDirectProduct : public MakeFreeProduct
{
public:

  MakeDirectProduct( class SMFPGroup& g1, class SMFPGroup& g2) 
    : MakeFreeProduct(g1,g2)
    { }
  
  void takeControl( );              // overrides ComputationManager

};

class MakeFactorGroup : public FastComputation
{
public:

  MakeFactorGroup( class SMFPGroup& F, class SMSubgroup& H) 
    : theGroup( F ), theSubgroup( H ) 
    { }
  
  void takeControl( );              // overrides ComputationManager

private:
  class SMFPGroup& theGroup;
  class SMSubgroup& theSubgroup;
};

// -------------------------- List ------------------------- //

class MakeListOfWords : public FastComputation
{
public:

  MakeListOfWords( EnumeratorProblem<Word>& e):
    enumerator( e ) { }
  
  void takeControl( );              // overrides ComputationManager
  
protected:
  
  EnumeratorProblem<Word>& enumerator;
    
};


// --------------------------- RipsConstruction ---------------------------- //


class MakeRipsConstruction : public FastComputation
{
public:

  MakeRipsConstruction(class SMFPGroup& G ) : theGroup( G ) { }

  void takeControl( );              // overrides ComputationManager
    
private:
    
  class SMFPGroup& theGroup;
};


#endif 

// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:  Interface of class HNNExtOfORGroup.
//            
// Principal Author: Dmitry Pechkin
//
// Status: in progress.
//
// Revision History:
//
// * 03/99 Dmitry B. changed status from protected to public for
//         Generator HNNExtOfORGroupGeneric::stableLetter( ) const;
//
// TO DO:
//
//

#ifndef __HNN_EXTENSION_OF_ORGROUP_H_
#define __HNN_EXTENSION_OF_ORGROUP_H_

#include "Set.h"
#include "Vector.h"
#include "VectorPtr.h"
#include "OneRelatorGroup.h"
#include "SubgroupOfOneRelatorGroup.h"
#include "OneRelatorGroupWithTorsion.h"
// class SubgroupOfOneRelatorGroup;

class HNNExtOfORGroupGeneric
{

public:

  enum NumberOfSubgroup { A = 0, B = 1 };
  enum Pinch { UP, DOWN };

//  Constructors: 

  HNNExtOfORGroupGeneric( const OneRelatorGroup& G, const Chars& stableGenName,
			  const ORGSubgroup& A, const ORGSubgroup& B);
  // stable generator has biggest order in constructing group = lastGenOf(G)+1.

  
  HNNExtOfORGroupGeneric( const HNNExtOfORGroupGeneric& H );

  virtual ~HNNExtOfORGroupGeneric( );

  HNNExtOfORGroupGeneric& operator=( const HNNExtOfORGroupGeneric& H );


//  Accessors/modifiers:

  virtual const ORGSubgroup& subgroup(NumberOfSubgroup i) const { 
    return *subgroups[i]; 
  }

  const OneRelatorGroup& basisGroup( ) const { return *theBasisGroup; }

  FPGroup getPresentation( ) const;

  Chars nameOfStableGenerator( ) const { return theNameOfStableGenerator; }

//  Methods which deal with group elements:

  virtual Trichotomy wordProblem( const Word& w ) const = 0;

  int lengthOf( const Word& w ) const;
  int lengthOf( const VectorOf<Word>& deco ) const;

  VectorOf<Word> decompositionOf( const Word& w ) const;
  VectorOf<Word> reducedDecompositionOf( const Word& w ) const;
  VectorOf<Word> normalDecompositionOf( const Word& w ) const;

  VectorOf<Word> cyclicallyReducedDecompositionOf( const Word& w, Word& conj )
    const;
  // It returns a decomposition w1 of w and a conjugator `conj' such that
  // w1 = w ^ conj.

  //  static Word compose( const VectorOf<Word>& deco );

  Word mappingFromSubgroup( NumberOfSubgroup subgrp, const Word& w ) const;

  ProductOfRelatorConjugates mappingDecompositionOf( 
    const NumberOfSubgroup& S, const Word& w, const Word& wInSBasis, 
    const Word& tail ) const;

  Generator stableLetter( ) const {
    return theBasisGroup->numberOfGenerators() + 1;
  }


//  Standard comparison operators:  

  virtual bool operator == ( const HNNExtOfORGroupGeneric& G ) const;

  friend inline ostream& operator << ( ostream& ostr, 
				       const HNNExtOfORGroupGeneric& H ) 
  {
    H.printOn( ostr );
    return ostr;
  }


protected:

  struct PinchStruct {
    Pinch type;
    int number;
    Word word;
  };

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Debugging stuff                                  //
  //                                                   //
  ///////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////
  //                                                   //
  //  I/O routines:                                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  void printOn( ostream& ostr ) const;

  void printDecomposition( ostream& ostr, const VectorOf<Word>& deco ) const;

  //  virtual HNNExtOfORGroupGeneric* readFrom( istream& istr, 
  //                                            Chars& errMesg ) const;

  virtual void debugPrint( ostream& ostr ) const;

  ///////////////////////////////////////////////////////
  //                                                   //
  //  IPC tools:                                       //
  //                                                   //
  ///////////////////////////////////////////////////////

  virtual void write( ostream& ostr ) const;
  virtual void read( istream& istr );

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Private helper stuff:                            //
  //                                                   //
  ///////////////////////////////////////////////////////

  int powerOfStableGen(int component, const VectorOf<Word>& deco ) const;

  bool suspectPinch(int component, const VectorOf<Word>& deco ) const;

  bool abelianizationTest( const PinchStruct& pinch, 
			   const VectorOf<int>& powersOfGens,
			   const VectorOf<bool>& subgroupGens ) const;

  PinchStruct formPinch(int component, const VectorOf<Word>& deco) const;

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Debugging stuff                                  //
  //                                                   //
  ///////////////////////////////////////////////////////

  int unusedGenerators( const Word& test, VectorOf<Word>& toNewGens,
			VectorOf<Word>& toOldGens ) const;

  void init( const HNNExtOfORGroupGeneric& H );

  // Data members:

  OneRelatorGroup *theBasisGroup;
  Chars theNameOfStableGenerator;
  ORGSubgroup *subgroups[2];
};


class HNNExtOfORGroup : public HNNExtOfORGroupGeneric
{

public:

//  Constructors: 

  HNNExtOfORGroup( const OneRelatorGroup& G,
		   const Chars& stableGenName,
		   const SubgroupOfOneRelatorGroup& A,
		   const SubgroupOfOneRelatorGroup& B);
  // stable generator has biggest order in constructing group = lastGenOf(G)+1.

//  Accessors/modifiers:

  // SubgroupOfOneRelatorGroup subgroup(int i) const { return subgroups[i]; }
  // OneRelatorGroup getBasisGroup( ) const { return theBasisGroup; }
  // FPGroup getPresentation( ) const;
  // Chars nameOfStableGenerator( ) const { return theNameOfStableGenerator; }

//  Methods which deal with group elements:

  Trichotomy wordProblem( const Word& test ) const;

  Trichotomy wordProblem( const Word& test, bool keepDetails,
    ProductOfRelatorConjugates& productOfRelatorConjugates ) const;

  //  int lengthOf( const Word& w ) const;
  //  int lengthOf( const VectorOf<Word>& deco ) const;
  //  VectorOf<Word> decompositionOf( const Word& w ) const;
  //  VectorOf<Word> reducedDecompositionOf( const Word& w ) const;
  //  VectorOf<Word> normalDecompositionOf( const Word& w ) const;
  //  VectorOf<Word> cyclicallyReducedDecompositionOf( const Word& w, 
  //                                                   Word& conj ) const;
  //  Word mappingFromSubgroup( int subgrp, const Word& w ) const;

  VectorOf<Word> reducedDecompositionOf( const Word& w, bool keepDetails,
    ProductOfRelatorConjugates& productOfRelatorConjugates ) const;


//  Standard comparison operators:  

  //  bool operator == ( const HNNExtOfORGroup& G ) const;


  ///////////////////////////////////////////////////////
  //                                                   //
  //  Debugging stuff                                  //
  //                                                   //
  ///////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////
  //                                                   //
  //  I/O routines:                                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  //  void printOn( ostream& ostr ) const;
  //  void printDecomposition( ostream& ostr, const VectorOf<Word>& deco )
  //    const;
  //  HNNExtOfORGroup* readFrom( istream& istr, Chars& errMesg ) const;
  //  void debugPrint( ostream& ostr ) const;

  ///////////////////////////////////////////////////////
  //                                                   //
  //  IPC tools:                                       //
  //                                                   //
  ///////////////////////////////////////////////////////

  //  void write( ostream& ostr ) const;
  //  void read( istream& istr );

private:

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Private helper stuff:                            //
  //                                                   //
  ///////////////////////////////////////////////////////

  //  int powerOfStableGen(int component, const VectorOf<Word>& deco ) const;
  //  bool suspectPinch(int component, const VectorOf<Word>& deco ) const;
  //  bool abelianizationTest( const Pinch& pinch, 
  //                           const VectorOf<int>& powersOfGens,
  //			       const VectorOf<bool>& subgroupGens ) const;
  //  Pinch formPinch(int component, const VectorOf<Word>& deco) const;


protected:

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Debugging stuff                                  //
  //                                                   //
  ///////////////////////////////////////////////////////

  //  Generator stableLetter( ) const;

  int unusedGenerators( const Word& test, VectorOf<Word>& toNewGens,
			VectorOf<Word>& toOldGens ) const;

 

protected:

  // Data members:

  //  OneRelatorGroup theBasisGroup;
  //  Chars theNameOfStableGenerator;
  //  VectorPtrOf<SubgroupOfOneRelatorGroup> subgroups;
  //  static const int A = 0; // number of subgroup A.
  //  static const int B = 1; // number of subgroup B.
};


class HNNExtOfORGroupWithTorsion : public HNNExtOfORGroupGeneric
{

public:

//  Constructors: 

  HNNExtOfORGroupWithTorsion( const OneRelatorGroupWithTorsion& G,
			      const Chars& stableGenName,
			      const SubgroupOfORGroupWithTorsion& A,
			      const SubgroupOfORGroupWithTorsion& B );
  // stable generator has biggest order in constructing group = lastGenOf(G)+1.


  //  Accessors/modifiers:

  // Inherited from HNNExtOfORGroup:
  //   SubgroupOfOneRelatorGroup subgroup(int i) const { return subgroups[i]; }
  //   OneRelatorGroupWithTorsion getBasisGroup( ) const;
  //   FPGroup getPresentation( ) const;
  //   Chars nameOfStableGenerator( ) const;

//  Methods which deal with group elements:

  Trichotomy wordProblem( const Word& test ) const;

  Trichotomy conjugacyProblem( const Word& u, const Word& v, 
			       Word& conjugator ) const;
  // 

  //  int lengthOf( const Word& w ) const;
  //  int lengthOf( const VectorOf<Word>& deco ) const;

  //  VectorOf<Word> decompositionOf( const Word& w ) const;
  //  Word mappingFromSubgroup( int subgrp, const Word& w ) const;
  //  bool operator == ( const HNNExtOfORGroup& G ) const;

  Trichotomy maximalRoot( const Word& w, Word& root, int& power ) const;

//private:


  ///////////////////////////////////////////////////////
  //                                                   //
  //  Debugging stuff                                  //
  //                                                   //
  ///////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////
  //                                                   //
  //  I/O routines:                                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  //  void printOn( ostream& ostr ) const;

  //  void printDecomposition( ostream& ostr, 
  //    const VectorOf<Word>& deco ) const;

  //  HNNExtOfORGroup* readFrom( istream& istr, Chars& errMesg ) const;

  //  void debugPrint( ostream& ostr ) const;

  ///////////////////////////////////////////////////////
  //                                                   //
  //  IPC tools:                                       //
  //                                                   //
  ///////////////////////////////////////////////////////

  //  void write( ostream& ostr ) const;
  //  void read( istream& istr );

private:

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Private helper stuff:                            //
  //                                                   //
  ///////////////////////////////////////////////////////

  //  int powerOfStableGen(int component, const VectorOf<Word>& deco ) const;
  //  bool suspectPinch(int component, const VectorOf<Word>& deco ) const;
  //  bool abelianizationTest( const Pinch& pinch, 
  //                           const VectorOf<int>& powersOfGens,
  //			       const VectorOf<bool>& subgroupGens ) const;
  //  Pinch formPinch(int component, const VectorOf<Word>& deco) const;
  //  Generator stableLetter( ) const;

protected:

  Trichotomy conjugacyProblem_cyclicallyReduced( const VectorOf<Word>& uDeco, 
    const VectorOf<Word>& vDeco, Word& conjugator ) const;

  class MaximalRootProblem
  {
  public:
    MaximalRootProblem( const Word& w );

    void solve( const HNNExtOfORGroupWithTorsion& group );

    Word root( ) const { return theRoot; }

    int power( ) const { return thePower; }

    Trichotomy answer( ) const { return theAnswer; }

  private:

    bool lexCheckOfStableLetters( const VectorOf<Word>& wDeco, 
				  const int rootLen ) const;

    void lengthN( const HNNExtOfORGroupWithTorsion& H, 
		  const VectorOf<Word>& wDeco );

    void setAnswer( const Word& maxRoot, const int maxPower );

    void adjustRoot( );

    // data members:
    const Word theWord;
    Trichotomy theAnswer;
    Word theRoot;
    int thePower;
    bool isSolved;
  };

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Debugging stuff                                  //
  //                                                   //
  ///////////////////////////////////////////////////////

private:

  // Data members:
};



//
//  Helper class:: DoubleCoset
//

class HNNDoubleCoset
{
public:
  HNNDoubleCoset( const SubgroupOfORGroupWithTorsion& Sa,
		  const SubgroupOfORGroupWithTorsion& Sbw );

  Trichotomy solve( const Word& f, Word& a, const Word& g, Word& b ) const;

private:

  Trichotomy oneRelatorWithTorsionSolution( 
    const Word& f, Word& a, const Word& g, Word& b ) const;

  Word minimalSpecialForm( const Word& w, 
			   const SubgroupOfORGroupWithTorsion S ) const;

  // data members:

  SubgroupOfORGroupWithTorsion A;
  SubgroupOfORGroupWithTorsion B;

};


#endif

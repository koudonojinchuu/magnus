// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: 
//
// Principal Author:
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


#ifndef _HNN_EXTENSION_H_
#define _HNN_EXTENSION_H_


#include "FPGroup.h"

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//      Class HNNExtensionRep                                            //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

class HNNExtensionRep : public FGGroupRep
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Constructors:                                                      //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  HNNExtensionRep( const FPGroup& G );
  // Construct an HNN-extension with given presentation G. 
  // A stable letter *must* have the largest order in generators of G. 
  // No subgroups specified because amalgamated subgroups are determined
  // by relators with the stable letter. The rest of the relators of group G 
  // defines set of relators of basis group. 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Representation methods:                                             //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  // Inherited from FGGroupRep:
  //   virtual PureRep* clone( ) const;
  // No need for clone( ) in derived abstract representation.

  static const Type theHNNExtensionType;

  static Type type( ) { return theHNNExtensionType; }
  // Dominates FGGroupRep::type()

  Type actualType( ) const { return type(); }
  // Overrides FGGroupRep::actualType();

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Types:                                                             //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  enum NumberOfSubgroup { A, B };

  enum Pinch { UP, DOWN };

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Accessors:                                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const FPGroup& getFPGroup() const { return theFPGroup; }

  virtual const FGGroup& getBasisGroup( ) const = 0;

  Generator stableLetter( ) const { return theNumberOfGenerators; }
 
  // @dp I don't know how to define accessor to subgroups of the group here.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods and operators which deal with the group:                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy isTrivial( ) const; 
  // Override FGGroupRep::isTrivial().

  Trichotomy isFinite( ) const; 
  // Override FGGroupRep::isFinite().

  Trichotomy isInfinite( ) const;
  // Override FGGroupRep::isInfinite().

  Trichotomy isAbelian( ) const; 
  // Override FGGroupRep::isAbelian().

  virtual Trichotomy isFree( ) const = 0; 
  // Returns YES if this group is free on its generators, NO if not,
  // and DONTKNOW if no answer can be determined.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods and operators which deal with subgroups:                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool isSubgroupTrivial( const VectorOf<Word>& vec ) const;
  
  bool isSubgroupAbelian( const VectorOf<Word>& vec ) const;
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods which deal with group elements:                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy areEqual( const Elt& e1, const Elt& e2 ) const {
    return wordProblem( e1 * e2.inverse() );
  }

  Elt eval( const Word& w ) const { return normalFormOf( w ); }
  // Overrides FGGroupRep::eval().

  int lengthOf( const Word& w ) const { return decompositionOf( w ).length() / 2; }
  // Counts number of stable letters and its inverses in the given word.

  Word reducedFormOf( const Word& w ) const {
    return compose( reducedDecompositionOf( w  ) ); 
  }
  // Returns a reduced form of the given word.

  Word normalFormOf( const Word& w ) const {
    return compose( normalDecompositionOf( w ) );
  }
  // Returns a normal form of the given word.

  Word cyclicallyReducedFormOf( const Word& w, Word conjugator ) const {
    return compose( cyclicallyReducedDecompositionOf( w, conjugator ) );
  }
  // Returns a cyclially reduced form of the given word.


  VectorOf<Word> decompositionOf( const Word& w ) const;
  VectorOf<Word> reducedDecompositionOf( const Word& w ) const;
  VectorOf<Word> normalDecompositionOf( const Word& w ) const;
  VectorOf<Word> cyclicallyReducedDecompositionOf( const Word& w, 
						   Word& conjugator ) const;

  static Word compose( const VectorOf<Word>& V );

  Trichotomy wordProblem( const Word& w ) const; 
  // overrides FGGroupRep::wordProblem().

  virtual Trichotomy  maximalRoot( const Word& w, Word& maxRoot, int& maxPower ) const = 0;
  // Returns `yes' if maxPower > 1, and `no' if maxPower == 1.
  // Returns `dontknow' if a result cannot be determined.
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O:                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void printOn( ostream& ) const;
  
  void printDecomposition( ostream& ostr, const VectorOf<Word>& deco ) const {
    printVectorOfWords( ostr, deco );
  }
  
  // GroupRep* readFrom( istream& istr, Chars& errMesg ) const;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void write( ostream& ostr ) const;
  // overrides FGGroupRep::write().
  
  void read( istream& istr );
  // overrides FGGroupRep::read().


protected:

  // Data members:

  FPGroup theFPGroup; // the presentation of the group
  //Generator theStableLetter; 
  //Chars theNameOfStableLetter;
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Protected functions:                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


  virtual Word mappingFromSubgroup( NumberOfSubgroup S, const Word& w ) const = 0;

  // Due the fact that the implementations of subgroups are different, 
  // we hide this through next interfacing members.

  virtual Word getGeneratorOfSubgroup( const NumberOfSubgroup subgrp, int gen ) const = 0;
  
  virtual int getNumberOfGeneratorsInSubgroup( const NumberOfSubgroup subgrp ) const = 0;

  virtual bool subgroupContains( const NumberOfSubgroup subgrp, const Word& w ) const  = 0;

  // virtual Word rewriteInSubgroupBasis( const NumberOfSubgroup subgrp, const Word& w ) const = 0;

  virtual Word rightRepresentative( const NumberOfSubgroup subgrp, const Word& w ) const = 0;

private:

  int powerOfStableGen( int component, const VectorOf<Word>& deco ) const;

  bool suspectPinch( int component, const VectorOf<Word>& decomposition ) const;
  Pinch formPinch( int component, const VectorOf<Word>& deco ) const;
};




///////////////////////////////////////////////////////////////////////////
//                                                                       //
//      Class HNNExtension                                               //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

class HNNExtension 
  : public DerivedObjectOf<FGGroup,HNNExtensionRep>
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Constructors:                                                      //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // No default constructor for pseudo-abstract class.
  // Copy constructor, operator=, and destructor supplied by compiler.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Types:                                                             //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  typedef HNNExtensionRep::NumberOfSubgroup NumberOfSubgroup;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Accessors:                                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const FPGroup& getFPGroup() const { return look()->getFPGroup(); }
  // Returns the presentation of the group.

  const FGGroup& getBasisGroup( ) const { return look()->getBasisGroup(); }
  // Returns the basis group of the HNN-extension. 

  Generator stableLetter( ) const { return look()->stableLetter(); }
  // Returns a stable letter.

  //  Due the fact that the implementations of subgroups are different, 
  //  we hide this through next interfacing members.
  // Subgroup getSubgroup( NumberOfSubgroup subgrp ) const;
  // virtual getGeneratorOfSubgroup( const NumberOfSubgroup subgrp, int gen ) = 0;
  // virtual getNumberOfGeneratorsInSubgroup( const NumberOfSubgroup subgrp ) = 0;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods and operators which deal with the group:                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  // Overrides pseudo-virtual functions from FGGroup:
  // 
  // Trichotomy isTrivial( ) const; // pseudo-virtual
  // Trichotomy isFinite( ) const; // pseudo-virtual
  // Trichotomy isInfinite( ) const; // pseudo-virtual
  // Trichotomy isAbelian( ) const; // pseudo-virtual

  Trichotomy isFree( ) const { return enhance()->isFree(); }
  // Returns YES if this group is free on its generators, NO if not,
  // and DONTKNOW if no answer can be determined.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods and operators which deal with subgroups:                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool isSubgroupTrivial( const VectorOf<Word>& vec ) const {
    return look()->isSubgroupTrivial( vec );
  }

  bool isSubgroupAbelian( const VectorOf<Word>& vec ) const {
    return look()->isSubgroupAbelian( vec );
  }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Methods which deal with group elements:                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Trichotomy wordProblem( const Word& w ) const;
  // Overrides pseudo-virtual function.

  int lengthOf( const Word& w ) const { return look()->lengthOf( w ); }

  Word reducedFormOf( const Word& w ) const {
    return look()->reducedFormOf( w );
  }
  // Returns a reduced form of the given word.

  Word normalFormOf( const Word& w ) const {
    return look()->normalFormOf( w );
  }
  // Returns a normal form of the given word.

  // Public data members:

  //@dp++ static Word Conjugator;

  Word cyclicallyReducedFormOf( const Word& w, Word conjugator ) const
    //@dp++ = HNNExtension::Conjugator) const 
  {
    return look()->cyclicallyReducedFormOf( w, conjugator );
  }
  // Returns a cyclially reduced form of the given word.
  
  VectorOf<Word> decompositionOf( const Word& w ) const {
    return look()->decompositionOf( w );
  }
  
  VectorOf<Word> reducedDecompositionOf( const Word& w ) const {
    return look()->reducedDecompositionOf( w );
  }
  
  VectorOf<Word> normalDecompositionOf( const Word& w ) const {
    return look()->normalDecompositionOf( w );
  }
  
  VectorOf<Word> cyclicallyReducedDecompositionOf( const Word& w, 
						   Word& conjugator) const {
    return look()->cyclicallyReducedDecompositionOf( w, conjugator );
  }

  static Word compose( const VectorOf<Word>& V ) ;
  
  virtual Trichotomy  maximalRoot( const Word& w, Word& maxRoot, int& maxPower ) const {
    return look()->maximalRoot( w, maxRoot, maxPower );
  }
  // Returns `yes' if maxPower > 1, and `no' if maxPower == 1.
  // Returns `dontknow' if a result cannot be determined.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O:                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void printDecomposition( ostream& ostr, const VectorOf<Word> deco ) const {
    look()->printDecomposition( ostr, deco );
  }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const HNNExtension& G )
  {
    G.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, HNNExtension& G )
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

  HNNExtension( HNNExtensionRep *newrep ) 
    : DerivedObjectOf<FGGroup,HNNExtensionRep> ( newrep ) { }

private:
  
  HNNExtension( ); // Hidden, not to be implemented.

};

#endif

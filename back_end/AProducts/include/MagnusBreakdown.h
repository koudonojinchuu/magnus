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
//  97/02/13 @dp Added VectorOf<Range> getSubscriptsTable() const.
//               Made  adjoinSubscript(), extractSubscript() public.
//
// Would we store all tree of computations? No! We need to provide 
// some repository to store all informations of computations. 
// This class only solves Magnus breakdown step by step.  



#ifndef _MAGNUS_BREAKDOWN_H_
#define _MAGNUS_BREAKDOWN_H_

#include "OneRelatorGroup.h"
#include "HNNExtOfORGroup.h"
#include "AbelianWord.h"
#include "Map.h"
#include "SuperGen.h"
#include "Range.h"
#include "Automorphism.h"


class MagnusBreakdown
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Constructors:                                                      //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  MagnusBreakdown( const OneRelatorGroup& G );
    
  // No default constructor, but it can be easily arranged by
  // combining default FPGroup constructor and previsious one.

  // Copy constructor and operator = provided by compiler
  
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Accessors:                                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  OneRelatorGroup getORGroup() const { return theORGroup; }
  // Returns a current node of Magnus breakdown.

  HNNExtOfORGroup getHNNPresentation() const;
  // The caller should call `findHNNPresentation' before 
  // and checks a presentation to be existed, 
  // otherwise this function causes error.

  int numberOfUsedGenerators() const { return (stable.exponent()==0 ? 1 : 2); }

  Generator stableGenerator() const { return stable.generator(); }

  bool hasAccompGenerator() const { 
    return (stable.exponent() == 0 ? false : true ); 
  }

  Generator accompGenerator() const;

  ListOf<Generator> getGeneratorsWithZeroExpSum() const { 
    return theGeneratorsWithZeroExpSum;
  }
  
  ListOf<Generator> getGeneratorsWithNonzeroExpSum() const {
    return theGeneratorsWithNonzeroExpSum;
  }

  ListOf<Generator> getDefaultBreakdownGenerators() const;

  int getExponentSumOf( const Generator& g ) const {
    return exponentsOfOldGenerators[ abs(ord(g))-1 ];
  }

  Word rewriteWordInOldGenerators( const Word& w ) const; 

  Word rewriteWordInNewGenerators( const Word& w ) const;

  Automorphism embeddingOfORGroups( ) const { return theEmbeddingOfORGroups; }

  Map toHNNPresentation( ) const { return mapToHNN; }

  Map toORGroup( ) const { return mapToORGroup; }

  int numberOfOldGenerators( ) const { return theNumberOfOldGenerators; }
  int numberOfNewGenerators( ) const { return theNumberOfNewGenerators; }

  VectorOf<Range> getSubscriptsTable() const { return subscriptsOfGenerator; }
  
  Generator adjoinSubscript(const Generator& oldGen, int subscript) const;
  Generator extractSubscript(const Generator& newGen, int& subscript) const;
  // @dp NOTE: these procedures work with the bigger one-relator group.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Start/continue:                                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy hasHNNPresentation() const { return haveHNNPresentation; }
  // `no' means the node is a (terminal) list. 
  // `dontknow' means the node is not investigated yet.

  bool findHNNPresentation();
  // Does one step of Magnus breakdown.

  bool findHNNPresentation( const Generator& stableGen );

  bool findHNNPresentation( const Generator& stableGen, 
			    const Generator& accompGen );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O routines:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  void printOn( ostream& ostr ) const;

  MagnusBreakdown *readFrom( istream& istr ) const;

  void debugPrint( ostream& ostr ) const;

protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Protected functions:                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void makeHNNPresentation( );
  bool chooseStableGenerator( );
  void makeSubscriptsTable( );
  void makeEmbedding();
  void makeTranslationTables();


  // Data members:

  // fixed data members:

  OneRelatorGroup	theORGroup;
  int			theNumberOfOldGenerators;
  ListOf<Generator>	theGeneratorsWithZeroExpSum;
  ListOf<Generator>	theGeneratorsWithNonzeroExpSum;
  VectorOf<int>		exponentsOfOldGenerators;

  
  // volatile data members:

  OneRelatorGroup	theLargerORGroup;
  HNNExtOfORGroup	theHNNPresentation;
  bool			haveHNNPresentation;
  Automorphism		theEmbeddingOfORGroups;
  Map			mapToHNN;
  Map			mapToORGroup;
  SuperGen		stable, accomp;
  Generator		stableNewGen;
  int			theNumberOfNewGenerators;
  VectorOf<Chars>	theNamesOfNewGenerators;
  VectorOf<Range>	subscriptsOfGenerator;
  VectorOf<int>		toNewGens;
  VectorOf<int>		toOldGens;

};

ProductOfRelatorConjugates liftUpProduct( 
  const ProductOfRelatorConjugates& mProduct, 
  const MagnusBreakdown& M, const OneRelatorGroup& G 
);


// --------- Inline members of MagnusBreakdown ------------------ //

inline HNNExtOfORGroup MagnusBreakdown::getHNNPresentation() const 
{
#if SAFETY > 0
  if( !haveHNNPresentation )
    error("Called MagnusBreakdown::getHNNPresentation() "
	  "when HNN presentation does not exist.");
#endif
  return theHNNPresentation;
}

inline Generator MagnusBreakdown::accompGenerator() const 
{
#if SAFETY > 0
  if( stable.exponent() == 0 )
    error("MagnusBreakdown::accompGenerator(): Calling for "
	  "an accompanying generator when it is not defined.");
#endif
  return accomp.generator();
}

#endif

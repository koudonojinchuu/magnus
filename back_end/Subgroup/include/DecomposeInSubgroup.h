// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: 
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
// Bugs:
//
// Special Notes:
//
// Next implementation steps:
//


#ifndef _DECOMPOSE_IN_SUBGROUP_H_
#define _DECOMPOSE_IN_SUBGROUP_H_


#include "SubgroupGraph.h"
#include "FreeGroup.h"
#include "../AProducts/include/AP-fixups.h"

// NOTE: Original generators of subgroup should be freely reduced.

class DecomposeInSubgroupOfFreeGroup
{
public:
  
  DecomposeInSubgroupOfFreeGroup( const int ambientRank, 
				  const VectorOf<Word>& gens );
  // Construct a subgroup of a free group. `gens' are given generators
  // of the subgroup.

  virtual ~DecomposeInSubgroupOfFreeGroup( ) { }

  virtual DecomposeInSubgroupOfFreeGroup* clone( ) const {
    return new DecomposeInSubgroupOfFreeGroup( *this );
  }

  int rankOfFreeGroup( ) const { return theAmbientRank; }
  // Returns the rank of the ambient free group.

  VectorOf<Word> generators( ) const { return theGenerators; }
  // Returns the given generators of the subgroup.

  const SubgroupGraph& graph() const { return theGraph; }
  // Returns an graph represinting the subgroup.

  VectorOf<Word> decomposeNielsenBasis( ) const {
    return nielsenInSubgroupGenerators;
  }
  // Returns a decomposition of a Nielsen basis for the subgroup
  // in the original subgroup generators.

  bool contains( const Word& w ) const { return theGraph.contains(w); }
  // Checks whether the subgroup contains the given word.

  Word decompose( const Word& w ) const;
  // Express the word `w' in the given subgroup generators.

  Trichotomy checkDecomposition( const Word& w ) const;

  //protected:

  DecomposeInSubgroupOfFreeGroup( const SubgroupGraph& G, 
				  const VectorOf<Word>& gens );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend 
  ostream& operator<(ostream& ostr, const DecomposeInSubgroupOfFreeGroup& d) {
    d.write( ostr );
    return ostr;
  }

  friend 
  istream& operator>(istream& istr, DecomposeInSubgroupOfFreeGroup& d );

  virtual bool readPiece( istream& istr, const class Timer& timer );


private:
  
  void init( );

protected:



  VectorOf<Word> makeMapBetweenNielsens( SubgroupGraph& S, 
					 const VectorOf<Word>& words ) const;


  // Data members:

  int theAmbientRank;
  VectorOf<Word> theGenerators;

protected:
  SubgroupGraph theGraph;
  VectorOf<Word> nielsenInSubgroupGenerators;

  virtual void write( ostream& ostr ) const;
  int n;

private:

  enum ReadingState { STOP, GENS, GRAPH, NIELSEN };

  // Data members:
  ReadingState readState;
};




class DecomposeInExpandingSubgroup : public DecomposeInSubgroupOfFreeGroup
{
public:

  DecomposeInExpandingSubgroup( int ambientRank,
				const VectorOf<Word>& constantSubgroup, 
				const VectorOf<Word>& expandingSubgroup );
  // Construct expanding subgroup of free group. `expandingGens' will be 
  // conjugated by some elements to expand the subgroup.

  DecomposeInSubgroupOfFreeGroup* clone( ) const {
    return (DecomposeInSubgroupOfFreeGroup *)
              new DecomposeInExpandingSubgroup(*this);
  }

  VectorOf<Word> constantGenerators( ) const { return theConstantGenerators; }
  // Returns the original constant generators of the subgroup.

  VectorOf<Word> expandingGenerators( ) const { return theExpandingGenerators;}
  // Returns the original expanding generators of the subgroup.

  VectorOf<Generator> expandingConjugators( ) const {
    return theExpandingConjugators;
  }
  // Returns a list of conjugators which are used to expand the subgroup.
  // When we expand the subgroup by a conjugator this one is appened
  // to the end of the list.

  void expandSubgroupByConjugation( const Generator& conjugator );
  // Expand the subgroup by a conjugator.

  void expressAsConjugateOfOriginalGenerator( const Generator& g, 
					      Generator& og, Word& conjugator)
    const;
  // It computes an expression of the given generator `g' in an *expanded*
  // subgroup in original generator of the subgroup and conjugator of last one.

  ProductOfRelatorConjugates fullDecomposition( const Word& w ) const;

  Trichotomy checkDecomposition( const Word& w ) const;

  bool readPiece( istream& istr, const class Timer& timer );

private:

  VectorOf<Word> shiftGenerators( const VectorOf<Word>& v, int shift );


  // Data members:

  SubgroupGraph theConstantGraph;
  SubgroupGraph theExpandingGraph;

  VectorOf<Word> theConstantGenerators;
  VectorOf<Word> theExpandingGenerators;  

  // Data for expanding subgroup:
  int theNumberOfExpandedGenerators;
  VectorOf<Generator> theExpandingConjugators;
  VectorOf<Word> Nc, NcInOld, Ne0InOld;

protected:
  virtual void write( ostream& ostr ) const;


private:
  enum ReadingState2 { STOP, BASECLASS, CONSTGRAPH, EXPANDGRAPH, CONSTGENS, 
		       EXPANDGENS, READNC,READNCINOLD, READNE0INOLD };

  // Data members:
  ReadingState2 readState2;
};


class DecomposeInSubgroupOfFPGroup : public DecomposeInExpandingSubgroup
{
public:

  DecomposeInSubgroupOfFPGroup( const FPGroup& G, const VectorOf<Word>& gens );

  DecomposeInSubgroupOfFreeGroup* clone( ) const {
    return (DecomposeInSubgroupOfFreeGroup *)
              new DecomposeInSubgroupOfFPGroup(*this);
  }

  FPGroup group() const { return theGroup; }

  void expandGraph( );

  int numberOfIterations( ) const { return theNumberOfIterations; }

  bool readPiece( istream& istr, const class Timer& timer );

private:

  FPGroup theGroup;
  Generator g;
  int theNumberOfIterations;

protected:
  virtual void write( ostream& ostr ) const;

private:
  enum ReadingState3 { STOP, NEWDATA };

  // Data members:
  ReadingState3 readState3;
};

#endif

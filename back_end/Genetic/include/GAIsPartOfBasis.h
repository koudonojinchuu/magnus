// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of WhiteHeadGA class
//
// Principal Author: Alexei Miasnikov
//
// Status: in progress
//
// Revision History:
//


#ifndef _GA_PART_OF_BASIS_H_
#define _GA_PART_OF_BASIS_H_

#include "RandomNumbers.h"
#include "FPGroup.h"
#include "FreeGroup.h"
#include "ACConfig.h"
#include "Associations.h"

// ------------------------------- GAIsPartOfBasis ------------------------- //


class GAIsPartOfBasis
{

public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
 /////////////////////////////////////////////////////////////////////////

  GAIsPartOfBasis( const FreeGroup& G, const ACConfig& config,
		   const VectorOf<Word>& v);
  
  GAIsPartOfBasis( const FreeGroup& G, const ACConfig& config,
		   const VectorOf<Word>&,const VectorOf<Word>&);
   
  GAIsPartOfBasis( const FreeGroup& G, const ACConfig& config,
		   const Word& w);
  
  // copy constructor supplied by compiler.
  
  // destructor supplied by compiler.
  
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy transform( ostream& , int );

  bool isPartOfBasis(ostream& out1, ostream& out2);
  bool isPartOfBasis(ostream& out1, const Chars& out2_f_name);

  VectorOf<Word> getAutomorphism() const { return theAuto;}

  const VectorOf<Word>& getCurrentBestWords()const { return bestWords; }
  const VectorOf<Word>& getFixedWords() const { return fixedWords; }

  int nOfGenerations() const { return g; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // OI:                                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  int fitness( const Word& u, std::ostream& out = cout, bool print = false );
  
  Word randomWord( );
  
private:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void initialize();

  void printWords(ostream& o, const VectorOf<Word>& v) const;

  Word mutate( const Word& u );
  
  int randomGen( );
  
  Word crossover1( const Word& w1,const Word& w2 );
  
  int genToInsertAfter( Generator g );
  
  int genToInsertBefore( Generator g );
  
  int genToInsert( Generator g1, Generator g2 );
  
  VectorOf<Word> computeAuto(const Word&);

  int computeFitness( const VectorOf<Word>& v ) const;

  int computeFitness1( const VectorOf<Word>& v ) const;

  int hamingDistance(const Word& w1,const Word& w2) const;

  int tupleHamingDistance(const VectorOf<Word>&,const VectorOf<Word>&)const;
  void permuteVector(VectorOf<Word>& v1, int move_to) const;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  FPGroup theGroup;
  ACConfig cfg;
  VectorOf<Word> theWords;

  VectorOf<Word> tmpWords;

  VectorOf<Word> bestWords;
  VectorOf<Word> fixedWords;

  Chars best_out_name;
  bool print_best_each_time;

  friend int main(int, char**);
 
  UniformRandom r;
  int numberOfGens;
  int numberOfTrans;
  int threshold;
  
  VectorOf<Word> transformations;
  VectorOf<Word> transferImage;
  VectorOf<Word> theAuto;
  AssociationsOf<int,int> reduces;

  int g;
  int sav_first_max;

  Word finalResult;
  Word bestTransformation;
  int total_tr;
  int swithc_tr;
};



#endif

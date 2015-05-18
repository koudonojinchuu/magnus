// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of ACGA class
//
// Principal Author: Alexei Miasnikov
//
// Status: in progress
//
// Revision History:
//


#ifndef _ACGA_H_
#define _ACGA_H_

#include "RandomNumbers.h"
#include "FPGroup.h"
#include "FreeGroup.h"
#include "ACConfig.h"
#include "Associations.h"
/*
class TransformationClass
{
 public:
  PopulationClass() {}

  PopulationClass(const Word&);

  PopulationClass(const PopulationClass& pc):
    theTransformation(pc.theTransformation),
    random_conjugates(pc.random_conjugates) { }

  operator = (const PopulationClass& pc){
    theTransformation = pc.theTransformation;
    random_conjugates = pc.random_conjugates;

 private:
  Word theTransformation;
  ListOf<Word> random_conjugates;
};
*/
// ------------------------------- ACGA ------------------------------------ //


class ACGA
{

public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  ACGA( const FPGroup& G, const ACConfig& config );
  
  // copy constructor supplied by compiler.
  
  // destructor supplied by compiler.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  //Trichotomy isTrivial( const Word& u, ostream& out );
  // returns yes if the genetic algorithm could prove that u
  // is trivial; returns dontknow after computing all generations

  bool transform( ostream& out, ostream& best_out );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // OI:                                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  int fitness(  Word& u, ListOf<Word>& conj );

  Word randomWord( );

  void print( Word& u, const ListOf<Word>& conj, ostream& out ) ;

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Word mutate( const Word& u );

  int randomGen( );
  
  Word crossover1( const Word& w1,const Word& w2 );
  
  Word crossover2( const Word& w1,const Word& w2 );
 
  int genToInsertAfter( Generator g );

  int genToInsertBefore( Generator g );

  int genToInsert( Generator g1, Generator g2 );

  Word randomPermutation();

  Word randomWordInGroup();
    
  void applyWhitehead( ostream& out);
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  FPGroup theGroup;
  ACConfig cfg;

  //friend main();
 
//  SetIterator<Word> I;
  UniformRandom r;
  int numberOfRelators;
  int numberOfGens;
  int numberOfTrans;
  VectorOf<Word> transformations;
  VectorOf<Word> relatorsVector;
  VectorOf<Word> transferImage;
  VectorOf<Word> currentBest;
  VectorOf<Chars> trNames;
  AssociationsOf<int,int> reduces;

  int g;
  Word finalResult;

  int numberOfGenTransf;
  int numberOfRelTransf;
  bool useWhitehead;
};



#endif

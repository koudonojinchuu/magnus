// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of TwoCommSolver
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _TwoCommSolver_h_
#define _TwoCommSolver_h_

#include "RandomNumbers.h"
#include "FreeGroup.h"
#include "GHNConfig.h"


// ---------------------------- TwoCommSolver ------------------------------ //


class TwoCommSolver
{

public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  TwoCommSolver( FreeGroup F );
  
  // copy constructor supplied by compiler.
  
  ~TwoCommSolver( );
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool isProductOfTwoComms( Word r, Word& x1, Word& x2,
			    Word& y1, Word& y2,
			    ostream* out = NULL );
       

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // OI:                                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Map getSolution( const Word& u, int& eqInd, int& conjInd,
		   ostream* out = NULL );
  // returns a solution for equation u found by genetic algorithm

  Word getImage( Map M, Map solution, int ind, int num );

  Word mutate( const Word& u );

  int randomGen( );
  
  Word randomWord( ); 
  
  Word crossover( const Word& u, const Word& v );

  int fitness( Word u, Word v, int& ) const;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  FreeGroup theGroup;
  GHNConfig cfg;
  VectorOf<Chars> *varNames;
  int numOfVars, numOfConsts, numOfEq;
  bool keepDetails;
  UniformRandom r;
  int maxWordLen;
  Word *equation;
  Chars popFile;
};

#endif






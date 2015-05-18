// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of GAEquationSolver, GraphicEquationSolver
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _GAEquationSolver_h_
#define _GAEquationSolver_h_

#include "RandomNumbers.h"
#include "FreeGroup.h"
#include "GHNConfig.h"


// -------------------------- GAEquationSolver ----------------------------- //


class GAEquationSolver
{

public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  GAEquationSolver( const FreeGroup& G, int NumOfVars, 
		    const GHNConfig& config );
  
  // copy constructor supplied by compiler.
  
  // destructor supplied by compiler.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Map getSolution( const Word& u, ostream* out = NULL, int& g = dummy_g );
  // returns a solution for equation u found by genetic algorithm


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

  Word mutate( const Word& u );

  int randomGen( );
  
  Word randomWord( ); 
  
  Word crossover( const Word& u, const Word& v );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  FreeGroup theGroup;
  GHNConfig cfg;
  int numOfVars, numOfGens, numOfConsts;
  bool keepDetails;
  UniformRandom r;
  const int maxWordLen;
  static int dummy_g;
};


// ------------------------ GraphicEquationSolver -------------------------- //


class GraphicEquationSolver
{

public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  GraphicEquationSolver( FreeGroup F, VectorOf<Chars> vNames, 
			 Word eq, const GHNConfig& config, Chars fn = "" );
  
  // copy constructor supplied by compiler.
  
  // destructor supplied by compiler.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Map getSolution( const Word& u, ostream* out = NULL );
  // returns a solution for equation u found by genetic algorithm


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

  Word mutate( const Word& u );

  char mutateChar( char u );

  int randomGen( );
  
  Word randomWord( ); 
  
  Word crossover( const Word& u, const Word& v );

  char crossoverChar( char u, char v );

  int fitness( Word u, Word v ) const;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  FreeGroup theGroup;
  GHNConfig cfg;
  VectorOf<Chars> varNames;
  int numOfVars, numOfConsts;
  bool keepDetails;
  UniformRandom r;
  int maxWordLen;
  Word equation;
  Chars popFile;
};

#endif






// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of DGESolver
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _DGESolver_h_
#define _DGESolver_h_

#include "FreeGroup.h"


// ------------------------------- DGESolver ------------------------------- //


class DGESolver
{

public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  DGESolver( FreeGroup F, VectorOf<Chars> vNames, Word eq );
  
  // copy constructor supplied by compiler.
  
  ~DGESolver( );

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool getSolution( const Word& u, Map& solution );
  // returns a solution for equation eq = u


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

  void defineRange( int curVar );
  // defines range for the variable and its inverse

  bool pickNextValue( int curVar );
  // tries to pick sutable value for curVar and its inverse

  bool findInverseOf( Word u, int p, int q, int& first, int& last );
  // finds inverse of u in w[p,q] and returns its coordinates in first and last
 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  struct VarInfo { bool isTrivial; int first, last, firstP, lastP; };
  // first and last - positions in u
  // firstP and lastP - possible range for the variable

  FreeGroup theGroup;
  VectorOf<Chars> varNames;
  int numOfVars, numOfConsts;
  Word equation, w;
  int wLen, eqLen;
   

  VarInfo *varInfo;    // current substitution
  VarInfo *invVarInfo; 
  // info about the inverses of the variables; isTrivial is not used here

  VectorOf<int> buffer;
  // filled with current value of the variables; used define variables range 

  VectorOf<int> invVarPos; // positions of the inverses in the equation

};

#endif






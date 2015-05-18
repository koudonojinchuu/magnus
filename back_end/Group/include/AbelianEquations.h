// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of the AbelianEquationSolver class.
//
// Principal Authors: Denis Serbin
//
// Status: In progress
//
// Revision History:
//
// Special Notes:
//
//


#ifndef _AbelianEquations_h_
#define _AbelianEquations_h_

#include "AbelianGroup.h"
#include "FPGroup.h"
#include "FreeGroup.h"
#include "File.h"
#include "Vector.h"
#include "Word.h"
#include <iostream.h>

class AbelianEquationsSolver {

public:
  
  AbelianEquationsSolver( const AbelianGroup& a , 
			  const VectorOf<Word>& v ,
			  int numOfVar );
  
  AbelianEquationsSolver( )
    : rawA( FPGroup() ),
    A( FPGroup() )
    { }
  
  void findSolutions( File& file , bool out = true );
  
  VectorOf<Word> getBasicSolutions() const
  {
    return x;
  }
  
  VectorOf< VectorOf< VectorOf<int> > > getTorsionPart() const
  {
    return torsion;
  }
  
  VectorOf< VectorOf< VectorOf<int> > > getParametrizedPart() const
  {
    return params;
  }
  
  int getSystemRank() const
  {
    return sysRank;
  }
  
  int getNumberOfParams() const
  {
    return numberOfVariables - sysRank;
  }
  
  bool solIsAllGroup( ) const;
  
  bool haveSolutions() const;
  
private:
  
  bool root( Word& , int ) const;
  
  void makeSystem();
  
  void printRawSystem( File& ) const;
  void printSystem( File& ) const;
  
  AbelianGroup rawA;
  AbelianGroup A;
  VectorOf<Word> rawSystem;
  VectorOf<Word> system;
  VectorOf<Word> b;
  int numberOfVariables;
  int sysRank;
  int haveSol; // -1 - system have no solutions
               //  0 - system have the group A as a set of solutions
               //  1 - the set of solutions isn't equal to A
  
  VectorOf<Word> x;
  VectorOf< VectorOf< VectorOf<int> > > torsion;
  VectorOf< VectorOf< VectorOf<int> > > params;  
};

#endif  





// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of class GroupFastChecks
//
// Principal Author: Dmitry Bormotov
//
// Status: Usable
//
// Usage: Should contain various fast checks applicable in BE & SM.
//
// Special Notes:
//
// Revision History:
//

#ifndef _GroupFastChecks_h_
#define _GroupFastChecks_h_

#include "FPGroup.h"


// --------------------------- GroupFastChecks ------------------------------ //


class GroupFastChecks {

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  GroupFastChecks( const FPGroup& G ) : 
    theGroup( G ),
    numOfGens( G.numberOfGenerators() ),
    relators( G.getRelators() )
  { }

  // Default & copy contructors, operator = provided by compiler.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool hasTrivialPresentation( ) {
    return ( numOfGens == 0 && relators.cardinality() == 0 );
  }
  // True if the entered presentation has form < ; >.


  bool numOfRelsLessNumOfGens( ) { 
    return ( relators.cardinality() < numOfGens ); 
  }
  // True if the number of relators is less than the number of generators.

  bool existsGenWithExpSumZeroInEveryRelator( Generator& );
  // True if there's such generator exponent sum of which is equal to
  // zero in every relator. The generator'll be return as well.

  VectorOf<int> getExpSumOfGen( const Generator& g);
  // Return exponent sums of g in the group relators.

  int GCDOfExpSumOfGen( const Generator& g );
  // Return the GCD of exponent sums of g in the group relators.


private:
 
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPGroup theGroup;     // the group to work with
  int numOfGens;        // number of generators of the group
  SetOf<Word> relators; // group relators 
};


#endif


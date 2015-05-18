// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of class ORProblems
//
// Principal Author: Dmitry Bormotov
//
// Status: provisional implementation
//
// Usage: 
//
// Special Notes:
//
//   All this functions should be in the class OneRelatorGroup, 
//   but we still don't have it. 
//
// Revision History:
//

#ifndef _OR_PROBLEMS_H_
#define _OR_PROBLEMS_H_

#include "FPGroup.h"


// ---------------------------- ORProblems --------------------------------- //


class ORProblems {

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ORProblems( int numOfGens, const Word& relator ) 
    : numberOfGenerators( numOfGens ), 
      theRelator( relator )
  { }
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool isTrivial( ) const;

  bool isAbelian( ) const;

  bool isFinite( ) const;

  bool isFree( ) const;

  int order( ) const;

  
private:
 
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int numberOfGenerators;

  Word theRelator;

};


inline bool ORProblems::isFinite( ) const
{
  return numberOfGenerators == 1;
}


inline int ORProblems::order( ) const
{
  if( isFinite() )
    return theRelator.length();
  else
    return 0;
}

#endif


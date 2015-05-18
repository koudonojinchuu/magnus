// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class ORProblems
//
// Principal Authors: Dmitry Bormotov
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


#include "ORProblems.h"
#include "OneRelatorGroup.h"
#include "GeneralWhitehead.h"

// ---------------------------- ORProblems --------------------------------- //


bool ORProblems::isTrivial( ) const
{
  int relLen = theRelator.length();

  if( numberOfGenerators == 0 && relLen == 0 )
    return true;
  
  if( numberOfGenerators != 1 || relLen != 1 )
    return false;
  
  return true;
}


bool ORProblems::isAbelian( ) const
{
  if( numberOfGenerators <= 1 )
    return true;
  if( numberOfGenerators > 2 )
    return false;

  // In case of 2 generators a and b, the group is abelian iff [a,b] = 1.

  VectorOf<Generator> v(4);
  v[0] = Generator(-1);
  v[1] = Generator(-2);
  v[2] = Generator( 1);
  v[3] = Generator( 2);

  // The current implementation of the word problem should work fast
  // in this case.
  
  OneRelatorGroup G( numberOfGenerators, theRelator );
  return G.wordProblem( Word(v) );
 
  /*@db old implementation

  ORWordProblem orwp( theRelator );
  orwp.findAnswer( Word(v) );
  if ( orwp.goodStatus() ) {
    return orwp.isTrivial();
  }
  else 
    error( "bool ORProblems::isAbelian( ) const : "
	   "ORWP has crashed.");
  */
}


bool ORProblems::isFree( ) const
{
  if( numberOfGenerators == 0 || theRelator.length() == 0 )
    return true;
  GeneralWhitehead GW( numberOfGenerators );
  GW.startComputation(theRelator);
  while( !GW.continueComputation() );
  return GW.extendsToFreeBasis();
}



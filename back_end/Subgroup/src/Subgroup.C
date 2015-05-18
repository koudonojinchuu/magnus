// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of SubgroupRep and Subgroup classes.
//
// Principal Authors: Stephane Collart, Roger Needham
//
// Status: Draft
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//

#include "Subgroup.h"
#include "SGofFreeGroup.h"
#include "CosetEnumerator.h"

Subgroup& Subgroup::addGenerator( const Word& w ) {
  change()->theGenerators.append(w);
  enhance()->theOrder = -1;
  // @stc not yet correct: the order does not necessarily become unknown
  return *this;
}


Subgroup& Subgroup::deleteGenerator( const Word& w ) {
  VectorOf<Word> theGenerators = look()->theGenerators;
  int i = theGenerators.indexOf(w);
  if ( i == -1 )
	 error("attempt to delete nonexistent word in Subgroup::deleteGenerator");
  else {
	 int len = theGenerators.length();
	 VectorOf<Word> temp(len - 1);
	 int k = 0;
	 for( int j = 0; j < len; ++j )
		if ( j != i ) temp[k++] = theGenerators[j];
	 change()->theGenerators = temp;
	 
	 enhance()->theOrder = -1;
	 // @stc not yet correct: the order does not necessarily become unknown
	 return *this;
  }
}



SubgroupRep::SubgroupRep( const FGGroup& parent, const SetOf<Word>& gens )
: FGGroupRep( gens.cardinality() ),
  theParentGroup( parent ),
  theGenerators(gens.cardinality())
{
  int i = 0;
  SetIterator<Word> I(gens);
  while( !I.done() ) {
	 theGenerators[i++] = I.value();
	 I.next();
  }
}



Elt SubgroupRep::eval( const Word& w ) const {
  #if SAFETY > 0
    if ( w.maxOccurringGenerator() > theNumberOfGenerators )
	   error("SubgroupRep::eval: attempt to evaluate word with no "
				"interpretation in parent group");
  #endif

  return w.replaceGenerators(theGenerators);
}


Trichotomy SubgroupRep::wordProblem( const Word& w ) const
{
  Word temp = w.replaceGenerators(theGenerators);
  return theParentGroup.wordProblem(temp);
  //@rn Without the temp, g++ 2.6.0 does not call the dtor!
}


Trichotomy SubgroupRep::conjugacyProblem( const Word& u, const Word& v ) const
{
/*
  return theParentGroup.conjugacyProblem(u.replaceGenerators(theGenerators),
													  v.replaceGenerators(theGenerators));
*/
  return DONTKNOW;
}


Word SubgroupRep::findRelator( )
{

  //@rn Obviously this is totally bogus
  
  int numGens = theGenerators.length();
  do {
	 do {
		//@rn
		Word temp = lastWordTried.nextCyclicallyReduced(numGens);
		lastWordTried = temp;

//@rn		lastWordTried = lastWordTried.nextCyclicallyReduced(numGens);
//@rn		cerr << "trying " << lastWordTried << endl;
	 } while ( wordProblem(lastWordTried) != YES );
  } while ( redundantRelator(lastWordTried) );
  
  relators |= lastWordTried;
  return lastWordTried;
}


Bool SubgroupRep::redundantRelator(const Word& u) {
  if ( relators.contains(u) ) return 1;
  Word v = u;
  for( int i = u.length() - 1; i; i--) {
	 v = v.cyclicallyPermute(1);
    if ( relators.contains(v) ) return TRUE;
  }

  v = u.inverse();
  if ( relators.contains(v) ) return 1;
  for( int i = u.length() - 1; i; i--) {
	 v = v.cyclicallyPermute(1);
    if ( relators.contains(v) ) return TRUE;
  }

  relators |= u;
  return FALSE;
}


//-------------------------- SubgroupRep ---------------------------//
//------------------------- static members -------------------------//

const Type SubgroupRep::theSubgroupType = Type( Type::unique() );








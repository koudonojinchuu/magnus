// Copyright (C) 1994-1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the MSCGroup class.
//
// Principal Author: Dmitry Bormotov, Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
// Special Notes:
//
// Discussion:
//
//  * Now we are using a metric condition C'(1/lambda) with lambda>5 only.
//    If we include a non-metric T-property then we will be able to solve
//    a wordProblem for C'(1/4)&T(4) and conjugacyProblem for C'(1/4)&T(4)
//    representation also.

#include <assert.h>
#include "MSCGroup.h"


//-------------------------------- MSCGroup --------------------------------//


int MSCGroup::order() const 
// A MSC group has a finite order iff the one is a finite cyclic group.
// The group satisfies this condition if have one relator of the
// form X^N=1. All other relators must have the form Y=1 or Y^-1=1 and
// number of ones is equal of numberOfGenerators-1 (so rank of free 
// subgroup is equal 0). 
//
// Scheme of proof:
//
//   Greendlinger, "On Dehn's Algorithms for the Conjugacy and Word
//   Problems, With Applications", Communications on Pure and
//   Applied Methematics, vol.XIII, pp.641-677 (1960) :
//
//   "Theorem VII. Let G be a group as defined in section 2, but having
//   at least two generators, and no defining relators of length one.
//   Then G is non-periodic."
//
//   Class of groups defined in section 2 contained class groups with
//   C'(1/6) condition.
//
// Note: The method gets the complete solution for C'(1/6) groups only.
//       It checks some degenarated cases of an FPgroup also.
{
  // Check degenerated cases.
  if( numberOfGenerators == 0 ) {
    // is the group a trivial one?
    return 1;
  }
  else if( symmetricRelators->cardinality() == 0 ) {
    // 1+ generators, no relators.
    return 0; // infinity
  }
    
  if( 1 <= mscLambda && mscLambda < 6 )	
    return -1;

  int numberOfCyclicSubgroups = 0;
  int orderOfCyclicSubgroup = 1;
  Generator genOfCyclicSubgroup;
  Word relator;
  const VectorOf<Word>& v = symmetricRelators->getRelators();
  int vLen = v.length();
  
  // A MSC group has a finite order iff the one is a finite cyclic group.
  // The group satisfies this condition if have one relator of the
  // form X^N=1. All other relators must have the form Y=1 or Y^-1=1 and
  // number of ones is equal of numberOfGenerators-1 (so rank of free 
  // subgroup is equal 0)
  
  for ( int i = 0; i < vLen; i++ ) {
    relator = v[i];
    if( relator.length() > 1 ) {
      Generator gen = relator[0];
      
      // Does the relator generate a finite cyclic subgroup ?
      
      if( relator.exponentSum(gen) == relator.length() ) {
	if( numberOfCyclicSubgroups == 0 ) {
	  ++numberOfCyclicSubgroups;
	  genOfCyclicSubgroup = gen;
	  orderOfCyclicSubgroup = relator.length();
	}
	else if( abs(ord(gen)) == abs(ord(genOfCyclicSubgroup)) ) {
	  assert( relator.length() == orderOfCyclicSubgroup );
	} 
	else 
	  return 0; // infinity
	
      }
      else
	return 0; // infinity
    }
  }
  
  // The relators have a simply form: one X^N=1 or X^-N=1 and some Y^1=1,
  // Y^-1=1. Determine the rank of the free subgroup.
  
  int rankOfFreeSubgroup = numberOfGenerators-symmetricRelators->cardinality();
  if( rankOfFreeSubgroup == 0 )
    return orderOfCyclicSubgroup; // finite order
  
  return 0;
}


Trichotomy MSCGroup::isTrivial() const {
  int ord = order();
  if( ord == 1 )  return YES;
  if( ord == INFINITE || ord > 1 )  return NO;

  // order() returned `DONTKNOW' because the group is not C'(1/6)
  return DONTKNOW;
}

Trichotomy MSCGroup::isFinite() const {
  int ord = order();
  if( ord > 0 )  return YES;
  if( ord == INFINITE )  return NO;
  
  // order() returned `DONTKNOW' because the group is not C'(1/6)
  return DONTKNOW;
}

Trichotomy MSCGroup::isInfinite() const {
  int ord = order();
  if( ord == INFINITE )  return YES;
  if( ord > 0 )  return NO;
  
  // order() returned `DONTKNOW' because the group is not C'(1/6)
  return DONTKNOW;
}

Trichotomy MSCGroup::isAbelian() const
// A MSC group is abelian iff the one is a cyclic group.
// A represenation of a group with C'(1/6) condition defines an abelian group
// iff the group is a cyclic group:
//    < x | x^n=1 > - of finite order  or
//    < x > - of infinite order
// We are rewriting the relators set by Tietze's transformations.
//
// Note: The method determines the complete solution for C'(1/6) groups only.
//       It checks some degenarated cases of an FPgroup also.
{
  // Check degenerated cases of FP groups.
  if ( numberOfGenerators == 0 || // is the group a trivial one?
       numberOfGenerators == 1 && symmetricRelators->cardinality() == 0 )
    return YES; // free group of rank 1.
  
  if ( 1 <= mscLambda && mscLambda < 6 )	
    return DONTKNOW;
  
  int numberOfCyclicSubgroups = 0;
  bool isFiniteOrderOfCyclicSubgroup = false; 
  Generator genOfCyclicSubgroup;
  Word relator;
  const VectorOf<Word>& v = symmetricRelators->getRelators();
  int vLen = v.length();
  
  // The set mustn't contain relators of length 3 and longer
  // if the group is abelian. When we remove all relators
  // of the forms X=1 or X^-1=1 the set must contain only one
  // relator of the form X^n=1 or XY=1 (include its symmetrised forms)
  
  for ( int i = 0; i < vLen; i++ ) {
    relator = v[i];
    if ( relator.length() > 1 ) {
      
      // check whether this relator defines a cyclic subgroup
      
      Generator gen = relator[0];
      if( relator.exponentSum(gen) == relator.length() ) {
	
	// relator X^n=1 defines a finite cyclic subgroup.
	// if second cyclic subgroup was found
	// then the group is not abelian.
	
	if( numberOfCyclicSubgroups == 0 ) {
	  isFiniteOrderOfCyclicSubgroup = true;
	  ++numberOfCyclicSubgroups;
	  genOfCyclicSubgroup = gen;
	}
	else
	  return NO;
      }
      else if( relator.length() == 2 ) {

	// a relator of the form XY=1. It defines
	// an infinite cyclic subgroup. Generators of
	// the relator can't be found in other relators
	// (exclude symmetrised forms of this relator).

	if( numberOfCyclicSubgroups == 0 ) {
	  isFiniteOrderOfCyclicSubgroup = false;
	  ++numberOfCyclicSubgroups;
	  genOfCyclicSubgroup = gen;
	}
	else
	  return NO;

	// if we have found second two-generators 
	// relator the group is not an abelian.
      }
      else
	return NO;
      // a long relator (length>2 and not a power of a generator)
      // was found. The group is not abelian.
    }
  }
  
  // The representation has one relator of the form XY=1 or X^N=1 at most.
  // and some relators of form Z=1, where X,Y,Z are generators.
  // Determines a rank of free subgroup now.
  
  int rankOfFreeSubgroup = numberOfGenerators - 
    symmetricRelators->cardinality();
  
  if( numberOfCyclicSubgroups == 0 &&         // infinite cyclic group or trivial one
      rankOfFreeSubgroup <= 1  || numberOfCyclicSubgroups == 1 &&
      (!isFiniteOrderOfCyclicSubgroup && rankOfFreeSubgroup == 1 || // infinite cyclic group
       isFiniteOrderOfCyclicSubgroup  && rankOfFreeSubgroup == 0) )    // finite cyclic group )
    return YES;
  
  return NO;
}

Trichotomy MSCGroup::isFree() const
// provisional implementation
{
  int ord = order();
  if( ord > 1 ) return NO;   // a finite cyclic group is not free.
  if( ord == 1 ) return YES; // a trivial group is free.
  if( ord == INFINITE && isAbelian() == YES ) //an infinite abelian 
    return YES;            // FP group is a free group of rank 1.
  return DONTKNOW;
}


// Methods dealing with group elements:

int compareWordsByLength(const void* ptr1, const void* ptr2);
// forward declaration, this function is called by shortenByRelators only.

Word MSCGroup::shortenByRelators(const Word& w) const {
  return shortenByRels->getShortenWord(w);
}
  

Elt MSCGroup::eval( const Word& w ) const {
  return shortenByRelators(w);
}

Trichotomy MSCGroup::wordProblem( const Word& w ) const {

  Word u = shortenByRelators(w);
  if ( u.length() == 0 ) return YES;
  if ( mscLambda >= 6 || mscLambda == 0 )
    return NO;
  return DONTKNOW;
}

/////////////  cyclicallyShortenByRelators  ///////////////////////////
//
// This method shortens its argument w by relators of the group.
// The word w is interpreted as the cyclic word.
// If word argument w contains a piece of some relator of group, and the
// rest of relator is smaller than this piece, procedure shortens w.
//
////////////////////////////////////////////////////////////////////////
/*
Word MSCGroup::cyclicallyShortenByRelators(const Word& w) const {
  
  Word W = w.cyclicallyReduce();
  if( W.length() == 0 ) return W;
  
  bool wasShortened;  // whether W was shortened after current iteration
  SymmetricRelatorsIterator I(*symmetricRelators);
  do
    { // Here we try to reduce W. If we can do it, we repeat the cycle.
      // Otherwise, leave it.
      
      wasShortened = false;
      int wLen = W.length();
      
      for (int shift = 0; shift < wLen && !wasShortened; shift++) {
	Word wRotated = W.cyclicallyPermute(shift);
	
	for(I.reset(); !I.done() && !wasShortened; I.next()) {
	  Word relator = I.value();			// current relator
	  int relatorLen = relator.length();	// and it length
	  if (relatorLen >= 2*wLen) continue;
	  int pieceLen = wRotated.agreementLength(relator);
	  if( 2*pieceLen <= relatorLen ) continue;
	  // common piece is too small, take the next relator
	  
	  Word tail = relator.terminalSegment(relatorLen - pieceLen).inverse();
	  // we can shorten the word if common segment will be
	  // replaced with inverted relator tail
	  W = wRotated.replaceSubword(0, pieceLen, tail);
	  if( W.length()>0 && Generator(W[0]) == inv(W[W.length()-1]) )
	    W = W.cyclicallyReduce();
	  wasShortened = true;
	}
      }
    } while( wasShortened && W.length() > 0 );
	return W;
}
*/


Word MSCGroup::cyclicallyShortenByRelators(const Word& w) const
{
  Word W = w.cyclicallyReduce();
  Word conjugator;
  return cyclicallyShortenByRelators(W, conjugator);
}

    
Word MSCGroup::cyclicallyShortenByRelators( const Word& w,
					    Word& conjugator) const
{
  Word W = w.freelyReduce();
  Word t = W.cyclicallyReduce();
  conjugator = W.initialSegment( (W.length() - t.length()) / 2 );
  W = t;
    
  if( W.length() == 0 ) return W;
  
  bool wasShortened;  // whether W was shortened after current iteration
  SymmetricRelatorsIterator I(*symmetricRelators);
  do
    { // Here we try to reduce W. If we can do it, we repeat the cycle.
      // Otherwise, leave it.
      
      wasShortened = false;
      int wLen = W.length();
      
      for (int shift = 0; shift < wLen && !wasShortened; shift++) {
	Word wRotated = W.cyclicallyPermute(shift);
	
	for(I.reset(); !I.done() && !wasShortened; I.next()) {
	  Word relator = I.value();			// current relator
	  int relatorLen = relator.length();	// and it length
	  if (relatorLen >= 2*wLen) continue;
	  int pieceLen = wRotated.agreementLength(relator);
	  if( 2*pieceLen <= relatorLen ) continue;
	  // common piece is too small, take the next relator
	  
	  Word tail = relator.terminalSegment(relatorLen - pieceLen).inverse();
	  // we can shorten the word if common segment will be
	  // replaced with inverted relator tail
	  
	  conjugator *= W.initialSegment(shift);
	  W = (wRotated.replaceSubword(0, pieceLen, tail)).freelyReduce();
	  if( W.length()>0 && Generator(W[0]) == inv(W[W.length()-1]) ) {
	    Word t = W.cyclicallyReduce();
	    conjugator *= W.initialSegment( (W.length() - t.length()) / 2 );
	    W = t;
	  }
	  wasShortened = true;
	}
      }
    } while( wasShortened && W.length() > 0 );
	return W;
}


void MSCGroup::calculateLambda()
{
  int lambda = 0;
  SymmetricRelatorsIterator I(*symmetricRelators);
  while( !I.done() ) {
    Word firstWord = I.value();
    SymmetricRelatorsIterator J = I;
    J.next();
 
    // compare firstWord with rest of the ss set.
    
    while( !J.done() ) {
      Word secondWord = J.value();
      int pieceLen = firstWord.agreementLength(secondWord);
      if ( pieceLen != 0 ) {
	int minLen = min(firstWord.length(), secondWord.length());
	if( pieceLen == minLen ) {
	  mscLambda = 1;
	  return;
	}
	if( lambda == 0 || minLen <= lambda * pieceLen ) {
	  lambda = (minLen - 1) / pieceLen;

	  // @stc the 0 test on lambda makes the value lambda == 0
	  // equivalent to infinity; the test slows the loop
	  // marginally, but guarantees semantic correctness;

	  if( lambda == 1 ) {
	    mscLambda = lambda;
	    return;
	  }
	}
      }
      J.next();
    }
    I.next();
  }
  mscLambda = lambda;
}


void MSCGroup::makeMSCGroup(int ngens, const SetOf<Word>& rels, int lambda) {

  numberOfGenerators = ngens; 
  symmetricRelators = new SymmetricRelators(rels);
  shortenByRels = new ShortenByRelators(*symmetricRelators);
  if( lambda == -1 ) { 
    calculateLambda();
    return;
  }

#if SAFETY > 1
  // checks whether given Lambda is equal to real Lambda.
  calculateLambda();
  
  if( lambda < -1 || mscLambda != 0 && ( lambda == 0 || lambda > mscLambda) )
    error("Called MSCGroup(int,const SetOf<Word>&,int) with"
	  " bad lambda value");
  
  if( lambda != mscLambda ) 
    warn("Called MSCGroup(int,const SetOf<Word>&,int) with "
	 "rough lambda value, exact one is used");
  return;
#endif

  mscLambda = lambda; 
}

// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the MSCGConjugacyProblem class
//           This is a solving of conjugacy problem for metric 
//           small cancellation groups with C'(1/6) condition.
//
// Principal Authors: Dmitry Bormotov, Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//


#include "MSCGConjugacyProblem.h"


// -------------------------- conjugacy problem --------------------------- //


// These methods determine whether two words u,v are conjugated
// in an MSC group with C'(1/6). It is true iff
//        u~ = ( ( v~ )^y )^x                                (I)
// where u~,v~ are some cyclic permutations of cyclically reduced forms of u,v
//       x, y  are some pieces of some two relators r1, r2 with
//             C'(1/6) condition:
//                max{ |r1|, |r2| } < 6 * max{ |u|, |v| },
//                   |w| is the length of a word w
//
// For groups C'(1/lambda) with lambda > 7 it's enough to check
// only one piece:
//           u~ = ( v~ )^x                      (II)


void MSCGConjugacyProblem::startComputation( ) {

#if SAFETY > 0
  if ( startStatus )
    error("the conjugacy problem has been already started");
#endif
  startStatus = true;
  doneStatus = false;
  areConjugate = DONTKNOW;
  theConjugator = Word();
/*
  Word t = U.cyclicallyReduce();
  UConjugator = U.terminalSegment( (U.length() - t.length()) / 2 );
  U = t;
  
  t = V.cyclicallyReduce();
  VConjugator = V.terminalSegment( (V.length() - t.length()) / 2 );
  V = t;
*/
  // take R-shortened forms for U, V
  U = MSCG.cyclicallyShortenByRelators(U, UConjugator);
  V = MSCG.cyclicallyShortenByRelators(V, VConjugator);

  ULen = U.length();
  VLen = V.length();

  // get quick answer for conjugacy problem if it's possible
  
  if( U == V ) {
    setCPResult(YES);
    return;
  }
  else if( U.length() == 0 || V.length() == 0 ) {
    setCPResult(NO);
    return;
  }
  
  SetOf<Word> cycU = SetOf<Word>(U);
  closeUnderCyclicPermutations(cycU);
    
  // quick test: whether U is a cyclic permutation of the word V.
  // (conjugacy problem in free groups)
  
  if( cycU.contains(V) ) {
    for ( int i = 0; i < ULen; i++ )
      if ( V == U.cyclicallyPermute(i) )
	theConjugator = U.initialSegment(i);
    setCPResult(YES);
    return;
  }
  
  // @dp This check works properly on all FP groups ?
  //     If it is true then code above must be moved in
  //     FPGroupRep::conjugacyProblem()
  
  cycV = SetOf<Word>(V);
  closeUnderCyclicPermutations(cycV);
    
  // if we use T-property then we should include this code:
  //    int maxLen = max(U.length(), V.length()) * (lambda<6? 8 : 6);
  // now with C'(1/6):

  int maxLen = 6 * max(U.length(), V.length());
  
  // make set of relators which have a length length lesser than 6*max{|u~|,|v~|}.

  SetOf<Word> relators;
  const VectorOf<Word>& v = MSCG.symmetricRelators->getRelators();
  int vLen = v.length();
  for ( int i = 0; i < vLen; i++ )
    if ( v[i].length() < maxLen) relators |= v[i];
  shortRelators = new SymmetricRelators(relators);
  shortIter = new SymmetricRelatorsIterator(*shortRelators);
  shortIter2 = new SymmetricRelatorsIterator(*shortRelators);
  
  state = CYCLE_BY_RELATORS;
  firstPart = true; 
}


bool MSCGConjugacyProblem::continueComputation()
{

#if SAFETY > 0
  if ( !startStatus )
    error("tried to continue conjugacy problem before it's started");
#endif

  if ( doneStatus )
    return doneStatus;
  
  if ( firstPart ) {

    // Checks the condition (II). For C'(1/8) representation
    // it is complete algorithm of solving conjugacy problem.

    if ( state == CYCLE_BY_RELATORS ) {
      if ( !shortIter->done() ) {
	relator = shortIter->value();
	relatorLen = relator.length();
	state = CYCLE_BY_PIECES;
	pieceLen = 1;
      }
      else {
	int lambda = MSCG.mscLambda;
	if( lambda >= 8 || lambda == 0 ) {
	  finishCP(NO);
	  return doneStatus;
	}
	firstPart = false;
      }
    }
    if ( state == CYCLE_BY_PIECES ) {
      if ( pieceLen <= relatorLen ) {
	Word piece = relator.initialSegment(pieceLen);
	for ( int i = 0; i < ULen; i++ ) {
	  Word cpU = U.cyclicallyPermute(i);
		  
	  // get a conjugacy of some cyclic permutation of U
	  // by a piece of a relator
	  
	  Word conjElt = MSCG.shortenByRelators(Word(cpU.conjugateBy(piece)));
	  
	  if( conjElt.length() == VLen && // cheap check
	      cycV.contains(conjElt) ) { // calculate the conjugator

	    cout << "cpU:" << cpU << endl;
	    cout << "Piece:" << piece << endl;
	    cout << "conjElt:" << conjElt << endl;

	    for ( int j = 0; j < VLen; j++ )  
	      if ( conjElt == V.cyclicallyPermute(j) ) {
		theConjugator = ( U.initialSegment(i) * piece *
		  V.terminalSegment(VLen-j) ).freelyReduce();
		break;
	      }
	    finishCP(YES);
	    return doneStatus;
	  }
	}
	pieceLen++;
      }
      else {
	state = CYCLE_BY_RELATORS;
	shortIter->next();
      }
    }
  }
  else {
    
    // try to find v~, u~, x, y for condition (I)

    if ( state == CYCLE_BY_RELATORS2 ) {
      
      if ( !shortIter2->done() ) {
	relatorOne = shortIter2->value();
	relatorOneLen = relatorOne.length();
	pieceOneLen = 1;
	state = CYCLE_BY_PIECES;
      }
      else {
	
	// there is the solution (negative answer) of conjugacy problem
	// for MSC groups with C'(1/6) condition only

	int lambda = MSCG.mscLambda;
	if( lambda >= 6 || lambda == 0 )
	  finishCP(NO);
	else finishCP(DONTKNOW);
	return doneStatus;
      }
    }
    
    if ( state == CYCLE_BY_PIECES ) { 
      if ( pieceOneLen <= relatorOneLen ) {
      //for(int pieceOneLen = 1; pieceOneLen <= relatorOneLen; pieceOneLen++) {
	pieceOne = relatorOne.initialSegment(pieceOneLen);
	shortIter->reset();
	state = CYCLE_BY_RELATORS;
      }
      else {
	state = CYCLE_BY_RELATORS2;
	shortIter2->next();
      }
    }

    if ( state = CYCLE_BY_RELATORS ) {
      if ( !shortIter->done() ) {
      	relatorTwo = shortIter->value();
	relatorTwoLen = relatorTwo.length();
	for(int pieceTwoLen = 1; pieceTwoLen <= relatorTwoLen; pieceTwoLen++) {
	  Word pieceTwo = relatorTwo.initialSegment(pieceTwoLen);
	  Word piece = (pieceOne * pieceTwo).freelyReduce();
	  for ( int i = 0; i < ULen; i++ ) {
	    Word cpU = U.cyclicallyPermute(i);
	    Word conjElt = MSCG.shortenByRelators(Word(cpU.conjugateBy(piece)));
	    if( conjElt.length() == V.length() && // cheap check
		cycV.contains(conjElt) ) { // calculate the conjugator
	      for ( int j = 0; j < VLen; j++ ) 
		if ( conjElt == V.cyclicallyPermute(j) ) {
		  theConjugator = ( U.initialSegment(i) * piece *
		    V.terminalSegment(j) ).freelyReduce();
		  break;
		}
	      finishCP(YES);
	      return doneStatus;
	    } 
	  }
	}
	shortIter->next();
      }
      else {
	state = CYCLE_BY_PIECES;
	pieceOneLen++;
      }
    }
  }

  return doneStatus;
}


void MSCGConjugacyProblem::setCPResult(Trichotomy result) {
  areConjugate = result;
  doneStatus = true;
//  startStatus = false;

  theConjugator = MSCG.shortenByRelators(
      UConjugator * theConjugator * VConjugator.inverse() );
}


void MSCGConjugacyProblem::finishCP(Trichotomy result) {
  delete shortIter;
  delete shortIter2;
  delete shortRelators;
  setCPResult(result);
}




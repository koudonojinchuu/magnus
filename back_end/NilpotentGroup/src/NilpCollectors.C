// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes CollectorToTheLeft
//
//
// Principal Author: Eugene Paderin
//
// Status: In progress
//
// Revision History:
//
// Special Notes:
//


#include "NilpCollectors.h"
#include "PolyWordIterator.h"


//#define COL_DEBUG

//==========================================================================
//=================== class CollectorToTheLeft =============================
//==========================================================================

void CollectorToTheLeft::collectingProcess(PolyWord& pw) const {
  for(int c = 1; c <= BC.theHirschNumber(); c++)
    collectLetter(pw, c);
}


//==========================================================================
//=================== class CollectorFromTheLeft =============================
//==========================================================================

void CollectorFromTheLeft::collectingProcess(PolyWord& w) const {

  //cout << "This is a collecting process." << endl;

  if(w.numberOfLetters() < 2) return;

  int unseen_length = w.numberOfLetters();

  PolyWordIterator iter(w);
  
  iter.startFromLeft();

  while( ! iter.done() ) {

    // The part of the word left to the current letter is always collected.
    // We must find out what to do with the current letter.


#ifdef COL_DEBUG
    int right_length = w.numberOfLetters() - iter.position();
    if(right_length < unseen_length) {
      unseen_length = right_length;
      cout << "Reaching letter " << iter.thisLetter() << endl;
    }
#endif

    if( w.numberOfLetters() < 2 ) break;

    if( iter.isFirst() ) {
      iter.stepRight();
      continue;
    }

    Letter leftLetter = iter.leftLetter();


    // if the pair (leftLetter, iter.thisLetter) is ordered, move right

    if( leftLetter.gen < iter.thisLetter().gen ) {
      iter.stepRight();
    }
    else {
      // otherwise, commute them
      
      PolyWord commuted = BC.commuteLetters(leftLetter, iter.thisLetter());

      if( ! commuted.isEmpty() ) {

	if( commuted.numberOfLetters() > 3 ||
	    commuted.firstLetter() != iter.thisLetter() ) {

	  // we have to collect one letter
	  
	  for(int c = ord(iter.thisLetter().gen); c <= BC.theHirschNumber(); c++)
	    NilpotentCollector::collectLetter(commuted, c);
	}
      }

      iter.stepLeft();
      iter.removeRightLetter();
      iter.insertRight(commuted);
      iter.removeThisLetter();
    }
  }
}

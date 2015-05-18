// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the ShortenByRelators class.
//
// Principal Author: Dmitry Bormotov, Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
// Special Notes:
//
//


#include "ShortenByRelators.h"


// ------------------------- Forward declarations ------------------------- //


int compareWordsByLength(const void* ptr1, const void* ptr2);
// this function is called by constructor only.


//------------------------ class ShortenByRelators -------------------------//


ShortenByRelators::ShortenByRelators(const SymmetricRelators& symmetricRelators) {
  
  sortRelatorsLen = 0;
  if ( symmetricRelators.cardinality() != 0 ) {
    // converts relators: Set -> array
    SymmetricRelatorsIterator I(symmetricRelators);
    for ( I.reset(); !I.done(); I.next() ) sortRelatorsLen++;
    
    sortRelators = new Word[sortRelatorsLen];
    I.reset();
    for ( int k = 0; !I.done(); k++, I.next() )
      sortRelators[k] = I.value();
    
    // When we are shorting the word `result', only relators of
    // length < 2*|result| are required. If we sorted the vector of
    // relators by their lengths, we can easy reduce the set of
    // relators which may be used to shorten the word `result'.
    
    qsort(sortRelators, sortRelatorsLen, sizeof(Word), compareWordsByLength);
  }
}


ShortenByRelators::~ShortenByRelators() {
  delete[] sortRelators;
}  


Word ShortenByRelators::getShortenWord(const Word& w) const {

  Word result = w.freelyReduce();
  if( result.length() == 0 || sortRelatorsLen == 0 ) return result;

  int rightBound = sortRelatorsLen;
  bool resultWasShortened = true;
  while ( resultWasShortened  &&  result.length() > 0  &&  rightBound > 0 ) {
    resultWasShortened = false;
    
    // shrinks the set of acceptable relators which length is not greater
    // than twice length of the word `result'

    int resultLen = result.length();
    while ( rightBound > 0  &&  sortRelators[rightBound-1].length() > 2 * resultLen )
      --rightBound;
    
    // Walk through set of relators and try to shorten `result'.
    for(int relNumber = 0; relNumber<rightBound && result.length()>0 &&
	!resultWasShortened; relNumber++) {
      const Word& relator = sortRelators[relNumber];
      int oldLen = result.length();
      result = result.shortenByRelator(relator);
      if( result.length() != oldLen ) {
	result = result.freelyReduce();
	resultWasShortened = true;
      }
    }
  }
  
  return result;
}


//--------------------Internal functions of ShortenByRelators-----------------//

int compareWordsByLength(const void* ptr1, const void* ptr2)
// Called from shortenByRelators::getShortenWord only
{
  Word* w1 = (Word*)ptr1;
  Word* w2 = (Word*)ptr2;
  int w1Len = w1->length();
  int w2Len = w2->length();
  
  if( w1Len > w2Len )
    return 1;
  else if( w1Len == w2Len )
    return 0;
  else return -1;
}









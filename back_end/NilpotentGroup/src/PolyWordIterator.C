// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of PolyWordIterator class.
//           
//
// Principal Author: Eugene Paderin
//
// Status: Experimental version
//
// Revision History:
//
// Special Notes:
//
//

#include "PolyWordIterator.h"

//--------- searchToLeft ----------------------------------------

// Starting from the current position and moving left
// searches a node containing g and makes it current.
// Returns true iff the node is found.

bool PolyWordIterator::searchToLeft( Generator g ) {
  g = abs(g);
  for( ; ! done(); stepLeft() )
    if( thisLetter().gen == g ) return true;
  return false;
}

//---------------- collectToRight --------------------------------

// Collects the current letter with its right neighbors. If resulting
// power is zero then the node is removed from chain and the iterator
// points to the left neighbor (if exists; otherwise, points to the 
// right one).
// Returns true iff collection was performed.
bool PolyWordIterator::collectToRight() {

  bool collected = false;

  // collect letters to right

  while( !isLast() ) {
    if( thisLetter().gen == rightLetter().gen) {
      thisLetter().power += rightLetter().power;
      removeRightLetter();
      collected = true;
    }
    else break;
  }

  // check whether the letter is empty

  if(thisLetter().power == 0 ) {
    removeThisLetter();
    collected = true;
    if(!isFirst() ) stepLeft();
  }
  return collected;
}

//---------------- splitToLeft ------------------------------

// "Splits" the current letter a^p into a * a^{p-1} if p>0, or
// a^-1 * a^{p+1} if p<0. Iterator points to the right part.
// If p is 1 or -1, nothing happens.

void PolyWordIterator::splitToLeft() {

  int p = thisLetter().power;
  if( p == 1 || p == -1 ) return;

  if (p > 0) {
    --thisLetter().power;
    insertLeft( Letter(thisLetter().gen, 1) );
  }
  else {
    ++thisLetter().power;
    insertLeft( Letter(thisLetter().gen, -1) );
  }	       
}

//------------------- insertRight ----------------------------

// Inserts w between the current letter and its right neighbor.
// The iterator is not moved.
void PolyWordIterator::insertRight(const PolyWord& w) {

  if(w.isEmpty() ) return;
  if(iter == NULL)
    error("Attempt to use PolyWordIterator when in done state");

  PolyWordNode *newFirst, *newLast;
  w.duplicate(newFirst, newLast);
  
  if( isLast() ) {
    PolyWordRep::bind(iter, newFirst);
    theRep.last = newLast;
  }
  else {
    PolyWordRep::bind( newLast, iter->next );
    PolyWordRep::bind(iter, newFirst);
  }

  theRep.theNumberOfNodes += w.numberOfLetters();
}

//------------------- insertRight -------------------------------

// Inserts a Letter between the current letter and its right neighbor.
// The iterator is not moved.
void PolyWordIterator::insertRight(const Letter& let) {

  if(iter == NULL)
    error("Attempt to use PolyWordIterator when in done state");

  PolyWordNode *newNode = new PolyWordNode(let);
  
  if( isLast() ) {
    PolyWordRep::bind(iter, newNode);
    theRep.last = newNode;
  }
  else {
    PolyWordRep::bind( newNode, iter->next );
    PolyWordRep::bind(iter, newNode);
  }

  ++theRep.theNumberOfNodes;
}

//------------------- insertLeft ----------------------------
  
// Inserts w between the current letter and its left neighbor.
// The iterator is not moved.

void PolyWordIterator::insertLeft(const PolyWord& w) {

  if(w.isEmpty() ) return;
  if(iter == NULL)
    error("Attempt to use PolyWordIterator when in done state");

  PolyWordNode *newFirst, *newLast;
  w.duplicate(newFirst, newLast);
  
  if( isFirst() ) {
    PolyWordRep::bind(newLast, iter);
    theRep.first = newFirst;
  }
  else {
    PolyWordRep::bind( iter->prev, newFirst );
    PolyWordRep::bind( newLast, iter);
  }

  theRep.theNumberOfNodes += w.numberOfLetters();
}

//------------------- insertLeft ---------------------------------

// Inserts a Letter between the current letter and its left neighbor.
// The iterator is not moved.
void PolyWordIterator::insertLeft(const Letter& let) {

  if(iter == NULL)
    error("Attempt to use PolyWordIterator when in done state");

  PolyWordNode *newNode = new PolyWordNode(let);
  
  if( isFirst() ) {
    PolyWordRep::bind(newNode, iter);
    theRep.first = newNode;
  }
  else {
    PolyWordRep::bind(iter->prev, newNode);
    PolyWordRep::bind(newNode, iter);
  }

  ++theRep.theNumberOfNodes;
}

//----------------- removeThisLetter -----------------------------

// removes the current letter. The iterator points to its right neighbor.

void PolyWordIterator::removeThisLetter() {

  if(iter == NULL)
    error("Attempt to remove NULL pointer while iterating PolyWord");

  PolyWordNode *nxt = iter->next;
  theRep.removeTheLetter(iter);
  iter = nxt;
  if(iter == NULL) iter = theRep.last;
}

//--------------- removeLeftLetter ---------------------------

// removes the left neighbor of the current letter.

void PolyWordIterator::removeLeftLetter() {

  if(iter == NULL)
    error("Attempt to remove NULL pointer while iterating PolyWord");

  theRep.removeTheLetter(iter->prev);
}

//------------------- removeRightLetter -----------------------

// removes the right neighbor of the current letter.

void PolyWordIterator::removeRightLetter() {

  if(iter == NULL)
    error("Attempt to remove NULL pointer while iterating PolyWord");

  theRep.removeTheLetter(iter->next);
}

//-------------------- exchangeToLeft ------------------------

// Exchanges the current letter and its left neighbor. The iterator
// points to the same letter as before (i.e. moves left together with 
// the letter).

void PolyWordIterator::exchangeToLeft() {
  Letter tmp = thisLetter();
  thisLetter() = leftLetter();
  stepLeft();
  thisLetter() = tmp;
}

  void decreaseLeftLetter();

//--------------- decreaseLeftLetter ---------------------------

// decreases the abs( leftLetter.power ) by 1.
// If the result is 0, removes the letter

void PolyWordIterator::decreaseLeftLetter() {

  if(iter == NULL || iter->prev == NULL)
    error("Attempt to access NULL pointer while iterating PolyWord");

  int& p = leftLetter().power;
  if( p > 0 )  --p;
  else if( p > 0 ) ++p;
  else error("Letter with power 0 found while iterating PolyWord");

  if( p == 0) removeLeftLetter();
}


//------------------ position --------------------------------------

// Returns the number of current letter, 1-based.
// 0 means done().
// This is a long running procedure, since the pointer travels to
// the beginning of the word.

int PolyWordIterator::position() const {

  if(iter == NULL) return 0;

  PolyWordNode *it = iter;

  int count = 1;
  
  for(  ; it != NULL; it = it->prev )
    ++count;

  return count;
}





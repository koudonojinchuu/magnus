
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of PolyWordIterator class.
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

#ifndef _POLYWORD_ITERATOR_H_
#define _POLYWORD_ITERATOR_H_

#include "PolyWord.h"

class PolyWordIterator {

public:

  //////////////////////////////////////////////////////////
  //
  //  Constructors
  //
  //////////////////////////////////////////////////////////
  
  PolyWordIterator(PolyWord& w) 
    : theRep( *w.change() ), iter(0) {}

  PolyWordIterator(PolyWordRep& w) 
    : theRep(w), iter(0) {}

  // copy constructor and destructor supplied by compiler

  //////////////////////////////////////////////////////////
  //
  //  Walking thru the word
  //
  //////////////////////////////////////////////////////////

  void startFromLeft() { iter = theRep.first; }
  // initialize iterator to point to the first letter

  void startFromRight() { iter = theRep.last; }
  // initialize iterator to point to the last letter

  void stepRight() { if(iter) iter = iter->next; }
  // move one step right

  void stepLeft() { if(iter) iter = iter->prev; }
  // move one step left

  bool done() const { return (iter == 0); }
  // True iff one of the ends was reached (collection is done)

  bool searchToLeft( Generator g );
  // Starting from the current position and moving left
  // searches a node containing g and makes it current.
  // Returns true iff the node is found.

  int position() const;
  // Returns the number of current letter, 1-based.
  // 0 means done().
  // This is a long running procedure, since the pointer travels to
  // the beginning of the word.

  ///////////////////////////////////////////////////////////
  //
  //  Fetching a Letter
  //
  ///////////////////////////////////////////////////////////

  // Error if a letter cannot be fetched

  Letter& thisLetter() const { 
    if( iter ) return iter->value;
    else error("Out-of-bound access at PolyWordIterator");
  }
  // The current letter

  Letter& leftLetter() const { 
    if( iter && iter->prev ) return iter->prev->value;
    else error("Out-of-bound access at PolyWordIterator");
  }
  // The letter left to the current one

  Letter& rightLetter() const { 
    if( iter && iter->next ) return iter->next->value;
    else error("Out-of-bound access at PolyWordIterator");
  }
  // The letter right to the current one

  bool isFirst() const { return iter == theRep.first; }
  // True iff the current letter is the first one

  bool isLast() const { return iter == theRep.last; }
  // True iff the current letter is the last one

  ///////////////////////////////////////////////////////////
  //
  //  PolyWord transformations
  //
  ///////////////////////////////////////////////////////////

  bool collectToRight();
  // collects the current letter with its right neighbors. If resulting
  // power is zero then the node is removed from chain and the iterator
  // points to the left neighbor (if exists; otherwise, points to the 
  // right one).
  // Returns true iff collection was performed.

  void splitToLeft();
  // "Splits" the current letter a^p into a * a^{p-1} if p>0, or
  // a^-1 * a^{p+1} if p<0. Iterator points to the right part.
  // If p is 1 or -1, nothing happens.

  void insertRight(const PolyWord& w);
  // Inserts w between the current letter and its right neighbor.
  // The iterator is not moved.

  void insertLeft(const PolyWord& w);
  // Inserts w between the current letter and its left neighbor.
  // The iterator is not moved.

  void insertRight(const Letter& let);
  // Inserts let between the current letter and its right neighbor.
  // The iterator is not moved.

  void insertLeft(const Letter& let);
  // Inserts let between the current letter and its left neighbor.
  // The iterator is not moved.

  void removeThisLetter();
  // removes the current letter. The iterator points to its right neighbor.

  void removeLeftLetter();
  // removes the left neighbor of the current letter.

  void removeRightLetter();
  // removes the right neighbor of the current letter.

  void decreaseLeftLetter();
  // decreases the abs( leftLetter.power ) by 1.
  // If the result is 0, removes the letter

  void exchangeToLeft();
  // Exchanges the current letter and its left neighbor. The iterator
  // points to the same letter as before.

private:

  PolyWordRep& theRep;
  PolyWordNode *iter;
};


class ConstPolyWordIterator : private PolyWordIterator {

public:

  //////////////////////////////////////////////////////////
  //
  //  Constructors
  //
  //////////////////////////////////////////////////////////
  
  ConstPolyWordIterator(const PolyWord& w) 
    : PolyWordIterator( (PolyWordRep&)*w.look() ) {}

  ConstPolyWordIterator(const PolyWordRep& w) 
    : PolyWordIterator( (PolyWordRep&)w ) {}

  // copy constructor and destructor supplied by compiler

  //////////////////////////////////////////////////////////
  //
  //  Walking thru the word
  //
  //////////////////////////////////////////////////////////

  PolyWordIterator::startFromLeft;
  // initialize iterator to point to the first letter

  PolyWordIterator::startFromRight;
  // initialize iterator to point to the last letter

  PolyWordIterator::stepRight;
  // move one step right

  PolyWordIterator::stepLeft;
  // move one step left

  PolyWordIterator::done;
  // True iff one of the ends was reached (collection is done)

  PolyWordIterator::searchToLeft;
  // Starting from the current position and moving left
  // searches a node containing g and makes it current.
  // Returns true iff the node is found.

  PolyWordIterator::position;
  // Returns the number of current letter, 1-based.
  // 0 means done().
  // This is a long procedure, since the pointer travels to
  // the beginning of the word.

  ///////////////////////////////////////////////////////////
  //
  //  Fetching a Letter
  //
  ///////////////////////////////////////////////////////////

  // Error if a letter cannot be fetched

  const Letter& thisLetter() const { 
    return PolyWordIterator::thisLetter(); 
  }
  // The current letter

  const Letter& leftLetter() const { 
    return PolyWordIterator::leftLetter(); 
  }
  // The letter left to the current one

  const Letter& rightLetter() const { 
    return PolyWordIterator::rightLetter(); 
  }
  // The letter right to the current one

  PolyWordIterator::isFirst;
  // True iff the current letter is the first one

  PolyWordIterator::isLast;
  // True iff the current letter is the last one

};

#endif


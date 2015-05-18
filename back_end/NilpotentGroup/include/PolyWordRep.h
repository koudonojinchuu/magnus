// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of PolyWordRep class.
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

#ifndef _POLY_WORD_REP_H_
#define _POLY_WORD_REP_H_

#include "Letter.h"
#include "PureRep.h"

class PolyWord;

//------------------ PolyWordNode ----------------------------

struct PolyWordNode {

  //------------------------------------------------------------
  // Constructors
  //------------------------------------------------------------

  PolyWordNode(const Letter& e, PolyWordNode *prv = NULL, PolyWordNode *nxt = NULL)
    : value(e), prev(prv), next(nxt) {}

  PolyWordNode(PolyWordNode *prv = NULL, PolyWordNode *nxt = NULL)
    : prev(prv), next(nxt) {}

  // copy constructor and destructor provided by compiler

  //------------------------------------------------------------
  //	Data
  //------------------------------------------------------------

  Letter value;
  PolyWordNode *prev;
  PolyWordNode *next;
};

//--------------- PolyWordRep ----------------------------------

class PolyWordRep : public PureRep {

  //--------------------------------------------------------
  //	Constructors
  //--------------------------------------------------------

public:
  
  PolyWordRep() : first(NULL), last(NULL), theNumberOfNodes(0) {}

  PolyWordRep( const Letter& let) 
    : first(new PolyWordNode(let) ), last(first), theNumberOfNodes(1) {}

  PolyWordRep( const Word& w);

  PolyWordRep(const class AbelianWord& aw);

  // copy constructor
  PolyWordRep(const PolyWordRep& src)
    : theNumberOfNodes(src.theNumberOfNodes) {
      src.duplicate(first, last);
    }

  // destructor
  ~PolyWordRep()  { clear(); }

  //--------------------------------------------------------
  //	Accessors to word properties
  //--------------------------------------------------------

  int length() const;
  // length of the word (sum of absolute values of powers).
  // The word considered to be not freely reduced. So, the length of
  // a^2 a^-1 is 3, not 1.

  int numberOfLetters() const { return theNumberOfNodes; }
  // number of Letters (length of the list)

  bool isCollected() const;
  // returns true iff the word is collected


  const Letter& firstLetter() const {
    if( theNumberOfNodes == 0)
      error("Attempt to access NULL pointer in PolyWordRep::firstLetter()");
    return first->value;
  }

  const Letter& lastLetter() const {
    if( theNumberOfNodes == 0)
      error("Attempt to access NULL pointer in PolyWordRep::lastLetter()");
    return last->value;
  }

  //--------------------------------------------------------
  //	I/O
  //--------------------------------------------------------

  void printOn (ostream& s) const;
  // output

  ostream& write(ostream& s) const;
  // IPC output

  istream& read(istream& s);
  // IPC input

  void debugInfo() const;
  // Prints debug info to cout

  //--------------------------------------------------------
  //	General methods
  //--------------------------------------------------------

  Word toWord() const;
  // cast to common Word (not reduced)

  Chars toChars( const VectorOf<Chars>& name ) const;
  // outputs word to string using given Chars as names of generators

  void freelyReduce();
  // freely reduces the word

  PolyWord inverse() const;
  // returns the inverse word

  void append(const PolyWord& w);
  // appends w to the end of this word

  void append(const Letter& w);
  // appends w to the end of this word

  void clear();
  // clears the list

  void removeFirstLetter() {
    removeTheLetter(first);
  }
  // removes the first letter      

  PolyWord raiseToPower( int power ) const;
  // Raises the word to given power (can be negative or 0)

  Chars inTermsOf(const VectorOf<Chars>& v) const;
  //Writes the word in readable form replacing generator g by string v[g-1].
  
  //-----------------------------------------------------
  //  For internal use
  //-----------------------------------------------------

  void duplicate(PolyWordNode*& ptrToFirst, PolyWordNode*& ptrToLast) const;
  // Duplicates the list and writes addresses of the new copy's
  // first and last nodes to ptrToFirst and ptrToLast.

  // binds two nodes together
  static inline void bind(PolyWordNode *leftNode, PolyWordNode *rightNode) {
    if(leftNode != NULL)  leftNode->next = rightNode;
    if(rightNode != NULL) rightNode->prev = leftNode;
  }

  // PolyWordIterator is a friend of PolyWord and PolyWordRep
  friend class PolyWordIterator;
  friend class ConstPolyWordIterator;

  virtual PureRep *clone() const {
    return new PolyWordRep(*this);
  }

  void removeTheLetter( PolyWordNode *theLetter);
  // removes the letter the argument points to  


private:

  PolyWordNode *first, *last;
  int theNumberOfNodes;

};



#endif


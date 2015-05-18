// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of PolyWord class.
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
//  * You can find set of methods being incomplete. For example, there is
//    firstLetter() method, but no lastLetter(). The reason is that I added
//    only those methods I needed.
//

#ifndef _POLY_WORD_H_
#define _POLY_WORD_H_

#include "Letter.h"
#include "PureRep.h"
#include "PolyWordRep.h"

class PolyWord : public ObjectOf<PolyWordRep> {

  //--------------------------------------------------------
  //	Constructors
  //--------------------------------------------------------

public:
  
  PolyWord() : ObjectOf<PolyWordRep>( new PolyWordRep() ) {}

  PolyWord( const Letter& let) : 
    ObjectOf<PolyWordRep>( new PolyWordRep(let) ) {}

  PolyWord( const Word& w) :
    ObjectOf<PolyWordRep>( new PolyWordRep(w) ) {}

  PolyWord( const AbelianWord& aw) :
    ObjectOf<PolyWordRep>( new PolyWordRep(aw) ) {}

  PolyWord(PolyWordRep* rep) :
    ObjectOf<PolyWordRep>(rep) { enhance()->addRef(); }     


  // copy constructor and destructor provided by compiler

  //--------------------------------------------------------
  //	Accessors to word properties
  //--------------------------------------------------------

  int length() const {
    return look()->length();
  }
  // length of the word (sum of absolute values of powers).

  int numberOfLetters() const { 
    return look()->numberOfLetters(); 
  }
  // number of Letters (length of the list)

  bool isEmpty() const { return numberOfLetters() == 0; }

  bool isCollected() const {
    look()->isCollected();
  }
  // returns true iff the word is collected

  //--------------------------------------------------------
  //	Accessors to word elements
  //--------------------------------------------------------

  const Letter& firstLetter() const {
    return look()->firstLetter();
  }
  // returns reference to the first letter

  const Letter& lastLetter() const {
    return look()->lastLetter();
  }
  // returns reference to the last letter

  // To access the other word elements, use PolyWordIterator
  friend class PolyWordIterator;
  friend class ConstPolyWordIterator;

  //--------------------------------------------------------
  //	I/O
  //--------------------------------------------------------

  void printOn (ostream& s) const {
    look()->printOn(s);
  }
  // output

  friend ostream& operator < (ostream& s, const PolyWord& w) {
    return w.look()->write(s);
  }
  // IPC output

  friend istream& operator > (istream& s, PolyWord& w) {
    return w.change()->read(s);
  }
  // IPC input
 
  void debugInfo() const {
    look()->debugInfo();
  }
  // Prints debug info to cout

 
  //--------------------------------------------------------
  //	General methods
  //--------------------------------------------------------

  operator Word() const {
    return look()->toWord();
  }
  // cast to common Word (not reduced)

  Chars toChars( const VectorOf<Chars>& names ) const {
    return look()->toChars(names);
  }
  // outputs word to string using given Chars as names of generators

  void freelyReduce() {
    change()->freelyReduce();
  }
  // freely reduces the word

  PolyWord inverse() const {
    return look()->inverse();
  }
  // returns the inverse word

  void append(const PolyWord& w) {
    change()->append(w);
  }
  // appends w to the end of this word

  void append(const Letter& w) {
    change()->append(w);
  }
  // appends w to the end of this word

  void clear() { change()->clear(); }
  // clears the list

  void duplicate(PolyWordNode*& ptrToFirst, PolyWordNode*& ptrToLast) const {
    look()->duplicate(ptrToFirst, ptrToLast);
  }
  // duplicates the list and writes addresses of the new copy's
  // first and last nodes to ptrToFirst and ptrToLast.

  void removeFirstLetter() {
    change()->removeFirstLetter();
  }
  // removes first letter (node) from the list

  PolyWord raiseToPower( int power ) const {
    return look()->raiseToPower(power);
  }
  // Raises the word to given power (can be negative or 0)

};

//----------------- Inline global functions -----------------

inline ostream& operator<<(ostream& s, const PolyWord& w) {
  w.printOn(s);
  return s;
}
// stream output

inline PolyWord operator * (const PolyWord& p1, const PolyWord& p2) {
  PolyWord res(p1);
  res.append(p2);
  return res;
}
// catenation of words


inline PolyWord commutator(const PolyWord& w1, const PolyWord& w2) {
  return w1.inverse() * w2.inverse() * w1 * w2;
}
// returns commutator [w1, w2]

inline PolyWord commutatorOfInverses(const PolyWord& w1, const PolyWord& w2) {
  return w1 * w2 * w1.inverse() * w2.inverse();
}
// returns commutator [w1^-1, w2^-1]

PolyWord evaluateWord( const Word& w, const VectorOf<PolyWord>& pw );
// Evaluates the word w by replacing its generators ai -> pw[i].

PolyWord evaluateWord(const PolyWord& pw, const VectorOf<PolyWord>& v);
//Evaluates the word replacing generator g by word v[g-1].
//@ep cannot make it a class member due to gcc template bugs

#endif



// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of PolyWordRep class.
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

#include "PolyWord.h"
#include "PolyWordIterator.h"
#include "AbelianWord.h"
#include "IPC.h"
#include <stdio.h>


// constructs PolyWord from Word
// here we use the fact that commutators of class 1 have the same
// internal representation as Free Nilpotent Group generators

PolyWordRep::PolyWordRep( const Word& w)
: first(NULL), last(NULL), theNumberOfNodes(0) {
  if(w.length() == 0) return;
  Letter let(w[0]);
  for(int i = 1; i < w.length(); i++) {
    if( abs(w[i]) == let.gen ) let.collectWith(w[i]);
    else {
      if( let.power != 0 ) append(let);
      let = w[i];
    }
  }
  if( let.power != 0 ) append(let);
}


PolyWordRep::PolyWordRep( const AbelianWord& aw )
: first(NULL), last(NULL), theNumberOfNodes(0) {

  for(int i = 0; i < aw.numberOfGenerators(); i++) {

    int g = aw[i].as_long();
    if( g != 0 )
      append( Letter(i+1, g) );
  }
}



int PolyWordRep::length() const {
  int len = 0;
  ConstPolyWordIterator iter(*this);
  for( iter.startFromLeft(); ! iter.done(); iter.stepRight() )
    len += abs( iter.thisLetter().power );
  return len;
}
// length of the word (sum of absolute values of powers).

Word PolyWordRep::toWord() const {
  Word w;
  ConstPolyWordIterator iter(*this);
  for( iter.startFromLeft(); ! iter.done(); iter.stepRight() )
    w *= (Word)iter.thisLetter();
  return w;
}
// cast to common Word (not reduced)

void PolyWordRep::printOn (ostream& s) const {

  if( theNumberOfNodes == 0 )
    s << "1";
  else {
    ConstPolyWordIterator iter(*this);
    iter.startFromLeft();
    s << iter.thisLetter();
    for(iter.stepRight(); ! iter.done(); iter.stepRight() )
      s << " * " << iter.thisLetter();
  }
}
// output

void PolyWordRep::freelyReduce() {
  PolyWordIterator iter(*this);
  for( iter.startFromLeft(); ! iter.isLast(); )
    if( ! iter.collectToRight() ) iter.stepRight();
}
// freely reduces the word

PolyWord PolyWordRep::inverse() const {
  PolyWord w; 
  ConstPolyWordIterator iter(*this);
  for( iter.startFromRight(); ! iter.done(); iter.stepLeft() )
    w.append( inv( iter.thisLetter() ) );
  return w;
}
// returns the inverse word

void PolyWordRep::append(const PolyWord& w) {

  if(w.isEmpty() ) return;

  PolyWordNode *newFirst, *newLast;
  w.duplicate(newFirst, newLast);

  if( theNumberOfNodes == 0 )
    first = newFirst;
  else
    bind(last, newFirst);

  last = newLast;
  theNumberOfNodes += w.numberOfLetters();
}
// appends w to the end of this word


void PolyWordRep::append(const Letter& w) {

  if(w.power == 0 ) return;
  
  PolyWordNode *newNode = new PolyWordNode(w);
  if( theNumberOfNodes == 0 )
    first = newNode;
  else
    bind(last, newNode);
  
  last = newNode;
  ++theNumberOfNodes;
}
// appends w to the end of this word


void PolyWordRep::clear() {
  PolyWordIterator iter(*this);
  for( iter.startFromLeft(); !iter.done(); iter.removeThisLetter() );
  theNumberOfNodes = 0;
  first = last = NULL;
}
// clears the list

void PolyWordRep::duplicate(PolyWordNode*& ptrToFirst, 
                            PolyWordNode*& ptrToLast) const {  
  // if nothing to duplicate
  if(theNumberOfNodes == 0) {
    ptrToFirst = ptrToLast = NULL;
    return;
  }

  PolyWordNode* ptr_new;
  ptr_new = ptrToFirst = new PolyWordNode(*first);

  for(PolyWordNode *ptr_old = first->next;
      ptr_old != NULL; ptr_old = ptr_old->next ) {
    bind( ptr_new, new PolyWordNode(*ptr_old) );
    ptr_new = ptr_new->next;
  }
  ptrToLast = ptr_new;
}
// duplicates the list and writes addresses of the new copy's
// first and last nodes to ptrToFirst and ptrToLast.


void PolyWordRep::removeTheLetter( PolyWordNode *theLetter) {

  if(theLetter == NULL)
    error("Attempt to remove NULL pointer while iterating PolyWord");

  PolyWordNode *nxt = theLetter->next;
  PolyWordRep::bind(theLetter->prev, nxt);
  if( theLetter == first ) first = nxt;
  if(nxt == NULL) last = theLetter->prev;
  delete theLetter;
  --theNumberOfNodes;
}

bool PolyWordRep::isCollected() const {

  ConstPolyWordIterator iter(*this);

  for(iter.startFromLeft(); ! iter.isLast(); iter.stepRight() ) {
    if( ord( iter.thisLetter().gen ) >= ord( iter.rightLetter().gen ) )
      return false;
  }
  return true;
}

// IPC output
ostream& PolyWordRep::write(ostream& s) const {
  s < theNumberOfNodes;

  ConstPolyWordIterator iter(*this);

  for(iter.startFromLeft(); ! iter.done(); iter.stepRight() ) {
    s < iter.thisLetter();
  }
  return s;
}

// IPC input
istream& PolyWordRep::read(istream& s) {
  clear();
  int nodes;
  s > nodes;
  for(int i = 0; i < nodes; i++) {
    Letter let;
    s > let;
    append(let);
  }
  return s;
}

// outputs word to string using given Chars as names of generators
Chars PolyWordRep::toChars( const VectorOf<Chars>& name ) const {
  
  Chars res;
  ConstPolyWordIterator iter(*this);

  if( theNumberOfNodes == 0 ) return Chars("1");

  for ( iter.startFromLeft(); !iter.done(); iter.stepRight() ) {
    Letter s = iter.thisLetter();
    res += name.val( ord(s.gen) - 1 );
    if(s.power != 1)
      res += Chars("^") + Chars(s.power);
    res += (Chars)" ";
  }
  return res;
}


PolyWord PolyWordRep::raiseToPower( int power ) const {

  if(power == 0)
    return PolyWord();

  PolyWord pw((PolyWordRep*)this);

  if(power < 0) {
    pw = pw.inverse();
    power = -power;
  }

  PolyWord res;

  for(int i = 0; i < power; i++)
    res.append(pw);

  res.freelyReduce();
  return res;
}


void PolyWordRep::debugInfo() const {

  int nNodes = 0;
  int nNegNodes = 0;
  int nZeroNodes = 0;
  
  int nLetters = 0;
  int nNegLetters = 0;

  ConstPolyWordIterator iter(*this);

  for(iter.startFromLeft(); ! iter.done(); iter.stepRight() ) {

    int pow = iter.thisLetter().power;

    nNodes += 1;
    nLetters += abs(pow);

    if(pow < 0) {
      ++nNegNodes;
      nNegLetters += abs(pow);
    }

    if(pow == 0) {
      ++nZeroNodes;
    }

  }

  cout << "Word info. Nodes: " << nNodes << " Negative: " << nNegNodes
       << " Letters: " << nLetters << " Negative: " << nNegLetters
       << " Zero nodes: " << nZeroNodes << endl;
}

// Evaluates the word w by replacing its generators ai -> pw[i].
PolyWord evaluateWord( const Word& w, const VectorOf<PolyWord>& pw ) {

  PolyWord res;

  for(int i = 0; i < w.length(); i++) {

    int gen = ord(w[i]);

    if( gen > 0 )
      res.append( pw.val(gen - 1) );
    else
      res.append( pw.val(-gen - 1).inverse() );
  }

  return res;
}


// Evaluates the word  by replacing its generators i -> v[i-1].
PolyWord evaluateWord(const PolyWord& pw, const VectorOf<PolyWord>& v ) {

  PolyWord res;
  
  ConstPolyWordIterator iter(pw);

  for ( iter.startFromLeft(); !iter.done(); iter.stepRight() ) {
    int gen = ord(iter.thisLetter().gen);
    int pow = iter.thisLetter().power;

    PolyWord app;

    if( pow > 0 )
      app = v.val(gen - 1);
    else
      app = v.val(gen - 1).inverse();

    for(int i = 0; i < abs(pow); i++)
      res.append(app);
  }

  return res;
}



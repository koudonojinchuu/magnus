// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of WordOrder.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//

#ifndef _WordOrder_H_
#define _WordOrder_H_

#include "Word.h"
#include "Vector.h"
// #include "DFSARep.h"
#include "Set.h"
#include "DiffHistoryRep.h"
#include "WordOrderRep.h"
typedef int State; 
// @sr for now this file needs to be independent of DFSARep.
// So for now we'll typedef State to be int, but that's only temporary.
// That's because a declaration of SetOf<DiffHistory> is necessary in Set.C
// (for each of the various DiffHistory types)
// and that can't be dependent on the FSA hierarchy.

class WordOrder : public ObjectOf<WordOrderRep> {
public:
//constructor
  WordOrder() : ObjectOf< WordOrderRep > (new WordOrderRep()){};

  WordOrder(const Chars & oType) : 
    ObjectOf< WordOrderRep > (
    (oType==Chars("ShortLex")? new ShortLexRep() :
      (oType==Chars("WtShortLex")? new WtShortLexRep():
        (oType==Chars("WtLex")? new WtLexRep(): new WordOrderRep())))) {};

  WordOrder(const Chars & oType, int numOfSymbols) : 
    ObjectOf< WordOrderRep > (
    (oType==Chars("ShortLex")? new ShortLexRep(numOfSymbols) :
      (oType==Chars("WtShortLex")? new WtShortLexRep(numOfSymbols):
        (oType==Chars("WtLex")? new WtLexRep(numOfSymbols): new WordOrderRep(numOfSymbols))))) {};

  WordOrder(const Chars & oType,const VectorOf<int> & v) : 
    ObjectOf< WordOrderRep > (
    (oType==Chars("ShortLex")? new ShortLexRep(v) :
      (oType==Chars("WtShortLex")? new WtShortLexRep(v):
        (oType==Chars("WtLex")? new WtLexRep(v): new WordOrderRep(v))))) {};
// v is interpreted as specifying an order of generators for shortlex,
// but as weights for the weighted orders.
// To specify both orders and weights for the weighted orders, two
// vectors must be given as arguments

// The next constructor cannot be used for ShortLex
  WordOrder(const Chars & oType,const VectorOf<int> & o,const VectorOf<int> & w)
   : ObjectOf< WordOrderRep > (
     (oType==Chars("WtShortLex")? new WtShortLexRep(o,w):
        (oType==Chars("WtLex")? new WtLexRep(o,w): new WordOrderRep(o)))) {};

  int signature(const Word & w1,const Word & w2) const {
    return look()->signature(w1,w2); }
  // return 1 if w1<w2, -1 if w1>w2, 0 if w1=w2
    
  int signature(Generator g,Generator h) const { return look()->signature(g,h);};
  // return 1 if g<h, -1 if g>h and 0 if g=h
  int signature(int i,int j) const { return look()->signature(i,j);}
  // returns signature(g,h), where g is the i-th and h the j-th generator in
  // the order.

  void balancedEquationFromRelator(const Word & w,Word & lhs,Word & rhs) const
    { look()->balancedEquationFromRelator(w,lhs,rhs);}
  // sets lhs equal to w1, rhs to w2, where w1>=w2 in the word order
  // w=w1*w2^-1 and w1 is as short as possible such that this can happen.

  int historyBound(const VectorOf<Word> & diffs) const{ 
    return look()->historyBound(diffs);}
  DiffHistoryRep * buildDiffHistoryRep() const { 
    return look()->buildDiffHistoryRep();};
  DiffHistoryRep * buildDiffHistoryRep(State d,int g,int h) const  {
    return look()->buildDiffHistoryRep(d,g,h);};
// build a difference history for the word difference g^-1h, attached
// to state d of the difference machine, bearing in mind the mysterious bound M,
// which might mean the history becomes trivial.

  DiffHistoryRep * update 
     (const DiffHistoryRep & dh,State d,int g,int h,const Word & wd) const {
    return look()->update(dh,d,g,h,wd); };
// Given a difference history dh associated with pairs of words w,u,
// construct the difference history associated with pairs wg,uh and the
// state d of the difference machine (which is of course the target
// of the state for dh under the pair of symbols (g,h)

  Bool reduction(const DiffHistoryRep & dh,int g,int h) const {
    return look()->reduction(dh,g,h); };
// returns YES if the difference history contains a pair w,u of words
// such that wg=_G uh and uh < wg in the word order.

  Bool possibleReduction(const AheadInfoRep & ai,int g) const {
    return look()->possibleReduction(ai,g); };

  AheadInfoRep *  update(const AheadInfoRep & ai,int g) const {
    return look()->update(ai,g); };


  DiffHistoryVtxRep * buildDiffHistoryVtxRep() const { 
    return look()->buildDiffHistoryVtxRep();};
  DiffHistoryVtxRep * buildDiffHistoryVtxRep(State d,int g,int h)
const  {
    return look()->buildDiffHistoryVtxRep(d,g,h);};
// build a difference history for the word difference g^-1h

  DiffHistoryVtxRep * update 
     (const DiffHistoryVtxRep & dh,State d,int g,int h,DiffHistoryVtx * ptr) const
{
    return look()->update(dh,d,g,h,ptr); };
// Given a DiffHistoryVtx * dhp associated with pairs of words w,u, 
// construct the difference history associated with pairs wg,uh

  Bool reduction(const DiffHistoryVtxRep & dh,int g,int h) const {
    return look()->reduction(dh,g,h); };
// returns YES if the difference history contains a pair w,u of words
// such that wg=_G uh and uh < wg in the word order.

  Bool possibleReductionAhead(const DiffHistoryVtxRep & dh,int g) const {
    return look()->possibleReductionAhead(dh,g); };

  Chars getOrderType() const { return look()->getOrderType(); }
  int getNumSymbols() const { return look()->getNumSymbols(); }
  int getSymbolIndex(int i) const { return look()->getSymbolIndex(i);}
// i takes values from 1 to getNumSymbols() below
  Generator getWeight(int i) const { return look()->getWeight(i); }
  int getWeight(const Word & w) const { return look()->getWeight(w); }
  Generator getSymbol(int i) const { return look()->getSymbol(i); }
// getPosition(0 is the inverse of getSymbol()
  int getPosition(Generator g) const { return look()->getPosition(g); }
  int selfInverse(Generator g) const { return look()->selfInverse(g); }
  Word inverse(const Word & w) const { return look()->inverse(w); }
protected:
  typedef ObjectOf<WordOrderRep> R;
  WordOrder( WordOrderRep *p ) : R(p) {}
};
#endif

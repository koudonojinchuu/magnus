// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of DiffHistory class.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//

#ifndef _DiffHistory_H_
#define _DiffHistory_H_

#include "Word.h"
#include "Vector.h"
// #include "DFSARep.h"
typedef int State; 
#include "DiffHistoryRep.h"
#include "WordOrder.h"
// @sr for now this file needs to be independent of DFSARep.
// So for now we'll typedef State to be int, but that's only temporary.
// That's because a declaration of SetOf<DiffHistory> is necessary in Set.C
// and that can't be dependent on the FSA hierarchy.
// If that weren't necessary, this could all be in DiffMachineRep.h

// A DiffHistory consists of a word difference d together with additional 
// information about its origin.
// A word difference  corresponds to a state of the difference
// machine used for word reduction and to construct the automata for
// an automatic group, and also to a word equal in the group
// to some products w^-1u, for pairs of words w,u. The additional
// information relates to those pairs w,u associated with d which are
// relevant in the particular context in which the word difference is
// being used. The whole point of carrying this information is to
// find out whether for some such w,u, w is a prefix of some w' and u a prefix
// of some u' where w'>u' in the word ordering.
// For different word orders, different information needs to be attached
// to a word difference, therefore there are different types of DiffHistories
// for different word orders.

class DiffHistory : public ObjectOf<DiffHistoryRep> {
public:
//constructor
  DiffHistory() : 
    ObjectOf< DiffHistoryRep > (new DiffHistoryRep()){};
  DiffHistory(const WordOrder & word_order) : 
  ObjectOf<DiffHistoryRep> ( word_order.buildDiffHistoryRep() ){};
  DiffHistory(State d,int g,int h,const WordOrder & word_order) : 
  ObjectOf<DiffHistoryRep> ( word_order.buildDiffHistoryRep(d,g,h) ){};
  DiffHistory
    (const DiffHistory & dh,State d,int g,int h,const Word & wd,const WordOrder & word_order) :
  ObjectOf<DiffHistoryRep> (word_order.update(*dh.look(),d,g,h,wd) )
  {};
//hash function
  int hash() const {return look()->hash();};

  Bool empty() const { return look()->empty();};
  State getDiff() const {return look()->getDiff();};
  int operator == ( const DiffHistory & dh ) const {
	 return ((look() == dh.look()) || (*look() == *dh.look()));
  }

  int operator != ( const DiffHistory & dh ) const { return !( *this == dh ); }

  Bool sameLengthWords() const 
// return yes if the difference histroy contains a pair of words w,u
// of the same length
  {return look()->sameLengthWords();} ;


  void improveBy(const DiffHistory & dh) {
// add to the current difference history any information held by the DiffHistory
// dh which improves it.
    change()->improveBy(*dh.look());
  };

  Bool reduction(int g,int h,const WordOrder & word_order) const
// returns yes if for some pair of words w,u in the difference history of d,
// wg reduces to uh, where uh is earlier than wg in the word order
    { return word_order.reduction(*look(),g,h);}

  Bool possibleReductionAhead() const
// returns yes if it is possible (i.e. cannot be immediately ruled out) that
// for some pair of words w,u in the difference history of d,
// w reduces to some longer word uH, which precedes it in the word order.
    { return look()->possibleReductionAhead();}

  AheadInfoRep *  buildAheadInfoRep() const
    { return look()->buildAheadInfoRep();}

  void printOn(ostream &str = cout) const { look()->printOn(str); }

  inline friend ostream& operator << 
        ( ostream& ostr, const DiffHistory& dh ) {
    dh.look()->printOn(ostr);
    return ostr;
  }

protected:
  typedef ObjectOf<DiffHistoryRep> R;
  DiffHistory( DiffHistoryRep *p ) : R(p) {}
};

// AheadInfo hierarchy is used in situations where it is necessary to
// look ahead. More precisely, we use this to invesigate the possibility
// that, where a given word difference d is associated with a pair of words w,u
// (such that d=_G w^-1u), w is equal in the group G to a word u', of which
// u is a prefix, which is longer than w, but less than it in the word ordering.

class AheadInfo : public ObjectOf<AheadInfoRep> {
public:
//constructor
  AheadInfo() : 
    ObjectOf< AheadInfoRep > (new AheadInfoRep()){};
  AheadInfo(const DiffHistory & dh) : 
  ObjectOf<AheadInfoRep> ( dh.buildAheadInfoRep() ){};
  AheadInfo
    (const AheadInfo & ai,int g,const WordOrder & word_order) :
  ObjectOf<AheadInfoRep> (word_order.update(*ai.look(),g) ){};
  Bool possibleReduction(int g,const WordOrder & word_order) const
// returns yes if it is possible (i.e. cannot be immediately ruled out) that
// there is a reduction of w to the longer word got by adding g to the word currently stored.
    { return word_order.possibleReduction(*look(),g);}

protected:
  typedef ObjectOf<AheadInfoRep> R;
  AheadInfo( AheadInfoRep *p ) : R(p) {}
};

class DiffHistoryVtx : public ObjectOf<DiffHistoryVtxRep> {

public:
//constructor
  DiffHistoryVtx() : 
    ObjectOf< DiffHistoryVtxRep > (new DiffHistoryVtxRep()){};
  DiffHistoryVtx(const WordOrder & word_order) : 
  ObjectOf<DiffHistoryVtxRep> 
    (word_order.buildDiffHistoryVtxRep() ){};
  DiffHistoryVtx(State d,int g,int h,const WordOrder & word_order)
    :ObjectOf<DiffHistoryVtxRep>
            ( word_order.buildDiffHistoryVtxRep(d,g,h)){};
  DiffHistoryVtx
    (DiffHistoryVtx * dhp,State d,int g,int h,const WordOrder & word_order) :
  ObjectOf<DiffHistoryVtxRep> (word_order.update(*(dhp->look()),d,g,h,dhp) )
  {};
 
  Bool reduction(int g,int h,const WordOrder & word_order) const
// returns yes if, where this vertex corresponds to d=w^-1u,
// wg reduces to uh, where uh is earlier than wg in the word order
    { return word_order.reduction(*look(),g,h);}

  Trichotomy betterThan(DiffHistoryVtx* dhp) const
// Only to be called if the word differences match.
// If the word differences match,
// returns yes if whenever *dhp predicts a reduction, the current
// DiffHistoryVtx predicts one too and there could be situations
// where the current DiffHistoryVtx predicts a reduction but *dhp 
// does not, no if *dhp predicts a
// reduction whenever the current DiffHistoryVtx does,
// dontknow if neither of the above is true (or it seems too much work 
// to find out).
    { return look()->betterThan(*(dhp->look()));}

  Bool possibleReductionAhead() const
// returns yes if, where this vertex corresponds to d=w^-1u,
// it is possible that
// w reduces to some longer word uH, which precedes it in the word order.
    { return look()->possibleReductionAhead();}

  Bool possibleReductionAhead(int g,const WordOrder & word_order) const
// returns yes if, where this vertex corresponds to d=w^-1u,
// and given that wg does not reduce, it is possible that
// w reduces to some longer word ugH (H non-trivial), which 
// precedes it in the word order.
    { return word_order.possibleReductionAhead(*look(),g);}

  void printOn(ostream &str = cout) const {
    look()->printOn(str); }

  inline friend ostream& operator << 
        ( ostream& ostr, const DiffHistoryVtx& dh ) {
    dh.look()->printOn(ostr);
    return ostr;
  }
  State getDiff() const {return look()->getDiff();};
  int getGenerator() const { return look()->getGenerator();}
  DiffHistoryVtx *getBackptr() const { return look()->getBackptr();}
  int getLength() const { return look()->getLength();}
protected:
  typedef ObjectOf<DiffHistoryVtxRep> R;
  DiffHistoryVtx( DiffHistoryVtxRep *p ) : R(p) {}
};

 #endif

// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of WordOrderRep and related classes.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//

#ifndef _WordOrderRep_H_
#define _WordOrderRep_H_

#include "Word.h"
#include "Vector.h"
#include "RefCounter.h"
// #include "DFSARep.h"
#include "Set.h"
#include "DiffHistoryRep.h"
typedef int State; 
// @sr for now this file needs to be independent of DFSARep.
// So for now we'll typedef State to be int, but that's only temporary.
// That's because a declaration of SetOf<DiffHistory> is necessary in Set.C
// (for each of the various DiffHistory types)
// and that can't be dependent on the FSA hierarchy.


class WordOrderRep : public RefCounter {
public:
//constructors
  WordOrderRep() {};
  WordOrderRep(const Chars & oType ) : orderType(oType),numSyms(0), order(0),
    posn(0), invposn(0) {};
  WordOrderRep(const VectorOf<int> & o) : numSyms(o.length()), order(o),
    posn(o.length()), invposn(o.length()) {
    int i;
    for (i=0;i<numSyms;i++){
      posn[i]=0; invposn[i]=0; 
           // We MUST initialise invposn to be zero, since entries which
           // remain set to zero indicate involutions
    }
    for (i=1;i<=numSyms;i++){
      int j = order[i-1];
      if (j>0) posn[j-1]=i; else invposn[-j-1]=i;
    }
  };
  WordOrderRep(int numOfSymbols) : numSyms(numOfSymbols), order(0),
    posn(0), invposn(0) {
    int ngens = numOfSymbols/2;
    for (int i=1;i<=ngens;i++){
      order.append(i);
      order.append(-i);
      posn.append(2*i-1);
      invposn.append(2*i);
    }
  }
  WordOrderRep(const Chars & oType, int numOfSymbols) : 
    orderType(oType), numSyms(numOfSymbols), order(0), posn(0), invposn(0) {
    for (int i=1;i<=numOfSymbols/2;i++){
      order.append(i);
      order.append(-i);
      posn.append(2*i-1);
      invposn.append(2*i);
    }
  };
  WordOrderRep(const Chars & oType, const VectorOf<int> & o) : 
    orderType(oType), numSyms(o.length()), order(o), posn(o.length()), 
      invposn(o.length()) {
    int i;
    for (i=0;i<numSyms;i++){
      posn[i]=0; invposn[i]=0; 
           // We MUST initialise invposn to be zero, since entries which
           // remain set to zero indicate involutions
    }
    for (i=1;i<=numSyms;i++){
      int j = order[i-1];
      if (j>0) posn[j-1]=i; else invposn[-j-1]=i;
    }
  };
// copy constructor
  WordOrderRep (const WordOrderRep & word_order) { 
    order=word_order.order;
    orderType = word_order.orderType;
    numSyms = word_order.numSyms;
    posn = word_order.posn;
    invposn = word_order.invposn;
  };
  virtual WordOrderRep *clone() const {};
//destructor
  ~WordOrderRep() {};
  virtual int signature(const Word & w1,const Word & w2) const {return 1;};
  virtual int signature(int i, int j) const {return 1;};
  virtual int signature(Generator g,Generator h) const {return 1;};
  virtual void balancedEquationFromRelator
                      (const Word & w,Word & lhs,Word & rhs) const {};
  virtual int historyBound(const VectorOf<Word> & diffs) const{ return 0;}
  virtual DiffHistoryRep * buildDiffHistoryRep() const  {};
  virtual DiffHistoryRep * buildDiffHistoryRep(State d,int g,int h) const  {};
// build a difference history for the word difference g^-1h, attached
// to state d of the difference machine.

  virtual DiffHistoryRep * update 
     (const DiffHistoryRep & dh,State d,int g,int h,const Word & wd) const {};
// Given a difference history dh associated with pairs of words w,u,
// construct the difference history associated with pairs wg,uh and the
// state d of the difference machine (which is of course the target
// of the state for dh under the pair of symbols (g,h))

  virtual Bool reduction(const DiffHistoryRep & dh,int g,int h) const {};
// returns YES if the difference history contains a pair w,u of words
// such that wg=_G uh and uh < wg in the word order.

  virtual Bool possibleReduction(const AheadInfoRep & ai,int g) const {};
// returns YES if it is possible (i.e. cannot yet be ruled out)
// that there is a reduction to a longer word. 

  virtual AheadInfoRep *  update(const AheadInfoRep & ai,int g) const {};

  virtual DiffHistoryVtxRep * buildDiffHistoryVtxRep() const  {};
  virtual DiffHistoryVtxRep 
      * buildDiffHistoryVtxRep(State d,int g,int h) const  {}; 
// build a difference history Vtx for the word difference g^-1h,
// attached to state d of the difference machine.

  virtual DiffHistoryVtxRep * update 
     (const DiffHistoryVtxRep & dh,State d, int g, int h, DiffHistoryVtx * ptr)
             const {}; 
// Given a difference history vertex dh associated with pairs of
//words w,u,  
// construct the difference history Vtx associated with
// pairs wg,uh and the  
// state d of the difference machine (which is of course thetarget  
// of the state for dh under the pair of symbols (g,h))

  virtual Bool reduction(const DiffHistoryVtxRep & dh,int g,int h)
const {};
// returns YES if the difference history contains a pair w,uof words 
// such that wg=_G uh and uh < wg in the word order.

  virtual Bool possibleReductionAhead(const DiffHistoryVtxRep & dh,int g)
const {}; 

// returns YES if it is possible (i.e. cannot yet be ruled out) 
// that there is a reduction of w to a longer word, with ug as a 
// proper prefix. 

  Chars getOrderType() const { return orderType; }
  int getNumSymbols() const { return numSyms; }
  int getSymbolIndex(int i) const { return order[i-1];}
  Generator getSymbol(int i) const { Generator g(order[i-1]); return g; }
  int getPosition(Generator g) const { 
    return (ord(g)>0? posn[ord(g)-1]: invposn[-ord(g)-1]);
  } // returns 0 if the generator isn't in the alphabet

  Word inverse(const Word & w) const{
    int len = w.length();
    VectorOf<Generator> W(len);
    for (int i=0; i<len;i++)
      W[i]= (selfInverse(w[len-1-i])? w[len-1-i]: inv(w[len-1-i]));
    return (Word)W;
  }
// Returns yes if the inverse generator is not a symbol.
// It doesn't make sense to call this unless ord(g) is positive
// but this is checked to avoid crashes.
  Bool selfInverse(Generator g) const {
    return (ord(g)>0 && invposn[ord(g)-1]==0);
  }
  virtual int getWeight(int i) const { return 1;};
  virtual int getWeight(Generator g) const { return 1;};
  virtual int getWeight(const Word & w) const { return w.length();};
protected:
  VectorOf<int> order;
  VectorOf<int> posn; // posn[i] stores position in order of generator
                      // indexed by i, as a number in range 1..numSyms
  VectorOf<int> invposn; // invposn[i] stores position in order of generator
                      // indexed by -i, as a number in range 1..numSyms
  int numSyms;
private:
  Chars orderType;
};

class ShortLexRep : public WordOrderRep {
public:
  ShortLexRep() : WordOrderRep("ShortLex") {};

  ShortLexRep(const VectorOf<int> &  o) : WordOrderRep("ShortLex",o) { };

  ShortLexRep(int numOfSymbols) : WordOrderRep("ShortLex",numOfSymbols) { };

//copy constructor
  ShortLexRep( const ShortLexRep& wsl ) : 
    WordOrderRep(wsl) { }

  WordOrderRep *clone() const { return new ShortLexRep(*this); }

  int signature(const Word & w1,const Word & w2) const { 
    if (w1==w2) return 0;
    else if (w1.length()<w2.length()) return 1;
    else if (w1.length()>w2.length()) return -1;
    else {
      int j=0;
      while (j<w1.length()){
        if (getPosition(w1[j])<getPosition(w2[j])) return 1;
        else if (getPosition(w1[j])>getPosition(w2[j])) return -1;
        else j++;
      }
    }
  } 

  int signature(int i, int j) const {
    if (i==j) return 0;
    else if (i<j) return 1;
    else return -1;
  }

  int signature(Generator g,Generator h) const {
    return signature(getPosition(g),getPosition(h));
  }

  void balancedEquationFromRelator(const Word & w,Word & lhs,Word & rhs) const {
// This code is complicated by the fact that we have to make sure that
// neither side of the equation contains the inverse symbol of an involution
// if that's not in our alphabet. We might even have got such symbols into
// the relation, because of the way magnus turns relations in input into
// relators

    int len=w.length();
    int i;
    int len2=len - (len+1)/2;

// copy the word, replacing each g^-1 for which g is an involution by g
    VectorOf<Generator> W;
    for (i=0;i<len;i++)
      W.append((ord(w[i])<0 && selfInverse(inv(w[i])))? inv(w[i]) : w[i]);

    lhs=((Word)W).initialSegment((len+1)/2);
    VectorOf<Generator> ww;
    for (i=1;i<=len2;i++){
      Generator g = W[len-i];
      ww.append(selfInverse(g)? g : inv(g));
    }
    rhs=ww;
    if (signature(lhs,rhs)==1){ // lhs is smaller, so switch them
      rhs=lhs;
      lhs=ww;
    }
  }

  int historyBound(const VectorOf<Word> & diffs) const{ return 0;}

  DiffHistoryRep * buildDiffHistoryRep() const { 
    SLDiffHistoryRep * dhp = new SLDiffHistoryRep();
      return dhp;
  }

  DiffHistoryRep * buildDiffHistoryRep(State d,int g,int h) const { 
    if (g>0 && h>0 && g!=h){
      SLDiffHistoryRep * dhp = 
        new SLDiffHistoryRep(d,(g>h? 1: -1),0);
      return dhp;
    }
    else if (g>0 && h==0){
      SLDiffHistoryRep * dhp = new SLDiffHistoryRep(d,0,1);
      return dhp;
    }
    else { 
      cerr << "Parameters out of range for DiffHistoryRep constructor."<< endl;
      SLDiffHistoryRep * dhp =  new SLDiffHistoryRep;
      return dhp;
    }
  }

  DiffHistoryRep * update
    (const DiffHistoryRep & dh,State d,int g,int h,const Word & wd) const {
// the parameter wd isn't actually used in the function for a ShortLex 
// DiffHistory, but the generality of the code requres it to be a parameter
    const SLDiffHistoryRep& ddh = (SLDiffHistoryRep&)dh;
    if (g>0 && h>0 && ddh.getC0()){
      SLDiffHistoryRep * dhp = new SLDiffHistoryRep
              (d,ddh.getC0(),0);
      return dhp;
    }
    else if (g>0 && h==0){
      SLDiffHistoryRep * dhp = new SLDiffHistoryRep (d,0,1);
      return dhp;
    }
    else { 
      cerr << "Parameters out of range for update."<< endl;
      SLDiffHistoryRep * dhp =  new SLDiffHistoryRep;
      return dhp;
    }
  }

  Bool reduction(const DiffHistoryRep & dh,int g,int h) const {
    const SLDiffHistoryRep& ddh = (SLDiffHistoryRep&)dh;
    if (g>0 && h>0 && ddh.getC0()==1){
      return YES;
    } 
    else if (g>0 && h==0) return YES;
    return NO;
  }

  Bool possibleReduction(const AheadInfoRep & ai,int g) const { 
     return NO;
  };

  AheadInfoRep *  update(const AheadInfoRep & ai,int g) const {
// this function won't actually get used, since it is not possible
// in shortlex to have w>u where w is shorter than u.
// It is necessary to have a function returning something of the right
// type to get  compilation.
     return new SLAheadInfoRep (); }

  DiffHistoryVtxRep * buildDiffHistoryVtxRep() const { 
    SLDiffHistoryVtxRep * dhp = new SLDiffHistoryVtxRep();
      return dhp;
  }

  DiffHistoryVtxRep
    * buildDiffHistoryVtxRep(State d,int g,int h) const
{ 
    if (g>0 && h>0 && g!=h){
      SLDiffHistoryVtxRep * dhp = 
        new SLDiffHistoryVtxRep(d,h,0,1,(g>h? 1: -1));
      return dhp;
    }
    else if (g>0 && h==0){
      SLDiffHistoryVtxRep * dhp = new
             SLDiffHistoryVtxRep(d,0,0,1,0);
      return dhp;
    }
    else { 
      cerr <<
 "Parameters out of range for DiffHistoryVtxRep constructor."
         << endl;
      SLDiffHistoryVtxRep * dhp =  new SLDiffHistoryVtxRep;
      return dhp;
    }
  }

  DiffHistoryVtxRep * update
    (const DiffHistoryVtxRep & dh,State d,int g,int h, DiffHistoryVtx * ptr)
       const {
    const SLDiffHistoryVtxRep & ddh = (SLDiffHistoryVtxRep &)dh;
    SLDiffHistoryVtxRep * dhp = new SLDiffHistoryVtxRep
              (d,h,ptr,dh.getLength()+1,ddh.getLexComp());
    return dhp;
  }
   

  Bool reduction(const DiffHistoryVtxRep & dh,int g,int h) const {
    const SLDiffHistoryVtxRep& ddh = (SLDiffHistoryVtxRep&)dh;
    if (g>0 && (h==0 || ddh.getLexComp()==1)) return YES;
    else return NO;
  }

  Bool possibleReduction(const DiffHistoryVtxRep & dh,int g) const { 
     return NO;
  };


  int getWeight(int i) const { return 1;};
  int getWeight(Generator g) const { return 1;};
  int getWeight(const Word & w) const { return w.length();};
};

class WtShortLexRep : public WordOrderRep {
public:
  WtShortLexRep() : WordOrderRep("WtShortLex"){};

  WtShortLexRep(int numOfSymbols) : 
    WordOrderRep("WtShortLex",numOfSymbols), weight(0) { 
    for (int i=1;i<=numOfSymbols;i++)
      weight.append(1);
  };

  WtShortLexRep(const VectorOf<int>  & w) : 
    WordOrderRep("WtShortLex",w.length()), weight(w) { 
  };

  WtShortLexRep(const VectorOf<int> & o,const VectorOf<int> & w) : 
    WordOrderRep("WtShortLex",o) , weight(w) {};

//copy constructor
  WtShortLexRep( const WtShortLexRep& wsl ) : 
    WordOrderRep(wsl), weight(wsl.weight) { }

  WordOrderRep *clone() const { return new WtShortLexRep(*this); }

  int signature(const Word & w1,const Word & w2) const { 
    if (w1==w2) return 0; 
    else {
      int len1=w1.length();
      int len2=w2.length();
      int wt1=0;
      int wt2=0;
      int j=0;
      while (j<len1) wt1 += weight[getPosition(w1[j++])-1];
      j=0;
      while (j<len2) wt2 += weight[getPosition(w2[j++])-1];
      if (wt1<wt2) return 1; 
      else if (wt1>wt2) return -1; 
      else if (w1.length()<w2.length()) return 1; 
      else if (w1.length()>w2.length()) return -1; 
      else { 
        j=0;
        while (j<len1){ 
          if (getPosition(w1[j])<getPosition(w2[j])) return 1; 
          else if (getPosition(w1[j])>getPosition(w2[j])) return -1;
          else j++; 
        }
      }
    } 
  }

  int signature(int i, int j) const {
    if (i==j) return 0;
    else if (weight[i-1]<weight[j-1] || (weight[i-1]==weight[j-1] && i<j))
      return 1;
    else return -1;
  }

  int signature(Generator g,Generator h) const {
    return signature(getPosition(g),getPosition(h));
  }

  void balancedEquationFromRelator(const Word & w,Word & lhs,Word & rhs) const {
    int len=w.length();
    int i;
    int wt1=0; int wt2=0;

// copy the word, replacing each g^-1 for which g is an involution by g
    VectorOf<Generator> W;
    for (i=0;i<len;i++)
      W.append((ord(w[i])<0 && selfInverse(inv(w[i])))? inv(w[i]) : w[i]);

    int j=0;
    while (j<len) wt1 += weight[getPosition(W[j++])-1];
    VectorOf<Generator> ww;
    j=len;
    while (--j>=0) {
      Generator g=W[j];
      wt1 -= weight[getPosition(g)-1];
      wt2 += weight[getPosition(g)-1];
      if  (wt1>=wt2 ) ww.append(selfInverse(g)? g : inv(g));
      else break;
    }
    lhs = ((Word)W).initialSegment(j+1);
    rhs=ww;
    if (signature(lhs,rhs)==1){ // lhs is smaller, so switch them
      rhs=lhs;
      lhs=ww;
    }
  }

  int historyBound(const VectorOf<Word> & diffs) const { 
    int Max=0;
    for (int i=0;i<diffs.length();i++){
      Word w=diffs[i];
      int len=w.length();
      int wt=0;
      for (int j=0;j<len;j++) wt += weight[getPosition(w[j])-1];
      if (wt>Max) Max = wt;
    }
    return Max;
  }

  DiffHistoryRep * buildDiffHistoryRep() const { 
    WtSLDiffHistoryRep * dhp = new WtSLDiffHistoryRep();
      return dhp;
  }

  DiffHistoryRep * buildDiffHistoryRep(State d,int g,int h) const { 
    if (g>0 && h>0 && g!=h){
      WtSLDiffHistoryRep * dhp = 
        new WtSLDiffHistoryRep(d,(g>h? 1: -1),weight[g-1] - weight[h-1],0,0);
      return dhp;
    }
    else if (g>0 && h==0){
      WtSLDiffHistoryRep * dhp = 
        new WtSLDiffHistoryRep(d,0,0,1,(weight[g-1]>0?1: weight[g-1]));
      return dhp;
    }
    else { 
      cerr << "Parameters out of range for DiffHistoryRep constructor."<< endl;
      WtSLDiffHistoryRep * dhp =  new WtSLDiffHistoryRep;
      return dhp;
    }
  }

  DiffHistoryRep * update
    (const DiffHistoryRep & dh,State d,int g,int h,const Word & wd) const {
    const WtSLDiffHistoryRep& ddh = (WtSLDiffHistoryRep&)dh;
    if (g>0 && h>0 && ddh.getC0()){
      int w0 = ddh.getW0()+weight[g-1]-weight[h-1];
      int c0 = (w0>= -getWeight(wd) ? ddh.getC0() : 0);
      WtSLDiffHistoryRep * dhp = new WtSLDiffHistoryRep
              (d,c0,w0,0,0);
      return dhp;
    }
    else if (g>0 && h==0){
      int w1;
      if (ddh.getC0() && ddh.getC1()){
        w1 = (ddh.getW0()>=ddh.getW1()? 
// choose the bigger weight difference in dh 
// add the weight of g to it, and then set w1 to be the minimum of that and 1
         (ddh.getW0()+weight[g-1]>0?1:ddh.getW0() + weight[g-1]):
         (ddh.getW1()+weight[g-1]>0?1:ddh.getW1() + weight[g-1]));
      }
      else if (ddh.getC0())
         w1 = (ddh.getW0()+weight[g-1]>0?1:ddh.getW0() + weight[g-1]);
      else if (ddh.getC1())
         w1 = (ddh.getW1()+weight[g-1]>0?1:ddh.getW1() + weight[g-1]);

      int c1 = (w1 >= -getWeight(wd)? 1: 0);
      WtSLDiffHistoryRep * dhp = new WtSLDiffHistoryRep (d,0,0,c1,w1); 
      return dhp;
    }
    else { 
      cerr << "Parameters out of range for update."<< endl;
      WtSLDiffHistoryRep * dhp =  new WtSLDiffHistoryRep;
      return dhp;
    }
  }

  Bool reduction(const DiffHistoryRep & dh,int g,int h) const {
    const WtSLDiffHistoryRep& ddh = (WtSLDiffHistoryRep&)dh;
    if (g>0 && h>0 && ddh.getC0()){

// For some w,u wg=_G uh where wg and uh have equal length, but wg>uh. 
// Either wt(wg)>wt(uh) 
// or wt(wg)=wt(uh) but uh precedes wg lexicographically.

      if (ddh.getW0() + weight[g-1] - weight[h-1]>0 || 
        (ddh.getW0() + weight[g-1]-weight[h-1]==0 && ddh.getC0()==1)) 
           return YES;
    } 

    else if (g>0 && h==0){

// For some w,u, wg=_G u, where l(wg)>l(u) and wt(wg)>=wt(u).
// Either l(w)=l(u) (getC0() returns a non-zero value), so l(wg)=l(u)+1
// or l(w)>l(u) (getC1() returns a non-zero value) and so certainly l(wg)>l(u)

      if ((ddh.getC0()!=0 && ddh.getW0() + weight[g-1] >= 0)
         || (ddh.getC1()!=0 && ddh.getW1() + weight[g-1] >= 0)) return YES;
    }

    return NO;
  }

  Bool possibleReduction(const AheadInfoRep & ai,int g) const { 
     const WtSLAheadInfoRep & aai = (WtSLAheadInfoRep &)ai;
     return (aai.getWtDiff() > weight[g-1]);};

  AheadInfoRep * update(const AheadInfoRep & ai,int g) const {
     const WtSLAheadInfoRep & aai = (WtSLAheadInfoRep &)ai;
     WtSLAheadInfoRep * aip = 
              new WtSLAheadInfoRep (aai.getWtDiff()-weight[g-1]); 
     return aip; 
  }

  DiffHistoryVtxRep * buildDiffHistoryVtxRep() const { 
    WtSLDiffHistoryVtxRep * dhp = new WtSLDiffHistoryVtxRep();
      return dhp;
  }

  DiffHistoryVtxRep * buildDiffHistoryVtxRep(State d,int g,int h) const
{ 
    if (g>0 && h>0 && g!=h){
      WtSLDiffHistoryVtxRep * dhp = 
        new WtSLDiffHistoryVtxRep(d,h,0,1,(g>h? 1: -1),weight[g-1] -
weight[h-1]);
      return dhp;
    }
    else if (g>0 && h==0){
      WtSLDiffHistoryVtxRep * dhp = 
        new WtSLDiffHistoryVtxRep(d,0,0,1,0,weight[g-1]);
      return dhp;
    }
    else { 
      cerr << "Parameters out of range for DiffHistoryVtxRep constructor."<< endl;
      WtSLDiffHistoryVtxRep * dhp =  new WtSLDiffHistoryVtxRep;
      return dhp;
    }
  }

  DiffHistoryVtxRep * update
    (const DiffHistoryVtxRep & dh,State d,int g,int h, DiffHistoryVtx * ptr)
       const {
    const WtSLDiffHistoryVtxRep & ddh = (WtSLDiffHistoryVtxRep &)dh;
    if (g>0 && h>0){
       WtSLDiffHistoryVtxRep * dhp = new WtSLDiffHistoryVtxRep
              (d,h,ptr,dh.getLength()+1,ddh.getLexComp(),
                 ddh.getWtDiff() + weight[g-1] - weight[h-1]);
      return dhp;
    }
    else if (g>0 && h==0){
     WtSLDiffHistoryVtxRep * dhp = new WtSLDiffHistoryVtxRep
              (d,0,ptr,dh.getLength()+1,ddh.getLexComp(),
                 ddh.getWtDiff() + weight[g-1] );
      return dhp;
    }
    else if (g==0 && h>0){
     WtSLDiffHistoryVtxRep * dhp = new WtSLDiffHistoryVtxRep
              (d,h,ptr,dh.getLength(),ddh.getLexComp(),
                 ddh.getWtDiff() - weight[h-1] );
      return dhp;
    }
    else {
      cerr << "Parameters out of range in WtLexHistoryVtxRep::update()"<<endl;
      return new WtLexDiffHistoryVtxRep;
    }
  }

  Bool reduction(const DiffHistoryVtxRep & dh,int g,int h) const {
    const WtSLDiffHistoryVtxRep& ddh = (WtSLDiffHistoryVtxRep&)dh;
    if (g>0 && h>0 ){

// dh corresponds to the difference w^-1u.
// The function returns true if wg >uh.
// if g and h are nonzero, w and u are assumed to have the same length.
// if g is nonzero, and h=0, w is at least as long as u.
// if h is nonzero, and g=0, u is at least as long as w.
// Either wt(wg)>wt(uh) 
// or wt(wg)=wt(uh) but uh precedes wg lexicographically.
// or wt(w)<wt(uh) 

      if (ddh.getWtDiff() + weight[g-1] - weight[h-1]>0 || 
        (ddh.getWtDiff() + weight[g-1]-weight[h-1]==0 &&
                    ddh.getLexComp()==1)) 
           return YES;
    } 

    else if (g>0 && h==0){

      if (ddh.getWtDiff() + weight[g-1] >= 0)
         return YES;
    }

    else if (g==0 && h>0){

      if (ddh.getWtDiff() > weight[h-1] )
         return YES;
    }

    return NO;
  }

  Bool possibleReductionAhead(const DiffHistoryVtxRep & dh,int g) const { 
     const WtSLDiffHistoryVtxRep & ddh = (WtSLDiffHistoryVtxRep &)dh;
     return (ddh.getWtDiff() > weight[g-1] +1);};
// Where dh represents the difference w^-1u,
// if there's a possible reduction of w to some u' with ug a proper
// prefix of u', w must have higher weight than some ugh,
// and so weight more than one less than ug.

  int getWeight(int i) const { return weight[i-1];};
  int getWeight(Generator g) const { return weight[getPosition(g)-1];};
  int getWeight(const Word & w) const { 
    int wt=0,i=0,len=w.length();
    while (i<len) wt += weight[getPosition(w[i++])-1];
    return wt;
 };

private:
  VectorOf<int> weight;
};

class WtLexRep : public WordOrderRep {
public:
  WtLexRep() : WordOrderRep("WtLex"){};

  WtLexRep(int numOfSymbols) : 
    WordOrderRep("WtLex",numOfSymbols), weight(0) { 
    for (int i=1;i<=numOfSymbols;i++)
      weight.append(1);
  };

  WtLexRep(const VectorOf<int>  & w) : 
    WordOrderRep("WtLex",w.length()), weight(w) { 
  };

  WtLexRep(const VectorOf<int> & o,const VectorOf<int> & w) : 
    WordOrderRep("WtLex",o) , weight(w) {};

//copy constructor
  WtLexRep( const WtLexRep& wsl ) : 
    WordOrderRep(wsl), weight(wsl.weight) { }

  WordOrderRep *clone() const { return new WtLexRep(*this); }

  int signature(const Word & w1,const Word & w2) const {  
    if (w1==w2) return 0; 
    else {
      int len1=w1.length();
      int len2=w2.length();
      int wt1=0;
      int wt2=0;
      int j=0;
      while (j<len1) wt1 += weight[getPosition(w1[j++])-1];
      j=0;
      while (j<len2) wt2 += weight[getPosition(w2[j++])-1];
      if (wt1<wt2) return 1; 
      else if (wt1>wt2) return -1; 
      else { 
        j=0;
        while (j<len1 && j<len2){ 
          if (getPosition(w1[j])<getPosition(w2[j])) return 1; 
          else if (getPosition(w1[j])>getPosition(w2[j])) return -1;
          else j++; 
        }
        // at this stage, one word is simply a prefix of the other
        if (len1<len2) return 1;
        else return -1; 
      }
    } 
  }

  int signature(int i, int j) const {
    if (i==j) return 0;
    else if (weight[i-1]<weight[j-1] || (weight[i-1]==weight[j-1] && i<j))
      return 1;
    else return -1;
  }
  int signature(Generator g, Generator h) const {
    return signature(getPosition(g),getPosition(h));
  }

  void balancedEquationFromRelator(const Word & w,Word & lhs,Word & rhs) const {
    int len=w.length();
    int wt1=0; int wt2=0;
    int i;
// copy the word, replacing each g^-1 for which g is an involution by g
    VectorOf<Generator> W;
    for (i=0;i<len;i++)
      W.append((ord(w[i])<0 && selfInverse(inv(w[i])))? inv(w[i]) : w[i]);

    int j=0;
    while (j<len) wt1 += weight[getPosition(W[j++])-1];
    VectorOf<Generator> ww;
    j=len;
    while (j-->=0) {
      Generator g=W[j];
      wt1 -= weight[getPosition(g)-1];
      wt2 += weight[getPosition(g)-1];
      if  (wt1>=wt2 ) ww.append(selfInverse(g)? g : inv(g));
      else break;
    }
    lhs = ((Word)W).initialSegment(j+1);
    rhs=ww;
    if (signature(lhs,rhs)==1){ // lhs is smaller, so switch them
      rhs=lhs;
      lhs=ww;
    }
  }

  int historyBound(const VectorOf<Word> & diffs) const { 
    int Max=0;
    for (int i=0;i<diffs.length();i++){
      Word w=diffs[i];
      int len=w.length();
      int wt=0;
      for (int j=0;j<len;j++) wt += weight[getPosition(w[j])-1];
      if (wt>Max) Max = wt;
    }
    return Max;
  }
  DiffHistoryRep * buildDiffHistoryRep() const { 
    WtLexDiffHistoryRep * dhp = new WtLexDiffHistoryRep();
      return dhp;
  }

  DiffHistoryRep * buildDiffHistoryRep(State d,int g,int h) const { 
    if (g>0 && h>0 && g!=h){
      WtLexDiffHistoryRep * dhp = 
        new WtLexDiffHistoryRep(d,(g>h? 1: -1),weight[g-1] - weight[h-1],0,0);
      return dhp;
    }
    else if (g>0 && h==0){
      WtLexDiffHistoryRep * dhp = 
        new WtLexDiffHistoryRep(d,0,0,1,(weight[g-1]>0?1: weight[g-1]));
      return dhp;
    }
    else { 
      cerr << "Parameters out of range for DiffHistoryRep constructor."<< endl;
      WtLexDiffHistoryRep * dhp =  new WtLexDiffHistoryRep;
      return dhp;
    }
  }

  DiffHistoryRep * update
    (const DiffHistoryRep & dh,State d,int g,int h,const Word & wd) const {
    const WtLexDiffHistoryRep& ddh = (WtLexDiffHistoryRep&)dh;
    if (g>0 && h>0 && ddh.getC0()){
      int w0 = ddh.getW0()+weight[g-1]-weight[h-1];
      int c0 = (w0>= -getWeight(wd) ? ddh.getC0(): 0);
      WtLexDiffHistoryRep * dhp = new WtLexDiffHistoryRep
              (d,c0,w0,0,0);
      return dhp;
    }
    else if (g>0 && h==0){
      int c1,w1;
      if (ddh.getC0() && ddh.getC1()){
// choose the bigger weight difference in dh 
// add the weight of g to it, and then set w1 to be the minimum of that and 1
// c1 is set to be ddh.getC0() if ddh.getW0()>ddh.getW1(),
// to be ddh.getC1() if ddh.getW1()>ddh.getW0(), and otherwise the
// larger of ddh.getC0() and ddh.getC1()

        if (ddh.getW0()>ddh.getW1()){
          w1 = (ddh.getW0()+weight[g-1]>0?1:ddh.getW0() + weight[g-1]);
          c1 = ddh.getC0();
        }
        else if (ddh.getW1()>ddh.getW0()){
          w1 = (ddh.getW1()+weight[g-1]>0?1:ddh.getW1() + weight[g-1]);
          c1 = ddh.getC1();
        }
        else {
          w1 = (ddh.getW1()+weight[g-1]>0?1:ddh.getW1() + weight[g-1]);
          c1 = (ddh.getC1()>=ddh.getC0()? ddh.getC1(): ddh.getC0());
        }
      }
      else if (ddh.getC0()){
         w1 = (ddh.getW0()+weight[g-1]>0?1:ddh.getW0() + weight[g-1]);
         c1 = ddh.getC0();
      }
      else if (ddh.getC1()){
         w1 = (ddh.getW1()+weight[g-1]>0?1:ddh.getW1() + weight[g-1]);
         c1 = ddh.getC1();
      } 
      if (w1 < -getWeight(wd)) c1=0;
      WtLexDiffHistoryRep * dhp = new WtLexDiffHistoryRep (d,0,0,c1,w1); 
      return dhp;
    }
    else { 
      cerr << "Parameters out of range for update."<< endl;
      WtLexDiffHistoryRep * dhp =  new WtLexDiffHistoryRep;
      return dhp;
    }
  }

  Bool reduction(const DiffHistoryRep & dh,int g,int h) const {
    const WtLexDiffHistoryRep& ddh = (WtLexDiffHistoryRep&)dh;
    if (g>0 && h>0 && ddh.getC0()){

// For some w,u wg=_G uh where wg and uh have equal length, but wg>uh. 
// Either wt(wg)>wt(uh) 
// or wt(wg)=wt(uh) but uh precedes wg lexicographically.

      if (ddh.getW0() + weight[g-1] - weight[h-1]>0 || 
        (ddh.getW0() + weight[g-1]-weight[h-1]==0 && ddh.getC0()==1)) 
           return YES;
    } 

    else if (g>0 && h==0){

// For some w,u, wg=_G u, where l(wg)>l(u),
// either wt(wg)>wt(u) or wt(wg)=wt(u) but wg>u lexicographically
// Either l(w)=l(u) (getC0() returns a non-zero value- if this is -1
// we must have wt(wg)>wt(u))
// or l(w)>l(u) (getC1() returns a non-zero value  - if this is -1
// we must have wt(wg)>wt(u))

      if ((ddh.getC0()==1 && ddh.getW0() + weight[g-1] >= 0)
       || (ddh.getC0()== -1 && ddh.getW0() + weight[g-1] > 0)
       || (ddh.getC1()==1 && ddh.getW1() + weight[g-1] >= 0)
       || (ddh.getC1()== -1 && ddh.getW1() + weight[g-1] > 0))
          return YES;
    }

    return NO;
  }

  Bool possibleReduction(const AheadInfoRep & ai,int g) const { 
     const WtLexAheadInfoRep & aai = (WtLexAheadInfoRep &)ai;
     return (aai.getWtDiff() > weight[g-1] || 
        (aai.getWtDiff()==weight[g-1] && aai.getSign()==1 ));};

  AheadInfoRep * update(const AheadInfoRep & ai,int g) const {
     const WtLexAheadInfoRep & aai = (WtLexAheadInfoRep &)ai;
     WtLexAheadInfoRep * aip = 
       new WtLexAheadInfoRep (aai.getWtDiff()-weight[g-1],aai.getSign()); 
     return aip; 
  }

  DiffHistoryVtxRep * buildDiffHistoryVtxRep() const { 
    WtLexDiffHistoryVtxRep * dhp = new WtLexDiffHistoryVtxRep();
      return dhp;
  }

  DiffHistoryVtxRep * buildDiffHistoryVtxRep(State d,int g,int h) const { 
    if (g>0 && h>0 && g!=h){
      WtLexDiffHistoryVtxRep * dhp = 
        new WtLexDiffHistoryVtxRep(d,h,0,1,(g>h? 1: -1),weight[g-1] -
weight[h-1]);
      return dhp;
    }
    else if (g>0 && h==0){
      WtLexDiffHistoryVtxRep * dhp = 
        new WtLexDiffHistoryVtxRep(d,0,0,1,0,weight[g-1]);
      return dhp;
    }
    else { 
      cerr << 
"Parameters out of range for DiffHistoryVtxRep constructor."<< endl;
      WtLexDiffHistoryVtxRep * dhp =  new WtLexDiffHistoryVtxRep;
      return dhp;
    }
  }

  DiffHistoryVtxRep * update
    (const DiffHistoryVtxRep & dh,State d,int g,int h, DiffHistoryVtx * ptr) const {
    const WtLexDiffHistoryVtxRep& ddh = (WtLexDiffHistoryVtxRep&)dh;
    if (g>0 && h>0){
     WtLexDiffHistoryVtxRep * dhp = new WtLexDiffHistoryVtxRep
       (d,h,ptr,dh.getLength()+1,ddh.getLexComp(),
               ddh.getWtDiff()+weight[g-1] - weight[h-1]);
      return dhp;
    }
    else if (g>0 && h==0){
     WtLexDiffHistoryVtxRep * dhp = new WtLexDiffHistoryVtxRep
       (d,0,ptr,dh.getLength()+1,ddh.getLexComp(),
               ddh.getWtDiff()+weight[g-1]);
      return dhp;
    }
    else if (g==0 && h>0){ 
     WtLexDiffHistoryVtxRep * dhp = new WtLexDiffHistoryVtxRep
       (d,h,ptr,dh.getLength(),ddh.getLexComp(),
               ddh.getWtDiff()-weight[h-1]);
      return dhp;
    }
    else {
      cerr << "Parameters out of range in WtLexHistoryVtxRep::update()"<<endl;
      return new WtLexDiffHistoryVtxRep;
    }
  }

  Bool reduction(const DiffHistoryVtxRep & dh,int g,int h) const {
    const WtLexDiffHistoryVtxRep& ddh = (WtLexDiffHistoryVtxRep&)dh;
// dh corresponds to the difference w^-1u.
// The function returns true if wg >uh.
// if g and h are nonzero, w and u are assumed to have the same length.
// if g is nonzero, and h=0, w is at least as long as u.
// if h is nonzero, and g=0, u is at least as long as w.
    if (g>0 && h>0){
      if (ddh.getWtDiff() + weight[g-1] - weight[h-1]>0 || 
        (ddh.getWtDiff() + weight[g-1]-weight[h-1]==0 &&
           ddh.getLexComp()==1)) 
           return YES;
    } 

    else if (g>0 && h==0){
      if (ddh.getWtDiff() + weight[g-1]>0 || 
        (ddh.getWtDiff() + weight[g-1]==0 &&
               ddh.getLexComp()==1)) 
          return YES;
    }

    else if (g==0 && h>0){
      if (ddh.getWtDiff() > weight[h-1] || 
        (ddh.getWtDiff() -weight[h-1]==0 &&
               ddh.getLexComp()==1)) 
          return YES;
    }

    return NO;
  }

  Bool possibleReductionAhead(const DiffHistoryVtxRep & dh,int g) const { 
     const WtLexDiffHistoryVtxRep & ddh = (WtLexDiffHistoryVtxRep &)dh;
     return (ddh.getWtDiff() > weight[g-1] +2||
       (ddh.getWtDiff()==weight[g-1]+1&& ddh.getLexComp()==1));};
// Where dh represents the difference w^-1u,
// if there's a possible reduction of w to some u' with ug a proper
// prefix of u', w must either have higher weight than some ugh,
// and so weight more than one less than ug, or the same weight
// as some ugh which it precedes lexicographically.

  int getWeight(int i) const { return weight[i-1];};
  int getWeight(Generator g) const { return weight[getPosition(g)-1];};
  int getWeight(const Word & w) const { 
    int wt=0,i=0,len=w.length();
    while (i<len) wt += weight[getPosition(w[i++])-1];
    return wt;
 };

private:
  VectorOf<int> weight;
};


class InvPairWreathRep : public WordOrderRep {
public:
  InvPairWreathRep() : WordOrderRep("InvPairWreath") {};

  InvPairWreathRep(const VectorOf<int> & o) : WordOrderRep("InvPairWreath",o) {};


  //int signature(const Word & w1,const Word & w2) const { return 1; } 
  // define this properly

  int signature(int g, int h) const { return 1; } //  temporary

};

#endif

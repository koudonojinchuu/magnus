// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of DiffHistoryRep and related classes.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//

#ifndef _DiffHistoryRep_H_
#define _DiffHistoryRep_H_

#include "Word.h"
#include "Vector.h"
#include "RefCounter.h"
// #include "DFSARep.h"
typedef int State; 
// @sr for now this file needs to be independent of DFSARep.
// So for now we'll typedef State to be int, but that's only temporary.
// That's because a declaration of SetOf<DiffHistory> is necessary in Set.C
// and that can't be dependent on the FSA hierarchy.
// If that weren't necessary, this could all be in DiffMachineRep.h

class DiffHistoryVtx;

// First the AheadInfoRep hierarchy, one derived class for each word order.

class AheadInfoRep : public RefCounter {
public:
//constructor
  AheadInfoRep() {};
// copy constructor
  AheadInfoRep (const AheadInfoRep & ai) {}; 
  virtual AheadInfoRep *clone() const {};
//destructor
  virtual ~AheadInfoRep() {};
};

class SLAheadInfoRep : public AheadInfoRep {
public:
//constructor
  SLAheadInfoRep() {};
// copy constructor
  SLAheadInfoRep(const SLAheadInfoRep & ai) {}; 
  AheadInfoRep *clone() const { return new SLAheadInfoRep(*this); }
//destructor
  ~SLAheadInfoRep() {};
private:
};

class WtSLAheadInfoRep : public AheadInfoRep {
public:
//constructor
  WtSLAheadInfoRep() {};
  WtSLAheadInfoRep(int wd) {wtdiff=wd;};
// copy constructor
  WtSLAheadInfoRep (const WtSLAheadInfoRep & ai) {wtdiff=ai.wtdiff;}; 
  AheadInfoRep *clone() const { return new WtSLAheadInfoRep(*this); }
//destructor
  ~WtSLAheadInfoRep() {};
  int getWtDiff() const { return wtdiff;}
private:
  int wtdiff;
};

class WtLexAheadInfoRep : public AheadInfoRep {
public:
//constructor
  WtLexAheadInfoRep(): sign(0) {};
  WtLexAheadInfoRep(int wd,int sgn): wtdiff(wd), sign(sgn) {};
// copy constructor
  WtLexAheadInfoRep(const WtLexAheadInfoRep & ai) : 
     wtdiff(ai.wtdiff), sign(ai.sign){}; 
  AheadInfoRep *clone() const { return new WtLexAheadInfoRep(*this); }
//destructor
  ~WtLexAheadInfoRep() {};
  int getWtDiff() const { return wtdiff;}
  int getSign() const { return sign;}
private:
  int wtdiff; 
  int sign;
};


// Now the DiffHistoryRep hierarchy. There is one type for each word order.
// In the comments below d is always a word difference, which has arise
// as various products of the form w^-1u

class DiffHistoryRep : public RefCounter {
public:
//constructor
  DiffHistoryRep() {};
//copy constructor
  DiffHistoryRep(const DiffHistoryRep & dh) {};
  virtual DiffHistoryRep *clone() const {};
//destructor
  virtual ~DiffHistoryRep() {};

//hash function
  virtual int hash() const {};
  virtual Bool empty() const {};

  virtual State getDiff() const {};
//operators
  virtual int operator== ( const DiffHistoryRep & dh) const {
  };
  virtual DiffHistoryRep&  operator= ( const DiffHistoryRep & dh ) {};

  virtual Bool sameLengthWords() const {};
// returns YES if d has arisen as the word difference from two words
// of equal length

  virtual void improveBy(const DiffHistoryRep & dh) {};
//  add in to the current word difference history the information carried
// by the difference history dh (which is associated associated with the same
// word difference)

  virtual Bool possibleReductionAhead() const {};
// return YES if the possibility cannot be ruled out that some w is equal
// in G to a longer word u', of which u is a prefix, and such that u<w.

  virtual AheadInfoRep *  buildAheadInfoRep() const {};
// constuct and return an AheadInfoRep * for this DiffHistory 

  virtual void printOn(ostream &ostr = cout) const {};

  virtual inline ostream& operator << ( const DiffHistoryRep& dh ) {};
};

class SLDiffHistoryRep : public DiffHistoryRep {
// Together with each word difference d, integers c0,c1
// are stored, where
// c0 = 1 if d=_G w^-1u for some w, u with l(w)=l(u) and w>u
//    =  -1 if for all w,u with d=_G w^-1u and l(w)=l(u), w<u
//    = 0 if d does not arise as the word difference for 
//        words of equal length.
// c1 = 1 if d = _G w^-1u for some w,u with l(w)>l(u)
//    = 0 otherwise.
//
public:
//constructors
  SLDiffHistoryRep(): d(0),c0(0),c1(0){};
  SLDiffHistoryRep(State D,int C0,int C1):
    d(D),c0(C0),c1(C1){};
//copy constructor
  SLDiffHistoryRep(const SLDiffHistoryRep & dh) : 
    d(dh.d), c0(dh.c0), c1 (dh.c1) {};
  DiffHistoryRep *clone() const { return new SLDiffHistoryRep(*this); }
//destructor
  ~SLDiffHistoryRep() {};
//hash function
  int hash() const { return (int)d; };
  Bool empty() const { return (c0==0 && c1==0); };
//accessors
  int getDiff() const { return d;};
  int getC0() const { return c0;};
  int getC1() const { return c1;};

  //operators
  int operator== ( const DiffHistoryRep & dh) const
  {
    const SLDiffHistoryRep& ddh = (SLDiffHistoryRep&)dh;
    return (d == ddh.d && c0 == ddh.c0  && c1 == ddh.c1);
  }
 
  DiffHistoryRep&  operator= ( const DiffHistoryRep & dh )
  {
    const SLDiffHistoryRep& ddh = (SLDiffHistoryRep&)dh;
    d = ddh.d; c0 = ddh.c0; c1 = ddh.c1;
    return *this;
  }

  Bool sameLengthWords() const  { return (c0!=0);};

  void improveBy(const DiffHistoryRep & dh) 
  {
    const SLDiffHistoryRep& ddh = (SLDiffHistoryRep&)dh;
    if ((ddh.c0==0 || (c0!=0 && c0>= ddh.c0 ))
             && (ddh.c1==0 || (c1>= ddh.c1 )))
        return;
  // ddh is no better than the current SLDiffHistoryRep
    else {
      if (ddh.c0 && (c0==0 || ddh.c0 == 1)){ 
       c0 = ddh.c0;}
      if (ddh.c1 && c1==0 ) {
        c1 = ddh.c1; } 
    }
  }
  Bool possibleReductionAhead() const { return NO;}

  AheadInfoRep * buildAheadInfoRep() const { return new SLAheadInfoRep(); }

  void printOn(ostream &ostr = cout) const 
  {
    ostr << "d="<< d << " c0="<< c0 <<" c1="<<c1<<"; ";
  }
  
  inline friend ostream& operator << 
        ( ostream& ostr, const SLDiffHistoryRep& dh ) {
  		  ostr << "d="<< dh.d << " c0="<< dh.c0 <<
  			 " c1="<<dh.c1<<"; ";
    return ostr;
  }
  
  
private:
  State d;
  int c0, c1;
};

class WtSLDiffHistoryRep : public DiffHistoryRep {
// Together with each word difference d, integers c0,w0,c1,w1
// are stored, where
// w0 = max{wt(w)-wt(u):l(u)=l(w),u \neq w, d=_G w^-1u }, 
// where that set is non-empty,
// and is otherwise meaningless. c0=0 if the set above is empty, 1 if there
// is some u with wt(w)-wt(u)=w0, l(u)=l(w), d=_G w^-1u and u<w in shortlex,
// and -1 if for all such u, w<u in shortlex.
// w1 = min{1,max{wt(w)-wt(u):l(u)<l(w), d_G w^-1u }, 
// where that set is non-empty,
// and is otherwise meaningless. c1=0 if that set if empty, 1 otherwise.
//
// Given such a difference history (d,c0,w0,c1,w1)
// 
// Where g is a generator, and g^-1d=_G id,
//    if either c0 is nonzero and w0+wt(g)>=0 
//       or c1 is non-zero and w1+wt(g)>=0,
//    then for some w where d=_G w^-1u, wg is equal in G to a 
//       shorter word of no greater weight.
//
//  Where g,h are generators and g^-1dh=_G id,
//     c0 is non-zero,
//     and either w0+wt(g)-wt(h)>0
//         or w0+wt(g)-wt(h)=0 and c0=1,
//     then wg reduces to uh, of the same length as wg,
//         but preceding it in weighted shortlex
//         (in the first case it has lesser weight, in the
//          second it has the same weight but is lexicographically earlier)
//
//   Where H is a word of length >= 2 with H^-1dg=id, c0 is non-zero
//   and g^-1d(h_1h_2..h_i) is in D for all prefixes h_1h_2..h_i of H
//   and w0 + wt(g) - wt(h_1) - ... wt(h_n) > 0,
//   then wg reduces to uH which is longer, but of lower weight.
//

public:
//constructors
  WtSLDiffHistoryRep(): d(0),c0(0),w0(0),c1(0),w1(0){};
  WtSLDiffHistoryRep(State D,int C0,int W0,int C1,int W1):
    d(D),c0(C0),w0(W0),c1(C1),w1(W1){};
//copy constructor
  WtSLDiffHistoryRep(const WtSLDiffHistoryRep & dh):
    d(dh.d), c0(dh.c0), w0(dh.w0), c1(dh.c1), w1(dh.w1){};
  DiffHistoryRep *clone() const { return new WtSLDiffHistoryRep(*this); }
//destructor
  ~WtSLDiffHistoryRep() {};
//hash function
  int hash() const { return (int)d; };
  Bool empty() const { return (c0==0 && c1==0); };
//accessors
  int getDiff() const { return d;};
  int getC0() const { return c0;};
  int getW0() const { return w0;};
  int getC1() const { return c1;};
  int getW1() const { return w1;};

  //operators
  int operator== ( const DiffHistoryRep & dh) const
  {
    const WtSLDiffHistoryRep& ddh = (WtSLDiffHistoryRep&)dh;
    return (d == ddh.d && c0 == ddh.c0 && w0 == ddh.w0 
                 && c1 == ddh.c1 && w1 == ddh.w1);
  }
 
  DiffHistoryRep&  operator= ( const DiffHistoryRep & dh )
  {
    const WtSLDiffHistoryRep& ddh = (WtSLDiffHistoryRep&)dh;
    d = ddh.d; c0 = ddh.c0; w0 = ddh.w0; c1 = ddh.c1; w1 = ddh.w1;
    return *this;
  }
  Bool sameLengthWords() const  { return (c0!=0);};
  void improveBy(const DiffHistoryRep & dh) 
  {
    const WtSLDiffHistoryRep& ddh = (WtSLDiffHistoryRep&)dh;
    if (ddh.c0 && (c0==0 || w0 < ddh.w0 || (w0==ddh.w0 && ddh.c0 == 1))){ 
      c0 = ddh.c0; w0 = ddh.w0;}
    if (ddh.c1 && (c1==0 || w1 < ddh.w1)){ 
      c1 = ddh.c1; w1 = ddh.w1;}
  }
  Bool possibleReductionAhead() const { return (c0 && w0 > 1);}

  AheadInfoRep * buildAheadInfoRep() const { 
    WtSLAheadInfoRep *  aip = new WtSLAheadInfoRep(w0); return aip; }

  void printOn(ostream &ostr = cout) const 
  {
    ostr << "d="<< d << " c0="<< c0 <<" w0="<<w0<<
          " c1="<<c1<<" w1="<<w1<<"; ";
  }
  
  inline friend ostream& operator << 
        ( ostream& ostr, const WtSLDiffHistoryRep& dh ) {
  		  ostr << "d="<< dh.d << " c0="<< dh.c0 <<
  			 " w0="<<dh.w0<<" c1="<<dh.c1<<" w1="<<dh.w1<<"; ";
    return ostr;
  }
  
  
private:
  State d;
  int c0, c1;
  int w0, w1;
};

class WtLexDiffHistoryRep : public DiffHistoryRep {
// There are only very minor differences between this class and
// WtSLDifferenceRep. The same data is held, but c1 can also take the value -1.
// The function improveBy is slightly different, and the AheadInfo needs to
// carry more information. The situations in which reduction can occur are
// also slightly different.
// Much of the code is a copy of WtSLDiffHistoryRep code, but it seems safer
// to keep the 2 classes separate, because of the minor differences which could
// lead to confusion if code were shared (or, for example if one class were
// derived from another). 

// Together with each word difference d, integers c0,w0,c1,w1
// are stored, where
// w0 = max{wt(w)-wt(u):l(u)=l(w),u \neq w, d=_G w^-1u }, 
// where that set is non-empty,
// and is otherwise meaningless. c0=0 if the set above is empty, 1 if there
// is some u with wt(w)-wt(u)=w0, l(u)=l(w), d=_G w^-1u and u<w in lex,
// and -1 if for all such u, w<u in lex.
// w1 = min{1,max{wt(w)-wt(u):l(u)<l(w), d_G w^-1u }, 
// where that set is non-empty,
// and is otherwise meaningless. c1=0 if the set above is empty, 1 if there
// is some u with wt(w)-wt(u)=w0, l(u)<l(w), d=_G w^-1u and u<w in lex,
// and -1 if for all such u, w<u in lex.
//
// Given such a difference history (d,c0,w0,c1,w1)
// 
// Where g is a generator, and g^-1d=_G id,
//    if either c0 is nonzero and w0+wt(g)>0 
//       or c0=1 and w0+wt(g)=0
//       or c1 is non-zero and w1+wt(g)>0,
//       or c1=1 and w1+wt(g)=0
//    then for some w where d=_G w^-1u, wg is equal in G to a 
//       word u which is either of lesser weight, or of the same weight, 
//       but preceding it in lex.
//
//  Where g,h are generators and g^-1dh=_G id,
//     c0 is non-zero,
//     and either w0+wt(g)-wt(h)>0
//         or w0+wt(g)-wt(h)=0 and c0=1,
//     then wg reduces to uh, of the same length as wg,
//         but preceding it in weighted lex
//         (in the first case it has lesser weight, in the
//          second it has the same weight but is lexicographically earlier)
//
//   Where H is a word of length >= 2 with H^-1dg=id, c0 is non-zero
//   and g^-1d(h_1h_2..h_i) is in D for all prefixes h_1h_2..h_i of H
//   and w0 + wt(g) - wt(h_1) - ... wt(h_n) > 0,
//   or  w0+wt(g)-wt(h_1)- .. wt(h_n)=0 and c0=1
//   then wg reduces to uH which is longer, either of lower weight than wg
//   or of the same weight as wg, and preceding it lexicographically.
//

public:
//constructors
  WtLexDiffHistoryRep(): d(0),c0(0),w0(0),c1(0),w1(0){};
  WtLexDiffHistoryRep(State D,int C0,int W0,int C1,int W1):
    d(D),c0(C0),w0(W0),c1(C1),w1(W1){};
//copy constructor
  WtLexDiffHistoryRep(const WtLexDiffHistoryRep & dh):
    d(dh.d), c0(dh.c0), w0(dh.w0), c1(dh.c1), w1(dh.w1) {};
  DiffHistoryRep *clone() const { return new WtLexDiffHistoryRep(*this); }
//destructor
  ~WtLexDiffHistoryRep() {};
//hash function
  int hash() const { return (int)d; };
  Bool empty() const { return (c0==0 && c1==0); };
//accessors
  int getDiff() const { return d;};
  int getC0() const { return c0;};
  int getW0() const { return w0;};
  int getC1() const { return c1;};
  int getW1() const { return w1;};

  //operators
  int operator== ( const DiffHistoryRep & dh) const
  {
    const WtLexDiffHistoryRep& ddh = (WtLexDiffHistoryRep&)dh;
    return (d == ddh.d && c0 == ddh.c0 && w0 == ddh.w0 
                 && c1 == ddh.c1 && w1 == ddh.w1);
  }
 
  DiffHistoryRep&  operator= ( const DiffHistoryRep & dh )
  {
    const WtLexDiffHistoryRep& ddh = (WtLexDiffHistoryRep&)dh;
    d = ddh.d; c0 = ddh.c0; w0 = ddh.w0; c1 = ddh.c1; w1 = ddh.w1;
    return *this;
  }
  Bool sameLengthWords() const  { return (c0!=0);};
  void improveBy(const DiffHistoryRep & dh) 
  {
    const WtLexDiffHistoryRep& ddh = (WtLexDiffHistoryRep&)dh;
    if (ddh.c0 && (c0==0 || w0 < ddh.w0 || (w0==ddh.w0 && ddh.c0 == 1))){ 
      c0 = ddh.c0; w0 = ddh.w0;}
    if (ddh.c1 && (c1==0 || w1 < ddh.w1 || (w1==ddh.w1 && ddh.c1 == 1))){ 
      c1 = ddh.c1; w1 = ddh.w1;}
  }
  Bool possibleReductionAhead() const { 
     return ((c0 && w0 > 0) || (w0 > 1 && c0==1));}

  AheadInfoRep * buildAheadInfoRep() const { 
    WtLexAheadInfoRep *  aip = new WtLexAheadInfoRep(w0,c0); return aip; }

  void printOn(ostream &ostr = cout) const 
  {
    ostr << "d="<< d << " c0="<< c0 <<" w0="<<w0<<" c1="<<c1<<" w1="<<w1<<"; ";
  }
  
  inline friend ostream& operator << 
        ( ostream& ostr, const WtLexDiffHistoryRep& dh ) {
  		  ostr << "d="<< dh.d << " c0="<< dh.c0 <<
  			 " w0="<<dh.w0<<" c1="<<dh.c1<<" w1="<<dh.w1<<"; ";
    return ostr;
  }
  
  
private:
  State d;
  int c0, c1;
  int w0, w1;
};


// Now the DiffHistoryVtxRep hierarchy. There is one type for each word order. 
// In the comments below d is always a word difference, which has arisen
// as various products of the form w^-1u

class DiffHistoryVtxRep : public RefCounter {
public:
//constructor
  DiffHistoryVtxRep(): diff(0), gen(0), backptr(0),len(0) {};
  DiffHistoryVtxRep(State D,int G,DiffHistoryVtx * ptr,int L):
     diff(D), gen(G), backptr(ptr),len(L) {};
//copy constructor
  DiffHistoryVtxRep(const DiffHistoryVtxRep & dh) {
    diff = dh.diff; gen = dh.gen; backptr = dh.backptr; len = dh.len;
  };
  DiffHistoryVtxRep *clone() const { return new DiffHistoryVtxRep(*this);};
//destructor
  ~DiffHistoryVtxRep() {};

  State getDiff() const {return diff;};
  int getGenerator() const { return gen;}
  DiffHistoryVtx *getBackptr() const { return backptr;}
  int getLength() const { return len;}
//operators
  int operator== ( const DiffHistoryVtxRep & dh) const {
  diff == dh.diff; gen==dh.gen; backptr == dh.backptr; len==dh.len;};
  DiffHistoryVtxRep&  operator= ( const DiffHistoryVtxRep & dh ) {
    diff = dh.diff; gen = dh.gen; backptr = dh.backptr; len = dh.len;
    return *this;
  };

  virtual Trichotomy betterThan(const DiffHistoryVtxRep & dh) const {};
// Only to be called if the word differences match.
// If the word differences match,
// returns yes if whenever dh predicts a reduction, the current
// DiffHistoryVtx predicts one too and there could be situations
// where the current DiffHistoryVtx predicts a reduction but dh 
// does not, no if dh predicts a
// reduction whenever the current DiffHistoryVtx does,
// dontknow if neither of the above is true (or it seems too much work 
// to find out).

  virtual Bool possibleReductionAhead() const {};
// return YES if the possibility cannot be ruled out that some w is equal
// in G to a longer word u', of which u is a prefix, and such that u<w.


  void printOn(ostream &ostr = cout) const {
    ostr << "diff="<< diff <<", gen= "<<gen<<", backptr = "<<backptr
         <<", len="<<len<<"; ";
  };

  inline friend ostream& operator << (ostream & ostr, const DiffHistoryVtxRep& dh ) {
    ostr << "diff="<< dh.diff <<", gen= "<<dh.gen<<", backptr = "<<dh.backptr
         <<", len="<<dh.len<<"; ";
    return ostr;
  };
private:
  State diff;
  int gen;
  DiffHistoryVtx * backptr;
  int len;
};


class SLDiffHistoryVtxRep : public DiffHistoryVtxRep {

public:
//constructors
  SLDiffHistoryVtxRep(): DiffHistoryVtxRep(),lexComp(0){};
  SLDiffHistoryVtxRep(State D,int G,DiffHistoryVtx * ptr,int L,int C):
    DiffHistoryVtxRep(D,G,ptr,L),lexComp(C) {};
//copy constructor
  SLDiffHistoryVtxRep(const SLDiffHistoryVtxRep & dh) 
    : DiffHistoryVtxRep(dh), lexComp(dh.lexComp){};
  DiffHistoryVtxRep *clone() const { 
    return new SLDiffHistoryVtxRep(*this); } 
//destructor
  ~SLDiffHistoryVtxRep() {};
//accessors
  int getLexComp() const { return lexComp;};

  //operators
/*
  int operator== ( const DiffHistoryVtxRep & dh) const
  {
LOOK in FSA hierarchy
    const SLDiffHistoryVtxRep& ddh = (SLDiffHistoryVtxRep&)dh;
    return (SOMETHING && lexComp = ddh.lexComp);
  }
*/
 
  DiffHistoryVtxRep&  operator= ( const DiffHistoryVtxRep & dh )
  {
    (DiffHistoryVtxRep)*this = dh;
    const SLDiffHistoryVtxRep& ddh = (SLDiffHistoryVtxRep&)dh;
    lexComp = ddh.lexComp;
    return *this;
  }

   Trichotomy betterThan(const DiffHistoryVtxRep & dh) const {
// Only to be called if the word differences match.
// If the word differences match,
// returns yes if whenever dh predicts a reduction, the current
// DiffHistoryVtx predicts one too and there could be situations
// where the current DiffHistoryVtx predicts a reduction but dh 
// does not, no if dh predicts a
// reduction whenever the current DiffHistoryVtx does,
// dontknow if neither of the above is true (or it seems too much work 
// to find out).

    if (getDiff()!=dh.getDiff()) return dontknow;

    int g1,g2;

// If one DiffHistoryVtx corresponds to a shorter word and one to a word
// the same length as w, neither can be considered to be better than
// the other
    if (((g1=getGenerator())==0 && (g2=dh.getGenerator()))||
         g1 & g2==0) return dontknow;

// If both words are shorter than w, either DiffHistoryVtx is as
// good as the other, so we return no

    if (g1==0 && g2==0) return no;

// If both words are the same length as w, we return yes if the
// current DiffHistoryVtx corresponds to a word which precedes w
// lexicographically, but dh does not.
// Otherwise dh is just as good as the current DiffHistoryVtx, so
// we return no.
     
    const SLDiffHistoryVtxRep& ddh = (SLDiffHistoryVtxRep&)dh;
    if (lexComp==1 && ddh.lexComp== -1) return yes;
    else return no; 
  }

  Bool possibleReductionAhead() const { return no;}

  void printOn(ostream &ostr = cout) const 
  {
    DiffHistoryVtxRep::printOn(ostr);
    ostr << " lexComp="<< lexComp <<"; ";
  }
  
  inline friend ostream& operator << 
        ( ostream& ostr, const SLDiffHistoryVtxRep& dh ) {
    DiffHistoryVtxRep ddh = (DiffHistoryVtxRep)dh;
    ostr << ddh << ", lexComp="<< dh.lexComp <<"; ";
    return ostr;
  }
  
  
private:
  int lexComp; // =1 if, where d=w^-1u, w>u in lex, -1 if w<u
                      // 0 if not set
};

// at the moment the two classes below are identical.
// if they don't change we could probably manage with one!

class WtSLDiffHistoryVtxRep : public DiffHistoryVtxRep {

public:
//constructors
  WtSLDiffHistoryVtxRep(): DiffHistoryVtxRep(),lexComp(0), wtdiff(0){};
  WtSLDiffHistoryVtxRep(State D,int G,DiffHistoryVtx * ptr,int L,
                         int C, int WD):
    DiffHistoryVtxRep(D,G,ptr,L),lexComp(C), wtdiff(WD) {};
//copy constructor
  WtSLDiffHistoryVtxRep(const WtSLDiffHistoryVtxRep & dh) 
    : DiffHistoryVtxRep(dh), lexComp(dh.lexComp), wtdiff(dh.wtdiff) {};
  DiffHistoryVtxRep *clone() const { 
    return new WtSLDiffHistoryVtxRep(*this); } 
//destructor
  ~WtSLDiffHistoryVtxRep() {};
//accessors
  int getLexComp() const { return lexComp;};
  int getWtDiff() const { return wtdiff;};

  //operators
/*
  int operator== ( const DiffHistoryVtxRep & dh) const
  {
LOOK in FSA hierarchy
    const WtSLDiffHistoryVtxRep& ddh = (WtSLDiffHistoryVtxRep&)dh;
    return (SOMETHING && lexComp = ddh.lexComp && wtdiff == ddh.wtdiff);
  }
*/
 
  DiffHistoryVtxRep&  operator= ( const DiffHistoryVtxRep & dh )
  {
    (WtSLDiffHistoryVtxRep)*this = dh;
    const WtSLDiffHistoryVtxRep& ddh = (WtSLDiffHistoryVtxRep&)dh;
    lexComp = ddh.lexComp; wtdiff = ddh.wtdiff;
    return *this;
  }

   Trichotomy betterThan(const DiffHistoryVtxRep & dh) const {
// Only to be called if the word differences match.
// If the word differences match,
// returns yes if whenever dh predicts a reduction, the current
// DiffHistoryVtx predicts one too and there could be situations
// where the current DiffHistoryVtx predicts a reduction but dh 
// does not, no if dh predicts a
// reduction whenever the current DiffHistoryVtx does,
// dontknow if neither of the above is true (or it seems too much work 
// to find out).

    if (getDiff()!=dh.getDiff()) return dontknow;

    int g1,g2;

// If one DiffHistoryVtx corresponds to a shorter word and one to a word
// the same length as w, neither can be considered to be better than
// the other
    if (((g1=getGenerator())==0 && (g2=dh.getGenerator()))||
         g1 & g2==0) return dontknow;

// If both words are shorter than w, 
// the current history is better if it has a larger weight difference,
// but otherwise dh is at least as good as the other, so we return no.

    const WtSLDiffHistoryVtxRep& ddh = (WtSLDiffHistoryVtxRep&)dh;
    if (g1==0 && g2==0){
      if (wtdiff > ddh.wtdiff) return yes;
      else return no;
    }

// If both words are the same length as w, we return yes if the
// current DiffHistoryVtx has larger weight difference than dh,
// or if it has the same weight difference as dh and corresponds 
// to a word which precedes w lexicographically, but dh does not.
// Otherwise dh is just as good as the current DiffHistoryVtx, so
// we return no.
     
    if ((wtdiff>ddh.wtdiff) ||
    (wtdiff==ddh.wtdiff && lexComp==1 && ddh.lexComp== -1))
      return yes;
    else return no; 
  }

  Bool possibleReductionAhead() const { return (wtdiff > 1);}

  void printOn(ostream &ostr = cout) const 
  {
    DiffHistoryVtxRep::printOn(ostr);
    ostr << " lexComp="<< lexComp <<" wtdiff="<<wtdiff<<"; ";
  }
  
  inline friend ostream& operator << 
        ( ostream& ostr, const WtSLDiffHistoryVtxRep& dh ) {
    DiffHistoryVtxRep ddh = (DiffHistoryVtxRep)dh;
    ostr << ddh << ", lexComp="<< dh.lexComp <<" wtdiff="<<dh.wtdiff<<"; ";
    return ostr;
  }
  
  
private:
  int lexComp;
  int wtdiff;
};

class WtLexDiffHistoryVtxRep : public DiffHistoryVtxRep {
public:
//constructors
  WtLexDiffHistoryVtxRep(): DiffHistoryVtxRep(),lexComp(0), wtdiff(0){};
  WtLexDiffHistoryVtxRep(State D,int G,DiffHistoryVtx * ptr,int L,
                         int C, int WD):
    DiffHistoryVtxRep(D,G,ptr,L),lexComp(C), wtdiff(WD) {};
//copy constructor
  WtLexDiffHistoryVtxRep(const WtLexDiffHistoryVtxRep & dh) 
    : DiffHistoryVtxRep(dh), lexComp(dh.lexComp), wtdiff(dh.wtdiff) {};
  DiffHistoryVtxRep *clone() const { 
    return new WtLexDiffHistoryVtxRep(*this); } 
//destructor
  ~WtLexDiffHistoryVtxRep() {};
//accessors
  int getLexComp() const { return lexComp;};
  int getWtDiff() const { return wtdiff;};

  //operators
/*
  int operator== ( const DiffHistoryVtxRep & dh) const
  {
LOOK in FSA hierarchy
    const WtLexDiffHistoryVtxRep& ddh = (WtLexDiffHistoryVtxRep&)dh;
    return (SOMETHING && lexComp = ddh.lexComp && wtdiff == ddh.wtdiff);
  }
*/
 
  DiffHistoryVtxRep&  operator= ( const DiffHistoryVtxRep & dh )
  {
    (WtLexDiffHistoryVtxRep)*this = dh;
    const WtLexDiffHistoryVtxRep& ddh = (WtLexDiffHistoryVtxRep&)dh;
    lexComp = ddh.lexComp; wtdiff = ddh.wtdiff;
    return *this;
  }

   Trichotomy betterThan(const DiffHistoryVtxRep & dh) const {
// Only to be called if the word differences match.
// If the word differences match,
// returns yes if whenever dh predicts a reduction, the current
// DiffHistoryVtx predicts one too and there could be situations
// where the current DiffHistoryVtx predicts a reduction but dh 
// does not, no if dh predicts a
// reduction whenever the current DiffHistoryVtx does,
// dontknow if neither of the above is true (or it seems too much work 
// to find out).

    if (getDiff()!=dh.getDiff()) return dontknow;

    int g1,g2;

// If one DiffHistoryVtx corresponds to a shorter word and one to a word
// the same length as w, neither can be considered to be better than
// the other
    if (((g1=getGenerator())==0 && (g2=dh.getGenerator()))||
         g1 & g2==0) return dontknow;

// If both words are shorter than w, 
// the current history is better if it has a larger weight difference,
// but otherwise dh is at least as good as the other, so we return no.

    const WtLexDiffHistoryVtxRep& ddh = (WtLexDiffHistoryVtxRep&)dh;
    if (g1==0 && g2==0){
      if (wtdiff > ddh.wtdiff) return yes;
      else return no;
    }

// If both words are the same length as w, we return yes if the
// current DiffHistoryVtx has larger weight difference than dh,
// or if it has the same weight difference as dh and corresponds 
// to a word which precedes w lexicographically, but dh does not.
// Otherwise dh is just as good as the current DiffHistoryVtx, so
// we return no.
     
    if ((wtdiff>ddh.wtdiff) ||
    (wtdiff==ddh.wtdiff && lexComp==1 && ddh.lexComp== -1))
      return yes;
    else return no; 
  }


  Bool possibleReductionAhead() const { 
     return (wtdiff>1 ||(lexComp==1 && wtdiff > 0));}

  void printOn(ostream &ostr = cout) const 
  {
    DiffHistoryVtxRep::printOn(ostr);
    ostr << " lexComp="<< lexComp <<" wtdiff="<<wtdiff<<"; ";
  }
  
  inline friend ostream& operator << 
        ( ostream& ostr, const WtLexDiffHistoryVtxRep& dh ) {
    DiffHistoryVtxRep ddh = (DiffHistoryVtxRep)dh;
    ostr << ddh << ", lexComp="<< dh.lexComp <<
          " wtdiff="<<dh.wtdiff<<"; ";
    return ostr;
  }
  
  
private:
  int lexComp;
  int wtdiff;
};


#endif

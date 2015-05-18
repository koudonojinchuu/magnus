// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of DiffMachineRep class.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//
// * 8/96 Dmitry B. made porting to gcc 2.7.2.
//

#include <iostream.h>
#include <string.h>
#include "Set.h"
#include "Word.h"
#include "Vector.h"
#include "DiffMachineRep.h"
#include "WordOrder.h"
#include "DiffHistory.h"
#include "DFSAParser.h"

void DiffMachineRep::readFrom(istream &str)
{
   DFSAParser P(str,WORDS,YES);
   P.parseDFSA(this);
   P.setDFSA((GroupDFSARep *)this); // need to make sure we use the 
                              // GroupDFSARep version of the function
   diffs = P.getStateWordLabels();
}

void DiffMachineRep::printAccepting(ostream & str) const
{
  str << "       accepting := [1]";
}

void DiffMachineRep::printStates(ostream &str) const
{
  int numOfStates = getNumStates();
  int numOfGens = getNumGenerators();
  int numOfSymbols = getNumSymbols();
  int i;
  str << "          states := rec("<< endl;
  str << "                type := \"words\","<< endl;
  str << "                size := "<< numOfStates << "," << endl;
  str << "            alphabet := [";
  for (i=1;i<=numOfSymbols;i++){
      str << getSymbolName(i);
      if (i<numOfSymbols) str << ",";
      else str << "]," << endl;
  }
  str << "              format := \"sparse\","<< endl;
  str << "               names := ["<<endl;
  for (i=1;i<=numOfStates;i++) {
    str << "                ["<< i << ",";
    GAPprintWord(str,diffs[i]);
    if (i<numOfStates) str << "],"<<endl;
    else str <<"]"<< endl;
  }
  str << "              ]"<< endl;
  str << "               )";
}

void DiffMachineRep::setNumStates(int numOfStates)
{
  //@sr, what about the word differences?
  int oldNumStates = getNumStates();
  if (oldNumStates<=numOfStates){
    for (int i=oldNumStates+1;i<=numOfStates;i++){
      Word w; 
      diffs.append(w); // initially each word difference has the default 
                       // value of the identity word
    }
  }
  else diffs.shrink(0,numOfStates+1);
  DFSARep::setNumStates(numOfStates);
}

void 
DiffMachineRep::addDifferencesEqn
        (const Word & w1,const Word & w2,const WordOrder & word_order)
{
  State s,t;
  s = startState();
  int len1 = w1.length();
  int len2 = w2.length();
  int len = (len1 > len2 ? len1 : len2 );
  int i=0;
  Generator x,xx,y;
  Word wd;
  for (i=0;i<len1 && i<len2;i++){
    x=w1[i];
    xx = (selfInverse(x) ? x : inv(x));
    y=w2[i];
    t=target(s,x,y);
    if (t==failure()){
      if (i==len-1) t=startState();
      else {
        wd = xx*getDifference(s)*y;
        rewrite(wd,word_order); 
        for (t=1;t<=getNumStates();t++)
          if (getDifference(t)==wd) break;
        if (t>getNumStates()){
          t=newState();
          setDifference(t,wd);
        }
      }
      setTarget(s,x,y,t);
    }
    s=t;
  }
  for (i=len1;i<len2;i++){
    y=w2[i];
    t=targetOn2ndString(s,y);
    if (t==failure()){
      if (i==len-1) t=startState();
      else {
        wd = getDifference(s)*y;
        rewrite(wd,word_order); 
        for (t=1;t<=getNumStates();t++)
          if (getDifference(t)==wd) break;
        if (t>getNumStates()){
          t=newState();
          setDifference(t,wd);
        }
        setTargetOn2ndString(s,y,t);
      }
      s=t;
    }
  }
  for (i=len2;i<len1;i++){
    x=w1[i];
    xx = (selfInverse(x) ? x : inv(x));
    t=targetOn1stString(s,x);
    if (t==failure()){
      if (i==len-1) t=startState();
      else {
        wd = xx*getDifference(s);
        rewrite(wd,word_order); 
        for (t=1;t<=getNumStates();t++)
          if (getDifference(t)==wd) break;
        if (t>getNumStates()){
          t=newState();
          setDifference(t,wd);
        }
        setTargetOn1stString(s,x,t);
      }
    }
    s=t;
  }
}

void 
DiffMachineRep::closeUnderSubstrings(int mode,const WordOrder & word_order)
// mode could be 1, limited transitions or 2 all possible transitions
{
  State s,ss,t,u,v,vv;
  int i,j,k;
  int numGens = generatorNames.length();
  Word w0,w,ww;

  // Word through all the states in order (of course the number of
  // states should increase as we work through the list).
  for (s=1;s<=getNumStates();s++){
    Bool tNew=no, uNew=no;
    int oldNumStates=getNumStates();
    w0 = getDifference(s);
    int len = w0.length();
    if (len <2) continue;
    Generator a = w0[0];
    Generator b=w0[len-1];
    b= (selfInverse(b) ? b : inv(b));
    // b is the inverse of the last generator of w0, or the last generator
    // itself if it's an involution, and the inverse symbol isn't in the 
    // alphabet
    
    if ((t=targetOn2ndString(s,b))==0){
      t=newState();
      tNew=yes;
      if (t%100==0){
        cout << "Building new state, numbered "<< t << 
        " while dealing with  state "<< s << endl;
        cout << "New word difference has length " << len-1 << endl;
      }
      setDifference(t,w0.initialSegment(len-1));
// we put in the obvious transitions between s and t now, because in mode 1
// we don't want to add all possible transitions
      setTargetOn2ndString(s,b,t);
      b= (selfInverse(b) ? b : inv(b));
      setTargetOn2ndString(t,b,s);
    }
    if ((u=targetOn1stString(s,a))==0){
      u=newState();
      uNew=yes;
      if (u%100==0){
        cout << "Building new state, numbered "<< u << 
        " while dealing with  state "<< s << endl;
        cout << "New word difference has length " << len-1 << endl;
      }
      setDifference(u,w0.terminalSegment(len-1));
// we put in the obvious transitions between s and u now, because in mode 1
// we don't want to add all possible transitions
      setTargetOn1stString(s,a,u);
      a= (selfInverse(a) ? a : inv(a));
      setTargetOn1stString(u,a,s);
    }
    if (tNew && u==t) setTargetOn1stString(s,a,u);
    
    if (tNew==0 && uNew==0) continue;

    if (mode==1) continue;
    // in mode 1 we don't want to add in any new transitions between t and u and
    // the other states other than those we've already added.

    // If either t or u is new compute transitions from the other states into
    // t or u.
    for (ss=1; ss<=oldNumStates;ss++){
    
      w=getDifference(ss);
      for (i=1;i<=numGens;i++){
        Generator x(i); 
        Generator xx= (selfInverse(x)? x : inv(x)); 
// xx is the inverse of x, or x itself if x is an involution
        for (j=1;j<=numGens;j++){
          Generator y(j); 
          Generator yy= (selfInverse(y)? y: inv(y));
// yy is the inverse of y, or y itself if y is an involution

          if (target(ss,x,y)==0){
            ww = xx*w*y;
            rewrite(ww,word_order);
            if (tNew && ww==getDifference(t))
              setTarget(ss,x,y,t);
            else if (uNew && ww==getDifference(u))
              setTarget(ss,x,y,u);
          }

          if (yy!= y && target(ss,x,yy)==0){
            ww = xx*w*yy;
            rewrite(ww,word_order);
            if (tNew && ww==getDifference(t))
              setTarget(ss,x,yy,t);
            else if (uNew && ww==getDifference(u))
              setTarget(ss,x,yy,u);
          }

          if (xx==x) continue;

          if (target(ss,xx,y)==0){
            ww = x*w*y;
            rewrite(ww,word_order);
            if (tNew && ww==getDifference(t))
              setTarget(ss,xx,y,t);
            else if (uNew && ww==getDifference(u))
              setTarget(ss,xx,y,u);
          }
        
          if (yy!=y && target(ss,xx,yy)==0){
            ww = x*w*yy;
            rewrite(ww,word_order);
            if (tNew && ww==getDifference(t))
              setTarget(ss,xx,yy,t);
            else if (uNew && ww==getDifference(u))
              setTarget(ss,xx,yy,u);
          }

        }

        if (targetOn1stString(ss,x)==0){
          ww = xx*w;
          rewrite(ww,word_order);
          if (tNew && ww==getDifference(t))
            setTargetOn1stString(ss,x,t);
          else if (uNew && ww==getDifference(u))
            setTargetOn1stString(ss,x,u);
        }

        if (xx==x) continue;

        if (targetOn1stString(ss,xx)==0){
          ww = x*w;
          rewrite(ww,word_order);
          if (tNew && ww==getDifference(t))
            setTargetOn1stString(ss,xx,t);
          else if (uNew && ww==getDifference(u))
            setTargetOn1stString(ss,xx,u);
        }
      }

      for (j=1;j<=numGens;j++){
        Generator y(j); 
        Generator yy= (selfInverse(y)? y : inv(y));

        if (targetOn2ndString(ss,y)==0){
          ww = w*y;
          rewrite(ww,word_order);
          if (tNew && ww==getDifference(t))
            setTargetOn2ndString(ss,y,t);
          else if (uNew && ww==getDifference(u))
            setTargetOn2ndString(ss,y,u);
        }

        if (yy==y) continue;

        if (targetOn2ndString(ss,yy)==0){
          ww = w*yy;
          rewrite(ww,word_order);
          if (tNew && ww==getDifference(t))
            setTargetOn2ndString(ss,yy,t);
          else if (uNew && ww==getDifference(u))
            setTargetOn2ndString(ss,yy,u);
        }
      }
    }

    // If t or u is new, compute transitions from t or u.

    if (tNew) v=t; else if (uNew) v=u; else v=0;

    while (v) {
    
      w=getDifference(v);
      /*cout << "New state "<<v<< "w "<< w << endl;*/
      for (i=1;i<=numGens;i++){
        Generator x(i); 
        Generator xx= (selfInverse(x) ? x : inv(x));
        for (j=1;j<=numGens;j++){
          Generator y(j); 
          Generator yy= (selfInverse(y) ? y : inv(y));

          ww = xx*w*y;
          /*cout << "ww "<< ww << endl;*/
          rewrite(ww,word_order);
          vv=0;
          for (ss=1;ss<=getNumStates();ss++){
            if (ww==getDifference(ss)){
              vv=ss;
              break;
            }
          }
          setTarget(v,x,y,vv);

          if (yy!=y){
            ww = xx*w*yy;
            rewrite(ww,word_order);
            vv=0;
            for (ss=1;ss<=getNumStates();ss++){
              if (ww==getDifference(ss)){
                vv=ss;
                break;
              }
            }
            setTarget(v,x,yy,vv);
          }

          if (xx==x) continue;

          ww = x*w*y;
          rewrite(ww,word_order);
          vv=0;
          for (ss=1;ss<=getNumStates();ss++){
            if (ww==getDifference(ss)){
              vv=ss;
              break;
            }
          }
          setTarget(v,xx,y,vv);
        
          if (yy!=y){
            ww = x*w*yy;
            rewrite(ww,word_order);
            vv=0;
            for (ss=1;ss<=getNumStates();ss++) {
              if (ww==getDifference(ss)){
                vv=ss;
                break;
              }
            }
            setTarget(v,xx,yy,vv);
          }
        }

        ww = xx*w;
        rewrite(ww,word_order);
        vv=0;
        for (ss=1;ss<=getNumStates();ss++) {
          if (ww==getDifference(ss)){
            vv=ss;
            break;
          }
        }
        setTargetOn1stString(v,x,vv);

        if (xx==x) continue;

        ww = x*w;
        rewrite(ww,word_order);
        vv=0;
        for (ss=1;ss<=getNumStates();ss++) {
          if (ww==getDifference(ss)){
            vv=ss;
            break;
          }
        }
        setTargetOn1stString(v,xx,vv);

      }

      for (j=1;j<=numGens;j++){
        Generator y(j); 
        Generator yy=(selfInverse(y)? y: inv(y));;

        ww = w*y;
        rewrite(ww,word_order);
        vv=0;
        for (ss=1;ss<=getNumStates();ss++) {
          if (ww==getDifference(ss)){
            vv=ss;
            break;
          }
        }
        setTargetOn2ndString(v,y,vv);

        if (yy==y) continue;

        ww = w*yy;
        rewrite(ww,word_order);
        vv=0;
        for (ss=1;ss<=getNumStates();ss++) {
          if (ww==getDifference(ss)){
            vv=ss;
            break;
          }
        }
        setTargetOn2ndString(v,yy,vv);

      }
      if (v==t && uNew) v=u;
      else break;
    }
  }

}

void 
DiffMachineRep::closeUnderInverses(const WordOrder & word_order)
{
  bool collapse=no;
  int numDiffs = getNumStates();
  int i,j;
  VectorOf<int> inverse(2*numDiffs+1);
  VectorOf<int> reduction(2*numDiffs+1);
// we make those arrays long, because of the new states which may need
// to be defined, to correspond to the inverses of existing word differences
  inverse[1]=1; 
  for (i=2;i<=2*numDiffs;i++) inverse[i]=0;
  do {
    Word w;
    State s,ss,t,tt,u,uu;
    int numDiffs = getNumStates();
    if (collapse){ cerr <<
      "After loop in inverse closure number of states is "
                       <<numDiffs << endl;
    }
    for (i=0;i<reduction.length();i++) reduction[i]=i;
    int numSyms = getNumSymbols();
    collapse=no;
    for (s=1;s<=getNumStates();s++){
      if ((ss=inverse[s])==0){
        w=word_order.inverse(getDifference(s));
        rewrite(w,word_order);
        ss = newState();
        setDifference(ss,w);
        inverse[s]=ss; 
        inverse[ss]=s;
      }
      else if (inverse[s]<s) continue; // in that case this pair of
                         // word differences has already been considered.
      for (i=1;i<=numSyms;i++){
          for (j=1;j<=numSyms;j++){
  
            t=targetInt(s,i,j);
            u=targetInt(ss,j,i);
            if (t==0 && u==0) continue;
            else if (t==0){
              if (inverse[u]==0){
                w=word_order.inverse(getDifference(u));
                rewrite(w,word_order);
                t = newState();
                setDifference(t,w);
                inverse[t]=u; 
                inverse[u]=t;
              }
              else t=inverse[u];
              setTargetInt(s,i,j,t);
            }
            else if (u==0){
              if (inverse[t]==0){
                w=word_order.inverse(getDifference(t));
                rewrite(w,word_order);
                u = newState();
                setDifference(u,w);
                inverse[t]=u; 
                inverse[u]=t;
              }
              else u=inverse[t];
              setTargetInt(ss,j,i,u);
            }
            else {
              if (inverse[t]==0) 
                inverse[t]=u;
              if (inverse[u]==0) 
                inverse[u]=t;
              if ((tt=inverse[t])!=u){
/*
                cerr << "Found a coincidence. Inverse of "
                            << t << "is not "<< u << endl;
*/
                collapse=yes;
                uu=u;
                while (reduction[tt]<tt) tt=reduction[tt];
                while (reduction[uu]<uu) uu=reduction[uu];
                if (tt<uu) reduction[uu]=tt;
                else if (tt>uu) reduction[tt]=uu;
/*
                cerr << tt << "=" << uu << endl;
*/
              }
              if ((uu=inverse[u])!=t){
/*
              cerr << "Found a coincidence. Inverse of "
                            << u << "is not "<< t << endl;
*/
                collapse=yes;
                tt=t;
                while (reduction[tt]<tt) tt=reduction[tt];
                while (reduction[uu]<uu) uu=reduction[uu];
                if (tt<uu) reduction[uu]=tt;
                else if (tt>uu) reduction[tt]=uu;
/*
                cerr << tt << "=" << uu << endl;
*/
              }
            } 
          }
          t=targetIntOn1stString(s,i);
          u=targetIntOn2ndString(ss,i);
          if (t==0 & u==0) continue;
          else if (t==0){
            if (inverse[u]==0){
              w=word_order.inverse(getDifference(u));
              rewrite(w,word_order);
              t = newState();
              setDifference(t,w);
              inverse[t]=u; 
              inverse[u]=t;
            }
            else t=inverse[u];
            setTargetIntOn1stString(s,i,t);
          }
          else if (u==0){
            if (inverse[t]==0){
              w=word_order.inverse(getDifference(t));
              rewrite(w,word_order);
              u = newState();
              setDifference(u,w);
              inverse[t]=u; 
              inverse[u]=t;
            }
            else u=inverse[t];
            setTargetIntOn2ndString(ss,i,u);
          }
          else {
            if (inverse[t]==0) 
              inverse[t]=u;
            if (inverse[u]==0) 
              inverse[u]=t;
            if ((tt=inverse[t])!=u){
/*
            cerr << "Found a coincidence. Inverse of "
                          << t << "is not "<< u << endl;
*/
              collapse=yes;
              uu=u;
              while (reduction[tt]<tt) tt=reduction[tt];
              while (reduction[uu]<uu) uu=reduction[uu];
              if (tt<uu) reduction[uu]=tt;
              else if (tt>uu) reduction[tt]=uu;
/*
                cerr << tt << "=" << uu << endl;
*/
            }
            if ((uu=inverse[u])!=t){
/*
            cerr << "Found a coincidence. Inverse of "
                          << u << "is not "<< t << endl;
*/
              collapse=yes;
              tt=t;
              while (reduction[tt]<tt) tt=reduction[tt];
              while (reduction[uu]<uu) uu=reduction[uu];
              if (tt<uu) reduction[uu]=tt;
              else if (tt>uu) reduction[tt]=uu;
/*
                cerr << tt << "=" << uu << endl;
*/
            }
          } 
        }
  
        for (j=1;j<=numSyms;j++){
  
          t=targetIntOn2ndString(s,j);
          u=targetIntOn1stString(ss,j);
          if (t==0 & u==0) continue;
          else if (t==0){
            if (inverse[u]==0){
              w=word_order.inverse(getDifference(u));
              rewrite(w,word_order);
              t = newState();
              setDifference(t,w);
              inverse[t]=u; 
              inverse[u]=t;
            }
            else t=inverse[u];
            setTargetIntOn2ndString(s,j,t);
          }
          else if (u==0){
            if (inverse[t]==0){
              w=word_order.inverse(getDifference(t));
              rewrite(w,word_order);
              u = newState();
              setDifference(u,w);
              inverse[t]=u; 
              inverse[u]=t;
            }
            else u=inverse[t];
            setTargetIntOn1stString(ss,j,u);
          }
          else {
            if (inverse[t]==0) 
              inverse[t]=u;
            if (inverse[u]==0) 
              inverse[u]=t;
            if ((tt=inverse[t])!=u){
/*
            cerr << "Found a coincidence. Inverse of "
                          << t << "is not "<< u << endl;
*/
              collapse=yes;
              uu=u;
              while (reduction[tt]<tt) tt=reduction[tt];
              while (reduction[uu]<uu) uu=reduction[uu];
              if (tt<uu) reduction[uu]=tt;
              else if (tt>uu) reduction[tt]=uu;
/*
                cerr << tt << "=" << uu << endl;
*/
            }
            if ((uu=inverse[u])!=t){
/*
            cerr << "Found a coincidence. Inverse of "
                          << u << "is not "<< t << endl;
*/
              collapse=yes;
              tt=t;
              while (reduction[tt]<tt) tt=reduction[tt];
              while (reduction[uu]<uu) uu=reduction[uu];
              if (tt<uu) reduction[uu]=tt;
              else if (tt>uu) reduction[tt]=uu;
/*
                cerr << tt << "=" << uu << endl;
*/
            }
          } 
        }
     }
     numDiffs = getNumStates();
// sort out the `holes' in the array reduction.
    s=1; ss=1;
    while (s<=numDiffs){
      if (reduction[s]<s){  
        reduction[s]=reduction[reduction[s]]; s++; continue; }
      reduction[s]=ss; s++; ss++; 
    }
    s=1;ss=1;
    while (s<=numDiffs){
      if (reduction[s]<ss){ s++; continue;}
      for (i=1;i<=numTransits;i++)
        setTargetInt(ss,i,reduction[targetInt(s,i)]);
      setDifference(ss,getDifference(s));
      inverse[ss]=reduction[inverse[s]];
      s++; ss++;
    }
    numDiffs = ss-1;
    if (collapse==1) 
// we don't have to worry about the inverse array unless we loop
      for (s=numDiffs+1;s<inverse.length();s++)
        inverse[s]=0;
    setNumStates(numDiffs);
  } while (collapse);
}
  
struct vertex { 
  Generator gen; int state; int len;
  struct vertex *backptr;
}; 

Bool
DiffMachineRep::rewrite(Word & w) const
{
// This function is based on Derek Holt's algorithm in the automatic groups
// software.

// We read through the word w which is to be reduced, one generator at a time.
// Let x be the generator in position posn, and let z be the generator
// in position posn+1
// Where a is any prefix of the word w finishing before that position,
// all word differences equal to a^-1b where b has the same length as a have 
// been found, and form the vertices of a tree. 
// At the vertex corresponding to a^-1b, the corresponding state of the
// word difference machine, the length of b and the last generator of b
// is stored. The length of b is signed, with a negative sign indicating that
// a<b in shortlex and a positive sign indicating that b<a.
// In addition if a!=b a back pointer points back to the vertex corresponding to
// a'^-1b' where a' and b' are the maximal prefixes of a and b 
//
// Thus by tracing backwards from the vertex we identify b and reduce w
// by replacing the latter part of the substring a by the latter part of b
// (that is, not touching the common prefixes of a and b)

// After the generator x is read we loop through all word differences
// a^-1b such that a is a prefix of w of length exactly posn, 
// looking for a possible
// reduction of a*x (or of the next longest prefix of w) and at the same
// time we compute word differences of the form (a*x)^-1(b*y) ready
// for the next round.

// Let s0 be the state of the difference machine corresponding to a^-1b,
// and let v0 be the corresponding vertex of the tree.
// If the pair (x,_) maps s0 to the start state, then a*x reduces to b,
// if the pair (x,_) followed by (z,_) maps s0 to the start state,
// then a*x*z reduces to b.
// If (x,y) maps s0 to the start state and b<a in short lex,
// then b*y<a*x and a*x reduces to b*y,
// otherwise (a*x)^-1(b*y) should be added to the tree if it's not 
// already there, and if it is there (and so is equal to some 
// (a*x)^-1*b') but with a negative sign attached, then if b*y<a*x
// the vertex should be modified.
// 
// After a substitution we backtrack in w to a point just before the beginning
// of the substitution, and discard vertices in the tree which come from
// longer prefixes of w.
// When we make a substitution, we have to go back one position more than
// might be expected, because of our policy of looking ahead for substitutions
// that reduce the length by 2.
//
// We continue reading from left to right in w until we reach the end of w.
// The Vector vlist stores the vertices of the tree, as found. 
// The Bool Vector seen records which states of the difference machine
// correspond to vertices in the current tree.
// LastV[n] stores the index in vlist of the last vertex corresponding to
// a prefix of w of length n, for use in backtracking. 
//  
  Bool ans=NO;
  int lastv,diffct,len,oldlen,
      newlen,i,j,posn;
  char atStart,donesub;
  State s0,s;
  vertex * v0, * v;
  VectorOf<vertex*> vlist(0,YES);

  int numOfStates = getNumStates();
  int numOfGens = getNumGenerators();

  Bool * seen = new Bool [numOfStates+1];

  if (w.length()==0) return(0);
  Word id;

  lastv= -1; 
  int * Lastv = new int [w.length()+1];
  Lastv[0]= -1;
// Lastv[n]+1 is the number of vertices that have been defined 
// after reading the first n elements of the word. These vertices 
// are vlist[0],...,vlist[Lastv[n]].

// Now we start reading the word. 
  posn = -1;
  while (++posn<w.length()) { 
    for (i=1;i<=numOfStates;i++) seen[i]=NO;
    Generator x= w[posn];
    int g=ord(x);
// The next loop is over the word differences a^-1b such that a is the prefix
// of the word up to the entry in position posn-1
    s0 = startState();
    while (1) { 
      atStart= (s0==startState());
// First look for a possible substitution of a shorter string 
      s = targetOn1stString(s0,x);
      if (s==startState()){ 
// word so far is equal to a shorter word whose length is one less than itself. 
// Make substitution.
        ans = YES;
        len=0;
        if (!atStart)
        { v=v0;
          len = abs(v->len);
          VectorOf<Generator> ww(len);
          i=len-1;
          do
          { ww[i--]= v->gen;
            v=v->backptr;
          } while (v);
          w = w.replaceSubword(posn-len,posn+1,(Word)ww);
        }
        else{
          w = w.replaceSubword(posn,posn+1,id);
        }
        posn= posn>len ? posn-len-2 : -1;
        if (lastv > (i=Lastv[posn+1])){
          while (lastv>i){
            v=vlist[lastv--];
            delete v;
          }
          vlist.shrink(0,lastv+1);
        }
        break;
      }
      else if (s && posn<w.length()-1){
        if (targetOn1stString(s,w[posn+1])==startState()){
// word so far is equal to a shorter word whose length is two less than itself. 
// Make substitution.
          ans = YES;
          len=0;
          if (!atStart)
          { v=v0;
            len = abs(v->len);
            VectorOf<Generator> ww(len);
            i=len-1;
            do
            { ww[i--]= v->gen;
              v=v->backptr;
            } while (v);
            w = w.replaceSubword(posn-len,posn+2,(Word)ww);
          }
          else {
            w = w.replaceSubword(posn,posn+2,id);
          }
          posn= posn>len ? posn-len-2 : -1;
          if (lastv > (i=Lastv[posn+1])){
            while (lastv>i){
              v=vlist[lastv--];
              delete v;
            }
            vlist.shrink(0,lastv+1);
          }
          break;
        }
      }

      donesub=0;
// Now we loop over the generator that is a candidate for substitution
// at this point.
      int h=0;
      while (h!= -numOfGens){
        if (h<=0) h--; h *= -1; // we run through generators in shortlex order
        Generator y(h);
        if (s=target(s0,x,y)) { 
          if (s==startState()) { 
            ans = YES;
            if (atStart) { 
              if (g!=h && (abs(h)<abs(g) || (g== -h && h>0))) {
                w = w.replaceSubword(posn,posn+1,(Word)y);
                x=y; g=h;
              }
            }
            else if (v0->len > 0) {
// Make a substitution (by a string of equal length). 
              v=v0;
              len = abs(v->len);
              VectorOf<Generator> ww(len+1);
              ww[len]=y;
              i=len-1;
              do
              { ww[i--]= v->gen;
                v=v->backptr;
              } while (v);
              w = w.replaceSubword(posn-len,posn+1,(Word)ww);
              posn= posn>len ? posn-len-2 : -1;
              donesub=1;
              if (lastv > (i=Lastv[posn+1])){
                while (lastv>i){
                  v=vlist[lastv--];
                  delete v;
                }
                vlist.shrink(0,lastv+1);
              }
              break;
            }
          }
          else {
            if (seen[s]) for (i=Lastv[posn]+1;;i++)
    // We have this word difference stored already, but we will check to see if
    // the current string precedes the existing one.
            { v=vlist[i];
              if (v->state == s)
              { oldlen=v->len;
                newlen= atStart ? 
                    ((g!=h && (abs(h)<abs(g) || (g== -h && h>0))) ? 1 : -1) : 
                         (j=(v0->len))>0 ? j+1 : j-1;
                if (newlen>oldlen)
    // The new string is better than the existing one 
                { v->gen=y;
                  v->len=newlen;
                  v->backptr= atStart ? 0 : v0;
                }
                break;
              }
            }
            else
    // This is a new word difference at this level, so we define a new vertex in
    // graph.
            { lastv++;
    // Define the new vertex. 
              v=new vertex;
              newlen= atStart ? 
                 ((g!=h && (abs(h)<abs(g) || (g== -h && h>0))) ? 1 : -1) : 
                         (j=(v0->len))>0 ? j+1 : j-1;
              v->gen=y; v->state=s;
              v->len=newlen;
              v->backptr= atStart ? 0 : v0;
              vlist.append(v);
              seen[s]=YES;
            }
          }
        } 
      }
       
      if (donesub) break;

// Go on to next word difference from previous level. 
      if (atStart)
      { if (posn==0) break;
        diffct=Lastv[posn-1]+1;
      }
      else diffct++;
      if (diffct>Lastv[posn]) break;
      v0 = vlist[diffct];
      s0=v0->state;
    } // end of loop over word differences at previous level 
    Lastv[posn+1]=lastv;
  }

  while (lastv>=0){
    v=vlist[lastv--];
    delete v;
  }
  w = w.freelyReduce();
  return(ans);
}

Bool
DiffMachineRep::rewrite(Word & v,WordOrder const & word_order) const
{
//  This function is a generalisation of Derek Holt's algorithm
//  above which works for any word order.

// We read through the word v which is to be reduced, one generator at
//a time. 
// Let x be the generator in position posn.
// Where a is any prefix of the word v finishing before that position,
// all word differences equal to a^-1b  where u is no longer than
// w have been found, and form the vertices of a tree.
// Let dhv be one such vertex. At dhv is stored
// 1) the corresponding state of the word difference machine, d
// 2) the length of the word w, the maximal suffix of  a such
// that d = w^-1u (for a suffix w of a)
// 3) an integer labelling the last generator y of u, if u has the
// same length as w, otherwise 0 - the integer is h where column
// h of the difference machine represents transitions on y
// 4) a pointer to the a `previous' vertex corresponding to the
// word difference w'^-1u', where w' is the max. prefix of w
// and u' is the max prefix of u if u has the same length as w,
// but otherwise is equal to u. Additional information
// varies according to the word order and indicates the
// relationship between w and u in the order. A structure in
// the DiffHistoryVtx class is used to carry the information.

  Bool ans=NO;
  int len, redlen, i,j,posn;
  int vno; //the position on the queue of the vertex currently being processed
  int lastvno; //the position of the last vertex on the queue
  int firstvno;// the position of the first vertex on the queue in the current
               // layer (if there is one)
  char doneSub=0;
  VectorOf<DiffHistoryVtx *> dhvList(1,YES); // the list of vertices.
   // The first vertex is stored in position 1 rather than 0, and posn. 0 
   // isn't used.
   // The reason for this is simply that the shrink() function won't allow a
   // vector to be shrunk to one of zero length. 
  DiffHistoryVtx * dhv=0, * ptr, * Dhv;
  State d, dd, D;
  State d0=startState();

  int numOfStates = getNumStates();

  Generator x,y;
  int xx,yy; 
// the integers corresponding to the positions of x and y in the
// word order (and also as alphabet symbols for the difference
// machine)

  Bool * seen = new Bool [numOfStates+1];
  Trichotomy bT=dontknow;

  if (v.length()==0) return(0);
  Word id;

  int * LastDhv = new int [v.length()+1];
   
// LastDhv[posn] is the highest index of a vertex defined after reading 
// reading the entries in positions 0..posn of the word. These vertices 
// are dhvList[0],...,dhvList[LastDhv[posn]].
// NB  Don 't think this is the Lastv of the previous function. because it
// isn't (we'd need to translate posn by 1)

// Now we start reading the word. 
  
  posn = -1;
  firstvno=1;
  lastvno=0;
  while (++posn<v.length()) { 

    doneSub=0;
    for (i=1;i<=numOfStates;i++) seen[i]=NO; 
                // currently we're not using this
    Generator x= v[posn];
    int xx=getPosition(x);

// First we look for reductions of x itself, and if there are
// none construct vertices for each x^-1y, y a generator or identity
// which are word differences.

    d = d0;
    dhv=0;
    int len=0;
    if  ((dd=targetIntOn1stString(d,xx))==d0){
     // x reduces to the identity
      v = v.replaceSubword(posn,posn+1,id);
      doneSub=1; ans=yes;
      posn--;
      break;
    }
    else if (dd!=failure()){
      DiffHistoryVtx * ddhv = new DiffHistoryVtx (dd,xx,0,word_order);
    // We check to see if there's another vertex for
    // the same difference already in this layer, If so,
    // then if there is one which predicts a reduction every time that
    // ddhv does we discard ddhv. Otherwise, if there is vertex all of
    // whose reduction predictions are also carried by ddhv, while ddhv
    // might also carry reduction predictions, we remove that vertex
    // from the queue, and add dhv to the end. If neither of these
    // situations clearly occurrs, we add ddhv to the end of the queue. 
    // In the three situations above, the function betterThan returns
    // no, yes, dontknow.
      if (seen[dd]){
        bT=dontknow;
        for (i=firstvno;i<=lastvno;i++){
          if (((Dhv=dhvList[i])->getDiff())!=dd) continue;
          bT=ddhv->betterThan(Dhv);
          if (bT==yes) { 
            while (i<lastvno){ dhvList[i]=dhvList[i+1];i++;}
            dhvList[lastvno]=ddhv;
            break;
          }
          else if (bT==no){ delete ddhv; break;}
      }
        if (bT==dontknow){ dhvList.append(ddhv); lastvno++; }
      }
      else { dhvList.append(ddhv); lastvno++; seen[dd]=yes; }
    }
    for (yy=1;yy<=numSymbols;yy++){
      y = getSymbol(yy);
      if  ((dd=targetInt(d,xx,yy))==d0){
        if (word_order.signature(xx,yy)== -1){
          v=v.replaceSubword(posn,posn+1,(Word)y);
          doneSub=1; ans=yes;
          posn--;
          break;
        }
      }
      else if (dd!=failure()){
        DiffHistoryVtx * ddhv = new DiffHistoryVtx(dd,xx,yy,word_order);
        if (ddhv->possibleReductionAhead()){
          // look for reduction to a longer word
          int zz=1;
          Generator z = getSymbol(zz);
          D=dd;
          Dhv = ddhv;
          Bool ahead=yes;
          int extralen=1;
          do {
            if  ((D=targetInt(D,0,zz))==d0){
              if (Dhv->reduction(0,zz,word_order)){
                redlen = len+1+extralen;
                VectorOf<Generator> w(redlen);
                w[redlen-1]=z;
                extralen--;
                i=redlen-2;
                while (Dhv) {
                  w[i--]= getSymbol(Dhv->getGenerator());
                  if (ahead){
                    ptr=Dhv->getBackptr();
                    // Backtrack until the back pointer is zero,
                    // and delete all vertices on the backtrack ahead of
                    // dhv (and so, including ddhv).
                    if (ptr==dhv) ahead=no;  
                    delete Dhv;
                    Dhv = ptr;
                  }
                  else Dhv = Dhv->getBackptr();
                }
                v = v.replaceSubword(posn,posn+1,(Word)w);
                posn--;
                doneSub=1; ans=yes;
                break;
              }
            }
            else if (D!=failure() && 
              Dhv->possibleReductionAhead(zz,word_order)){
                // move ahead
                Dhv = new DiffHistoryVtx(Dhv,D,0,zz,word_order);
                extralen++;
                zz=1;
                z=getSymbol(yy);
            }
            else { // backtrack as necessary and try another symbol
              while (zz==numSymbols){
                ptr=Dhv->getBackptr();
                // when the back pointer is dhv we've reached dhv, so we
                // should stop backtracking. We delete all vertices we 
                // meet on the backtrack, except for ddhv, which we shall
                // add to our list of vertices, unless we find a reduction
                // later on in this search.
                if (ptr==dhv){
                  ahead=no; break;
                }
                else {
                  zz = Dhv->getGenerator();
                  delete Dhv;
                  Dhv=ptr;
                  extralen--;
                }
              }
              if (ahead==0) break;
              else {
                D = Dhv->getDiff();
                zz++;
                z=getSymbol(zz);
                continue;
              }
            }
          } while (ahead);
        } // end of search for reduction to a longer word
        if (!doneSub) {
    // We check to see if there's another vertex for
    // the same difference already in this layer, If so,
    // as described above, we may delete that vertex from the queue or not, 
    // and may add ddhv to the queue or not, according to the value returned
    // by the function betterThan
          if (seen[dd]){
            bT=dontknow;
            for (i=firstvno;i<=lastvno;i++){
              if (((Dhv=dhvList[i])->getDiff())!=dd) continue;
              bT=ddhv->betterThan(Dhv);
              if (bT==yes) { 
                while (i<lastvno){ dhvList[i]=dhvList[i+1];i++;}
                dhvList[lastvno]=ddhv;
                break;
              }
              else if (bT==no){ delete ddhv; break;}
            }
            if (bT==dontknow){ dhvList.append(ddhv); lastvno++; }
          }
          else { dhvList.append(ddhv); lastvno++; seen[dd]=yes; }
        } 
      }
      if (doneSub) break;
    }
 
    // Now we work through the vertices created at the last round (i.e.
    // those correponding to non-trivial suffices of the prefix
    // a of v which was read before x, and see if one of these
    // points to a reduction of ax. If not we create the next
    // layer of vertices.

    if (doneSub==0 && posn>0) { 
      if (posn==1) vno = 1;
      else vno = LastDhv[posn-2]+1; 
      // the position on the list of the first of the vertices
      // created in the last round.
      while (vno<= LastDhv[posn-1]){
        dhv = dhvList[vno];
        len = dhv->getLength();
        d = dhv->getDiff(); 
        if  ((dd=targetIntOn1stString(d,xx))==d0){
          if (dhv->reduction(xx,0,word_order)){
            redlen = len;
            while (dhv->getGenerator()==0){
              dhv = dhv->getBackptr();
              if ((--redlen)==0) break;
            }
            if (redlen==0)
              v = v.replaceSubword(posn-len,posn+1,id);
            else { 
              VectorOf<Generator> w(redlen);
              i=redlen-1;
              do { 
                w[i--]= getSymbol(dhv->getGenerator());
                dhv=dhv->getBackptr();
              } while (dhv);
              v = v.replaceSubword(posn-len,posn+1,(Word)w);
            } 
            posn = (posn>len-1 ? posn-len-1 : -1);
            doneSub=1; ans=yes;
            break;
          }
        }
        else if (dd!=failure()){
          DiffHistoryVtx * ddhv= new DiffHistoryVtx(dhv,dd,xx,0,word_order);
    // We check to see if there's another vertex for
    // the same difference already in this layer, If so,
    // as described above, we may delete that vertex from the queue or not, 
    // and may add ddhv to the queue or not, according to the value returned
    // by the function betterThan
          if (seen[dd]){
            bT=dontknow;
            for (i=firstvno;i<=lastvno;i++){
              if (((Dhv=dhvList[i])->getDiff())!=dd) continue;
              bT=ddhv->betterThan(Dhv);
              if (bT==yes) { 
                while (i<lastvno){ dhvList[i]=dhvList[i+1];i++;}
                dhvList[lastvno]=ddhv;
                break;
              }
              else if (bT==no){ delete ddhv; break;}
            }
            if (bT==dontknow){ dhvList.append(ddhv); lastvno++; }
          }
          else { dhvList.append(ddhv); lastvno++; seen[dd]=yes; }
        }
        if (dhv->getGenerator()) for (yy=1;yy<=numSymbols;yy++){
          y = getSymbol(yy);
          if  ((dd=targetInt(d,xx,yy))==d0){
            if (dhv->reduction(xx,yy,word_order)){
              VectorOf<Generator> w(len+1);
              w[len]=y;
              i=len-1;
              while (dhv) { 
                w[i--]= getSymbol(dhv->getGenerator());
                dhv=dhv->getBackptr();
              } 
              v = v.replaceSubword(posn-len,posn+1,(Word)w);
              posn= (posn>len-1 ? posn-len-1 : -1);
              doneSub=1; ans=yes;
              break;
            }
          }
          else if (dd!=failure()){
            DiffHistoryVtx * ddhv = new DiffHistoryVtx(dhv,dd,xx,yy,word_order);
            if (ddhv->possibleReductionAhead()){
            // look for a reduction to a longer word
              int zz=1;
              Generator z = getSymbol(zz);
              D=dd;
              Dhv = ddhv;
              Bool ahead=yes;
              int extralen=1;
              do {
                if  ((D=targetInt(D,0,zz))==d0){
                  if (Dhv->reduction(0,zz,word_order)){
                    redlen = len+1+extralen;
                    VectorOf<Generator> w(redlen);
                    w[redlen-1]=z;
                    i=redlen-2;
                    while (Dhv) {
                      w[i--]= getSymbol(Dhv->getGenerator());
                      if (ahead){
                        ptr=Dhv->getBackptr();
                        // Backtrack until the back pointer is zero,
                        // and delete all vertices on the backtrack ahead of
                        // dhv (and so, including ddhv).
                        delete Dhv;
                        Dhv = ptr;
                        if (ptr==dhv) ahead=no;
                      }
                      else Dhv = Dhv->getBackptr();
                    }
                    v = v.replaceSubword(posn-len,posn+1,(Word)w);
                    posn= (posn>len-1 ? posn-len-1 : -1);
                    doneSub=1; ans=yes;
                    break;
                  }
                }
                else if (D!=failure() &&
                  Dhv->possibleReductionAhead(zz,word_order)){
                    // move ahead
                  Dhv = new DiffHistoryVtx(Dhv,D,0,zz,word_order);
                  extralen++;
                  zz=1;
                  z=getSymbol(yy);
                }
                else { // backtrack as necessary and try another symbol
                  while (zz==numSymbols){
                    ptr=Dhv->getBackptr();
                // when the back pointer is dhv we've reached dhv, so we
                // should stop backtracking. We delete all vertices we 
                // meet on the backtrack, except for ddhv, which we shall
                // add to our list of vertices, unless we find a reduction
                // later on in this search.
                    if (ptr==dhv){
                      ahead=no; break;
                    }
                    else {
                      zz = Dhv->getGenerator();
                      delete Dhv;
                      Dhv=ptr;
                      extralen--;
                    }
                  }
                  if (ahead==0) break;
                  else {
                    D = Dhv->getDiff();
                    zz++;
                    z=getSymbol(zz);
                    continue;
                  }
                }
              } while (ahead);
            } // end of search for reduction to a longer word
            
            if (doneSub==1) break;
            else {
    // We check to see if there's another vertex for
    // the same difference already in this layer, If so,
    // as described above, we may delete that vertex from the queue or not, 
    // and may add ddhv to the queue or not, according to the value returned
    // by the function betterThan
              if (seen[dd]){
                bT=dontknow;
                for (i=firstvno;i<=lastvno;i++){
                  if (((Dhv=dhvList[i])->getDiff())!=dd) continue;
                  bT=ddhv->betterThan(Dhv);
                  if (bT==yes) { 
                    while (i<lastvno){ dhvList[i]=dhvList[i+1];i++;}
                    dhvList[lastvno]=ddhv;
                    break;
                  }
                  else if (bT==no){ delete ddhv; break;}
                }
                if (bT==dontknow){ dhvList.append(ddhv); lastvno++; }
              }
              else { dhvList.append(ddhv); lastvno++; seen[dd]=yes; }
            }
          }
        }
        if (doneSub==1) break;
        vno++;
      }
    }
  // at this stage we've either both checked the symbol last read for a
  // reduction of the symbol and also gone through all the vertices made in
  // last round and found no reduction, or we have found a reduction,
  // backtracked to the beginning of it and jumped out of the loop
    if (doneSub){
      if (lastvno!=dhvList.length()-1){
        cerr << "ERROR1: lastvno="<<lastvno<<"Last list position="
                   <<dhvList.length()-1<<endl;
      }
      if (posn== -1){
        while (lastvno>0){
          dhv=dhvList[lastvno--]; delete dhv;
        }
        dhvList.shrink(0,1);
        firstvno=1;
      }
      else {
        while (lastvno>LastDhv[posn]){
          dhv=dhvList[lastvno--]; delete dhv;
        }
        dhvList.shrink(0,LastDhv[posn]+1);
        firstvno=LastDhv[posn]+1;
      }
      if (lastvno!=dhvList.length()-1){
        cerr << "ERROR2: lastvno="<<lastvno<<"Last list position="
                   <<dhvList.length()-1<<endl;
      }
    }
    else {
      LastDhv[posn] = dhvList.length()-1;
      firstvno=LastDhv[posn]+1;
    }
  }    

  if (lastvno!=dhvList.length()-1){
     cerr << "ERROR3: lastvno="<<lastvno<<"Last list position="
               <<dhvList.length()-1<<endl;
  }
  while (lastvno>0){
    dhv=dhvList[lastvno--];
    delete dhv;
  }
  return(ans);
}

GroupDFSA
DiffMachineRep::convertToGroupDFSA(GroupDFSARep * R) const
{
  GroupDFSA D(R);
  return D;
}

GroupDFSA
DiffMachineRep::wordAcceptor(const WordOrder & word_order) const
{
// This function constructs the word acceptor for the set of minimal
// representatives of a weight shortlex ordering, given the word difference
// machine. The order of generators corresponds to the ordering of the
// symbols in the transition table of the fsa, the integer vector `weight'
// specifies the individual weights attached to those generators:
// if all weights are set to 1, the ordering is the
// standard shortlex ordering. 
//
// All the calculation is done in the function buildWordAcceptor below,
// at the GroupDFSARep level
  GroupDFSARep *  Wp = new GroupDFSARep("",getGeneratorNames(),word_order);
  buildWordAcceptor(*Wp,word_order);
  GroupDFSA WA=convertToGroupDFSA(Wp);
  return WA;
}

void
DiffMachineRep::buildWordAcceptor(GroupDFSARep & WA, const WordOrder & word_order) const
{
// This function constructs the word acceptor for the set of minimal
// representatives of a specified linear order, given the word difference
// machine.
//
// The idea:
//
// The state s reached by tracing out a non-trivial accepted word w corresponds to 
// the set of all word differences d equal in G to some w^-1u where u
// is no longer than w, together with some additional info about u and w,
// that is a set of DiffHistory's
// Precisely what additional info is needed depends on the specified 
// linear order:  details of the different kinds of DiffHistory's
// are found in DiffHistoryRep.h.
// 
// The accept state s maps under g to a failure state if
// 1) the target of the start state under g is failure
// - in this case g itself reduces
// or 
// 2) g^-1d =_G id, and examination of the difference history
//    shows that wg is equal to a shorter word which precedes 
//    it in the ordering.
// 3) there exists a generator h with g^-1dh = id, and examination
//     of the difference history shows that
//     wg reduces to uh of the same length, which precedes
//     it in the ordering.
//
// - a YES value returned by the function DiffHistory::reduction(),
//      with arguments (g,0,word_order) in case (2)
//                     (g,h,word_order) in case (3)
//      identifies these possibilities
// or
// 4) there exists a word H (of length >= 2) with H^-1dg = id, 
//    and g^-1d(h_1h_2..h_i) is in D for all prefixes h_1h_2..h_i of H
//    and examination of the difference history shows that
//    wg reduces to uH which is longer, but precedes it in the ordering.
//    
//   - a YES value returned by the function DiffHistory::possibleReductionAhead
//    indicates the need to construct a AheadInfo from the DiffHistory,
//    and then look ahead. An actual reduction is then indicated by a YES
//    value returned from the function reductionAhead
//
// Otherwise s->sg, an accept state, which is again a set of word differences,
// found as products g^-1d and g^-1dh for d in S, as described below.
// A word difference may arise in sg from more than one source, 
// with different associated difference histories.
// The difference history associated with the new difference is
// clearly dependent on all of those.
//
// Given a word difference d (and its associated history) in the set 
// associated with s, if  g^-1d is non-trivial and in D, 
// g^-1d, together with an updated history, is added to sg
// if for some h, g^-1dh is non-trivial and in D,
// g^-1dh, together with an updated history, is added to sg
// In addition the word differences  associated with the target of the startstate
// under g are added to sg.
//
// The target of the start state under g is failure if some generator g is
// equal to the identity, or some product g^-1h, for a second generator h<g,
// or some product g^-1H, for a word H of length >=2, with H preceding g in
// the ordering. for which all products
// g^-1(h_1h_2..h_i) are in D. Otherwise the state is associated with the
// set of all those g and g^-1h above which are actually in D.

// The algorithm:
// the machine is built out from the start state.
// First the targets of the startstate under each generator are constructed
// and put on a queue to be dealt with. From then on, states are taken off
// the queue one at a time, and their targets under each generator computed.  
// Each time a new state is found it is put onto the end of the queue.  
// The word acceptor is complete when the queue is empty.

  WA.setNumStates(1);
  WA.setCategory(WA.startState(),1);

  int numSymbols = WA.getNumSymbols();
  VectorOf< SetOf< DiffHistory > >  stateList(2);
// position 0 is unused, and position 1 corresponds to the start state of WA.
  SetOf<DiffHistory> emptySet;
  stateList[0] = stateList[1] = emptySet;
  State d,dd; // of difference machine
  State s,t,u,v; // of word acceptor
  SetOf<DiffHistory> S;
  SetOf<DiffHistory> Sg;
  DiffHistory stored;
  VectorOf<State> parent(2,YES);
  parent[0]=0;
  parent[1]=1;
  VectorOf<State> renumber(2,YES);
  renumber[0]=0;
  renumber[1]=1;
  VectorOf<State> deadStates(0,YES);
  int g,h;
  Bool fails;
  State d0 = startState(); // start state of the difference machine

  int K=0; //length of longest word difference
  for (int i=0; i<diffs.length(); i++)
    if (diffs[i].length()>K) K = diffs[i].length();
  cerr << "Diff machine has "<<getNumStates() <<" states."<< endl;
  s= WA.startState();
  for (g=1;g<=numSymbols;g++){
    Sg = emptySet;
    fails=NO;
    if  ((dd=targetIntOn1stString(d0,g))==d0)
      fails = YES;
    else if (dd!=failure()){
      DiffHistory ddh(dd,g,0,word_order);
      Sg.adjoinElement(ddh);  
    }
    if (fails==0) for (int h1=1;h1<=numSymbols;h1++){
      if  ((dd=targetInt(d0,g,h1))==d0){
        if (word_order.signature(g,h1)== -1){  fails = YES; break; }
      }
      else if (dd!=failure()){
        DiffHistory ddh(dd,g,h1,word_order);
        if (ddh.possibleReductionAhead()){
          AheadInfo ai(ddh);
          if (reductionAhead(dd,ai,word_order)){
            fails = YES;
            break;
          }
        }
        SetIterator<DiffHistory> J(Sg); 
        while (!J.done()){ 
          stored= J.value();  
          if (stored.getDiff()==ddh.getDiff()) break;  
          J.next();  
        }  
        if (J.done()) Sg.adjoinElement(ddh);  
        else if (!(stored==ddh)){  
          Sg.removeElement(stored);   
          ddh.improveBy(stored);  
          Sg.adjoinElement(ddh);  
        }  
      }
    }
    if (fails==1){
       WA.setTargetInt(s,g,WA.failure());
    }
    else {
      t=0;
      for (int i=2;i<=WA.getNumStates();i++){
        if (stateList[i]==Sg){
          t=i;
          break;
        }
      }
      if (t==0){
        stateList.append(Sg);
        t=WA.newState();
        renumber.append(t);
        parent.append(s);
        WA.setCategory(t,1);
      }
      WA.setTargetInt(s,g,t);
    }
  }

  s = 2;

  while (s <= WA.getNumStates()){
    S = stateList[s];
    for (g=1;g<=numSymbols;g++){
      t=WA.targetInt(startState(),g);
      if (t==WA.failure()) fails = YES;
      else {
        Sg = stateList[t]; // the word differences for proper suffixes of sg
        fails=NO;
        SetIterator<DiffHistory> I(S);
        while (!I.done()){
          DiffHistory dh = I.value();
          d = dh.getDiff();
          if (d==1) {
            int i;
            cout << "What the hell is going on here? " << endl;
            cout << "Trivial word difference for state "<< s << endl;
            cout << "Difference histories held at this state:-"<< endl;
            cout << S;
            cout << endl;
            while (0!=1) { i=0; i=1; i=2; i=3; i=4; i=5;}
          }
          if  ((dd=targetIntOn1stString(d,g))==d0){
            if (dh.reduction(g,0,word_order)){
              fails = YES; break;
            }
          }
          else if (dd!=failure()){
            DiffHistory ddh(dh,dd,g,0,getDifference(dd),word_order);
            if (ddh.empty()==1){ 
/*
              cout << "Difference history obtained from "<< endl
              << dh << endl 
              << " on generator "<< g <<" out of bounds. "<< endl;
*/
              I.next();
              continue;
            }
            SetIterator<DiffHistory> J(Sg);  
            while (!J.done()){ 
              stored= J.value();  
              if (stored.getDiff()==ddh.getDiff()) break;  
              J.next();  
            }  
            if (J.done()) Sg.adjoinElement(ddh);  
            else if (!(stored==ddh)){  
              Sg.removeElement(stored);   
              ddh.improveBy(stored);  
              Sg.adjoinElement(ddh);  
            }  
          }
          if (dh.sameLengthWords()==0){ I.next(); continue;}
          for (h=1;h<=numSymbols;h++){
            if  ((dd=targetInt(d,g,h))==d0){
              if (dh.reduction(g,h,word_order)){
                fails=YES;
                break;
              }
            }
            else if (dd!=failure()){
              DiffHistory ddh(dh,dd,g,h,getDifference(dd),word_order);
              if (ddh.empty()==1){
/*
                cout << "Difference history obtained from "<< endl
                << dh << endl 
          << " on generators "<< g <<" and "<< h <<" out of bounds. "<< endl;
*/
                continue;
              }
              if (ddh.possibleReductionAhead()){
                AheadInfo ai(ddh);
                if (reductionAhead(dd,ai,word_order)){
                  fails = YES;
                  break;
                }
              }
              SetIterator<DiffHistory> J(Sg);  
              while (!J.done()){ 
                stored= J.value();  
                if (stored.getDiff()==ddh.getDiff()) break;  
                J.next();  
              }  
              if (J.done()) Sg.adjoinElement(ddh);  
              else if (!(stored==ddh)){  
                Sg.removeElement(stored);   
                ddh.improveBy(stored);  
                Sg.adjoinElement(ddh);  
              }  
            }
          }
          I.next();
        }
      }
      if (fails==1){
        WA.setTargetInt(s,g,WA.failure());
      }
      else {
        t=0;
        for (int i=2;i<=WA.getNumStates();i++){
          if (stateList[i]==Sg){
            t=renumber[i];
            break;
          }
        }
        if (t==0){
          stateList.append(Sg);
          t=WA.newState();
        if (t%100==0)
          cout << "Building new state, numbered "<< t << 
                 " while finding targets of state "<< s << endl;
          renumber.append(t);
          parent.append(s);
          WA.setCategory(t,1);
        }
        WA.setTargetInt(s,g,t);
      }
    }
    
    u = 0;
    for (g=1;g<=numSymbols;g++){
      t = parent[WA.targetInt(s,g)];
      u = (t>u? t : u);
    }
    for (t=u;t<s;t++){
      if (renumber[t]<t) continue;
      for (g=1;g<=numSymbols;g++)
        if (WA.targetInt(t,g)!=WA.targetInt(s,g)) break;
      if (g==numSymbols+1) // t and s have all the same targets
        break;
    }
    if (t<s && WA.category(s)==WA.category(t)){
      renumber[s]=t;
      // reset all targets to s as targets to t
      for (u=parent[s];u<s;u++){
        for (int g1=1;g1<=numSymbols;g1++){
          v = WA.targetInt(u,g1);
          if (v==s) WA.setTargetInt(u,g1,t);
        }
      }
      WA.clearTransits(s);
      deadStates.append(s);
    }
    s++;
  }
}


Bool
DiffMachineRep::reductionAhead
       (State d,const AheadInfo & ai, const WordOrder & word_order) const
// This function returns true if there is a path from vertex d to the start
// which is labelled by the pair (empty_string,w) for some word w,
// which leads to a reduction.
{
  for (int g=1;g<=numSymbols;g++){
    State dd;
    if (ai.possibleReduction(g,word_order)==0 ||
       (dd = targetIntOn2ndString(d,g))==0 )
      continue;
    if (dd==startState()) return YES;
    else {
      AheadInfo aai(ai,g,word_order);
      if (reductionAhead(dd,aai,word_order)) return YES;
    }
  }
  return NO;
}

void 
DiffMachineRep::buildDifferenceMachine(DiffMachineRep & D,
     const SetOf<Word> & differences,const WordOrder & word_order) const
{
// the set of word differences is not assumed to consist of reduced words
  int i,j;
  int numDiffs=differences.cardinality();
  D.setNumStates(numDiffs);
  int numGens = generatorNames.length();
  SetOf<Word> reducedDifferences; 
    // in case the set differences contains some non-reduced words we make a 
    // reduced copy. It should make the search for differences quicker to 
    // have them in a set.
  SetIterator<Word> I(differences);
  Word w,ww;
  State s,t;
  s=D.startState();
  D.setDifference(s,w); // w is initialised as the identity
  s=2;
// the identity must be the first word difference, 
// so has already been dealt with 
  while (!I.done()){
    w = I.value();
    rewrite(w,word_order);
    if (w.length()>0 && reducedDifferences.contains(w)==0){
      reducedDifferences.adjoinElement(w);
      D.setDifference(s,w);
      s++;
    }
    I.next();
  }
  numDiffs= s-1;
  cerr << "There are "<<numDiffs <<" reduced word differences."<< endl;
  D.setNumStates(numDiffs);  // reset numDiffs
  for (s=1;s<=numDiffs;s++){
    cerr << "Building images of state number "<<s<< endl;
    D.setCategory(s,0); // irrelevant, but maybe it's better to have something set
    w = D.getDifference(s);
    cerr << w << endl;
    for (i=1;i<=numGens;i++){
      Generator x(i); 
      Generator xx= (word_order.selfInverse(x)? x : inv(x));
       // xx is set to the inverse of x unless x is an involution, when it is 
       // set to be x

      for (j=1;j<=numGens;j++){
        Generator y(j); 
        Generator yy= (word_order.selfInverse(y) ? y: inv(y));
       // yy is set to the inverse of y unless y is an involution, when it is 
       // set to be y

        ww = xx*w*y;
        rewrite(ww,word_order); 
        if (ww.length()==0) t=1; 
// the identity is automatically in the set
        else if (reducedDifferences.contains(ww))
          for (t=2;t<=numDiffs;t++){ if (D.getDifference(t)==ww) break;}
        else t=0;
        D.setTarget(s,x,y,t); 

        if (yy!=y){
          ww = xx*w*yy;
          rewrite(ww,word_order);
          t=0;
          if (ww.length()==0) t=1;
          else if (reducedDifferences.contains(ww))
            for (t=2;t<=numDiffs;t++){ if (D.getDifference(t)==ww) break;}
          else t=0;
          D.setTarget(s,x,yy,t);
        }

        if (xx==x) continue;

        ww = x*w*y;
        rewrite(ww,word_order); 
        if (ww.length()==0) t=1;
        else if (reducedDifferences.contains(ww))
          for (t=2;t<=numDiffs;t++){ if (D.getDifference(t)==ww) break;}
        else t=0;
        D.setTarget(s,xx,y,t);
        
        if (yy!=y){
          ww = x*w*yy;
          rewrite(ww,word_order);
          if (ww.length()==0) t=1;
          else if (reducedDifferences.contains(ww))
            for (t=2;t<=numDiffs;t++){ if (D.getDifference(t)==ww) break;}
          else t=0;
          D.setTarget(s,xx,yy,t);
        }
      }

      ww = xx*w;
      rewrite(ww,word_order);
      if (ww.length()==0) t=1;
      else if (reducedDifferences.contains(ww))
        for (t=2;t<=numDiffs;t++){ if (D.getDifference(t)==ww) break;}
      else t=0;
      D.setTargetOn1stString(s,x,t);

      if (xx==x) continue;

      ww = x*w;
      rewrite(ww,word_order);
      if (ww.length()==0) t=1;
      else if (reducedDifferences.contains(ww))
        for (t=2;t<=numDiffs;t++){ if (D.getDifference(t)==ww) break;}
      else t=0;
      D.setTargetOn1stString(s,xx,t);
    }
    for (j=1;j<=numGens;j++){
      Generator y(j); 
      Generator yy=(word_order.selfInverse(y)? y : inv(y));

      ww = w*y;
      rewrite(ww,word_order);
      if (ww.length()==0) t=1;
      else if (reducedDifferences.contains(ww))
        for (t=2;t<=numDiffs;t++){ if (D.getDifference(t)==ww) break;}
      else t=0;
      D.setTargetOn2ndString(s,y,t);

      if (yy==y) continue;

      ww = w*yy;
      rewrite(ww,word_order);
      if (ww.length()==0) t=1;
      else if (reducedDifferences.contains(ww))
        for (t=2;t<=numDiffs;t++){ if (D.getDifference(t)==ww) break;}
      else t=0;
      D.setTargetOn2ndString(s,yy,t);
    }
  }
}

void 
DiffMachineRep::rebuildDifferenceMachine(const WordOrder & word_order)
{
  bool collapse;
  bool rewrites;
  int numTransits = getNumTransits();
  do {
    collapse=no;
    rewrites=no;
    int numDiffs = getNumStates();
    cerr << "Rebuilding difference machine with "<<numDiffs<<" states."<< endl;
    VectorOf<Word> newDiffs(1);
    VectorOf<int> renumber(numDiffs+1);
// renumber is an array in which the new state numbers for word differences
// are stored as they are found. The entries are all the integers in the range 
// 1..lastDiff.
    renumber[0]=0;
    State s,ss,t,u;
    int i;
    int lastDiff=0;
    for (s=1;s<=numDiffs;s++){
        Word w = getDifference(s);
      if (rewrite(w,word_order)) rewrites=yes;
      for (ss=1;ss<=lastDiff;ss++) if (newDiffs[ss]==w) break;
      renumber[s]=ss;
/*
      cerr << "State numbered "<< s<<" renumbered as "<< renumber[s] << endl;
*/
      if (ss>lastDiff){ lastDiff++; newDiffs.append(w); }
    }
    if (rewrites==0) break;
  
    VectorOf<int> reduction(lastDiff+1);
    for (s=0;s<=lastDiff;s++) reduction[s]=s;
// reduction is an array in which further coincidences between word differences
// are stored as they are found.
// It will then be used to renumber states. 
    s=1; ss=1;
    while (s<=numDiffs){
      if (renumber[s]<ss){ 
        for (i=1;i<=numTransits;i++){
          if ((t=renumber[targetInt(s,i)])!=0 && 
                 (u=targetInt(renumber[s],i))!=t){
            collapse=yes;
            if (u==0) setTargetInt(renumber[s],i,t);
            else {
              while (t>reduction[t]) t=reduction[t];
              while (u>reduction[u]) u=reduction[u];
              if (t<u){
                reduction[u]=t;
                if (word_order.signature(newDiffs[u],newDiffs[t])==1)
                  newDiffs[t]=newDiffs[u];
              }
              else if (t>u){
                reduction[t]=u;
                if (word_order.signature(newDiffs[t],newDiffs[u])==1)
                  newDiffs[u]=newDiffs[t];
              }
            }
          }
        }
        s++; continue;
      }
      setDifference(ss,newDiffs[ss]);
      for (i=1;i<=numTransits;i++)
        setTargetInt(ss,i,renumber[targetInt(s,i)]);
      s++; ss++;
    }
// sort out the `holes' in the array reduction.
    s=1; ss=1;
    if (collapse==1) while (s<=lastDiff){
      if (reduction[s]<s){  
        reduction[s]=reduction[reduction[s]]; s++; continue; }
      reduction[s]=ss; s++; ss++; 
    }
    setNumStates(lastDiff);
    s=1; ss=1;
    while (s<=lastDiff){
      if (reduction[s]<ss){ s++; continue;}
      for (i=1;i<=numTransits;i++)
        setTargetInt(ss,i,reduction[targetInt(s,i)]);
      setDifference(ss,getDifference(s));
      s++;
      ss++;
    }
    lastDiff = ss-1;
    setNumStates(lastDiff);
  } while (collapse==1);
}

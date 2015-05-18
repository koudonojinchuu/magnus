// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of KBMachineRep class.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//
// * 08/96 Dmitry B. implemented IPC tools.
//

#ifndef _KBMachineRep_H_
#define _KBMachineRep_H_

#include <Integer.h>

#include "Word.h"
#include "Vector.h"
/*
#include "RKBPackage.h"
*/
#include "DFSARep.h"


class KBMachineRep : public GroupDFSARep {
public:
  KBMachineRep() : 
    GroupDFSARep(),
    numRules(0),
    lhs(0),
    rhs(0),
    length(1,1),
    ruleIndex(1,1) { length[0]=ruleIndex[0]=0;}

  KBMachineRep(const VectorOf<Chars> & genNames) : 
    GroupDFSARep("",genNames),
    numRules(0),
    lhs(1),
    rhs(1), 
    length(1,1),
    ruleIndex(1,1) {length[0]=ruleIndex[0]=0;}

  KBMachineRep(const VectorOf<Chars> & genNames,const WordOrder & word_order) : 
    GroupDFSARep("",genNames,word_order),
    numRules(0),
    lhs(1),
    rhs(1), 
    length(1,1),
    ruleIndex(1,1) {length[0]=ruleIndex[0]=0;}

  KBMachineRep(const VectorOf<Chars> & genNames,int numOfRules) : 
    GroupDFSARep("",genNames),
    numRules(numOfRules),
    lhs(numRules+1), 
    rhs(numRules+1),
    length(1,1),
    ruleIndex(1,1) {length[0]=ruleIndex[0]=0;}

  KBMachineRep(const VectorOf<Chars> & genNames,
                            const WordOrder & word_order,int numOfRules) : 
    GroupDFSARep("",genNames,word_order),
    numRules(numOfRules),
    lhs(numRules+1), 
    rhs(numRules+1),
    length(1,1),
    ruleIndex(1,1) {length[0]=ruleIndex[0]=0;}

  KBMachineRep(const VectorOf<Chars> & genNames,int numOfRules,int numOfStates)
       : GroupDFSARep("",genNames,numOfStates,1),
    numRules(numOfRules),
    lhs(numRules+1), 
    rhs(numRules+1),
    length(numOfStates+1,1),
    ruleIndex(numOfStates+1,1) {
      for (int i=0;i<=numOfStates;i++) length[i]=ruleIndex[i]=0;
    }

  KBMachineRep(const VectorOf<Chars> & genNames,const WordOrder & word_order,
      int numOfRules,int numOfStates)
       : GroupDFSARep("",genNames,word_order,numOfStates,1),
    numRules(numOfRules),
    lhs(numRules+1), 
    rhs(numRules+1),
    length(numOfStates+1,1),
    ruleIndex(numOfStates+1,1) {
      for (int i=0;i<=numOfStates;i++) length[i]=ruleIndex[i]=0;
    }

  ~KBMachineRep() { }
  KBMachineRep( const KBMachineRep& K ) :  // Copy constructor does deep copy.
    GroupDFSARep(K), numRules(K.numRules), lhs(K.lhs), rhs(K.rhs),
    length(K.length), ruleIndex(K.ruleIndex) { }


  KBMachineRep & operator = ( const KBMachineRep & K )
  {
    DFSARep& temp = *this;
    temp = (DFSARep)K;
    numRules = K.numRules;
    lhs = K.lhs; rhs = K.rhs;
    length = K.length; ruleIndex = K.ruleIndex;
    return *this;  
  }


  FSARep *clone() const { return new KBMachineRep(*this); }

  Bool operator == ( const KBMachineRep& K ) const { 
    if (equalDFSA(K)==0) return 0;
    else if (numRules != K.numRules || lhs != K.lhs || rhs != K.rhs)
      return 1;
    else for (State s=1;s<=getNumStates();s++){
      if (length[s]!=K.length[s]) return 0;
      else if (ruleIndex[s]!=K.ruleIndex[s]) return 0;
    }
    return 1;
  }

  Bool accepts(Word w) const {};
  Bool rejectsInState(Word w, int& state) const {};
  Bool nextAcceptedWord(Word& w) const {};

//  Bool nextAcceptedWord(Word w, int*& history) const {}; //@rn bad arg
  void minimize() {};
  
//  void readFrom(istream &str = cin);
  void printOn(ostream &str = cout) const ;
  void oldFormatPrintOn(ostream &str = cout) const ;

  void setNumStates(int i);
  void setNumRules(int i) {numRules = i;};
  int getNumRules() {return numRules;};
  int getLength(State s) const { return length[s];}
  void setLength(State s,int i) { length[s] = i;}
  int getRuleIndex(State s) const { return ruleIndex[s];}
  void setRuleIndex(State s,int i) { ruleIndex[s] = i;}
  Word getLHS(int i) { return lhs[i];}
  Word getRHS(int i) { return rhs[i];}
  void setLHS(int i,Word w) { lhs[i]=w;}
  void setRHS(int i,Word w) { rhs[i]=w;}
  Bool  rewrite(Word & w) const;
  // Now defined as a DFSA function
  //Integer sizeLanguage() const;
  //Bool finiteLanguage() const {return (sizeLanguage() != -1);}


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void write( ostream& ostr ) const
  {
    GroupDFSARep::write(ostr);
    ostr < numRules < lhs < rhs < length < ruleIndex;
  }

  void read( istream& istr ) 
  {
    GroupDFSARep::read(istr);
    istr > numRules > lhs > rhs > length > ruleIndex;
  }


private:

  int numRules;
  VectorOf<Word> lhs;       // @rn Really need to save these?
  VectorOf<Word> rhs;       //@sr Yes - we need them for rewriting.

  VectorOf<int> length;
  // length[s] is the length of a shortest path to state s.

  VectorOf<int> ruleIndex;
  // the prefix of length length[s] of the left hand
  // side of the i-th rule traces a path to the state s
  // if ruleIndex[s] = i.

};
#endif


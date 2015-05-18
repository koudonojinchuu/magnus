// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: The deterministic finite state automaton class
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//

#ifndef _DiffMachine_H_
#define _DiffMachine_H_

#include "Word.h"
#include "Set.h"
#include "Vector.h"
#include "FSA.h"
#include "DFSA.h"
#include "DiffMachineRep.h"

class RKBPackage;
class DiffMachine : public FSA {
// this should surely be derived from DFSA but I can't get the
// constructor to work except by doing this
  friend class RKBPackage; 
  typedef DFSARep::State State;
public:
  DiffMachine( ) : FSA( new DiffMachineRep() ) { };

  DiffMachine( const VectorOf<Chars> genNames ) :
  FSA( new DiffMachineRep( genNames) ) { }

  DiffMachine( const VectorOf<Chars> genNames,const WordOrder & word_order ) :
  FSA( new DiffMachineRep( genNames,word_order) ) { }
  
  void addDifferencesEqn
       (const Word & w1,const Word & w2,const WordOrder & word_order)
    { change()->addDifferencesEqn(w1,w2,word_order);}
  void closeUnderSubstrings(int mode,const WordOrder & word_order)
    { change()->closeUnderSubstrings(mode,word_order);}
  void closeUnderInverses(const WordOrder & word_order)
    { change()->closeUnderInverses(word_order);}
  Bool rewrite(Word & w) const { return look()->rewrite(w); }
  Bool rewrite(Word & w,const WordOrder & word_order) const
    { return look()->rewrite(w,word_order);}
  Word getDifference(State s) const { return look()->getDifference(s);}
  GroupDFSA wordAcceptor(const WordOrder & word_order) const { 
     return look()->wordAcceptor(word_order); } 
  void buildDifferenceMachine(DiffMachine & D,
   const SetOf<Word> & differences, const WordOrder & word_order){
     look()->buildDifferenceMachine(*(D.change()),differences,word_order);
  }
  void rebuildDifferenceMachine(const WordOrder & word_order) {
     change()->rebuildDifferenceMachine(word_order);
  }


protected:
  
  DiffMachine( DiffMachineRep * rep ) : FSA((FSARep *)rep) { }
  // Need to shadow inherited look() and change(), to get right ptr types.
  const DiffMachineRep *look() const { 
    return (DiffMachineRep*)FSA::look(); }
  DiffMachineRep *change() { 
    return (DiffMachineRep*)FSA::change(); }
};
#endif

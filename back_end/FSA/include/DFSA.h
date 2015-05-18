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

#ifndef DFSA_H_
#define DFSA_H_

#include "FSA.h"
#include "DFSARep.h"

class DFSA : public FSA {
typedef DFSARep::State State;
public:
// constructors for DFSA.
// Name is only needed for input/output, and may be set to "" (in which 
// case a non-empty but meaningless string will be assigned for output.

  DFSA( ) : FSA( new DFSARep() ) { }

  DFSA(const VectorOf<Chars> & genNames) : FSA( new DFSARep("",genNames) ) { }

  DFSA(Chars name,const VectorOf<Chars> & genNames) 
     : FSA( new DFSARep(name,genNames) ) { }

  DFSA( const VectorOf<Chars> & genNames, int numOfStates) 
     : FSA( new DFSARep("",genNames, numOfStates,1) ) { }

  DFSA( const Chars & Name, const VectorOf<Chars> & genNames, int numOfStates) 
     : FSA( new DFSARep(Name, genNames, numOfStates,1) ) { }

  DFSA( const VectorOf<Chars> & genNames, int numOfStates, int numOfStrings) 
     : FSA( new DFSARep("", genNames, numOfStates, numOfStrings ) ) { }

  DFSA( const Chars & Name, const VectorOf<Chars> & genNames, int numOfStates, int numOfStrings) 
     : FSA( new DFSARep(Name, genNames, numOfStates, numOfStrings ) ) { }

  State target(State s,Generator g) const { return look()->target(s,g);}
  int getNumStates() const { return look()->getNumStates();}

protected:
  DFSA( DFSARep * rep ) : FSA((FSARep *)rep) { }
  // Need to shadow inherited look() and change(), to get right ptr types.
  const DFSARep *look() const { return (DFSARep*)FSA::look(); }
  DFSARep *change() { return (DFSARep*)FSA::change(); }
};

class GroupDFSA : public FSA {
// A GroupDFSA is a DFSA for which the alphabet is actually the set of
// all symbols of the listed alphabet together with their formal
// inverses. Transits are defined on symbols and their inverses,
// and words in those symbols with both positive and negative exponents
// may be read in.
// This class should be used if the symbols of the FSA are the generators
//(and their inverses) of a finitely generated group.

friend class DiffMachineRep;
typedef DFSARep::State State;

public:
// constructors for GroupDFSA.
  GroupDFSA( ) : FSA( new GroupDFSARep() ) { }

  GroupDFSA(const VectorOf<Chars> & genNames) 
     : FSA( new GroupDFSARep("",genNames) ) { }

  GroupDFSA(const VectorOf<Chars> & genNames, const WordOrder & word_order) 
     : FSA( new GroupDFSARep("",genNames, word_order) ) { }


  GroupDFSA(Chars name,const VectorOf<Chars> & genNames) 
     : FSA( new GroupDFSARep(name,genNames) ) { }

  GroupDFSA(Chars name,const VectorOf<Chars> & genNames,
                                             const WordOrder & word_order) 
     : FSA( new GroupDFSARep(name,genNames,word_order) ) { }

  GroupDFSA( const Chars & Name, const VectorOf<Chars> & genNames, int numOfStates) 
     : FSA( new GroupDFSARep(Name, genNames, numOfStates,1) ) { }

  GroupDFSA( const VectorOf<Chars> & genNames, const WordOrder & word_order,
                                                             int numOfStates) 
     : FSA( new GroupDFSARep("",genNames, word_order,numOfStates,1) ) { }

  GroupDFSA( const VectorOf<Chars> & genNames, int numOfStates, int numOfStrings) 
     : FSA( new GroupDFSARep("", genNames, numOfStates, numOfStrings ) ) { }

  GroupDFSA( const VectorOf<Chars> & genNames, const WordOrder & word_order,
                                            int numOfStates, int numOfStrings) 
     : FSA( new GroupDFSARep("", genNames, word_order, 
                                    numOfStates, numOfStrings ) ) { }

  GroupDFSA( const Chars & Name, const VectorOf<Chars> & genNames, int numOfStates, int numOfStrings) 
     : FSA( new GroupDFSARep(Name, genNames, numOfStates, numOfStrings ) ) { }
  State target(State s,Generator g) const { return look()->target(s,g);}
  int getNumStates() const { return look()->getNumStates();}

protected:
  GroupDFSA( GroupDFSARep * rep ) : FSA((FSARep *)rep) { }
  // Need to shadow inherited look() and change(), to get right ptr types.
  const GroupDFSARep *look() const { return (GroupDFSARep*)FSA::look(); }
  GroupDFSARep *change() { return (GroupDFSARep*)FSA::change(); }
};

#endif

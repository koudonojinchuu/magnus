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

#ifndef _GenMult_H_
#define _GenMult_H_

#include "Word.h"
#include "Set.h"
#include "Vector.h"
#include "FSA.h"
#include "DFSA.h"
#include "GenMultRep.h"

class GenMult : public FSA {
// this should surely be derived from DFSA but I can't get the
// constructor to work except by doing this
public:
  GenMult( ) : FSA( new GenMultRep() ) { };

  GenMult( const VectorOf<Chars> &  genNames ) :
  FSA( new GenMultRep( genNames) ) { }

  GenMult( const VectorOf<Chars> & genNames, const WordOrder & word_order) :
  FSA( new GenMultRep( genNames,word_order) ) { }

protected:
  // Need to shadow inherited look() and change(), to get right ptr types.
  const GenMultRep *look() const { 
    return (GenMultRep*)FSA::look(); }
  GenMultRep *change() { 
    return (GenMultRep*)FSA::change(); }
};
#endif

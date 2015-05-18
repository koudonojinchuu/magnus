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

#ifndef _KBMachine_H_
#define _KBMachine_H_

#include <Integer.h>
#include "Vector.h"
#include "FSA.h"
#include "DFSA.h"
#include "KBMachineRep.h"

class RKBPackage;
class KBMachine : public FSA {
// this should surely be derived from GroupDFSA but I can't get the
// constructor to work except by doing this
  friend class RKBPackage;
public:
  KBMachine( ) : FSA( new KBMachineRep() ) { }

  KBMachine( const VectorOf<Chars> & genNames ) : 
    FSA( new KBMachineRep( genNames ) ) { }

  KBMachine( const VectorOf<Chars> & genNames,const WordOrder & word_order ) : 
    FSA( new KBMachineRep( genNames,word_order) ) { }

  KBMachine( const VectorOf<Chars> & genNames,const WordOrder & word_order,
          int numOfRules) :
    FSA( new KBMachineRep( genNames,word_order,numOfRules) ) { }

  KBMachine( const VectorOf<Chars> & genNames,const WordOrder & word_order,
        int numOfRules, int numOfStates) :
    FSA( new KBMachineRep( genNames,numOfRules,numOfStates) ) { }

  void oldFormatPrintOn(ostream & str=cout) const {  look()->oldFormatPrintOn(str); }
  // for use with old code, which should one day be updated.

  Bool rewrite(Word & w) const { return look()->rewrite(w); }

  //Bool finiteLanguage() const { return look()->finiteLanguage(); }

  //Integer sizeLanguage() const { return look()->sizeLanguage(); }

protected:
  KBMachine( KBMachineRep * rep ) : FSA((FSARep *)rep) { }
  // Need to shadow inherited look() and change(), to get right ptr types.
  const KBMachineRep *look() const { 
    return (KBMachineRep*)FSA::look(); }
  KBMachineRep *change() { 
    return (KBMachineRep*)FSA::change(); }
};
#endif

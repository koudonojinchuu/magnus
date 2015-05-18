// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: The finite state automaton class
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//
// * 08/96 Dmitry B. implemented IPC tools.
//

#ifndef _FSA_H_
#define _FSA_H_

#include <Integer.h>
#include "global.h"
#include "ObjectOf.h"
#include "FSARep.h"

class FSA : public ObjectOf<FSARep> {
public:
  Bool accepts(Word w) const { return look()->accepts(w); }
  Bool rejectsInState(Word w, int& state) const {
	 return look()->rejectsInState(w, state);
  }
  Bool nextAcceptedWord(Word& w) const {
	 return look()->nextAcceptedWord(w);
  }
//Bool nextAcceptedWord(Word w, int*& history) const = 0; // bad arg
  void minimize() { change()->minimize(); }
  Integer sizeLanguage() const { return look()->sizeLanguage();}
  Bool finiteLanguage() const { return look()->finiteLanguage();}
  
  void readFrom(istream &str = cin) { change()->readFrom(str); }
  void printOn(ostream &str = cout) const { look()->printOn(str); }
  void setName(const Chars & name) { change()->setName(name);}
  int getNumStates() const { return look()->getNumStates();}
  int operator == ( const FSA & F ) const {
         return ((look() == F.look()) || (*look() == *F.look()));
  }

  int operator != ( const FSA & F ) const {
         return ((look() != F.look()) && (!(*look() == *F.look())));
  }
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const FSA& fsa )
  {
    fsa.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, FSA& fsa )
  {
    fsa.change()->read(istr);
    return istr;
  }


protected:
  typedef ObjectOf<FSARep> R;
  FSA( FSARep *p ) : R(p) {}
};
#endif

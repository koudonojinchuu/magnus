// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of FSARep class.
//
// Principal Author: Sarah Rees
//
// Status: in progress
//
// Revision History:
//
// * 08/96 Dmitry B. implemented IPC tools.
//

#ifndef _FSA_REP_H_
#define _FSA_REP_H_

#include <Integer.h>
#include "global.h"
#include "RefCounter.h"
#include "Word.h"


class FSARep : public RefCounter {
public:
  virtual ~FSARep() {};
  virtual FSARep *clone() const = 0;
  virtual Bool operator == ( const FSARep& ) const = 0;

  virtual Bool accepts(Word w) const = 0;
  virtual Bool rejectsInState(Word w, int& state) const = 0;
  virtual Bool nextAcceptedWord(Word& w) const = 0;

//virtual Bool nextAcceptedWord(Word w, int*& history) const = 0; //@rn bad arg
  virtual void minimize() = 0;
  virtual Integer sizeLanguage() const = 0;
  virtual Bool finiteLanguage() const = 0;

  virtual void readFrom(istream &str = cin) = 0;
  virtual void printOn(ostream &str = cout) const = 0;
  virtual void setName(const Chars & name)=0;
  virtual int getNumStates() const =0;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  virtual void write( ostream& ostr ) const = 0;

  virtual void read( istream& istr ) = 0;

};

// FSARep *fsaUnion(FSA A1, FSA A2); //friend function
// FSARep *fsaIntersection(FSA A1, FSA A2); //friend function (or operator?)
// other boolean
#endif

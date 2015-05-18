// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition and implementation of utility
//           template <class T> class DCell
//
// DCell is the basic component of externally double linked lists.
//
// Principal Author: Sergey Lyutikov, Roger Needham
//
// Status: Useable.
//
// Revision History:
// 07/96 Alexey M. implemented IPC

#ifndef _DCELL_H_
#define _DCELL_H_

#include <iostream.h>
#include "IPC.h"

template<class T> class DCell {

public:

  T      contents;
  DCell*  previousCell;
  DCell*  nextCell;
  
  DCell() {nextCell = previousCell = NULL; }  
  DCell(const DCell& C) : contents(C.contents) { 
	nextCell = previousCell = NULL;
  }

  DCell(const T& e, DCell* previous = NULL, DCell* next = NULL) :
	contents(e) {
	previousCell = previous;
	nextCell = next;
  }
  
  ~DCell() { nextCell = previousCell = NULL; } // Compiler ~'s contents.
 
 /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const DCell& DC )
  {
    ostr < DC.contents;
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, DCell& DC )
  {
    istr > DC.contents;
    return istr;
  }

};

#endif  // _DCELL_H_

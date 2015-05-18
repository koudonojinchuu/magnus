// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition and implementation of utility
//           template <class T> class Cell
//
// Cell is the basic component of externally linked lists.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:


#ifndef _CELL_H_
#define _CELL_H_

//#include "Word.h"

#include <iostream.h>
#include "IPC.h"
//@njz:
#include "error.h"
//


//@db 2.91

template<class T> class Cell;

template<class T>
ostream& operator < ( ostream& ostr, const Cell<T>& C ) 
{
  C.writeContents( ostr );
  return ostr;
}

template<class T>
istream& operator > ( istream& istr, Cell<T>& C ) 
{
  C.readContents( istr );
  return istr;
}

//@db end 


template<class T> class Cell {

public:

  Cell*  nextCell;

  Cell() : nextCell(NULL), contents(NULL) { }
  
  Cell(const Cell& C) : nextCell(NULL) {
    if (C.contents)
         contents = new T(*(C.contents));
    else
         contents = NULL;
  }
  Cell(const T& e, Cell* next = NULL) : nextCell(next) { contents = new T(e);}
  
  ~Cell() { delete contents; } // Compiler ~'s contents.
  // @stc @rn: why do you nullify?

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < <T>( ostream& ostr, const Cell& C );

  friend istream& operator > <T>( istream& istr, Cell& C );
  
  void readContents(istream& istr);
  
  void writeContents(ostream& ostr) const;
   
  
  inline T getContents() {
        #if ( SAFETY > 0 )
                if (contents)
                    return *contents;
                else
		  //@njz
		  //   		    error("Reading noninicialized contents "
		  //						"in Cell::getContents");
		  error("Reading noninicialized contents Cell::getContents");
                  //
        #endif
        return *contents;
  }

  inline void setContents(const T& t) { 
     if (!contents) {contents = new T(t);}
     else *contents = t;
  }

private :
  T*     contents;
};

#endif

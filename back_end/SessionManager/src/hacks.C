// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:
//
// Principal Authors: Dmitry Pechkin
//
// Status: Under trial.
//
// Revision History:
//

#include "List.h"
#include "Map.h"
//@rn It's all in List.h, so g++ 2.5.8 won't choke on templates.

/////////////////////////////////////////////////////////////////////////
//                                                                     //
// IPC tools:                                                          //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

template <class T>
 void  ListRep<T>::write( ostream& ostr ) const
{
  ostr < len;
  if ( len != 0 ){
      ostr < *root;
      CellType* from = root->nextCell;
      while ( from ) {
        ostr < *from;
        from = from->nextCell;
      }
    }
}

//-------------------------------------------------------------------------//

template <class T>
 void  ListRep<T>::read( istream& istr ) {
    CellType* r;
    while ( root ) {
      r = root->nextCell;
      delete root;
      root = r;
    }
  istr > len;
  if (len ==0 ){
    last = NULL;
    root = NULL;
  }
  else   {
   delete last;
   last = new CellType();
   root = new CellType();
   istr > *root;
   CellType* from = root;
   for (int i=1;i < len;i++){
       from->nextCell = new CellType();
       from = from->nextCell;
       istr > *from;
   }
      last = from;
      last->nextCell = NULL; 
  }
}

template class ListRep<Map>;

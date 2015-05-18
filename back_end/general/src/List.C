// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:
//
// Principal Authors: Stephane Collart, Roger Needham
//
// Status: Useable.
//
// Revision History:
//
// 07/96 Aleksey M. implemented IPC tools

#include "List.h"
#include "Vector.h"
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
//---------------------- class ListIteratorRep ----------------------------------

template <class T>
 void ListIteratorRep<T>::write( ostream& ostr ) const
{
  int curPoint = 1;
  int savCur=1;
  if ( theList.look()->len != 0 ){
      Cell<T>* from = theList.look()->root->nextCell;
      while ( from ) {
        curPoint++;
        if (from == current) { savCur = curPoint;}
        from = from->nextCell;
      }
   ostr < savCur; 
  }
}
template <class T>
 void  ListIteratorRep<T>::read( istream& istr ) {
     int curPos;
     istr > curPos;   
     Cell<T>* from = theList.look()->root;
     for (int i=1; i<curPos;i++) from = from->nextCell;
     delete current;
     current = new Cell<T> ();
     current = from;
}
template class ListRep<int>;
template class ListRep<VectorOf<int> >;
template class ListIteratorRep<int>;

// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:
//
// Principal Authors: Sergey Lyutikov
//
// Status: Useable.
//
// Revision History:
// 07/96 Aleksey M. implemented IPC tools
//

#include "DList.h"


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
template <class T>
 void  DListRep<T>::write( ostream& ostr ) const
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
/*******************************/
template <class T>
 void  DListRep<T>::read( istream& istr ) {
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
       from->nextCell->previousCell = new CellType(*from);
       from = from->nextCell;
       istr > *from;
   }
   last = from;
   last->nextCell  = NULL;
  }
}
//---------------------- class DListIteratorRep ----------------------------------

template <class T>
 void DListIteratorRep<T>::write( ostream& ostr ) const
{
  int curPoint = 1;
  int savCur=1;
  if ( theList.look()->len != 0 ){
      DCell<T>* from = theList.look()->root->nextCell;
      while ( from ) {
        curPoint++;
        if (from == current) { savCur = curPoint;}
        from = from->nextCell;
      }
   ostr < savCur; 
  }
}
template <class T>
 void  DListIteratorRep<T>::read( istream& istr ) {
     int curPos;
     istr > curPos;   
     DCell<T>* from = theList.look()->root;
     for (int i=1; i<curPos;i++) from = from->nextCell;
     delete current;
     current = new DCell<T> ();
     current = from;
}
template class DListIteratorRep<int>;
template class DListRep<int>;



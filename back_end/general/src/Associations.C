// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
//
// 07/96 Aleksey M. implemented IPC tools

#include "Associations.h"
#include "Chars.h"

// @stc all the functions had to be inlined and put in Associations.h because
// of g++ 2.5.8 incapacity to deal with templates.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

template <class Key, class Val> 
void AssociationsRep<Key, Val>::write( ostream& ostr ) const
{
   CellType* scan = theList;
   int len=0;
   while ( scan ) { // Association class does not store the length of it's list
     // so we have to count it and save in a stream
        len++;
	scan = scan->nextCell;
    }
  ostr < len;
  if ( len != 0 ){
      ostr < *theList;
      CellType* from = theList->nextCell;
      while ( from ) {
        ostr < *from;
        from = from->nextCell;
      }
    }
}
// -----------------------------------------------------------------------------------//
template <class Key, class Val>
 void  AssociationsRep<Key,Val>::read( istream& istr ) {
    CellType* r;
    int len;
    while ( theList ) {
      r = theList->nextCell;
      delete theList;
      theList = r;
    }
  istr > len;
  if (len ==0 ){ // If the list was empty, we put it equal to NULL
    theList = NULL;
  }
  else   {
   theList = new CellType();
   istr > *theList;
   CellType* from = theList;
   for (int i=1;i < len;i++){  // Define the list of elements, size of len
       from->nextCell = new CellType();
       from = from->nextCell;
       istr > *from;
   }
  }
}

//---------------------- class AssociationsIteratorRep ----------------------------------

template <class Key, class Val>
void AssociationsIteratorRep<Key, Val>::write( ostream& ostr ) const
{
  int curPoint = 1;
  int savCur=1;
  //@njz
  //     AssociationsRep<Key,Val>::CellType* from = theAssociations.look()->theList->nextCell;
  Cell< Association<Key,Val> >* from = theAssociations.look()->theList->nextCell;
  //
  while ( from ) {
    curPoint++;
    if (from == current) { savCur = curPoint;}
    from = from->nextCell;
  }
  ostr < savCur; 
}


template <class Key, class Val>
 void AssociationsIteratorRep<Key, Val>::read( istream& istr ) {
     int curPos;
     istr > curPos;   
     //@njz
     //     AssociationsRep<Key,Val>::CellType* from = theAssociations.look()->theList;
     Cell< Association<Key,Val> >* from = theAssociations.look()->theList;
     //
     for (int i=1; i<curPos;i++) from = from->nextCell;
     delete current;
     //@njz
     //     current = new  AssociationsRep<Key,Val>::CellType ();
     current = new  Cell< Association<Key,Val> > ();
     //
     current = from;
}

template class AssociationsRep<Chars,int>;
template class AssociationsIteratorRep<Chars,int>;


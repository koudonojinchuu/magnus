// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: implementation of utility
//           template <class T> class Cell
//
// Cell is the basic component of externally linked lists.
//
// Principal Author: 
//
// Status: Useable.
//
// Revision History:

#include "Chars.h"
#include "Word.h"
#include "Vector.h"
#include "Integer.h"
#include "Rational.h"
#include "Map.h"

#include "Cell.h"

template<class T> 
void Cell<T>::readContents( istream& istr )
{
  if (!contents) { contents = new T();}
  istr > *contents;
}

template<class T> 
void Cell<T>::writeContents( ostream& ostr ) const
{
  ostr < *contents;
}


template class Cell<int>;
template class Cell<Chars>;
template class Cell<Word>;
template class Cell<Elt>;
template class Cell<Generator>;
template class Cell<VectorOf<int> >;
template class Cell<Integer>;
template class Cell<Rational>;
//template class Cell<Map>;

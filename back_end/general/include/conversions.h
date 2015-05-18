// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Global template functions for converting between
//           container classes.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
//
// * 96/10/28 @dp fixed `makeListOf<T>( const SetOf<T>& )'.
//
// Discussion:
//
// * We use global functions to both break circular dependence, and to
//   avoid clashes between different requirements on the element types
//   of the various container classes.
//
// * The functions are inlined to work around the lack of template
//   repositories in gcc 2.6.3.
//
// Further implementation steps:
//
// * The other conversions.


#ifndef _CONVERSIONS_H_
#define _CONVERSIONS_H_


#include "List.h"
#include "Set.h"
#include "Vector.h"


template <class T> VectorOf<T> makeVectorOf(const ListOf<T>& L)
{
  VectorOf<T> result( L.length() );

  int i = 0;
  ListIterator< ListOf<T> > I(L);

  while ( !I.done() ) {
    result[i++] = I.value();
    I.next();
  }

  return result;
}


template <class T> VectorOf<T> makeVectorOf(const SetOf<T>& S)
{
  VectorOf<T> result( S.cardinality() );

  int ii = 0;
  SetIterator<T> I(S);

  while ( !I.done() ) {
    result[ii++] = I.value();
    I.next();
  }

  return result;
}


template <class T> SetOf<T> makeSetOf(const VectorOf<T>& V)
{
  int ii = V.length();

  SetOf<T> result( ii );

  while( --ii >= 0 ) result.adjoinElement( V[ii] );

  return result;
}


template <class T> SetOf<T> makeSetOf(const ListOf<T>& L)
{
  SetOf<T> result( L.length() );

  ListIterator< ListOf<T> > I( L );

  while ( !I.done() ) {
    result.adjoinElement( I.value() );
    I.next();
  }

  return result;
}


template <class T> ListOf<T> makeListOf(const VectorOf<T>& V)
{
  ListOf<T> result;

  int i = V.length();

  while ( --i >= 0 ) result.prepend( V[i] );

  return result;
}


template <class T> ListOf<T> makeListOf(const SetOf<T>& S)
{
  ListOf<T> result;

  SetIterator<T> I(S);

  while ( !I.done() ) {
    result.append( I.value() );
    I.next();
  }

  return result;
}

#endif

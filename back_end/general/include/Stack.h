// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition and implementation of classes: Stack<T>.
//
// Principal Author: Sergey Lyutikov
//
// Status: Useable.
//
// Revision History:
//
// Special Notes:
//
// * To instantiate StackOf<T>, class T must have an assignment
//   operator, a copy constructor, an == operator, and destructor.
//
// * Alternative implementation of StackOf class, uses ListRep as
//   the DataRep.
//

#ifndef _STACK_H_
#define _STACK_H_

#include "RefCounter.h"
#include "ObjectOf.h"
#include "List.h"
#include "Cell.h"


template < class T > class StackOf : public ObjectOf< ListRep<T> > {

public:

  // Constructors:

  StackOf( ) : ObjectOf< ListRep<T> >( new ListRep<T>() ) { }
  // Default constructor makes empty stack.

  StackOf( const T& t ) : ObjectOf< ListRep<T> >( new ListRep<T>(t) ) { }
  // Cast constructor T -> StackOf<T>

  // copy constructor, operator=, and destructor supplied by compiler.

  void push( const T& t ) { this->change()->insert( t, 0 ); }

  T pop( ) { 
    T res = this->look()->element( 0 ); 
    this->change()->removeElementOfIndex( 0 ); 
    return res;
  }

  void popAll( ) { 
    while ( this->look()->length() ) this->change()->removeElementOfIndex( 0 ); 
  }

  Bool isEmpty( ) const { return !this->look()->length(); }

  Bool isntEmpty( ) const { return this->look()->length(); }

};

#endif  // _STACK_H_

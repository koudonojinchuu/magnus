// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition and implementation of QueueOf<T> class.
//
// Principal Author: Sergey Lyutikov, Dmitry Pechkin
//
// Status: Useable.
//
// Revision History:
//
// Special Notes:
//
// * To instantiate QueueOf<T>, class T must have an assignment
//   operator, a copy constructor, an == operator, and destructor.
//
// * QueueOf class provides interface of queues for lists.
//
// * @dp Based on implementation of @sl's StackOf<T> class.

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "ObjectOf.h"
#include "List.h"

template < class T > class QueueOf : public ListOf<T> {

public:

  // Constructors:

  QueueOf( ) : ListOf<T>() { }
  // Default constructor makes empty Queue.

  QueueOf( const T& t ) : ListOf<T>(t) { }
  // Cast constructor T -> QueueOf<T>

  // copy constructor, operator=, and destructor supplied by compiler.

  void push( const T& t );// { append(t); }

  T pop( ) {
    T res = this->element( 0 );
    this->removeElementOfIndex( 0 );
    return res;
  }

  void popAll( ) {
    while ( this->length() ) this->removeElementOfIndex( 0 );
  }

  Bool isEmpty( ) const { return !this->length(); }

  Bool isntEmpty( ) const { return this->length(); }

};

#endif  // _QUEUE_H_

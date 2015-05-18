//   Copyright (C) 2000 The New York Group Theory Cooperative
//   See magnus/doc/COPYRIGHT for the full notice.
 
// Contents: 
//
// Principal Author: Dmitry Bormotov
//
// Status: tentative (to avoid gcc 2.95 bug under Solaris)
//
// Revision History:
//


#include "Queue.h"
#include "Word.h"
#include "VertexInfo.h"

template <class T>
void QueueOf<T>::push( const T& t ) 
{ 
  append(t); 
}

template class QueueOf<int>;
template class QueueOf<Word>;
template class QueueOf<VertexInfo>;



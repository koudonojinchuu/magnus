// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementations of class EasyList. 
//
// Principal Author:  Alexei Myasnikov
//
// Status: In development
//
// Usage:
//
// Discussion:
//
// Revision History:
//
// Next Implementation Steps:
//


#include "EasyList.h"
#include "error.h"
#include "CosetTables.h"

template <class T>
 EasyList<T>::EasyList( const EasyList& lr )
{
  
  if ( lr.root == NULL )
    current = root = last = NULL;
  else {
    EasyCell<T>* to = root = new EasyCell<T>( lr.root->content() );
    if (lr.root == lr.current)
      current = root;
    EasyCell<T>* from = lr.root->next();
    while ( from ) {
      to->setNext( new EasyCell<T>( from->content() ));
      to = to->next();
      if (from == lr.current)
	current = to;	
      from = from->next();
    }
    last = to;
    last->setNext( NULL); // Better safe than sorry
    end_was_red = lr.end_was_red;
  }
}


template <class T>
 bool EasyList<T>::operator == ( const EasyList& l ) const
{
    EasyCell<T>* scan1 = root;
    EasyCell<T>* scan2 = l.root;
    while ( scan1!=NULL && scan2 != NULL ) {
      if (scan1->content() != scan2->content())
	return false;
      if ((current == scan1 && l.current != scan2) ||
	  (current != scan1 && l.current == scan2))
	return false;
      scan1 = scan1->next();
      scan2 = scan2->next();
    }
    if ( scan1!=NULL || scan2 != NULL)
      return false;
    return true;
}

template <class T>
 EasyCell<T>* EasyList<T>::getCurrent() const
{
  // Do not allow to read from empty list
  if (!current)
    error ("Current was not initialized. Probably because of empty list.");
  EasyList<T>* This = (EasyList<T>* )this;
  // If read last element set flag to true
  if (!end_was_red && current == last)
    This->end_was_red = true;
  return current;
}

template <class T>
 bool EasyList<T>::next()
{
  //Can't iterate empty list
  if (!current)
    return false;
  // If not end, then move to the next
  if (current->next() != NULL){
    current = current->next();
    return true;
  }
  else 
    return false;
}

template <class T>
 void EasyList<T>::prepend(const T& t)
{
  EasyCell<T>* newRoot = new EasyCell<T>(t,root);
  root = newRoot;
  if (!current) current = root;
  if (!last) last = root;
}


template <class T>
 void EasyList<T>::append(const T& t)
{
  EasyCell<T>* newLast = new EasyCell<T>(t);
  if (last){
    last->setNext(newLast);
  }
  last = newLast; 
  if (!current) current = last;
  if (!root) root = last;
  end_was_red = false;
}


template <class T>
 void EasyList<T>::insertAfter(const T& t)
{
  if (!current || current == last)
    append(t);
  else {
    EasyCell<T>* newCell = new EasyCell<T>(t,current->next());
    current->setNext(newCell);
  }
}

template <class T>
 void EasyList<T>::insertBefore(const T& t)
{
  if (!current || current == root)
    prepend(t);
  else {
    T savCurrentContents = current->content();
    EasyCell<T>* savCurrentNext = current->next();
    current->setContent(t);
    current->setNext( new EasyCell<T>(savCurrentContents,savCurrentNext));
    current = current->next();
  }
}

template <class T>
 bool EasyList<T>::deleteFirst()
{
  if (root){
    EasyCell<T>* savRoot = root;
    root = root->next();
    if (current == savRoot)
      current = root;
    if (last == savRoot)
      last = root; 
    delete savRoot;
    return true;
  }
  return false;
}

template <class T>
 bool EasyList<T>::deleteCurrent()
{
  if (current){
    if (current == last){
      deleteLast();
      return true;
    }
    current->setContent(current->next()->content());
    EasyCell<T>* savNext = current->next();
    current->setNext(savNext->next());
    if (last == savNext)
      last = current;
    delete savNext;
    return true;
  }
  return false;
}

template <class T>
 bool EasyList<T>::deleteAfter()
{
  if (!current)
    return false;
  if (current->next()){
    EasyCell<T>* savNext = current->next();
    current->setNext( savNext->next());
    if (last == savNext)
      last = current;
    delete savNext;
    return true;
  }
  return false;
}

template <class T>
 bool EasyList<T>::setCurrentToIndex(int i)
{
  if (root){
    EasyCell<T>* from = root;
    for (int k=0;k<i;k++){
      if (from->next())
	from = from->next();
      else{
	current = from;
	return false;
      }
    }
    current = from;
    end_was_red = false;
    return true;
  }
  return false;
}

template <class T>
 bool EasyList<T>::setCurrent(EasyCell<T>* setpoint)
{
  if (root && last){
    end_was_red = false;
    current = setpoint;
    return true;
  }
  return false;
}
template <class T>
 bool EasyList<T>::search(const T& t, EasyCell<T>** p ) const
{
  EasyCell<T>* from = root;
  while (from){
    if (from->content() == t){
      *p = from;
      return true;
    }
    from = from->next();
  }
  return false;  
}

template <class T>
 bool EasyList<T>::deleteLast()
{
  if (last == root && root != NULL) {
    delete root;
    current = last = root = NULL;
    return true;
  }
  if (last && root){
    EasyCell<T>* from;
    if (current != last)
      from = current;
    else
      from = root;
    while (from->next() != last)
      from = from->next();
    last = from;
    if (current == from->next())
      current = from;
    delete from->next();
    last->setNext( NULL);
    return true;
  }
  return false;
}
template <class T>
 void EasyList<T>::printOn(ostream& ostr) const
{
  EasyCell<T>* from = root;
  while (from){
    ostr << from->content();
    if (current == from) ostr << "(cur)";
    if (from == last)
      ostr << ".";
    else
      ostr << "->";
    from = from->next();
  }
  ostr << endl;
}
//template class EasyList<int>;
template class EasyList< CosetRow* >;
template class EasyList< int >;


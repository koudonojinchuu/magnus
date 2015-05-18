// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of Set and SetIterator class.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
//
// * 01/96 Dmitry B. implemented IPC tools.
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#include "Set.h"
#include "Chars.h"

template <class T>
int SetData<T>::hashElement(const T & t) const { return t.hash();}

template <class T>
void SetData<T>::write( ostream& ostr ) const
{
#ifdef DEBUG
    ostr < Chars("Set<") < ' ';
#endif    

  ostr < numberOfElements < numberOfBuckets;

  for ( int i = 0; i < numberOfBuckets; i++ ) {

    CellType* scan = hashTable[i];

    if( scan == NULL )
      ostr < 0;
    else
      ostr < 1;
       
    while ( scan != NULL ) {
      ostr < *scan;
      scan = scan->nextCell;
      if( scan == NULL )
	ostr < 0;
      else
	ostr < 1;
    }
  }

#ifdef DEBUG
    ostr < ' ' < Chars(">Set");
#endif    
}


template <class T>
void SetData<T>::read( istream& istr )
{
  if ( hashTable ) {
    removeAllElements();
    delete [] hashTable;
    hashTable = NULL;
  }
  
#ifdef DEBUG
  {
    Chars header;
    istr > header;
    if( header != Chars("Set<") ) {
      error("Cannot load Set<T>: bad header of the data section.");
    }
  }
#endif

  istr > numberOfElements > numberOfBuckets;
  hashTable = new CellType*[numberOfBuckets];
  for( int i = 0; i < numberOfBuckets; i++ ) {
    int n;
    istr > n;
    if( n == 0)
      hashTable[i] = NULL;
    else {
      bool bFirst = true;
      CellType *scan, *old;
      
      while( true ) {
	scan = new CellType();
        istr > *scan;

	if( bFirst ) {
	  hashTable[i] = scan;
	  bFirst = false;
	}
	else
	  old->nextCell = scan;
	
	istr > n;
	if( n == 0 ) {
	  scan->nextCell == NULL;
	  break;
	}
	old = scan;
      }
    }
  }

#ifdef DEBUG
  {
    Chars footer;
    istr > footer;
    if( footer != Chars(">Set") ) {
      error("Cannot load Set<T>: bad footer of the data section.");
    }
  }
#endif
}


template <class T>
void SetOf<T>::shrinkToIntersectionWith(const SetOf<T>& S)
{
//@@rn  SetIterator< SetOf<T> > I(*this);
  SetIterator<T> I(*this);
  while ( !I.done() ) {
	 if ( !S.contains( I.value() ) ) removeElement( I.value() );
	 I.next();
  }
}


template <class T>
void SetOf<T>::adjoinElements(const SetOf<T>& S)
{
//@@rn  SetIterator< SetOf<T> > I(S);
  SetIterator<T> I(S);
  while ( !I.done() ) {
	 adjoinElement( I.value() );
	 I.next();
  }
}


template <class T>
void SetOf<T>::removeElements(const SetOf<T>& S)
{
//@@rn  SetIterator< SetOf<T> > I(S);
  SetIterator<T> I(S);
  while ( !I.done() ) {
	 removeElement( I.value() );
	 I.next();
  }
}


template <class T>
Bool SetOf<T>::contains(const SetOf<T>& S) const
{
//@@rn  SetIterator< SetOf<T> > I(S);
  SetIterator<T> I(S);
  while ( !I.done() ) {
	 if ( !contains( I.value() ) ) return 0;
	 I.next();
  }
  return 1;
}

/*
template <class T>
SetOf<T> setUnion(const SetOf<T>& S1, const SetOf<T>& S2)
{
  if ( S1.cardinality() < S2.cardinality() ) {
	 SetOf<T> result = S2;
//@@rn	 SetIterator< SetOf<T> > I(S1);
	 SetIterator<T> I(S1);
	 while ( !I.done() ) {
		result.adjoinElement( I.value() );
		I.next();
	 }
	 return result;
  } else {
	 SetOf<T> result = S1;
//@@rn	 SetIterator< SetOf<T> > I(S2);
	 SetIterator<T> I(S2);
	 while ( !I.done() ) {
		result.adjoinElement( I.value() );
		I.next();
	 }
	 return result;
  }
}


template <class T>
SetOf<T> setIntersection(const SetOf<T>& S1, const SetOf<T>& S2)
{
  SetOf<T> result;
  if ( S1.cardinality() < S2.cardinality() ) {
//@@rn	 SetIterator< SetOf<T> > I(S1);
	 SetIterator<T> I(S1);
	 while ( !I.done() ) {
		if ( S2.contains( I.value() ) )
		  result.adjoinElement( I.value() );
		I.next();
	 }
  } else {
//@@rn	 SetIterator< SetOf<T> > I(S2);
	 SetIterator<T> I(S2);
	 while ( !I.done() ) {
		if ( S1.contains( I.value() ) )
		  result.adjoinElement( I.value() );
		I.next();
	 }
  }
  return result;
}


template <class T>
SetOf<T> setMinus(const SetOf<T>& S1, const SetOf<T>& S2)
{
  SetOf<T> result;
//@@rn  SetIterator< SetOf<T> > I(S1);
  SetIterator<T> I(S1);
  while ( !I.done() ) {
	 if ( !S2.contains( I.value() ) )
		  result.adjoinElement( I.value() );
	 I.next();
  }
  return result;
}


template <class T>
SetOf<T> setSymmetricDifference(const SetOf<T>& S1, const SetOf<T>& S2)
{
  SetOf<T> result;
//@@rn  SetIterator< SetOf<T> > I1(S1);
  SetIterator<T> I1(S1);
  while ( !I1.done() ) {
	 if ( !S2.contains( I1.value() ) )
		  result.adjoinElement( I1.value() );
	 I1.next();
  }
//@@rn  SetIterator< SetOf<T> > I2(S2);
  SetIterator<T> I2(S2);
  while ( !I2.done() ) {
	 if ( !S1.contains( I2.value() ) )
		  result.adjoinElement( I2.value() );
	 I2.next();
  }
  return result;
}
*/

//int SetData<int>::hashElement(const int & t) const { return t; }
// A special hashElement function for integers,
// which cannot have a member function.

//void SetData<int>::write( ostream& ostr ) const { }
//void SetData<int>::read( istream& istr ) { }

//@rn  Since the template class SetOf has out-of-line member functions, we
//@rn  must explicitly instantiate every SetOf class we want. We do not have
//@rn  the option o finlining the functions in question. Hopefully this will
//@rn  change with gcc 2.7.x.


//template class SetOf<int>;

#include "Word.h"
template class SetData<Word>;
template class SetOf<Word>;

template class SetData<Elt>;
template class SetOf<Elt>;

#include "../../FSA/include/StatePair.h"
template class SetOf<StatePair>;
template <> void SetData<StatePair>::write( ostream& ostr ) const { }
template <> void SetData<StatePair>::read( istream& istr ) { }


#include "../../KB/include/DiffHistory.h"
//template class SetData<DiffHistory>;
template class SetOf<DiffHistory>;
template <> void SetData<DiffHistory>::write( ostream& ostr ) const { }
template <> void SetData<DiffHistory>::read( istream& istr ) { }


#include "../../SessionManager/include/OID.h"
//template class SetData<OID>;
template class SetOf<OID>;
template <> void SetData<OID>::write( ostream& ostr ) const { }
template <> void SetData<OID>::read( istream& istr ) { }

template class SetData<Generator>;
template class SetOf<Generator>;

//@dp & @ep
template <> int SetData<Chars>::hashElement(const Chars& t) const { return t.length();}
template class SetOf<Chars>;

#include "Integer.h"
template <> int SetData<Integer>::hashElement(const Integer & t) const 
  { return t.as_long(); }
template class SetData<Integer>;
template class SetOf<Integer>;

#include "Rational.h"
template <> int SetData<Rational>::hashElement(const Rational & t) const 
  { return t.numerator().as_long(); }
template class SetData<Rational>;
template class SetOf<Rational>;

/*@db
#include "KState.h"
int SetData<KState>::hashElement(const KState & s) const 
  { return s[0]; }
template class SetData<KState>;
template class SetOf<KState>;
*/

#include "Map.h"
template <> int SetData<Map>::hashElement(const Map & M) const 
  { return M.generatingImages(0).length(); }

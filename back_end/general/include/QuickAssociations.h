// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition and implementation of
//           template <class Key, class Val> class QuickAssociationsOf
//
// QuickAssociationsOf implements an associative array, currently with O(1)
// access time.
//
// This is dp's remake of Associations class. QuickAssociations
// class is a combination of Associations with Set (more exactly,
// Associations interface is combined with SetData).
//
//
// Principal Authors: Stephane Collart, Roger Needham, Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
// * @dp 1995/07 added: int QuickAssociationsOf<Key,Val>::cardinality() const
//
// Special remarks:
//
// * Requirements: Key and Val must have copy constructors and destructors,
//   Val must have operator=, and Key must have operator==.
//   Because an QuickAssociationsOf method returns a ListOf<Key>, 
//   and ListOf wants to output its elements using <<, there must also be an
//   ostream& operator << ( ostream&, const Type& ),
//   and a conversion of Key to Type.
//   @rn `inherited' requirements like this bear further consideration.
//
//   @dp Key must have hash() member function.
//
// * This copies Keys and Vals around, so non-user classes are less efficent.
//
// * The iterator template is parameterized by Key and Val, rather than
//   container class type, so g++ 2.5.8 will buy it. We'll switch to the
//   latter someday. Code which uses iterators will then break, but will be
//   easy to fix.
//
// * Val QuickAssociationsOf<Key,Val>::operator [ ] ( const Key& ) const
//   may be enhanced later to act as an lvalue. No code will break.
//
// * I left out the possibly handy method
//     Key QuickAssociationsOf<Key,Val>::keyOf( const Val& ) const
//   so as not to require Val to have operator==.
//
// * The return values of, e.g., valueOf would be more efficient as
//   references, but that seems to make the reference count go bad.
//
//


#ifndef _QUICK_ASSOCIATIONS_H_
#define _QUICK_ASSOCIATIONS_H_


#include "RefCounter.h"
#include "ObjectOf.h"
#include "List.h"
#include "Set.h"

#include <iostream.h>


//---------------------- class QuickAssociation -------------------------

template <class Key, class Val> class QuickAssociationsRep;

template <class Key, class Val> class QuickAssociation {
public:
  QuickAssociation(const Key& k, const Val& v) : key(k) { val = new Val(v); }
  
  QuickAssociation(const QuickAssociation& qa) : key(qa.key) {
    if( qa.val )
      val = new Val(*qa.val);
    else
      val = 0;
  }

  QuickAssociation& operator=( const QuickAssociation& qa ) {
    if( this != &qa ) {
      key = qa.key;
      if( val ) {
	if( qa.val )
	  *val = *qa.val;
	else {
	  delete val;
	  val = 0;
	}
      }
      else {
	if( qa.val ) 
	  val = new Val(*qa.val);
	else
	  val = 0;
      }
    }
    return *this;
  }

  ~QuickAssociation() {
    if( val ) {
      delete val;
      val = 0;
    }
  }

  int hash() const { return SetData<Key>(key).hashElement(key); }

  friend inline int operator ==(const QuickAssociation<Key,Val>& x, const QuickAssociation<Key,Val>& y) {
    return x.key == y.key; 
  }

  Key key;
  Val *val; 
private:
  friend class QuickAssociationsRep<Key,Val>;
  
  QuickAssociation(const Key& k) : key(k), val(0) { }
};

template <class Key, class Val>
inline ostream& operator <<(ostream& o, const QuickAssociation<Key,Val>& qa)
{
  o << qa.key << " : " << qa.val << " ";
  return o;
}


//---------------------- class QuickAssociationsRep --------------------------


template <class Key, class Val> class QuickAssociationsIteratorRep;
// Forward declaration for friend declarations.

template <class Key, class Val>
class QuickAssociationsRep : public SetData< QuickAssociation<Key,Val> > {

  typedef QuickAssociation<Key,Val> EltType;

public:

  QuickAssociationsRep( int size = 1 ) : Base (size) {}

  QuickAssociationsRep( const QuickAssociationsRep& ar ) : Base(ar) {}
  // Usual deep copy.

  // destructor supplied by compiler

  QuickAssociationsRep* clone( ) const { return new QuickAssociationsRep<Key,Val>(*this); }

  void unbind( const Key& k ) {
    EltType *elt = 0;
    if( seek(k, elt) ) {
      removeElement( *elt );
      delete elt;
    }
  }

  void bind( const Key& k, const Val& v ) {
    unbind(k);
    adjoinElement( EltType(k, v) );
  }

  Val val( const Key& k ) const {
    EltType *elt;
    seek(k, elt);
#if SAFETY > 0
    if ( !elt )
      error("called QuickAssociationsOf<Key,Val>::operator [ ] with unbound Key");
#endif
    Val value = *elt->val;
    delete elt;
    return value;
  }

  bool bound( const Key& k ) const {
    EltType *elt;
    if( seek(k, elt) ) {
      delete elt;
      return true;
    }
    return false;
  }


private:

  typedef SetData< QuickAssociation<Key,Val> > Base;

  friend class QuickAssociationsIteratorRep<Key,Val>;

  //@njz
  //  const bool seek( const Key& k, EltType* &elt ) const {
  //    EltType qa(k);
  //    int index = abs(hashElement(qa)) % numberOfBuckets;
  //    Base::CellType* search = hashTable[index];
  //    while ( search ) {
  //      if ( k == search->getContents().key ) {
  //	elt = new EltType(search->getContents());
  //	return true;
  //      }
  //      search = search->nextCell;
  //    }
  //    elt = 0;
  //    return false;
  //  }


  const bool seek( const Key& k, EltType* &elt ) const {
    EltType qa(k);
    int index = abs(hashElement(qa)) % this->numberOfBuckets;
    Cell< QuickAssociation<Key,Val> > * search = this->hashTable[index];
    while ( search ) {
      if ( k == search->getContents().key ) {
	elt = new EltType(search->getContents());
	return true;
      }
      search = search->nextCell;
    }
    elt = 0;
    return false;
  }
  //

  // No data members.

  //

};

//------------------- class QuickAssociationsOf --------------------------//


template <class Key, class Val> struct QuickAssocRef;

template <class Key, class Val>
class QuickAssociationsOf : public ObjectOf< QuickAssociationsRep<Key,Val> > {

public:

  QuickAssociationsOf( ) : ObjectOf<Rep>(new Rep()) { }
  // Default constructor makes an empty set of associations.

  // Copy constructor, operator=, and destructor supplied by compiler.

  Val operator [ ] ( const Key& k ) const { return valueOf(k); }
  // Operator synonym for valueOf.

  QuickAssocRef<Key,Val> operator [ ] ( const Key& k );

  Val valueOf( const Key& k ) const { return this->look()->val(k); }
  // Retrieves the Val associated with k. It is a fatal error if k is not
  // bound to any Val.

  void bind( const Key& k, const Val& v ) { this->change()->bind(k, v); }
  // Associates k with v. Overwrites any existing association.

  void unbind( const Key& k ) { this->change()->unbind(k); }
  // Breaks any association k has; not an error if none.

  bool bound( const Key& k ) const { return this->look()->bound(k); }
  // Says whether k is associated with anything.

  int cardinality() const { return this->look()->cardinality(); }

  ListOf<Key> keys( ) const;
  // Returns a list of all keys in this association list.
  // A similar function returning all values would require Val to
  // have an operator==.


private:

  typedef QuickAssociationsRep<Key,Val> Rep;

  friend class QuickAssociationsIteratorRep<Key,Val>;
};


//------------------------ struct QuickAssocRef ---------------------------//


template <class Key, class Val> struct QuickAssocRef {
  QuickAssociationsOf<Key,Val>& asref;
  const Key key;
  QuickAssocRef( QuickAssociationsOf<Key,Val>& a, const Key& k ) : asref(a), key(k) { }
  const Val& operator = ( const Val& val )
  { asref.bind(key,val); return val; }
  operator Val ( ) { return asref.valueOf(key); }
  operator void* ( ) { return (void*)asref.bound(key); }
};


//------------------------ QuickAssociationsOf ---------------------------//
//---------------------- inline functions ---------------------------//

template <class Key, class Val>
inline QuickAssocRef<Key,Val> QuickAssociationsOf<Key,Val>::operator [ ] ( const Key& k )
{ return QuickAssocRef<Key,Val>(*this,k); }


//-------------- class QuickAssociationsIteratorRep ----------------------//

template <class Key, class Val>
class QuickAssociationsIteratorRep : public RefCounter {

public:

  QuickAssociationsIteratorRep( const QuickAssociationsOf<Key,Val>& A )
    :	theAssociations(A) {
      reset();
  }

  // Use compiler's copy constructor, operator=, and destructor.

  QuickAssociationsIteratorRep *clone() const {
    return new QuickAssociationsIteratorRep(*this);
  }

  bool operator == ( const QuickAssociationsIteratorRep& QAIR ) const {
    return ( current == QAIR.current &&
	     theAssociations.look() == QAIR.theAssociations.look() );
  }
  // @rn compare theAssociations cell-by-cell?
  // We do not want logical == of theAssociations, since iteration order
  // would be different.


  Key key( ) const {
#if SAFETY > 0
    if ( current == NULL )
      error("tried to retrieve key from QuickAssociationsIterator which is done");
#endif
    return current->getContents().key;
  }

  Val value( ) const {
#if SAFETY > 0
    if ( current == NULL )
      error("tried to retrieve value from QuickAssociationsIterator which is done");
#endif
    return *current->getContents().val;
  }

  bool next( ) {
    if ( current != NULL )
      current = current->nextCell;
    if ( current == NULL ) {
      int numberOfBuckets = theAssociations.look()->numberOfBuckets;
      if ( bucketIndex >= numberOfBuckets ) return 0;
      CellType** hashTable = theAssociations.look()->hashTable;
      ++bucketIndex;
      while ( bucketIndex < numberOfBuckets && hashTable[bucketIndex] == NULL )
	++bucketIndex;
      if ( bucketIndex < numberOfBuckets )
	current = hashTable[bucketIndex];
    }
    return (current != NULL);
  }

  bool done( ) const { return (current == NULL); }

  void reset( ) {
    current = NULL;
    bucketIndex = -1;
    next();
  }


protected:

  typedef Cell< QuickAssociation<Key,Val> > CellType;

  // Data members:

  CellType*      current;
  int            bucketIndex;

  const QuickAssociationsOf<Key,Val> theAssociations;

  // If theSet is declared as SetOf<T>, g++ 2.5.8 & 2.6.0 claim that it
  // has incomplete type. Then why, prithee, is this ok?
  // The exact same scheme works in ListOf and AssociationsOf.
};


//---------------------- class QuickAssociationsIterator ---------------------


// Example of use:
//
//  QuickAssociationsOf<Chars,Word> wordTable;
//  Word w;
//  ...
//
//  // Remove all associations with w from wordTable:
//
//  QuickAssociationsIterator<Chars,Word> I(wordTable);
//
//  while ( !I.done() ) {
//    if ( I.value() == G ) wordTable.unbind( I.key() );
//    I.next();
//  }

// Note: You may alter the AssociationsOf over which you are iterating,
//       but the iterator uses the `old' copy.

template <class Key, class Val> class QuickAssociationsIterator :
public ObjectOf< QuickAssociationsIteratorRep<Key,Val> > {

public:

  QuickAssociationsIterator(const QuickAssociationsOf<Key,Val>& A) :
    ObjectOf<AIR>( new AIR(A) ) { }
  // Initialize an iterator with the Associations over which to iterate.

  // Copy constructor, operator=, and destructor supplied by compiler.

  // Copying or assigning an iterator is guaranteed to produce an
  // iterator which visits the (rest of the) same elements in the
  // same order as the original.

  bool operator == ( const QuickAssociationsIterator& I ) const {
    return ((this->look() == I.look()) || (*this->look() == *I.look()));
  }

  Key key( ) const { return this->look()->key(); }
  // Returns key of current association. Calling this is a fatal error
  // if done().

  Val value( ) const { return this->look()->value(); }
  // Returns value of current association. Calling this is a fatal error
  // if done().

  bool next( ) { return this->change()->next(); }
  // Advances this iterator.
  // Returns TRUE iff the iteration has not finished.
  // This may be called after it returns FALSE with no side effect.

  bool done( ) const { return this->look()->done(); }
  // Returns TRUE iff the iteration has finished. This may
  // be called after it returns TRUE with no side effect.

  void reset( ) { this->change()->reset(); }
  // Resets this iterator to the state it was in after construction.

  // @stc the following are added as a tentative supplementary
  // convenience
  QuickAssociationsIterator& operator ++ ( ) { next(); return *this; }
  QuickAssociationsIterator operator ++ ( int ) {
    QuickAssociationsIterator old = *this;
    next( );
    return old;
  } // @stc need to check whether the semantics of this one are consistent
  // ie. does cloning preserve iterator semantics?
  operator void* ( ) { return (void*)!done(); }

private:

  typedef QuickAssociationsIteratorRep<Key,Val> AIR;
};


//-------------------------- QuickAssociations -----------------------------//
//----------------------- associated functions ------------------------//

template <class Key, class Val> inline ostream& operator <<
( ostream& o, const QuickAssociationsOf<Key,Val>& a ) {

  QuickAssociationsIterator<Key,Val> iter(a);
  o << "-";
  while (iter) {
    o << "[" << iter.key() << ":" << iter.value() << "]" << "-";
    ++iter;
  }
  return o;
}
// @stc inlined here (instead of defintion in .C) to circumvent g++
// template problems


//---------------------- methods which iterate -----------------------------

template <class Key, class Val>
inline ListOf<Key> QuickAssociationsOf<Key,Val>::keys( ) const

{
  ListOf<Key> result;
  QuickAssociationsIterator<Key,Val> I(*this);
  while ( !I.done() ) {
    result.append( I.key() );
    I.next();
  }
  return result;
}

#endif

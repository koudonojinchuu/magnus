// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition and implementation of
//           template <class Key, class Val> class AssociationsOf
//
// AssociationsOf implements an associative array, currently with O(n)
// access time.
//
// Principal Authors: Stephane Collart, Roger Needham
//
// Status: Useable.
//
// Revision History:
//
// Special remarks:
//
// * Requirements: Key and Val must have copy constructors and destructors,
//   Val must have operator=, and Key must have operator==.
//   Because an AssociationsOf method returns a ListOf<Key>, and ListOf wants to
//   output its elements using <<, there must also be an
//   ostream& operator << ( ostream&, const Type& ),
//   and a conversion of Key to Type.
//   @rn `inherited' requirements like this bear further consideration.
//
// * This copies Keys and Vals around, so non-user classes are less efficent.
//
// * The iterator template is parameterized by Key and Val, rather than
//   container class type, so g++ 2.5.8 will buy it. We'll switch to the
//   latter someday. Code which uses iterators will then break, but will be
//   easy to fix.
//
// * Val AssociationsOf<Key,Val>::operator [ ] ( const Key& ) const
//   may be enhanced later to act as an lvalue. No code will break.
//
// * I left out the possibly handy method
//     Key AssociationsOf<Key,Val>::keyOf( const Val& ) const
//   so as not to require Val to have operator==.
//
// * The return values of, e.g., valueOf would be more efficient as
//   references, but that seems to make the reference count go bad.
//   07/96 Aleksey M. implemented IPC tools

#ifndef _ASSOCIATIONS_H_
#define _ASSOCIATIONS_H_


#include "RefCounter.h"
#include "ObjectOf.h"
#include "List.h"
#include "Cell.h"

#include <iostream.h>


//---------------------- class Association ----------------------------------

template <class Key, class Val> class Association {
public:
  Association(Key k, Val v) : key(k), val(v) { }
  Key key;
  Val val;
 
 /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const Association& A )
  {
    ostr < A.key;
    ostr < A.val;
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, Association& A )
  {
    istr > A.key;
    istr > A.val;
    return istr;
  }

};

//---------------------- class AssociationsRep -----------------------------


template <class Key, class Val> class AssociationsIteratorRep;
// Forward declaration for friend declarations.

template <class Key, class Val> class AssociationsRep : public RefCounter {

public:

  AssociationsRep( ) : theList(NULL), theCardinality(0) {}

  AssociationsRep( const AssociationsRep& ar ) {  // Usual deep copy.
    theCardinality = ar.theCardinality;
    theList = NULL;
    CellType* tmpList = NULL;
    CellType* oldList = ar.theList;
    while ( oldList ) {
      if ( theList == NULL ) {
	theList = new CellType(*oldList);
	tmpList = theList;
      } // @stc are you relying on the compiler to move this out of
      // the loop?
      else {
	tmpList->nextCell = new CellType(*oldList);
	tmpList = tmpList->nextCell;
      }
      oldList = oldList->nextCell;
    }
    if( tmpList )
      tmpList->nextCell = NULL; // Better safe than sorry
  }

  ~AssociationsRep( ) {
	 while ( theList ) {
		CellType* tmpList = theList;
		theList = theList->nextCell;
		delete tmpList;
	 }
  }
  
  AssociationsRep* clone( ) const { return new AssociationsRep(*this); }

  void unbind( const Key& k ) {
	 if ( theList->getContents().key == k ) {
		CellType* tmp = theList;
		theList = theList->nextCell;
		delete tmp;
		--theCardinality;
	 } else {
		CellType* pred = theList;
		CellType* scan = theList->nextCell;
		while ( scan ) {
		  if ( scan->getContents().key == k ) {
			 pred->nextCell = scan->nextCell;
			 delete scan;
			 --theCardinality;
			 break;
		  }
		  scan = scan->nextCell;
		}
	 }
  }

  void bind( const Key& k, const Val& v ) {
	 CellType* where = seek(k);
	 if ( where )
		where->getContents().val = v;
	 else {
		//@rn g++2.5.8 can get confused by:
		// theList = new CellType(Association<Key,Val>(k, v), theList);

		CellType* tmp = new CellType(Association<Key,Val>(k, v));
		tmp->nextCell = theList;
		theList = tmp;
		++theCardinality;
	 }
  }

  Val val( const Key& k ) const {
	 CellType* where = seek(k);
    #if SAFETY > 0
	   if ( where == NULL )
        error("called AssociationsOf<Key,Val>::operator [ ] with unbound Key");
    #endif
	 return where->getContents().val;
  }

  Bool bound( const Key& k ) const { return (seek(k) != NULL); }

  int cardinality() const { 
    return theCardinality;
  }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void  write( ostream& ostr ) const;
  void  read( istream& istr );


private:

  typedef Cell< Association<Key,Val> > CellType;

  friend class AssociationsIteratorRep<Key,Val>;

  AssociationsRep(CellType* L) { theList = L; }

  CellType* seek( const Key& k ) const {
	 CellType* scan = theList;
	 while ( scan ) {
		if ( scan->getContents().key == k ) return scan;
		scan = scan->nextCell;
	 }
	 return NULL;
  }

  // Data members:

  CellType* theList;
  int theCardinality;
};


//------------------- class AssociationsOf --------------------------//


template <class Key, class Val> struct AssocRef;

template <class Key, class Val>
class AssociationsOf : public ObjectOf< AssociationsRep<Key,Val> > {

public:

  AssociationsOf( ) : ObjectOf<Rep>(new Rep()) { }
  // Default constructor makes an empty set of associations.

  // Copy constructor, operator=, and destructor supplied by compiler.

  Val operator [ ] ( const Key& k ) const { return valueOf(k); }
  // Operator synonym for valueOf.

  AssocRef<Key,Val> operator [ ] ( const Key& k );

  Val valueOf( const Key& k ) const { return this->look()->val(k); }
  // Retrieves the Val associated with k. It is a fatal error if k is not
  // bound to any Val.

  void bind( const Key& k, const Val& v ) { this->change()->bind(k, v); }
  // Associates k with v. Overwrites any existing association.

  void unbind( const Key& k ) { this->change()->unbind(k); }
  // Breaks any association k has; not an error if none.

  Bool bound( const Key& k ) const { return this->look()->bound(k); }
  // Says whether k is associated with anything.

  ListOf<Key> keys( ) const;
  // Returns a list of all keys in this association list.
  // A similar function returning all values would require Val to
  // have an operator==.

  int cardinality() const { return this->look()->cardinality(); }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const AssociationsOf& A )
  {
    A.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, AssociationsOf& A)
  {
    A.change()->read(istr);
    return istr;
  }

private:

  typedef AssociationsRep<Key,Val> Rep;

  friend class AssociationsIteratorRep<Key,Val>;
};


//------------------------ struct AssocRef ---------------------------//


template <class Key, class Val> struct AssocRef {
	AssociationsOf<Key,Val>& asref;
	const Key key;
	AssocRef( AssociationsOf<Key,Val>& a, const Key& k ) : asref(a), key(k) { }
   const Val& operator = ( const Val& val )
		{ asref.bind(key,val); return val; }
	operator Val ( ) { return asref.valueOf(key); }
	operator void* ( ) { return (void*)asref.bound(key); }
};


//------------------------ AssociationsOf ---------------------------//
//---------------------- inline functions ---------------------------//

template <class Key, class Val>
inline AssocRef<Key,Val> AssociationsOf<Key,Val>::operator [ ] ( const Key& k )
	{ return AssocRef<Key,Val>(*this,k); }


//-------------- class AssociationsIteratorRep ----------------------//

template <class Key, class Val>
class AssociationsIteratorRep : public RefCounter {

public:

  AssociationsIteratorRep( const AssociationsOf<Key,Val>& A ) :
    theAssociations(A) {
	 current = theAssociations.look()->theList;
  }

  // Use compiler's copy constructor, operator=, and destructor.

  AssociationsIteratorRep *clone() const {
	 return new AssociationsIteratorRep(*this);
  }

  Bool operator == ( const AssociationsIteratorRep& AIR ) const {
	 return ( ( current == AIR.current ) && 
				 ( theAssociations.look() == AIR.theAssociations.look() ) );
  }
  // @rn compare theAssociations cell-by-cell?
  // We do not want logical == of theAssociations, since iteration order
  // would be different.

  Key key( ) const {
	 #if SAFETY > 0
	   if ( current == NULL )
		  error("tried to retrieve key from AssociationsIterator which is done");
    #endif
	 return current->getContents().key;
  }

  Val value( ) const {
	 #if SAFETY > 0
	   if ( current == NULL )
        error("tried to retrieve value from AssociationsIterator which is done");
    #endif
	 return current->getContents().val;
  }

  Bool next( ) {
	 if ( current != NULL )
		current = current->nextCell;
	 return (current != NULL);
  }

  Bool done( ) const { return (current == NULL); }

  void reset( ) { current = theAssociations.look()->theList; }

 /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void  write( ostream& ostr ) const;
  void  read( istream& istr );

private:

  // Data members:

  const AssociationsOf<Key,Val>& theAssociations;

  //@njz: redefined
  //  AssociationsRep<Key,Val>::CellType* current;
  Cell< Association<Key,Val> >* current;
  //
};


//---------------------- class AssociationsIterator ---------------------------


// Example of use:
//
//  AssociationsOf<Chars,Word> wordTable;
//  Word w;
//  ...
//
//  // Remove all associations with w from wordTable:
//  
//  AssociationsIterator<Chars,Word> I(wordTable);
//  
//  while ( !I.done() ) {
//    if ( I.value() == G ) wordTable.unbind( I.key() );
//    I.next();
//  }

// Note: You may alter the AssociationsOf over which you are iterating,
//       but the iterator uses the `old' copy.

template <class Key, class Val>
class AssociationsIterator :
public ObjectOf< AssociationsIteratorRep<Key,Val> > {

public:

  AssociationsIterator(const AssociationsOf<Key,Val>& A) :
    ObjectOf<AIR>( new AIR(A) ) { }
  // Initialize an iterator with the Associations over which to iterate.

  // Copy constructor, operator=, and destructor supplied by compiler.

  // Copying or assigning an iterator is guaranteed to produce an
  // iterator which visits the (rest of the) same elements in the
  // same order as the original.

  Bool operator == ( const AssociationsIterator& I ) const {
	 return ((this->look() == I.look()) || (*this->look() == *I.look()));
  }

  Key key( ) const { return this->look()->key(); }
  // Returns key of current association. Calling this is a fatal error
  // if done().

  Val value( ) const { return this->look()->value(); }
  // Returns value of current association. Calling this is a fatal error
  // if done().

  Bool next( ) { return this->change()->next(); }
  // Advances this iterator.
  // Returns TRUE iff the iteration has not finished.
  // This may be called after it returns FALSE with no side effect.

  Bool done( ) const { return this->look()->done(); }
  // Returns TRUE iff the iteration has finished. This may
  // be called after it returns TRUE with no side effect.

  void reset( ) { this->change()->reset(); }
  // Resets this iterator to the state it was in after construction.

  // @stc the following are added as a tentative supplementary
  // convenience
    AssociationsIterator& operator ++ ( ) { next(); return *this; }
    AssociationsIterator operator ++ ( int ) {
        AssociationsIterator old = *this;
        next( );
        return old;
    } // @stc need to check whether the semantics of this one are consistent
    // ie. does cloning preserve iterator semantics?
    operator void* ( ) { return (void*)!done(); }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const AssociationsIterator& AI )
  {
    AI.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, AssociationsIterator& AI)
  {
    AI.change()->read(istr);
    return istr;
  }

private:

  typedef AssociationsIteratorRep<Key,Val> AIR;
};


//-------------------------- Associations -----------------------------//
//----------------------- associated functions ------------------------//
 
template <class Key, class Val> inline ostream& operator <<
	( ostream& o, const AssociationsOf<Key,Val>& a ) {
 
    AssociationsIterator<Key,Val> iter(a);
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
inline ListOf<Key> AssociationsOf<Key,Val>::keys( ) const

{
  ListOf<Key> result;
  AssociationsIterator<Key,Val> I(*this);
  while ( !I.done() ) {
	 result.append( I.key() );
	 I.next();
  }
  return result;
}

#endif

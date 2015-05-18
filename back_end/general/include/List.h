// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition and implementation of ListOf, ListRep,
//           ListIterator, and ListIteratorRep template classes.
//
// Principal Authors: Stephane Collart, Roger Needham
//
// Status: Useable.
//
// Revision History:
//
// Special Notes:
//
// * To instantiate ListOf<T>, class T must have
//   1) An assignment operator
//   2) A copy constructor
//   3) An == operator
//   4) A destructor
//   and there must be an
//     ostream& operator << ( ostream&, const Type& ),
//   and a conversion of T to Type.
//
// * Some ListOf methods can safely ignore bad indices, but they post a
//   warning if SAFETY > 0 on the assumption that the caller is 
//   probably bugged.
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//
// 07/96 Aleksey M. implemented IPC tools


#ifndef _LIST_H_
#define _LIST_H_

#include "RefCounter.h"
#include "ObjectOf.h"
#include "Cell.h"

template <class T> class ListIteratorRep;
// Forward declaration for friend declarations.


//---------------------- class ListRep ----------------------------------

template <class T> class ListRep : public RefCounter {

public:

  ListRep( ) : root(NULL), last(NULL), len(0) { }

  ListRep( const T& t ) {
    root = last = new CellType(t);
    len = 1;
  }

  // copy constructor does deep copy
  ListRep( const ListRep& lr ) : len(lr.len) {
    
    if ( lr.root == NULL )
      root = last = NULL;
    else {
      CellType* to = root = new CellType( *lr.root );
      CellType* from = lr.root->nextCell;
      while ( from ) {
        to = to->nextCell = new CellType( *from );
        from = from->nextCell;
      }
      last = to;
      last->nextCell = NULL; // Better safe than sorry
    }
  }

  ~ListRep( ) {
    CellType* r;
    while ( root ) {
      r = root->nextCell;
      delete root;
      root = r;
    }
  }

  Bool operator == ( const ListRep& lr ) const {
    return ( len == lr.len && agreement(&lr, 0) == len );
  }

  ListRep* clone( ) { return new ListRep(*this); }

  void insert( const T& t, int i ) {
  #if SAFETY > 0
      if ( i < 0 || i > len )
        error("index argument to ListRep::insert out of bounds");
  #endif
    if ( i == 0 ) {
      if ( root )
        root = new CellType(t, root);
      else
        root = last = new CellType(t);
    }
    else if ( i == len ) {
      last = last->nextCell = new CellType(t);
    }
    else {
      CellType* scan = root;
      while ( --i ) {
       #if SAFETY > 0
          if ( scan == NULL )
            error("internal error, len and actual length differ in ListRep");
       #endif
        scan = scan->nextCell;
      }
      scan->nextCell = new CellType(t, scan->nextCell);
    }
    ++len;
  }

  void removeElement( const T& t ) {
    if ( root == NULL ) return;
    if ( root->getContents() == t ) removeElementOfIndex(0);
    else {
      CellType* prev = root;
      CellType* scan = root->nextCell;
      while ( scan != NULL ) {
        if ( scan->getContents() == t ) {
          if ( (prev->nextCell = scan->nextCell) == NULL ) last = prev;
          delete scan;
          --len;
          break;
        }
        prev = scan;
        scan = scan->nextCell;
      }
    }
  }

  void removeElementOfIndex( int i ) {
    if ( root == NULL ) return;
    #if SAFETY > 0
      if ( i < 0 ) {
        warn("ignoring negative index in ListRep::removeElementOfIndex");
        return;
      }
    #endif
    if ( i == 0 ) {
      CellType* tmp = root;
      root = root->nextCell;
      --len;
      delete tmp;
    }
    else {
      CellType* prev = root;
      CellType* scan = root->nextCell;
      while ( scan != NULL ) {
        if ( --i == 0 ) {
          if ( (prev->nextCell = scan->nextCell) == NULL ) last = prev;
          delete scan;
          --len;
          break;
        }
        prev = scan;
        scan = scan->nextCell;
      }
      #if SAFETY > 0
        if ( scan == NULL && i )
          warn("ignoring index > length in ListRep::removeElementOfIndex");
      #endif
    }
  }

  void splice( const ListRep* rp, int i, int j ) {
    #if SAFETY > 0
      if ( i < 0 || j < i || j > len )
        error("ListRep::splice indices out of bounds");
    #endif
    CellType* left = root;
    // When i > 0, set left to predecessor of cell with index i.
    for( int k = 1; k < i; k++ ) left = left->nextCell;
    // Remove sublist [i,j), setting up left & right for splicing.
    CellType* right;
    if ( i == 0 ) right = left;
    else right = left->nextCell;
    for( int k = i; k < j; k++ ) {
      CellType* tmp = right;
      right = right->nextCell;
      delete tmp;
    }
    // Attach rp between left and right
    CellType* copy = rp->root;
    if ( i == 0 ) {
      root = left = new CellType(copy->getContents());
      copy = copy->nextCell;
    }
    while ( copy != NULL ) {
      left = left->nextCell = new CellType(copy->getContents());
      copy = copy->nextCell;
    }
    left->nextCell = right;
    if ( right == NULL ) last = left;
    len += ( rp->len - (j - i) );
  }

  ListRep* sublist( int i, int j ) const {
    #if SAFETY > 0
      if ( i < 0 || j < i || j > len )
        error("ListRep::sublist indices out of bounds");
    #endif
    CellType* scan = root;
    for( int k = 0; k < i; k++ ) scan = scan->nextCell;
    ListRep* result = new ListRep;
    int result_len = 0;
    for( int k = i; k < j; k++ ) {
      result->insert(scan->getContents(), result_len++);
      scan = scan->nextCell;
    }
    return result;
  }
  

  ListRep* concatenate( const ListRep* rp ) const {
    ListRep* result = new ListRep;
    CellType* scan = root;
    int result_len = 0;
    while ( scan != NULL ) {
      result->insert(scan->getContents(), result_len++);
      scan = scan->nextCell;
    }
    scan = rp->root;
    while ( scan != NULL ) {
      result->insert(scan->getContents(), result_len++);
      scan = scan->nextCell;
    }
    return result;
  }

  ListRep* reverse( ) const {
    ListRep* result = new ListRep;
    CellType* scan = root;
    while ( scan != NULL ) {
      result->insert(scan->getContents(), 0);
      scan = scan->nextCell;
    }
    return result;
  }

  int length( ) const { return len; }

  T element( int i ) const {
    #if SAFETY > 0
      if ( i < 0 || i >= len )
        error("index argument to ListRep::element out of bounds");
    #endif
    CellType* scan = root;
    while ( i-- ) scan = scan->nextCell;
    return scan->getContents();
  }

  int indexOf( const T& t ) const {
    int result = 0;
    CellType* scan = root;
    while ( scan != NULL ) {
      if ( scan->getContents() == t ) return result;
      ++result;
      scan = scan->nextCell;
    }
    return -1;
  }

  int agreement( const ListRep* rp, int start ) const {
    CellType* scan1 = root;
    CellType* scan2 = rp->root;
    while ( start ) {
      --start;
      if ( scan2 == NULL ) {
        #if SAFETY > 0
          warn("ignoring start > length in ListRep::agreement");
        #endif
        return 0;
      }
      scan2 = scan2->nextCell;
    }
    int result = 0;
    while ( scan1 != NULL && scan2 != NULL &&
            scan1->getContents() == scan2->getContents() ) {
      ++result;
      scan1 = scan1->nextCell;
      scan2 = scan2->nextCell;
    }
    return result;
  }

  #ifdef DEBUG
    void debugPrint( ostream& ostr ) const { }
  #endif

  void printOn( ostream& ostr ) const {
    CellType* r = root;
    while ( r ) {
      ostr << " -> " << r->getContents();
      r = r->nextCell;
    }
  }
  //@rn imposes constraint on Associations Key type

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void  write( ostream& ostr ) const;
  void  read( istream& istr );

private:

  ListRep& operator = ( const ListRep& ); // prohibit assignment
  //@rn be systematic about this?

  typedef Cell<T> CellType;

friend class ListIteratorRep<T>;

  // Data members:

  CellType* root;
  CellType* last;

  int len;
};


//---------------------- class ListOf ----------------------------------

template <class T> class ListOf : ObjectOf< ListRep<T> > {

public:

  typedef T ListElementType; // Export the template parameter

  ListOf( ) : ObjectOf<Rep>( new Rep() ) { }
  // Default constructor makes empty list.

  ListOf( const T& t ) : ObjectOf<Rep>( new Rep(t) ) { }
  // Cast constructor T -> ListOf<T>.

  // Copy constructor, operator=, and destructor supplied by compiler.

  Bool operator == ( const ListOf& L ) const { return equalTo(L); }

  Bool operator != ( const ListOf& L ) const { return !equalTo(L); }

  T operator [ ] ( int i ) const { return element(i); }
  // For read access only. Zero-based indexing.

  Bool equalTo( const ListOf& L ) const {
    //@njz
    //    return ( *look() == *L.look() );
    return ( *this->look() == *L.look() );
    //
  }
  // TRUE iff L contains the same T's as this list, in the same order.

  //@njz
  //  T element( int i ) const { return look()->element(i); }
  T element( int i ) const { return this->look()->element(i); }
  //
  // Return the element of index i in this list. Zero-based indexing.
  // It is a fatal error if i < 0 or i >= length().

  //@njz
  //  int length( ) const { return look()->length(); }
  int length( ) const { return this->look()->length(); }
  //
  // Return length of this List.

  Bool contains( const T& t ) const {
    return ( indexOf(t) != -1 );
  }
  // TRUE iff this list contains t according to T::operator==.

  //@njz
  //  int indexOf( const T& t ) const { return look()->indexOf(t); }
  int indexOf( const T& t ) const { return this->look()->indexOf(t); }
  //
  // Returns the index of t in this list, or -1 if not here.
  // Uses T::operator== in search.
  // Zero-based indexing.

  Bool prefixOf( const ListOf& L ) const {
    return ( agreement(L) == length() );
  }
  // TRUE iff this list is a prefix of L.

  Bool properPrefixOf( const ListOf& L ) const {
    return ( length() < L.length() && prefixOf(L) );
  }
  // TRUE iff this list is a proper prefix of L.

  int agreement( const ListOf& L, int start = 0 ) const {
    //@njz
    //    return look()->agreement(L.look(), start);
    return this->look()->agreement(L.look(), start);
    //
  }
  // Returns the length of the maximal prefix of this list which matches
  // the suffix of L beginning at start. You get a warning if
  // start >= L.length(); the result is 0 in this case.

  ListOf sublist( int i, int j ) const {
    //@njz
    //    return ListOf( look()->sublist(i, j) );
    return ListOf( this->look()->sublist(i, j) );
    //
  }
  // Returns a copy of the sublist [i,j) of this list.
  // It is a fatal error if i < 0, j < i, or j > length().
  // Note if i == j this returns an empty list.

  friend ListOf concatenate( const ListOf& L1, const ListOf& L2 ) {
    return ListOf( L1.look()->concatenate(L2.look()) );
  }
  // Returns the concatenatation of L1 and L2.

  //@njz
  //  ListOf reverse( ) const { return ListOf( look()->reverse() ); }
  ListOf reverse( ) const { return ListOf( this->look()->reverse() ); }
  //
  // Returns a copy of this list in reverse order.

  void append( const T& t ) { this->change()->insert(t, length()); }
  // Appends t to this List.

  void prepend( const T& t ) { this->change()->insert(t, 0); }
  // Prepends t to this List.

  void insert( const T& t, int i ) { this->change()->insert(t, i); }
  // Make t the (i+1)th element of this List. Zero-based. The previous (i+1)th
  // element becomes the (i+2)th. It is a fatal error if i < 0 or i > length().

  void removeElement( const T& t ) { this->change()->removeElement(t); }
  // Remove t from this list. It is not an error if t is not in this list.

  void removeElementOfIndex( int i ) { this->change()->removeElementOfIndex(i); }
  // Remove the (i+1)th element of this list. Zero-based. You get a warning
  // if i < 0 or i >= length().

  void splice( const ListOf& L, int i, int j ) {
    this->change()->splice(L.look(), i, j);
  }
  // Replaces the sublist [i,j) of this list with L.
  // It is a fatal error if i < 0, j < i, or j > length().
  // Note if i == j this inserts L into this list by moving the suffix of
  // this list beginning at index i to the right.

  inline friend ostream& operator << ( ostream& ostr, const ListOf& l ) {
    l.look()->printOn(ostr);
    return ostr;
  }

  #ifdef DEBUG
    void debugPrint( ostream& ostr ) const {
      this->look()->debugPrint(ostr);
      ostr << endl;
      this->look()->printOn(ostr);
    }
  #endif
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const ListOf& L )
  {
    L.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, ListOf& L)
  {
    L.change()->read(istr);
    return istr;
  }


private:

  typedef Cell<T> CellType;
  typedef ListRep<T> Rep;

  friend class ListIteratorRep<T>;

  typedef ObjectOf<Rep> R;
  ListOf( Rep* p ) : R(p) { }
};


//---------------------- class ListIteratorRep ----------------------------------

template <class T> class ListIteratorRep : public RefCounter {

public:

  ListIteratorRep( const ListOf<T>& L ) : theList(L) { reset(); }

  // Use compiler's copy constructor, operator=, and destructor.

  ListIteratorRep *clone() const { return new ListIteratorRep(*this); }

  Bool operator == ( const ListIteratorRep& LIR ) const {
    return ( ( current == LIR.current ) && 
             ( ( theList.look() == LIR.theList.look() ) ||
               ( *(theList.look()) == *(LIR.theList.look()) ) ));
  }

  T value( ) const {
    #if SAFETY > 0
      if ( current == NULL )
        error("tried to retrieve value from ListIterator which is done");
    #endif
    return current->getContents();
  }

  Bool next( ) {
    if ( current != NULL )
      current = current->nextCell;
    return current != NULL;
  }

  Bool done( ) const { return current == NULL; }

  void reset( ) { current = theList.look()->root; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

    void  write( ostream& ostr ) const;

    void  read( istream& istr );

private:

  // Data members:

  const ListOf<T>& theList;
  Cell<T>* current;
};


//---------------------- class ListIterator ----------------------------------

// Example of use:
//
//  ListOf<Word> wordList;
//  Word w;
//  ...
//
//  ListIterator< ListOf<Word> > I(wordList);
//  
//  // Stupidly check for w:
//  while ( !I.done() ) {
//    if ( I.value() == w ) /* w is in wordList */
//    I.next();
//  }

template <class ListType>
class ListIterator :
public ObjectOf< ListIteratorRep<typename ListType::ListElementType> > {

public:

  typedef typename ListType::ListElementType T;

  ListIterator(const ListOf<T>& L) : ObjectOf<LIR>( new LIR(L) ) { }
  // Initialize an iterator with the list over which to iterate.

  // Copy constructor, operator=, and destructor supplied by compiler.

  // Copying or assigning an iterator is guaranteed to produce an
  // iterator which visits the (rest of the) same elements in the
  // same order as the original.

  Bool operator == ( const ListIterator& I ) const {
    return ((this->look() == I.look()) || (*this->look() == *I.look()));
  }

  T value( ) const { return this->look()->value(); }
  // Returns the current value. Calling this is a fatal error if done().

  Bool next( ) { return this->change()->next(); }
  // Advances this iterator.
  // Returns TRUE iff the iteration has not finished.
  // This may be called after it returns FALSE with no side effect.

  Bool done( ) const { return this->look()->done(); }
  // Returns TRUE iff the iteration has finished. This may
  // be called after it returns TRUE with no side effect.

  void reset( ) { this->change()->reset(); }
  // Resets this iterator to the state it was in after construction.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const ListIterator& LI )
  {
    LI.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, ListIterator& LI)
  {
    LI.change()->read(istr);
    return istr;
  }

private:

  typedef ListIteratorRep<T> LIR;
};


#endif

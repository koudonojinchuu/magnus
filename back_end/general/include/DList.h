// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition and implementation of DListOf, DListRep,
//           DListIterator, and DListIteratorRep template classes.
//
// Principal Authors: Sergey Lyutikov
//
// Status: Useable.
//
// Revision History:
//
// Special Notes:
//
// * class DListOf represents a double linked list.
//
// * To instantiate DListOf<T>, class T must have
//   1) An assignment operator
//   2) A copy constructor
//   3) An == operator
//   4) A destructor
//   and there must be an
//     ostream& operator << ( ostream&, const Type& ),
//   and a conversion of T to Type.
//
// * Some DListOf methods can safely ignore bad indices, but they post a
//   warning if SAFETY > 0 on the assumption that the caller is probably bugged.
//
// * Based on implementation of ListOf class.
//
// * IMPORTANT: DListIterator iterates a copy of the DList, 
//   not the list itself! So, if you change the list after constructing 
//   DListIterator, don't expect to get new values from the list.
//   ( The same can be said about ListIterator. )
//
// 07/96 Aleksey M. implemented IPC tools
 
#ifndef _DLIST_H_
#define _DLIST_H_

#include "RefCounter.h"
#include "ObjectOf.h"
#include "DCell.h"

template <class T> class DListIteratorRep;
// Forward declaration for friend declarations.


//---------------------- class DListRep ----------------------------------

template <class T> class DListRep : public RefCounter {

public:

  DListRep( ) : root(NULL), last(NULL), len(0) { }

  DListRep( const T& t ) {
	 root = last = new CellType(t);
	 len = 1;
  }

  // copy constructor does deep copy
  DListRep( const DListRep& lr ) : len(lr.len) {
	 
	 if ( lr.root == NULL )
		root = last = NULL;
	 else {
		CellType* to = root = new CellType( *lr.root );
		CellType* from = lr.root->nextCell;
		while ( from ) {
		  to->nextCell = new CellType( *from );
                  to->nextCell->previousCell = to;
                  to = to->nextCell;
		  from = from->nextCell;
		}
		last = to;
		last->nextCell = NULL; // Better safe than sorry
	 }
  }

  ~DListRep( ) {
	 CellType* r;
	 while ( root ) {
		r = root->nextCell;
		delete root;
		root = r;
	 }
  }

  Bool operator == ( const DListRep& lr ) const {
	 return ( len == lr.len && agreement(&lr, 0) == len );
  }

  DListRep* clone( ) { return new DListRep(*this); }

  void insert( const T& t, int i ) {
    #if SAFETY > 0
      if ( i < 0 || i > len )
		  error("index argument to DListRep::insert out of bounds");
    #endif
	 if ( i == 0 ) {
		if ( root )
		  root = new CellType(t, NULL, root);
		else
		  root = last = new CellType(t);
	 }
	 else if ( i == len ) {
		last = last->nextCell = new CellType(t, last);
	 }
	 else {
		CellType* scan = root;
		while ( --i ) {
         #if SAFETY > 0
	    if ( scan == NULL )
       	    error("internal error, len and actual length differ in DListRep");
         #endif
		  scan = scan->nextCell;
		}
		scan->nextCell = 
			new CellType(t, scan, scan->nextCell);
		scan = scan->nextCell;
		scan->nextCell->previousCell = scan;
	 }
	 ++len;
  }

  void insert( const DListRep* LR, int i ) {

         #if SAFETY > 0
                if ( i < 0 || i > len )
		  error("index argument to DListRep::insert out of bounds");
         #endif

         if ( LR->root == NULL ) return;

         CellType* copy = LR->root;
         CellType* scan = root;

	 if ( i == 0 ) {
		if ( root != NULL ) 
                  scan = root = new CellType( copy->contents, NULL, root );
                else 
		  scan = root = last = new CellType( *copy );
                copy = copy->nextCell;
         }
	 else if ( i == len ) scan = last;
	 else {
		while ( --i ) {
            #if SAFETY > 0
	    if ( scan == NULL )
       	    error("internal error, len and actual length differ in DListRep");
            #endif
		  scan = scan->nextCell;
		}
	 }
         while ( copy ) {
                scan = scan->nextCell = new CellType( copy->contents, scan, scan->nextCell );
                copy = copy->nextCell;
         }
	 if ( scan->nextCell == NULL ) last = scan;
         else scan->nextCell->previousCell = scan;
         len += LR->len;
  }

  void removeElement( const T& t ) {
	if ( root == NULL ) return;
	if ( root->contents == t ) removeElementOfIndex(0);
	else {
	   CellType* scan = root->nextCell;
	   while ( scan != NULL ) {
	     if ( scan->contents == t ) {
	        if ( (scan->previousCell->nextCell = scan->nextCell) 
			== NULL ) last = scan->previousCell;
		else scan->nextCell->previousCell = scan->previousCell;
		delete scan;
		--len;
		break;
	     }
	     scan = scan->nextCell;
	   }
       }
  }

  void removeElementOfIndex( int i ) {
	if ( root == NULL ) return;
	#if SAFETY > 0
	 if ( i < 0 ) {
	   warn("ignoring negative index in DListRep::removeElementOfIndex");
	   return;
	 }
	#endif
	if ( i == 0 ) {
		CellType* tmp = root;
		root = root->nextCell;
		if ( root ) root->previousCell = NULL;
		--len;
		delete tmp;
	 }
	 else {
		CellType* scan = root->nextCell;
		while ( scan != NULL ) {
		  if ( --i == 0 ) {
			if ( (scan->previousCell->nextCell = scan->nextCell) 
				== NULL ) last = scan->previousCell;
			else scan->nextCell->previousCell = scan->previousCell;
			delete scan;
			--len;
			break;
		  }
		  scan = scan->nextCell;
		}
		#if SAFETY > 0
	         if ( scan == NULL && i )
		 warn("ignoring index > length in DListRep::removeElementOfIndex");
      		#endif
	 }
  }


  void splice( const DListRep* rp, int i, int j ) {
	 #if SAFETY > 0
	   if ( i < 0 || j < i || j > len )
		  error("DListRep::splice indices out of bounds");
         #endif
	 CellType* left = root;
	 CellType* right;
	 CellType* copy = rp->root;
	 // When i > 0, set left to predecessor of cell with index i.
	 for( int k = 1; k < i; k++ ) left = left->nextCell;
	 // Remove sublist [i,j), setting up left & right for splicing.
	 if ( i == 0 ) right = left;
	 else right = left->nextCell;
	 for( int k = i; k < j; k++ ) {
		CellType* tmp = right;
		right = right->nextCell;
		delete tmp;
	 }
	 // Attach rp btween left and right
	 if ( i == 0 ) {
                if ( copy == NULL ) root = left = right;
                else {
		       root = left = new CellType(copy->contents);
		       copy = copy->nextCell;
                }
	 }
	 while ( copy != NULL ) {
		left->nextCell = new CellType(copy->contents);
                left->nextCell->previousCell = left;
                left = left->nextCell;
		copy = copy->nextCell;
	 }
	 left->nextCell = right;
	 if ( right == NULL ) last = left;
         else right->previousCell = left;
	 len += rp->len + i - j;
  }


  DListRep* sublist( int i, int j ) const {
	 #if SAFETY > 0
	   if ( i < 0 || j < i || j > len )
		  error("DListRep::sublist indices out of bounds");
    #endif
	 CellType* scan = root;
	 for( int k = 0; k < i; k++ ) scan = scan->nextCell;
	 DListRep* result = new DListRep;
	 int result_len = 0;
	 for(int k = i; k < j; k++ ) {
		result->insert(scan->contents, result_len++);
		scan = scan->nextCell;
	 }
	 return result;
  }
	 

  DListRep* concatenate( const DListRep* rp ) const {
	 DListRep* result = new DListRep;
	 CellType* scan = root;
	 int result_len = 0;
	 while ( scan != NULL ) {
		result->insert(scan->contents, result_len++);
		scan = scan->nextCell;
	 }
	 scan = rp->root;
	 while ( scan != NULL ) {
		result->insert(scan->contents, result_len++);
		scan = scan->nextCell;
	 }
	 return result;
  }

  DListRep* reverse( ) const {
	 DListRep* result = new DListRep;
	 CellType* scan = root;
	 while ( scan != NULL ) {
		result->insert(scan->contents, 0);
		scan = scan->nextCell;
	 }
	 return result;
  }

  int length( ) const { return len; }

  T element( int i ) const {
	 #if SAFETY > 0
	   if ( i < 0 || i >= len )
		  error("index argument to DListRep::element out of bounds");
    #endif
	 CellType* scan = root;
	 while ( i-- ) scan = scan->nextCell;
	 return scan->contents;
  }

  int indexOf( const T& t ) const {
	 int result = 0;
	 CellType* scan = root;
	 while ( scan != NULL ) {
		if ( scan->contents == t ) return result;
		++result;
		scan = scan->nextCell;
	 }
	 return -1;
  }

  int agreement( const DListRep* rp, int start ) const {
	 CellType* scan1 = root;
	 CellType* scan2 = rp->root;
	 while ( start ) {
		--start;
		if ( scan2 == NULL ) {
		  #if SAFETY > 0
		    warn("ignoring start > length in DListRep::agreement");
        #endif
		  return 0;
		}
		scan2 = scan2->nextCell;
	 }
	 int result = 0;
	 while ( scan1 != NULL && scan2 != NULL &&
				scan1->contents == scan2->contents ) {
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
		ostr << " <-> " << r->contents;
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

  DListRep& operator = ( const DListRep& ); // prohibit assignment
  //@rn be systematic about this?

  typedef DCell<T> CellType;

  friend class DListIteratorRep<T>;

  // Data members:

  CellType* root;
  CellType* last;

  int len;

};


//---------------------- class DListOf ----------------------------------

template <class T> class DListOf : ObjectOf< DListRep<T> > {

public:

  typedef T ListElementType; // Export the template parameter

  DListOf( ) : ObjectOf<Rep>( new Rep() ) { }
  // Default constructor makes empty list.

  DListOf( const T& t ) : ObjectOf<Rep>( new Rep(t) ) { }
  // Cast constructor T -> DListOf<T>.

  // Copy constructor, operator=, and destructor supplied by compiler.

  Bool operator == ( const DListOf& L ) const { return equalTo(L); }

  Bool operator != ( const DListOf& L ) const { return !equalTo(L); }

  T operator [ ] ( int i ) const { return element(i); }
  // For read access only. Zero-based indexing.

  Bool equalTo( const DListOf& L ) const {
	 return ( *this->look() == *L.look() );
  }
  // TRUE iff L contains the same T's as this list, in the same order.

  T element( int i ) const { return this->look()->element(i); }
  // Return the element of index i in this list. Zero-based indexing.
  // It is a fatal error if i < 0 or i >= length().

  int length( ) const { return this->look()->length(); }
  // Return length of this List.

  Bool contains( const T& t ) const {
	 return ( indexOf(t) != -1 );
  }
  // TRUE iff this list contains t according to T::operator==.

  int indexOf( const T& t ) const { return this->look()->indexOf(t); }
  // Returns the index of t in this list, or -1 if not here.
  // Uses T::operator== in search.
  // Zero-based indexing.

  Bool prefixOf( const DListOf& L ) const {
	 return ( agreement(L) == length() );
  }
  // TRUE iff this list is a prefix of L.

  Bool properPrefixOf( const DListOf& L ) const {
	 return ( length() < L.length() && prefixOf(L) );
  }
  // TRUE iff this list is a proper prefix of L.

  int agreement( const DListOf& L, int start = 0 ) const {
	 return this->look()->agreement(L.look(), start);
  }
  // Returns the length of the maximal prefix of this list which matches
  // the suffix of L beginning at start. You get a warning if
  // start >= L.length(); the result is 0 in this case.

  DListOf sublist( int i, int j ) const {
	 return DListOf( this->look()->sublist(i, j) );
  }
  // Returns a copy of the sublist [i,j) of this list.
  // It is a fatal error if i < 0, j < i, or j > length().
  // Note if i == j this returns an empty list.

  friend DListOf concatenate( const DListOf& L1, const DListOf& L2 ) {
	 return DListOf( L1.look()->concatenate(L2.look()) );
  }
  // Returns the concatenatation of L1 and L2.

  DListOf reverse( ) const { return DListOf( this->look()->reverse() ); }
  // Returns a copy of this list in reverse order.

  void append( const T& t ) { this->change()->insert(t, length()); }
  // Appends t to this List.

  void prepend( const T& t ) { this->change()->insert(t, 0); }
  // Prepends t to this List.

  void insert( const T& t, int i ) { this->change()->insert(t, i); }
  // Make t the (i+1)th element of this List. Zero-based. The previous (i+1)th
  // element becomes the (i+2)th. It is a fatal error if i < 0 or i > length().

  void insert( const DListOf& L, int i ) { this->change()->insert( L.look(), i ); }
  // The first element of L becomes the (i+1)th element of this list. 
  // Zero-based. The previous (i+1)th element follows the last element of L.
  // It is a fatal error if i < 0 or i > length().

  void removeElement( const T& t ) { this->change()->removeElement(t); }
  // Remove t from this list. It is not an error if t is not in this list.

  void removeElementOfIndex( int i ) { this->change()->removeElementOfIndex(i); }
  // Remove the (i+1)th element of this list. Zero-based. You get a warning
  // if i < 0 or i >= length().

  void splice( const DListOf& L, int i, int j ) {
         if ( L.look() == this->look() ) { 
	        DListOf<T> aCopy = L;
	        this->change()->splice( aCopy.look(), i, j );
         }
	 else this->change()->splice(L.look(), i, j);
  }
  // Replaces the sublist [i,j) of this list with L.
  // It is a fatal error if i < 0, j < i, or j > length().
  // Note if i == j this inserts L into this list by moving the suffix of
  // this list beginning at index i to the right.

  inline friend ostream& operator << ( ostream& ostr, const DListOf& l ) {
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

   friend ostream& operator < ( ostream& ostr, const DListOf& DL )
  {
    DL.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, DListOf& DL)
  {
    DL.change()->read(istr);
    return istr;
  }

private:

  typedef DCell<T> CellType;
  typedef DListRep<T> Rep;

  friend class DListIteratorRep<T>;

  typedef ObjectOf<Rep> R;
  DListOf( Rep* p ) : R(p) { }

};


//---------------------- class DListIteratorRep ----------------------------------

template <class T> class DListIteratorRep : public RefCounter {

public:

  DListIteratorRep( const DListOf<T>& L ) : theList(L) { reset(); }

  // Use compiler's copy constructor, operator=, and destructor.

  DListIteratorRep *clone() const { return new DListIteratorRep(*this); }

  Bool operator == ( const DListIteratorRep& LIR ) const {
	 return ( ( current == LIR.current ) && 
				 ( ( theList.look() == LIR.theList.look() ) ||
					( *(theList.look()) == *(LIR.theList.look()) ) ));
  }

  T value( ) const {
	 #if SAFETY > 0
	   if ( current == NULL )
		  error("tried to retrieve value from DListIterator which is done");
    #endif
	 return current->contents;
  }

  Bool next( ) {
	 if ( current != NULL )
		current = current->nextCell;
	 return current != NULL;
  }

  Bool previous( ) {
	if ( current != NULL )
		current = current->previousCell;
	return current != NULL;
  }

  Bool done( ) const { return current == NULL; }

  void reset( ) { current = theList.look()->root; }

  void setToEnd( ) { current = theList.look()->last; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void  write( ostream& ostr ) const;

  void  read( istream& istr );

private:

  // Data members:

  const DListOf<T>& theList;
  DCell<T>* current;

};


//---------------------- class DListIterator ----------------------------------

// Example of use:
//
//  DListOf<Word> wordList;
//  Word w;
//  ...
//
//  DListIterator< DListOf<Word> > I(wordList);
//  
//  // Stupidly check for w:
//  while ( !I.valid() ) {
//    if ( I.value() == w ) /* w is in wordList */
//    I.next();
//  }
//
//  or
//
//  I.setToEnd();
//  while ( !I.valid() ) {
//    if ( I.value() == w ) /* w is in wordList */
//    I.previous();
//  }
//

template <class ListType>
class DListIterator :
public ObjectOf< DListIteratorRep<typename ListType::ListElementType> > 
{

public:

  typedef typename ListType::ListElementType T;

  DListIterator(const DListOf<T>& L) : ObjectOf<LIR>( new LIR(L) ) { }
  // Initialize an iterator with the list over which to iterate.

  // Copy constructor, operator=, and destructor supplied by compiler.

  // Copying or assigning an iterator is guaranteed to produce an
  // iterator which visits the (rest of the) same elements in the
  // same order as the original.

  Bool operator == ( const DListIterator& I ) const {
	 return ((this->look() == I.look()) || (*this->look() == *I.look()));
  }

  T value( ) const { return this->look()->value(); }
  // Returns the current value. Calling this is a fatal error if done().

  Bool next( ) { return this->change()->next(); }
  // Advances this iterator.
  // Returns TRUE iff the iteration has not finished.
  // This may be called after it returns FALSE with no side effect.

  Bool previous( ) { return this->change()->previous(); }
  // Advances this iterator.
  // Returns TRUE iff the iteration has not finished.
  // This may be called after it returns FALSE with no side effect.

  Bool done( ) const { return this->look()->done(); }
  // Returns TRUE iff the iteration has finished. This may
  // be called after it returns TRUE with no side effect.

  Bool valid( ) const { return !this->look()->done(); }
  // Returns TRUE iff the iteration hasn't finished. This may
  // be called after it returns FALSE with no side effect.

  void reset( ) { this->change()->reset(); }
  // Set iterator pointer to the first element of the list.
  // Resets this iterator to the state it was in after construction.

  void setToEnd( ) { this->change()->setToEnd(); }
  // Set iterator pointer to the last element of the list.

 
 /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

   friend ostream& operator < ( ostream& ostr, const DListIterator& DLI )
  {
    DLI.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, DListIterator& DLI)
  {
    DLI.change()->read(istr);
    return istr;
  }

private:

  typedef DListIteratorRep<T> LIR;

};

#endif

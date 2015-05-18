// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definitions of class BTree.
//           
// Principal Author: Dmitry Bormotov
//
// Status: Very first implementation (under development)
//
// Usage:
//
// Revision History:
//


#ifndef _PBTREE_H_
#define _PBTREE_H_


#include "global.h"


template <class Key, class Value> class PBTree;
template <class Key, class Value> class PBTreeIterator;


//------------------------------ PBTreePage ---------------------------------//


// the class PBTreePage is not a public class and will be hidden

template <class Key, class Value> class PBTreePage
{

  friend class PBTree<Key,Value>;
  friend class PBTreeIterator<Key,Value>;

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  PBTreePage( int order ) : theOrder( order ), numOfKeys( 0 ), 
      parentLink( NULL )
  {
  #if SAFETY > 0
    if( order < 2 )
      error("template <class Key, class Value> class PBTree::PBTree( int ) : "
	    "the order of PBTree must be 2 or more.");
  #endif  
    
    //@njz
    //    keys = new (Key*)[order-1];
    //    values = new (Value*)[order-1];
    //    links = new (PBTreePage<Key,Value>*)[order];
    keys = new Key*[order-1];
    values = new Value*[order-1];
    links = new PBTreePage<Key,Value>*[order];
    //
    for( int i = 0; i < order; ++i )
      links[i] = NULL;
  }

  ~PBTreePage( ) 
  { 
    delete [] links;
    delete [] keys;
    delete [] values;
  }


private:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int theOrder;
  int numOfKeys;
  Key **keys;
  Value **values;
  PBTreePage **links;
  PBTreePage *parentLink;
  int numOfParentLink;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

  //friend int main(...);

#endif

};


//--------------------------------- PBTree ----------------------------------//


//@db 2.91

template <class Key, class Value> class PBTree;

template <class Key, class Value>
ostream& operator < ( ostream& ostr, const PBTree<Key,Value>& T )
{
  error("Operator ostr < PBTree not emplemeted yet");
}

template <class Key, class Value>
istream& operator > ( istream& ostr, const PBTree<Key,Value>& T )
{
  error("Operator istr > PBTree not emplemeted yet");
}

//@db end


template <class Key, class Value> class PBTree
{

  friend class PBTreeIterator<Key,Value>;

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  PBTree( int order = 6 ) : theOrder( order )
  {
    #if SAFETY > 0
      if( order < 3 )
	error("template <class Key, class Value> class PBTree::PBTree( int ) : "
	      "the order of PBTree must be 3 or more.");
    #endif  
      
    root = new PBTreePage<Key,Value>(order);
  }
  // Constructs a PBTree with "order" keys and values in each node.

  PBTree( const PBTree& );
  PBTree& operator = ( const PBTree& ){
    error( "PBTree& operator = ( const PBTree& ) : not emplemented yet");
  }
  //copy constructor not emplemented yet
 
  ~PBTree( ) 
  { 
    deleteAll(); 
    delete root;
  }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool remove( const Key& key );

  void insert( const Key& key, const Value& value );

  Value* search( const Key& key );
  // returns the corresponding value or NULL if there's no such key in
  // the tree.
 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O:                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  friend ostream& operator << ( ostream& ostr, const PBTree& T )
  {
     error("Operator ostr < PBTree not emplemeted yet");
  }

  friend ostream& operator < <Key,Value>( ostream& ostr, const PBTree& T );

  friend istream& operator > <Key,Value>( istream& ostr, const PBTree& T );

  friend bool operator == ( const PBTree& T,const PBTree& T1 )
   {
          error("Operator PBTree == PBTree not emplemeted yet");
   }
   void printAll();
protected:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Protected functions:                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  virtual void theKeyIsFound( const Key& key, Value& value ) { }
  /*
  { 
    //value += key;
    //error("PBTree::theKeyIsFound called.");
  }
  */

  bool search( const Key& key, const PBTreePage<Key,Value>& searchPage,
	       PBTreePage<Key,Value> **keyPage, int& position );
  
  void deleteKey( PBTreePage<Key,Value> *page, int position );

  void deleteAll( ) { deleteAllPages(root); }

  void deleteAllPages( PBTreePage<Key,Value> *page );


private:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int theOrder;
  PBTreePage<Key,Value> *root;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


/*
  void printOn( ostream& ) const;
*/

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

 // void printAll( );

  //friend int main(...);

#endif

};


//----------------------------- PBTreeIterator -------------------------------//

// Note: This iterator don't make a copy of PBTree. Be careful - don't 
// delete keys in the tree when iterating over them.


template <class Key, class Value> class PBTreeIterator 
{

  friend class PBTree<Key,Value>; 

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  PBTreeIterator( const PBTree<Key,Value>& T ) : tree( T ) { reset( ); }

  // No default constructor
  // Copy constructor provided by compiler (does logical deep copy).


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Status Queries:                                                     //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool done( ) { return bDone; }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void reset( );

  Value getValue( ) 
  { 
  #if SAFETY > 0
    if( bDone )
      error("Value PBTreeIterator<Key,Value>::value( ) : "
	    "Tried to retrieve value from iterator which is done.");
  #endif  
    
    return *value; 
  }

  Key getKey( ) 
  { 
  #if SAFETY > 0
    if( bDone )
      error("Value PBTreeIterator<Key,Value>::value( ) : "
	    "Tried to retrieve value from iterator which is done.");
  #endif  
    
    return *key; 
  }

  bool next( );


private:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const PBTree<Key,Value>& tree; 
  PBTreePage<Key,Value> *page;  // current page in tree 
  int linkNumber;    // current link in tree
  bool bDone;
  Key *key;
  Value *value;
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
};

#endif






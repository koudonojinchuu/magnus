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


#ifndef _BTREE_H_
#define _BTREE_H_


#include "global.h"

//@db porting 
template <class Key, class Value> class BTree;
template <class Key, class Value> class BTreeIterator;


//------------------------------- BTreePage ---------------------------------//


// the class BTreePage is not a public class and will be hidden

template <class Key, class Value> class BTreePage
{

  friend class BTree<Key,Value>;
  friend class BTreeIterator<Key,Value>;

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  BTreePage( int order ) : theOrder( order ), numOfKeys( 0 ), 
      parentLink( NULL )
  {
  #if SAFETY > 0
    if( order < 2 )
      error("template <class Key, class Value> class BTree::BTree( int ) : "
	    "the order of BTree must be 2 or more.");
  #endif  
    
    //@njz
    //    keys = new (Key*)[order-1];
    //    values = new (Value*)[order-1];
    //    links = new (BTreePage<Key,Value>*)[order];
    keys = new Key*[order-1];
    values = new Value*[order-1];
    links = new BTreePage<Key,Value>*[order];
    //
    for( int i = 0; i < order; ++i )
      links[i] = NULL;
  }

  ~BTreePage( ) 
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
  BTreePage **links;
  BTreePage *parentLink;
  int numOfParentLink;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

  //friend int main( );

#endif

};


//--------------------------------- BTree -----------------------------------//


template <class Key, class Value> class BTree
{

  friend class BTreeIterator<Key,Value>;

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  BTree( int order = 6 ) : theOrder( order )
  {
    #if SAFETY > 0
      if( order < 3 )
	error("template <class Key, class Value> class BTree::BTree( int ) : "
	      "the order of BTree must be 3 or more.");
    #endif  
      
    root = new BTreePage<Key,Value>(order);
  }
  // Constructs a BTree with "order" keys and values in each node.

  BTree( const BTree& );
  BTree& operator = ( const BTree& ){
    error( "BTree& operator = ( const BTree& ) : not emplemented yet");
  }
  //copy constractor not emplemented yet
 
  ~BTree( ) 
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
  friend ostream& operator << ( ostream& ostr, const BTree& T )
  {
     error("Operator ostr < BTree not emplemeted yet");
  }

  friend ostream& operator < ( ostream& ostr, const BTree& T )
   {
          error("Operator ostr < BTree not emplemeted yet");
   }
  friend istream& operator > ( istream& ostr, const BTree& T )
   {
          error("Operator istr > BTree not emplemeted yet");
   }
  friend bool operator == ( const BTree& T,const BTree& T1 )
   {
          error("Operator BTree == BTree not emplemeted yet");
   }
   void printAll();
protected:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Protected functions:                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  virtual void theKeyIsFound( BTreePage<Key,Value>& keyPage, int position ) { }

  bool search( const Key& key, const BTreePage<Key,Value>& searchPage,
	       BTreePage<Key,Value> **keyPage, int& position );
  
  void deleteKey( BTreePage<Key,Value> *page, int position );

  void deleteAll( ) { deleteAllPages(root); }

  void deleteAllPages( BTreePage<Key,Value> *page );


private:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int theOrder;
  BTreePage<Key,Value> *root;


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
  
  //friend int main( );

#endif

};


//----------------------------- BTreeIterator -------------------------------//

// Note: This iterator don't make a copy of BTree. Be careful - don't 
// delete keys in the tree when iterating over them.


template <class Key, class Value> class BTreeIterator 
{

  friend class BTree<Key,Value>; 

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  BTreeIterator( const BTree<Key,Value>& T ) : tree( T ) { reset( ); }

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
      error("Value BTreeIterator<Key,Value>::value( ) : "
	    "Tried to retrieve value from iterator which is done.");
  #endif  
    
    return *value; 
  }

  Key getKey( ) 
  { 
  #if SAFETY > 0
    if( bDone )
      error("Value BTreeIterator<Key,Value>::value( ) : "
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

  const BTree<Key,Value>& tree; 
  BTreePage<Key,Value> *page;  // current page in tree 
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






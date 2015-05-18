// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementations of class BTree.
//
// Principal Author: Dmitry Bormotov
//
// Status: Very first implementation (under development)
//
// Usage:
//
// Discussion:
//
// Revision History:
//
// Next Implementation Steps:
//


#include "BTree.h"


//---------------------------------- BTree ----------------------------------//


template <class Key, class Value>
bool BTree<Key,Value>::search
  ( const Key& key, const BTreePage<Key,Value>& searchPage,
    BTreePage<Key,Value> **keyPage, int& position )
{
  *keyPage = (BTreePage<Key,Value> *)&searchPage;
  position = 0;
  if( (*keyPage)->numOfKeys == 0 )
    return false;
  while( true ) {                                  // cycle over pages
    position = 0;
    while( true ) {                                // cycle over positions
      Key curKey = *((*keyPage)->keys[position]);
      if( key == curKey )
	return true;
      else
	if( key < curKey )
	  if( (*keyPage)->links[position] == NULL )
	    return false;
	  else {
	    (*keyPage) = (*keyPage)->links[position];
	    break;
	  }
	else {
	  ++position;
	  if( position >= (*keyPage)->numOfKeys )
	    if( (*keyPage)->links[position] == NULL )
	      return false;
	    else {
	      (*keyPage) = (*keyPage)->links[position];
	      break;
	    }
	}
    }
  }
}


template <class Key, class Value>
void BTree<Key,Value>::insert( const Key& key, const Value& value )
{
  BTreePage<Key,Value> *page;
  int position;
  if( search(key, *root, &page, position) ) {
    theKeyIsFound(*page, position);
  }
  else {
    BTreePage<Key,Value> *leftLink = NULL;
    BTreePage<Key,Value> *rightLink = NULL;
    Key *insertKey = new Key(key);
    Value *insertValue = new Value(value);

    while( true ) {

      bool isLeaf = ( leftLink == NULL );
      int i;
      int numOfKeys = page->numOfKeys;

      // insert current key in current page

      if( numOfKeys < theOrder-1 ) { // if the page has a free place

        // simplest insertion: skiping keys and links
	// and puting new key on a free place

	for( i = numOfKeys; i > position; --i ) {
	  page->keys[i] = page->keys[i-1];
	  page->values[i] = page->values[i-1];
	}
	++(page->numOfKeys);
	page->keys[position] = insertKey;
	page->values[position] = insertValue;

	if( !isLeaf ) {
	  for( i = numOfKeys; i > position; --i ) {
	    page->links[i+1] = page->links[i];
	    ++(page->links[i+1]->numOfParentLink);
	  }
	  page->links[position] = leftLink;
	  page->links[position+1] = rightLink;
	  leftLink->parentLink = page;
	  rightLink->parentLink = page;
	  leftLink->numOfParentLink = position;
	  rightLink->numOfParentLink = position+1;
	}
	break;  // finish the inserting
      }
      else {    // split the page

	// creating temporary keys and links with bigger sizes
	// ( really this is page with size equals order+1 )

	BTreePage<Key,Value>* links[theOrder+1];
	Key* keys[theOrder];
	Value* values[theOrder];
	//int size = sizeof(keys);
	for( i = 0; i < position; ++i ) {
	  keys[i] = page->keys[i];
	  values[i] = page->values[i];
	}
	for( i = position+1; i < theOrder; ++i ) {
	  keys[i] = page->keys[i-1];
	  values[i] = page->values[i-1];
	}
	keys[position] = insertKey;
	values[position] = insertValue;
	
	if( !isLeaf ) {
	  for( i = 0; i < position; ++i )
	    links[i] = page->links[i];
	  for( i = position+1; i < theOrder; ++i )
	    links[i+1] = page->links[i];
	  links[position] = leftLink;
	  links[position+1] = rightLink;
	  leftLink->parentLink = page;
	  rightLink->parentLink = page;
	  leftLink->numOfParentLink = position;
	  rightLink->numOfParentLink = position+1;
	}
	
	// creating a brother of current page
	// and redistribution keys and links

	numOfKeys = theOrder/2;
	BTreePage<Key,Value> *newBrotherPtr = 
	  new BTreePage<Key,Value>(theOrder);

	page->numOfKeys = numOfKeys;
	for( i = position; i < numOfKeys; ++i ) {
	  page->keys[i] = keys[i];
	  page->values[i] = values[i];
	}
	
	if( !isLeaf ) 
	  for( i = position; i <= numOfKeys; ++i ) {
	    page->links[i] = links[i];
	    page->links[i]->parentLink = page;
	    page->links[i]->numOfParentLink = i;
	  }
 
	int brotherNumOfKeys = theOrder-numOfKeys-1;
	newBrotherPtr->numOfKeys = brotherNumOfKeys;
	for( i = 0; i < brotherNumOfKeys; ++i ) {
	  newBrotherPtr->keys[i] = keys[numOfKeys+1+i];
	  newBrotherPtr->values[i] = values[numOfKeys+1+i];
	}

	if( !isLeaf ) 
	  for( i = 0; i <= brotherNumOfKeys; ++i ) {
	    newBrotherPtr->links[i] = links[numOfKeys+1+i];
	    newBrotherPtr->links[i]->parentLink = newBrotherPtr;
	    newBrotherPtr->links[i]->numOfParentLink = i;
	  }
  	
        // go to the next iteration

	leftLink = page;
	rightLink = newBrotherPtr;
	insertKey = keys[numOfKeys];
	insertValue = values[numOfKeys];

	if( !page->parentLink ) {
	  page = new BTreePage<Key,Value>(theOrder);
	  root = page;
	  position = 0;
	}
	else {
	  position = page->numOfParentLink;
	  page = page->parentLink;
	}
      }
    }
  }
}

template <class Key, class Value>
bool BTree<Key,Value>::remove( const Key& key )
{
  BTreePage<Key,Value> *page;
  int position;
  if( search(key, *root, &page, position) ) {
    deleteKey(page, position);
    return true;
  }
  else
    return false;
}


template <class Key, class Value>
void BTree<Key,Value>::deleteKey( BTreePage<Key,Value> *page, int position )
{
  bool isLeaf = ( page->links[0] == NULL );
  float minNumOfKeys = (theOrder - 1)/2;

  delete page->keys[position];
  delete page->values[position];

  if( !isLeaf ) {  // if it is not a leaf page

    // replace the key with its immediate successor

    BTreePage<Key,Value> *oldPage = page;  
    page = page->links[position+1];
    while( page->links[0] )
      page = page->links[0];
    oldPage->keys[position] = page->keys[0];
    oldPage->values[position] = page->values[0];
    position = 0;
  }

  // remove the key from the leaf

  for( int i = position; i < page->numOfKeys-1; ++i ) {
    page->keys[i] = page->keys[i+1];
    page->values[i] = page->values[i+1];
  }
  --(page->numOfKeys);
  
  while( page->numOfKeys < minNumOfKeys ) {

    if( page == root ) return;
    
    // reorganize the tree

    BTreePage<Key,Value> *parentLink = page->parentLink;
    int numOfParentLink = page->numOfParentLink;
    int parentNumOfKeys = parentLink->numOfKeys;
    int numOfKeys = page->numOfKeys;
    bool isLeaf = ( page->links[0] == NULL );
    
    BTreePage<Key,Value> *leftSiblingLink = NULL, *rightSiblingLink = NULL;
    int leftNumOfKeys = 0, rightNumOfKeys = 0;
    
    if( numOfParentLink > 0 ) {
      leftSiblingLink = parentLink->links[numOfParentLink-1];
      leftNumOfKeys = leftSiblingLink->numOfKeys;
    }
    if( numOfParentLink < parentNumOfKeys ) {
      rightSiblingLink = parentLink->links[numOfParentLink+1];
      rightNumOfKeys = rightSiblingLink->numOfKeys;
    }

    // if the left sibling has unnecessary key
    // then redestribute it and quit the deletion

    if( leftNumOfKeys > minNumOfKeys ) {

      for( int i = numOfKeys; i > 0; --i ) {
	page->keys[i] = page->keys[i-1];
	page->values[i] = page->values[i-1];
      }
      page->keys[0] = parentLink->keys[numOfParentLink-1];
      page->values[0] = parentLink->values[numOfParentLink-1];
      parentLink->keys[numOfParentLink-1] =
	leftSiblingLink->keys[leftNumOfKeys - 1];
      parentLink->values[numOfParentLink-1] =
	leftSiblingLink->values[leftNumOfKeys - 1];
      
      if( !isLeaf ) {
	for( int i = numOfKeys+1; i > 0; --i ) {
	  page->links[i] = page->links[i-1];
	  ++(page->links[i]->numOfParentLink);
	}
	page->links[0] = leftSiblingLink->links[leftNumOfKeys];
	page->links[0]->numOfParentLink = 0;
	page->links[0]->parentLink = page;
      }
      ++(page->numOfKeys);
      --(leftSiblingLink->numOfKeys);

      return;
    }

    // if the right sibling has unnecessary key
    // then redestribute it and quit the deletion

    if( rightNumOfKeys > minNumOfKeys ) {

      page->keys[numOfKeys] = parentLink->keys[numOfParentLink];
      page->values[numOfKeys] = parentLink->values[numOfParentLink];
      parentLink->keys[numOfParentLink] = rightSiblingLink->keys[0];
      parentLink->values[numOfParentLink] = rightSiblingLink->values[0];
      for( int i = 0; i < rightNumOfKeys - 1; ++i ) {
	rightSiblingLink->keys[i] = rightSiblingLink->keys[i+1];
	rightSiblingLink->values[i] = rightSiblingLink->values[i+1];
      }

      ++numOfKeys;
      
      if( !isLeaf ) {
	page->links[numOfKeys] = rightSiblingLink->links[0];
	page->links[numOfKeys]->numOfParentLink = numOfKeys;
	page->links[numOfKeys]->parentLink = page;
	for( int i = 0; i < rightNumOfKeys; ++i ) {
	  rightSiblingLink->links[i] = rightSiblingLink->links[i+1];
	  --(rightSiblingLink->links[i]->numOfParentLink);
	}
      }
      ++(page->numOfKeys);
      --(rightSiblingLink->numOfKeys);

      return;
    }

    // if the left sibling exist
    // then concatenate the page and the sibling,
    // demote their parent key into the combined page,
    // run this process for parent page
    
    if( leftSiblingLink ) {

      leftSiblingLink->keys[leftNumOfKeys] =
	parentLink->keys[numOfParentLink-1];
      leftSiblingLink->values[leftNumOfKeys] =
	parentLink->values[numOfParentLink-1];
      for( int i = 0; i < numOfKeys; i++ ) {
	leftSiblingLink->keys[leftNumOfKeys+1+i] = page->keys[i];
	leftSiblingLink->values[leftNumOfKeys+1+i] = page->values[i];
      }

      if( !isLeaf )
	for( int i = 0; i <= numOfKeys; i++ ) {
	  int num = leftNumOfKeys+1+i;
	  leftSiblingLink->links[num] = page->links[i];
	  leftSiblingLink->links[num]->parentLink = leftSiblingLink;
	  leftSiblingLink->links[num]->numOfParentLink = num;
	}
      
      leftSiblingLink->numOfKeys += numOfKeys+1;
      delete page;
      
      for( int i = numOfParentLink - 1; i < parentNumOfKeys - 1; ++i ) {
	parentLink->keys[i] = parentLink->keys[i+1];
	parentLink->values[i] = parentLink->values[i+1];
	parentLink->links[i+1] = parentLink->links[i+2];
	--(parentLink->links[i+1]->numOfParentLink);
      }

      if( --(parentLink->numOfKeys) == 0 && parentLink->parentLink == 0 ) {
	delete parentLink;
	root = leftSiblingLink;
	root->parentLink = NULL;
	return;
      }
      else {
	page = parentLink;
	continue;
      }
    }
    // if only the right sibling exist
    // then concatenate the page and the sibling,
    // demote their parent key into the combined page,
    // run this process for parent page
    
    page->keys[numOfKeys] = parentLink->keys[numOfParentLink];
    page->values[numOfKeys] = parentLink->values[numOfParentLink];
    for( int i = 0; i < rightNumOfKeys; i++ ) {
      page->keys[numOfKeys+1+i] = rightSiblingLink->keys[i];
      page->values[numOfKeys+1+i] = rightSiblingLink->values[i];
    }
    
    if( !isLeaf )
      for( int i = 0; i <= rightNumOfKeys; i++ ) {
	int num = numOfKeys+1+i;
	page->links[num] = rightSiblingLink->links[i];
	page->links[num]->parentLink = page;
	page->links[num]->numOfParentLink = num;
      }
    
    page->numOfKeys += rightNumOfKeys+1;
    delete rightSiblingLink;
    
    for( int i = numOfParentLink; i < parentNumOfKeys - 1; ++i ) {
      parentLink->keys[i] = parentLink->keys[i+1];
      parentLink->values[i] = parentLink->values[i+1];
      parentLink->links[i+1] = parentLink->links[i+2];
      --(parentLink->links[i+1]->numOfParentLink);
    }
    
    if( --(parentLink->numOfKeys) == 0 && parentLink->parentLink == 0 ) {
      delete parentLink;
      root = page;
      root->parentLink = NULL;
      return;
    }
    else 
      page = parentLink;
  }  
}


template <class Key, class Value>
void BTree<Key,Value>::deleteAllPages( BTreePage<Key,Value> *page )
{
  if( page == NULL )
    return;
  for( int i = 0; i < page->numOfKeys; ++i ) {
    delete page->keys[i];
    delete page->values[i];
  }
  for( int i = 0; i <= page->numOfKeys; ++i )
    if( page->links[i] != NULL )
      deleteAllPages( page->links[i] );
  if( page != root )
    delete page;
  else
    page->numOfKeys = 0;
}


template <class Key, class Value>
Value*  BTree<Key,Value>::search( const Key& key )
{
  BTreePage<Key,Value> *page;
  int position;
  if( search(key, *root, &page, position) )
    return page->values[position];
  else 
    return NULL;
}


#ifdef DEBUG

template <class Key, class Value>
void BTree<Key,Value>::printAll( )
{
  BTreeIterator<Key,Value> I(*this);
  if( I.done() ) {
    cout << "The tree is empty." << endl;
    return;
  }

  for( ; !I.done(); I.next() ) {
    cout << "Page: ";
    for( int i = 0; i < I.page->numOfKeys; i++ )
      cout << "(" << *(I.page->keys[i]) << "," << *(I.page->values[i]) << ") ";

    //for( int i = I.page->numOfKeys; i < I.tree.theOrder; i++ )
    cout << "  ";

    cout << " linkNumber: " << I.linkNumber
	 << " value: " <<  I.getValue() << endl;
  }
}

#endif


//----------------------------- BTreeIterator -------------------------------//


template <class Key, class Value>
void BTreeIterator<Key,Value>::reset( )
{
  page = tree.root;
  linkNumber = 0;
  
  if( page->numOfKeys == 0 ) {          // it's an empty tree
    bDone = true;
    return;
  }

  bDone = false;

  // go down as more as possible

  while( page->links[linkNumber] )
    page = page->links[linkNumber];

  key = page->keys[linkNumber];         // set the key
  value = page->values[linkNumber];     // set the value
}


template <class Key, class Value>
bool BTreeIterator<Key,Value>::next( )
{
#if SAFETY > 0
  if( bDone )
    error("Value BTreeIterator<Key,Value>::next( ) : "
	  "Tried to call next() when iterator is done.");
#endif
  
  ++linkNumber;                         // go right inside the page

  // go down as more as possible

  if( page->links[linkNumber] ) {
    while( page->links[linkNumber] ) {
      page = page->links[linkNumber];
      linkNumber = 0;
    }
  }

  while( linkNumber >= page->numOfKeys )  // while there are no more keys
                                          // in the page
    if( page->parentLink ) {              // if going up is possible
      linkNumber = page->numOfParentLink; // go up
      page = page->parentLink;
    }
    else {                                // else finish the iteration process
      bDone = true;
      return false;
    }
  key = page->keys[linkNumber];         // set the key
  value = page->values[linkNumber];     // set the value
  return true;
}

     
template class BTree<int,char>;
template class BTree<int,int>;
template class BTreeIterator<int,int>;




// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition and implementation of classes:
//           SetData, SetIteratorData, SetOf, SetIterator.
//
// Everything is globbed together for now to keep g++ 2.5.8 happy.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
//
// * 2/95 sr added
//      int hashElement(const T & t) const;
//      to avoid calling t.hash() directly. This accommodates SetOf<int>
//
// * 01/96 Dmitry B. implemented IPC tools.
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//
// Special Notes:
//
// * To instantiate SetOf<T>, class T must have
//   1) An assignment operator
//   2) A copy constructor
//   3) An == operator
//   4) A destructor
//   5) A method int hash() const  (need not return positive ints)
//
// * The iterator template is parameterized by element type rather than
//   container class type, so g++ 2.5.8 will buy it. We'll switch to the
//   latter someday. Code which uses iterators will then break, but will be
//   easy to fix.
//
// * The set-theoretic operators are not global, because I'm paranoid about
//   the following: since there is a conversion T -> SetOf<T>, T would aquire
//   a host of confusing new operators. For example, if
//     inline SetOf<T> operator & (const SetOf<T>& S1, const SetOf<T>& S2) {
//       return setIntersection(S1, S2);
//     }
//   then
//     T t1, t2; T t = t1 & t2;
//   would be caught by the compiler, but
//     T t1, t2, t3; if ( (t1 & t2) == t3 ) {...}
//   would not (assuming T has a default constructor).
//   Keeping the operators as members is not much of a burden; because of
//   the symmetry of the operators, one can recast any desired expression
//   so that a genuine SetOf<T> is the left operand.
//   Also, this decision is reversible without breaking existing code.
//
// Things to Deal With:
//
// * Adopt a consistent policy on the return type of assignment-like
//   operators (val vs. ref). This affects other classes as well.
//   To bear in mind / investigate is the effect the choice has on
//   generating supplementary constructor calls, and the fact that the
//   decision cascades through to assignment-like operators and
//   functions which call other ones.
//
// Implementation Notes:
//
// * SetOfs are implemented as open hash tables. They are rehashed (doubling
//   or halving number of buckets) when the number of elements exceeds
//   FULLNESS_FACTOR * (number of buckets) or falls below
//   (number of buckets) / FULLNESS_FACTOR.
//   The (cheap) decision whether to rehash is made during each insert and
//   delete, except that if the user specifies an initial size, the new size
//   will not go below the specified size until the user calls `rehash()'.
//
// * We could choose the number of buckets more cleverly, and keep track
//   of whether any bucket is getting too big. In that case, we'd resize
//   +/- a small number to spread things out.


#ifndef _SET_H_
#define _SET_H_

#include "global.h"
#include "RefCounter.h"
#include "PureRep.h"
#include "ObjectOf.h"
#include "Cell.h"


template <class T> class SetIteratorData;
// Forward declaration for friend declarations.


//---------------------- class SetData ----------------------------------


#define FULLNESS_FACTOR 2
// Must be >= 2.


template<class T>
class SetData : public RefCounter {

public:

  SetData(int size)
  // size is suggestion (possibly very bad) of eventual set cardinality.
  // The result represents an empty set.
  {
	 // Make sure size has form 2^n - 1.
	 if ( size < 0 ) size = 0;
	 userSize = ( size > 1 ) ? size : 0;
	 unsigned int realSize = 1;
	 while ( size >>= 1 ) realSize = (realSize << 1) + 1;
	 
	 numberOfBuckets = realSize;
	 hashTable = new CellType*[realSize];
	 
	 for( int i = 0; i < realSize; i++ )
		hashTable[i] = NULL;
	 
	 numberOfElements = 0;
  }

  SetData( const T& t )
  {
	 userSize = 0;
	 numberOfBuckets = 1;
	 hashTable = new CellType*[1];
	 hashTable[0] = new CellType(t);
	 numberOfElements = 1;
  }

  SetData(const SetData& sd)
  // Copy constructor does deep copy.
  {
	 userSize = sd.userSize;
	 numberOfElements = sd.numberOfElements;
	 numberOfBuckets = sd.numberOfBuckets;
	 hashTable = new CellType*[numberOfBuckets];
	 CellType *bucket;
	 CellType **newBucket;
	 
	 for( int i = 0; i < numberOfBuckets; i++ ) {
		hashTable[i] = NULL;
		newBucket = &hashTable[i];
		bucket = sd.hashTable[i];
		while ( bucket != NULL ) {
		  *newBucket = new CellType(*bucket);
		  newBucket = &((*newBucket)->nextCell);
		  bucket = bucket->nextCell;
		}
	 }
  }

  ~SetData()
  // Delete everything associated with this SetData. Reference counting
  // spares the T's if necessary.
  {
	 if ( hashTable ) {
		removeAllElements();
		delete [] hashTable;
		hashTable = NULL;
	 }
  }

  SetData* clone( ) const { return new SetData(*this); }

  Bool operator == ( const SetData& sd ) const
  {
	 if ( numberOfElements != sd.numberOfElements ) return FALSE;
	 for ( int i = 0; i < numberOfBuckets; i++ ) {
		CellType* scan = hashTable[i];
		while ( scan != NULL ) {
		  if ( !sd.contains(scan->getContents()) ) return FALSE;
		  scan = scan->nextCell;
		}
	 }
	 return TRUE;
  }

  int hashElement(const T & t) const;

  void rehash( Bool calledByUser = FALSE )
  {
	 if ( calledByUser ) userSize = 0;

	 int newNumberOfBuckets;

	 if ( numberOfElements > FULLNESS_FACTOR * numberOfBuckets )
		newNumberOfBuckets = (numberOfBuckets << 1) + 1;
	 else if ( numberOfElements < numberOfBuckets / FULLNESS_FACTOR ) {
		// Assumes FULLNESS_FACTOR >= 2, so won't resize to 0.
		if ( !userSize || (numberOfBuckets >> 1) >= userSize )
		  newNumberOfBuckets = numberOfBuckets >> 1;
		else return;
	 }
	 else return;

	 int i;

	 CellType **newHashTable = new CellType*[newNumberOfBuckets];
	 for ( i = 0; i < newNumberOfBuckets; i++ )
		newHashTable[i] = NULL;
	 
	 // Now cycle through the old set, rehash its elements, detach them and
	 // reinsert them in newHashTable.
	 
	 CellType *curCell, *tmp;
	 int index;
	 for( i = 0; i < numberOfBuckets; i++ ) {
		curCell = hashTable[i];
		while ( curCell != NULL ) {
		  tmp = curCell->nextCell;
		  
		  // Rehash and insert entire cell into newHashTable
		  index = abs(hashElement(curCell->getContents())) % newNumberOfBuckets;
		  curCell->nextCell = newHashTable[index];
		  newHashTable[index] = curCell;
		  
		  curCell = tmp;
		}
		hashTable[i] = NULL;
	 }
	 
	 // Put all private variables back together and clean up.
	 numberOfBuckets = newNumberOfBuckets;
	 delete [] hashTable;
	 hashTable = newHashTable;
  }

  void removeAllElements()
  // Make *this represent an empty set, possibly keeping, e.g., a large
  // but empty bucket table.
  {
	 for( int i = 0; i < numberOfBuckets; i++ ) {
		CellType *bucket = hashTable[i];
		while ( bucket != NULL ) {
		  CellType *tmp = bucket;
		  bucket = bucket->nextCell;
		  delete tmp;
		}
		hashTable[i] = NULL;
	 }
	 numberOfElements = 0;
  }

  int cardinality() const { return numberOfElements; }

  Bool contains(const T& e) const
  {
	 int index = abs(hashElement(e)) % numberOfBuckets;
	 CellType* search = hashTable[index];
	 while ( search != NULL ) {
		if ( e == search->getContents() ) return TRUE;
		search = search->nextCell;
	 }
	 return FALSE;
  }

  void adjoinElement(const T& e)
  {
	 rehash();
	 int index = abs(hashElement(e)) % numberOfBuckets;
	 CellType* search = hashTable[index];
	 while ( search != NULL ) {
		if ( e == search->getContents() ) return;
		search = search->nextCell;
	 }
	 search = new CellType(e);
	 search->nextCell = hashTable[index];
	 hashTable[index] = search;
	 ++numberOfElements;
  }

  void removeElement(const T& e)
  {
	 int index = abs(hashElement(e)) % numberOfBuckets;
	 CellType* search = hashTable[index];
	 if ( search != NULL )
		if ( e == search->getContents() )
		  hashTable[index] = search->nextCell;
		else {
		  CellType* prev = search;
		  while ( (search = search->nextCell) != NULL ) {
			 if ( e == search->getContents() ) {
				prev->nextCell = search->nextCell;
				break;
			 }
			 prev = search;
		  }
		}
	 if ( search != NULL ) {
		delete search;
		--numberOfElements;
		rehash();
	 }
  }

  void printOn(ostream& ostr) const {
	 ostr << "\nSet of cardinality: " << numberOfElements << "\n{" << endl;
	 for ( int i = 0; i < numberOfBuckets; i++ ) {
		CellType* scan = hashTable[i];
		while ( scan != NULL ) {
		  ostr << scan->getContents() << endl;
		  scan = scan->nextCell;
		}
	 }
	 ostr << "\n}" << endl;
  }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void write( ostream& ostr ) const;
 
  void read( istream& istr );
 

  #ifdef DEBUG
  void printStats() const
  // Print statistics for hash table (for testing efficiency).
  {
	 cout << "\n\nHash table statistics:\n\n";
	 cout << "User specified size = ";
	 if ( userSize ) cout << userSize << endl;
	 else cout << "(cleared by call to `rehash()')" << endl;
	 cout << "Number of entries = " << numberOfElements << endl;
	 cout << "Number of buckets = " << numberOfBuckets << endl;
	 
	 double sum = 0;
	 double sumSquares = 0;
	 long maxLen = 0;
	 long minLen = 1000000;
	 long numEmpties = 0;
	 long len;
	 CellType *cPtr;
	 for( int i = 0; i < numberOfBuckets; i++ ) {
		cPtr = hashTable[i];
		if ( !cPtr ) ++numEmpties;
		len = 0;
		while ( cPtr ) { len++; cPtr = cPtr->nextCell; }
		maxLen = max( len, maxLen );
		minLen = min( len, minLen );
		sum += len;
		sumSquares += len*len;
	 }
	 long numNonEmpty = numberOfBuckets-numEmpties;
	 double mean = sum / numNonEmpty;
	 cout << "Largest bucket size = " << maxLen << endl;
	 cout << "Smallest bucket size = " << minLen << endl;
	 cout << "Average non-empty bucket size = " << mean << endl;
	 cout << "Number of empty buckets = " << numEmpties;
	 cout << ", or " << 100*numEmpties/numberOfBuckets << "%" << endl;
	 cout << "Sample standard deviation in non-empty bucket size = sqrt(";
	 cout << ((sumSquares - 2*mean*sum + numNonEmpty*mean*mean)/(numNonEmpty-1)) << ")\n" << endl;
  }
  #endif

  #ifdef DEBUG
  void printRep() const
  // Print a legible representation of the hash table.
  {
	 cout << "\n\nHash table:\n-----------\n\n";
	 CellType *cPtr;
	 for( int i = 0; i < numberOfBuckets; i++ ) {
		cout << i << ": ";
		cPtr = hashTable[i];
		while ( cPtr ) {
		  cout << ". ";
		  cPtr = cPtr->nextCell;
		}
		cout << endl;
	 }
  }
  #endif
  

  //@njz: moved from protected to public
  typedef Cell<T> CellType;

  int         userSize;         // Initial size > 1 given by the user,
                                // or 0 if none specified. Reset to 0
                                // only by rehash(TRUE).
  int         numberOfElements;
  int         numberOfBuckets;  // Equal to 2^n - 1 for some n.
  CellType**  hashTable;
  //

  
//@db private: 
protected:

  //@njz: moved to be public
  //  typedef Cell<T> CellType;
  //

  friend class SetIteratorData<T>;


  // Data members:

  //@njz: moved to be public
  //  int         userSize;         // Initial size > 1 given by the user,
                                // or 0 if none specified. Reset to 0
                                // only by rehash(TRUE).
  //  int         numberOfElements;
  //  int         numberOfBuckets;  // Equal to 2^n - 1 for some n.
  //  CellType**  hashTable;
  //

};


//---------------------- class SetOf -----------------------------------


template<class T>
class SetOf : public ObjectOf< SetData<T> > {

public:

  typedef T SetElementType; // Export this

  SetOf( int size = 1 ) : ObjectOf<Rep>( new Rep(size) ) { }
  // Give rough advice about eventual cardinality. The result is an empty set.
  // If you specify size > 1, the underlying hash table never shrinks
  // below that size until you call `rehash()'.
  // Note that because of hash table doubling, it costs only O(n) to build
  // up a set of cardinality n from the default.

  SetOf( const T& t ) : ObjectOf<Rep>( new Rep(t) ) { }
  // Cast constructor wraps element in a set.

  // Use compiler's copy constructor, operator=, and destructor.

  // Standard set-theoretic operators, defined in terms of member
  // functions:

  Bool operator == ( const SetOf& S ) const {
	 return equalTo(S);
  }

  Bool operator != ( const SetOf& S ) const {
	 return !equalTo(S);
  }

  Bool operator < ( const SetOf& S ) const {
	 return S.properlyContains(*this);
  }

  Bool operator <= ( const SetOf& S ) const {
	 return S.contains(*this);
  }

  Bool operator > ( const SetOf& S ) const {
	 return properlyContains(S);
  }

  Bool operator >= ( const SetOf& S ) const {
	 return contains(S);
  }

  Bool operator >= ( const T& e ) const {
	 return contains(e);
  }

  SetOf operator & ( const SetOf& S ) const {
	 return setIntersection(*this, S);
  }

  SetOf operator &= ( const SetOf& S ) {
	 shrinkToIntersectionWith(S);
	 return *this;
  }

  SetOf operator | ( const SetOf& S ) const {
	 return setUnion(*this, S);
  }

  SetOf operator |= ( const SetOf& S ) {
	 adjoinElements(S);
	 return *this;
  }

  SetOf operator |= ( const T& e ) {
	 adjoinElement(e);
	 return *this;
  }

  SetOf operator - ( const SetOf& S ) const {
	 return setMinus(*this, S);
  }

  SetOf operator -= ( const SetOf& S ) {
	 removeElements(S);
	 return *this;
  }

  SetOf operator -= ( const T& e ) {
	 removeElement(e);
	 return *this;
  }

  SetOf operator ^ ( const SetOf& S ) const {
	 return setSymmetricDifference(*this, S);
  }

  // Member functions:

  int cardinality() const { return this->look()->cardinality(); }

  Bool equalTo(const SetOf& S) const {
	 return ((this->look() == S.look()) || (*this->look() == *S.look()));
  }
  // TRUE iff S is equal as a set to *this.

  Bool contains(const T& e) const { return this->look()->contains(e); }
  // TRUE iff *this contains a T == to e.

  Bool contains(const SetOf& S) const;
  // TRUE iff S is a subset of *this.

  Bool properlyContains(const SetOf& S) const {
	 return ( cardinality() > S.cardinality() && contains(S) );
  }
  // TRUE iff S is a proper subset of *this.

  void adjoinElement(const T& e) { this->change()->adjoinElement(e); }
  // Add e to *this. It is not an error if *this already contains e.

  void removeElement(const T& e) { this->change()->removeElement(e); }
  // Remove e from *this.
  // It is not an error if this set does not contain e.

  void shrinkToIntersectionWith(const SetOf& S);
  // Make *this the intersection of *this and S.

  void adjoinElements(const SetOf& S);
  // Adjoin each element of S to *this.

  void removeElements(const SetOf& S);
  // Remove each element of S from *this.

  void removeAllElements() { this->change()->removeAllElements(); }
  // Makes *this into an empty set.

  void rehash( ) { this->enhance()->rehash(TRUE); }
  // For tweaking performance. Calling this is never sematically wrong, and
  // in the worst case degrades performance by increasing the constant in O(n);
  // see the comment for the default constructor.

  friend ostream& operator << ( ostream& ostr, const SetOf& S ) {
	 S.look()->printOn(ostr);
	 return ostr;
  }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const SetOf& S )
  {
    S.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, SetOf& S )
  {
    S.change()->read(istr);
    return istr;
  }


  #ifdef DEBUG
    void printStats() const { this->look()->printStats(); }
    void printRep() const { this->look()->printRep(); }
  #endif



protected:

  typedef SetData<T> Rep;

  friend class SetIteratorData<T>;

//@rn  SetOf( Rep* p ) : ObjectOf<Rep>(p) { }

};


// /*@rn g++ 2.5.8 gets confused when you prototype these

template <class T>
SetOf<T> setUnion(const SetOf<T>&, const SetOf<T>&);

template <class T>
SetOf<T> setIntersection(const SetOf<T>&, const SetOf<T>&);

template <class T>
SetOf<T> setMinus(const SetOf<T>&, const SetOf<T>&);

template <class T>
SetOf<T> setSymmetricDifference(const SetOf<T>&, const SetOf<T>&);

// */



//---------------------- class SetIteratorData ------------------------------

template<class T>
class SetContainer : public ObjectOf< SetData<T> > {
public:
  friend class SetIteratorData<T>;
  SetContainer( const SetOf<T>& S ) : ObjectOf< SetData<T> >(S) { }
};
// A helper class which mimics a SetOf<T>, to get around the usual g++
// template fuckup. See the theSet member below.


template<class T>
class SetIteratorData : public PureRep {

public:

  SetIteratorData(const SetOf<T>& S) : theSet(S) { reset(); }

  // Use compiler's copy constructor, operator=, and destructor.

  PureRep *clone() const { return new SetIteratorData(*this); }

  Bool operator == ( const SetIteratorData& sid ) const {
	 return ( current == sid.current && theSet.look() == sid.theSet.look() );
  }

  T value( ) const {
	 #if SAFETY > 0
	   if ( current == NULL )
		  error("tried to retrieve value from SetIterator which is done");
    #endif
	 return current->getContents();
  }

  Bool next( ) {
	 if ( current != NULL )
		current = current->nextCell;
	 if ( current == NULL ) {
		int numberOfBuckets = theSet.look()->numberOfBuckets;
		if ( bucketIndex >= numberOfBuckets ) return 0;
		CellType** hashTable = theSet.look()->hashTable;
		++bucketIndex;
		while ( bucketIndex < numberOfBuckets && hashTable[bucketIndex] == NULL )
		  ++bucketIndex;
		if ( bucketIndex < numberOfBuckets )
		  current = hashTable[bucketIndex];
	 }
	 return (current != NULL);
  }

  Bool done( ) const { return (current == NULL); }

  void reset( ) {
	 current = NULL;
	 bucketIndex = -1;
	 next();
  }


private:

  typedef Cell<T> CellType;

  // Data members:

  CellType*      current;
  int            bucketIndex;

  const SetContainer<T> theSet;

  // If theSet is declared as SetOf<T>, g++ 2.5.8 & 2.6.0 claim that it
  // has incomplete type. Then why, prithee, is this ok?
  // The exact same scheme works in ListOf and AssociationsOf.
};



//---------------------- class SetIterator ------------------------------

// To iterate over the elements of a SetOf<T> S, just do:
//
//  SetIterator<T> I(S);
//  while ( !I.done() ) {
//    /* Do something with I.value() */
//    I.next();
//  }
//
// You may assign one SetIterator to another, so the following works:
//
//  SetIterator<T> I(S);
//  while( !I.done() ) {
//    SetIterator<T> J = I;
//    while( J.next() )
//      if ( I.value() == J.value() ) error("Set contains duplicates!");
//    I.next();
//  }
//  int count = 0;
//  I.reset();
//  while( !I.done() ) {
//    SetIterator<T> J(I);
//    do { if ( I.value() == J.value() ) ++count; } while( J.next() );
//    I.next();
//  }
//  if ( count != S.cardinality() ) error("I give up");
//
// Since I was reset, the two excursions of I through S are guaranteed to
// produce the same sequence of elements. In any case, J is guaranteed to look
// at the rest of what I is.
// You may alter S during the iteration, but I uses the old copy
// of S (see shrinkToIntersectionWith). You may alter the object returned by
// I.value(), but this will not effect S or I.


/* @@rn Changing parameterization for g++ 2.6.0

template <class SetOfType>
class SetIterator :
public ObjectOf< SetIteratorData<SetOfType::SetElementType> > {
*/

template <class T>
class SetIterator : public ObjectOf< SetIteratorData<T> > {

public:

//@@rn  typedef SetOfType::SetElementType T;

  SetIterator(const SetOf<T>& S) : ObjectOf<SID>( new SID(S) ) { }
  // Constructs iterator from set over which to iterate.

  // Copy constructor, operator=, and destructor supplied by compiler.

  Bool operator == ( const SetIterator& I ) const {
	 return ((this->look() == I.look()) || (*this->look() == *I.look()));
  }
  // TRUE iff the iterators will look at the same elements of the (physically)
  // same set in the same order. Valid at any point of the iteration.

  T value( ) const { return this->look()->value(); }
  // Returns the current T. Calling this is a fatal error if done().

  Bool next( ) { return this->change()->next(); }
  // Advances this iterator.
  // Returns TRUE iff the iteration has not finished.
  // This may be called after it returns FALSE with no side effect.

  Bool done( ) const { return this->look()->done(); }
  // Returns TRUE iff the iteration has finished. This may
  // be called after it returns TRUE with no side effect.

  void reset( ) { this->change()->reset(); }
  // Resets this iterator to the state it was in after construction.

protected:

  typedef SetIteratorData<T> ThisRep;
  typedef ObjectOf<ThisRep> Base;
 
  // Shadow representation accessors to get representations of the
  // right type in the members of this class:
 
  const ThisRep* look( ) const {
    return (const ThisRep*)Base::look();
  }
  ThisRep* enhance( ) const {
	 return (ThisRep*)Base::enhance();
  }
  ThisRep* change( ) {
	 return (ThisRep*)Base::change();
  }

  SetIterator( ThisRep* rep ) : Base(rep) { }
  // To wrap new representation

private:

  typedef SetIteratorData<T> SID;

};


//------------ SetOf functions which depend on SetIterator --------------------

/*
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
*/

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



#endif





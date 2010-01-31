//   Copyright (C) 1994 The New York Group Theory Cooperative
//   See magnus/doc/COPYRIGHT for the full notice.
 
// Contents: Definition of VectorPtrOf<T> and VectorPtrRep<T> class
//
// Principal Author: Dmitry Pechkin
//
// Status: under trial.
//
// Revision History:
//
// * 96/01/06 @dp Added functions in class VectorPtrOf<T>:
//     T val( int i ) const;
//     T& ref( int i );
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//
// * 3/99 Dmitry B. moved the following implementation to VectorPtr.C 
//   to satisfy new C++ standard:
//
//   VectorItemRef<T> VectorPtrOf<T>::operator [] ( int i );
//
//
// * 03/97 dp fixed VectorPtrRep::shrink(int start, int newlen);
//                  VectorPtrRep( const VectorPtrRep& vr );
//                  bool VectorPtrOf::operator == ( const VectorPtrOf& v ) const;
//
// * 04/97 dp added bool VectorPtrOf<T>::isValid(int) const.
//
// Special Notes:
//
// * These classes are remake of VectorOf<T> and VectorRep<T> classes
//   for one purpose only: class T may not have default constructor.
//   But new implementation is some slow.
//
// * Some problem exists: now on comparison of uninitialized element of
//   VectorPtrOf<T> and element of T error will not occur 
//   (VectorPtrItemRef::operator==() is optimized for this),
//   but if there is global operator==(T,T) error could be occur
//   because in cast operator VectorPtrItemRef::operator T() 
//   method VectorPtr<T>::val(int) is called which caused error
//   if the element is uninitialized. Should be behavoiur
//
// * They aren't derived from VectorOf<> and VectorRep<> because
//   now the last ones require some repairs to fit to general scheme of 
//   derived objects, e.g. like Group hierarchy.
//
// * To instantiate VectorPtrOf<T>, class T must have
//   1) A copy constructor
//   2) An assignment operator
//   3) An == operator
//   4) A destructor
//
// OTHER NOTES FROM Vector.h:
//
// Further implementation steps:
//
// * Analogs of the List methods would be nice.
//
// * Should negative indices count from the end of the vector?
//
// * VectorOf should also have cast constructors from SetOf, ListOf, etc,
//   in order to be able to conveniently use the latter where a VectorOf
//   is expected in a constructor etc.
//
// * Should some compatibility mechanism be installed between things
//   like say VectorOf<Elt> and VectorOf<Word>?
//
// Bugs:
//
// * g++ 2.5.8 and less can't find templates of non-inlined functions
//   so it is necessary to give explicit definitions in Vector.C of
//   every instance.
//
 
#ifndef _VECTOR_PTR_H_
#define _VECTOR_PTR_H_
 

#include <iostream>

#include "RefCounter.h"
#include "ObjectOf.h"

//@db porting
template < class T > class VectorItemRef;


template <class T> struct VectorPtrRep : public RefCounter {
  
  public :
  
  // copy constructor does deep copy
  
  VectorPtrRep( const VectorPtrRep& vr ) {
    len = vr.last - vr.first;
    first = 0;
    last = len;
    //@njz
    //    vec = new (T*)[len];
    vec = new T*[len];
    //
#if ( SAFETY > 1 )
    if( !vec )
      error("Cannot allocate memory in VectorPtrRep::VectorPtrRep(const VectorPtrRep& vr)");
#endif
    for( int i = 0; i < len; i++ ) {
      if( vr.vec[vr.first + i] ) {
	vec[i] = new T( *vr.vec[vr.first + i] );
#if ( SAFETY > 1 )
	if( !vec[i] )
	  error("Cannot allocate memory in VectorPtrRep::VectorPtrRep(const VectorPtrRep& vr)");
#endif
      }
      else
	vec[i] = NULL;
    }
    fastExpansion = false;
  }
  
  VectorPtrRep( int l ) {
    len = l;
    first = 0;
    last = len;
    //@njz
    //    vec = new (T*)[len];
    vec = new T*[len];
    //
#if ( SAFETY > 1 )
    if( !vec )
      error("Cannot allocate memory in VectorPtrRep::VectorPtrRep(int)");
#endif
    for(int i = 0; i<len; i++) vec[i] = NULL;
    fastExpansion = false;
  }
  // creates an uninitialized vector of length l
  
  VectorPtrRep( int l, bool e ) {
    len = l;
    first = 0;
    last = len;
    //@njz
    //    vec = new (T*)[len];
    vec = new T*[len];
    //
#if ( SAFETY > 1 )
    if( !vec )
      error("Cannot allocate memory in VectorPtrRep::VectorPtrRep(int, bool)");
#endif
    for(int i = 0; i<len; i++) vec[i] = NULL;
    fastExpansion = e;
  }
  // creates an uninitialized vector of length l

  ~VectorPtrRep( ) { 
    for(int i = 0; i<len; i++)
      delete vec[i];
    delete [] vec;
  }
  
  // standard cloning operation for representations
  
  VectorPtrRep* clone( ) { return new VectorPtrRep( *this ); }

  int length() const { return last - first; }

  bool isValid(int i) const { 
#if ( SAFETY > 0 )
    if ( i < 0 || i >= last - first ) 
      error("VectorPtrOf index out of bounds "
	    "in VectorPtrRep::isValid(int)");
#endif
    return ( vec[first+i] ? true : false );
  }

  void set(int i, const T& t) {
#if ( SAFETY > 0 )
    if ( i < 0 || i >= last - first ) 
      error("VectorPtrOf index out of bounds "
	    "in VectorPtrRep::set(int, const T&)");
#endif
    if (!vec[first+i]){
      delete vec[first+i];
      vec[first+i] = new T(t);
#if ( SAFETY > 1 )
      if( !vec[first+i] )
	error("Cannot allocate memory in VectorPtrRep::set(int,T)");
#endif
    }
    else *vec[first+i] = t;
  }

  // for reference access
  T& ref(int i) {
#if ( SAFETY > 0 )
    if ( i < 0 || i >= last - first ) 
      error("VectorPtrOf index out of bounds in T& VectorRep::ref(int) const");
    if( !isValid(i) )
      error("VectorPtrOf access denied to uninitialized element "
	    "in T& VectorPtrRep::ref(int) const");
#endif
    return *vec[first + i];
  }

  // for value access
  T val(int i) const {
#if ( SAFETY > 0 )
    if ( i < 0 || i >= last - first ) 
      error("VectorPtrOf index out of bounds in T VectorRep::val(int) const");
    if( !isValid(i) )
      error("VectorPtrOf access denied to uninitialized element "
	    "in T VectorPtrRep::val(int) const");
#endif
    return *vec[first + i];
  }

  VectorItemRef<T> operator [] ( int i ) { 
#if ( SAFETY > 0 )
    if ( i < 0 || i >= length() ) 
      error("VectorPtrOf index out of bounds in VectorRep::operator [](int)");
#endif
    if( isValid(i) )
      return VectorItemRef<T>(vec[i]);
    else
      return VectorItemRef<T>(&vec[i], 0);
  }

  void append( const T& t ) {
    if ( last < len ) {
      vec[last++] = new T(t);
    }
    else {
      if( fastExpansion && len ) len *= 2; else len++;
      //@njz
      //      T** new_vec = new (T *)[len];
      T** new_vec = new T*[len];
      //
 #if ( SAFETY > 1 )
      if( !new_vec )
	error("Cannot allocate memory in VectorPtrRep::append(T)");
#endif
     int j = 0;
      for( int i = first; i < last; i++ )
	new_vec[j++] = vec[i];
      delete [] vec;
      vec = new_vec;
      vec[j++] = new T(t);
#if ( SAFETY > 1 )
      if( !vec[j-1] )
	error("Cannot allocate memory in VectorPtrRep::append(T)");
#endif
      last = j;
      first = 0;
      for(int i = last; i<len; i++)
	vec[i] = NULL;
    }
  }

  void prepend( const T& t ) {
    if ( first > 0 ) {
      vec[--first] = new T(t);
    }
    else {
      if( fastExpansion && len ) len *= 2; else len++;
      //@njz
      //      T** new_vec = new (T *)[len];
      T** new_vec = new T*[len];
      //
#if ( SAFETY > 1 )
    if( !new_vec )
      error("Cannot allocate memory in VectorPtrRep::prepend(T)");
#endif
      int j = 0;
      new_vec[j++] = new T(t);
#if ( SAFETY > 1 )
    if( !new_vec[0] )
      error("Cannot allocate memory in VectorPtrRep::prepend(int)");
#endif
      for( int i = first; i < last; i++ )
	new_vec[j++] = vec[i];
      delete [] vec;
      vec = new_vec;
      last = j;
      first = 0;
      for(int i = last; i<len; i++)
	vec[i] = NULL;
    }
  }

  void shrink( int start, int newlen ) {
#if ( SAFETY > 0 )
    if ( start < 0 || first + start >= last || newlen > last - first )
      error("argument to VectorRep::shrink out of bounds");
#endif
    // The semantics are dangerous if we allow shrink to `expand' the VectorPtrOf:
    // a copy construction may throw the `extra' stuff away in between
    // calls to shrink.

    // free all elements of vector which are out of new bounds

    T** tmp;

    T** tmpStart = vec+first+start;
    for(tmp = vec+first; tmp<tmpStart; tmp++) {
      delete *tmp;
      *tmp = NULL;
    }

    T** tmpLast = vec+last;
    for(tmp = tmpStart+newlen; tmp<tmpLast; tmp++) {
      delete *tmp;
      *tmp = NULL;
    }

    first += start;
    last = first + newlen;
  }

  
  private :
  
  // assignment operator undesired : made inaccessible private
  VectorPtrRep& operator = ( const VectorPtrRep& );
  // { }//@rn

  // data members
  
  bool fastExpansion; // true if expansion should be done by doubling space 
  unsigned int first; // index of first valid entry
  unsigned int last;  // index + 1 of last valid entry
  unsigned int len;   // actual length of storage, so last - first <= len
  
  T** vec;
};



template <class T> class VectorPtrOf : public ObjectOf< VectorPtrRep<T> > {
  
  typedef VectorPtrRep< T > Rep;
  typedef ObjectOf< Rep > Base;
  
public:
  
  // copy constructor, operator=, and destructor supplied by compiler.
  
  VectorPtrOf( int len = 0 ) : Base( new Rep(len) ) { }

  VectorPtrOf( int len, bool e ) : Base( new Rep(len,e) ) { }
  // When e is true, the vector length doubles when an append or prepend
  // needs more space (instead of increasing by 1).

  VectorPtrOf( int len, const VectorPtrOf& v ) : Base( new Rep(len) ) {
    for (int i = 0; i < min( len, v.length() ); i++)
      if( v.look()->isValid(i) )
	this->enhance()->set(i, v[i]);
  }
  // to make a vector of given length, (partly) initialized with
  // (part of) another vector

  VectorPtrOf( int len, bool e, const VectorPtrOf& v ) : Base( new Rep(len,e)){
    for (int i = 0; i < min( len, v.length() ); i++) 
      if( v.look()->isValid(i) )
	this->enhance()->set(i, v[i]);
  }
  // See comment for VectorPtrOf( int len, bool e ).

  bool operator == ( const VectorPtrOf& v ) const
  {
    if (this->look() == v.look()) return true;
    if ( this->look()->length() != v.look()->length() ) return false;
    int i = this->look()->length();
    while ( i-- ) 
      if( this->look()->isValid(i) != v.look()->isValid(i) ||
	  this->look()->isValid(i) && !(this->look()->val(i) == v.look()->val(i)) ) 
	return false;
    return true;
  }
  
  bool operator != ( const VectorPtrOf& v ) const { return !(*this == v); }

  T operator [] ( int i ) const { return this->look()->val(i); }

  VectorItemRef<T> operator [] ( int i ) { return this->change()->operator [](i); }

  T val( int i ) const { return this->look()->val(i); }

  T& ref( int i ) { return this->change()->ref(i); }

  bool isValid( int i ) const { return this->look()->isValid(i); }
  
  int length( ) const { return this->look()->length(); }

  int hash() const { return this->look()->length(); }
  //@rn Replace this in specific template instances if you want
  //    any semblance of efficiency.
  
  int indexOf( const T& t ) const {
    int i = length();
    while ( i-- ) 
      if ( this->look()->isValid(i) && this->look()->val(i) == t ) return i;
    return -1;
  }
  // Returns the index of t in this VectorPtrOf, or -1 if not here.
  
  void append( const T& t ) { this->change()->append(t); }
  // Appends t to this VectorPtrOf.
  
  void prepend( const T& t ) { this->change()->prepend(t); }
  // Prepends t to this VectorPtrOf.

  void shrink( int newLength ) { this->change()->shrink(0, newLength); }
  void shrink( int start, int newLength )
  { this->change()->shrink(start, newLength); }

  // I/O :
 
  // @stc these should not be inlined here but its easier than
  // fighting with g++'s template shortcomings
  inline friend ostream& operator << ( ostream& o, const VectorPtrOf& v ) {
 
    o << "<";
    if ( v.length() == 0 )
      o << " ";
    else {
      if( v.isValid(0) )
	o << v[0];
      else
	o << "-?-";
    }
    for ( int i = 1; i < v.length(); i++ ) {
      o << ",";
      if( v.look()->isValid(i) )
	o << v[i];
      else
	o << "-?-"; 
    }
    o << ">";
    return o;
  }
 
private:

};


template < class T > class VectorItemRef {
  
public:
  
  // no default constructor because of reference members
  // destructor compiler-supplied
  
  T& operator = ( const T& t ) { 
    if (elPoint) 
      *elPoint = t;
    else {
      elPoint = *elAddr = new T(t);
#if ( SAFETY > 1 )
      if( !elPoint )
	error("Cannot allocate memory in VectorItemRef::operator=(T&)");
#endif
     
    }
    return *elPoint;
  }

  bool operator == ( const T& t ) { 
    if (elPoint)
      return *elPoint == t; 
    else
      return false;
  }
  // if a global operator == with two T arguments is defined, the above
  // is not necessary; if T only has a member operator == with one T
  // argument, then the above is necessary, since the ARM excludes
  // type conversion to apply a method.
 
  operator T( ) { 
    if (!elPoint)
      error("VectorPtrOf access denied to uninitialized element "
	    "in  VectorItemRef::operator T()");
    return *elPoint;
  }
  
private:

  friend class VectorPtrRep<T>; //@@rn only op[] when possible.

  VectorItemRef( T* p) : elPoint(p) { }
  // Hide this from unauthorized users.

  VectorItemRef( T** addr, T* p) : elPoint(0), elAddr(addr) { }
  // Hide this from unauthorized users.

  // data members


  T* elPoint;
  T** elAddr;

private:

  //@dp old notes: no assignment operator generated by compiler because of ref member.
  //@dp Really, compiler (gcc 2.7.2) generates default operator=(const VectorItemRef&)
  //    like for the structures that is wrong. We need this operator in expressions 
  //    like this:  a[i] = b[i] where b[i] must be initialized.
  VectorItemRef& operator = ( const VectorItemRef& ref );

  // make copy constructor inaccessible

  // @db porting

public: 

  VectorItemRef( const VectorItemRef& ref )
  {
    if( !ref.elPoint ) {
      error("VectorPtrOf<T>: access denied to uninitialized element t"
	    "in  VectorItemRef::operator=(const VectorItemRef& t)");
      
    }
    if( this != &ref ) {
      if( elPoint )
	*elPoint = *ref.elPoint;
      else
	elPoint = *elAddr = new T( *ref.elPoint );
    }
  }

};

#endif

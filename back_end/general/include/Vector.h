//   Copyright (C) 1994 The New York Group Theory Cooperative
//   See magnus/doc/COPYRIGHT for the full notice.
 
// Contents: Definition of VectorOf<T> and VectorRep<T> class
//
// Principal Author: Stephane Collart, Roger Needham
//
// Status: Useable.
//
// Revision History:
//
// * 27.10.94 StC gave VectorItemRef::operator= a reference return type
// * 11.11.94 Stc gave Vector a ctor to make a vector of specified length
//                initialised from another vector
// * 3/95 sr added
//        int hash() const
//        so that sets of vectors are (marginally) possible.
// * 3/95 sr added
//        VectorOf( int len, bool e )
//        VectorOf( int len, bool e, const VectorOf& v )
//
// * 01/96 Dmitry B. implemented IPC tools.
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//
// * 2/97 EP added a method:
//           const T& constref(int i) const;
//        to enhance the i-th element of the vector.
//
//
// * 02/97 dp fixed VectorRep::shrink(int start, int newlen).
//
// * 03/97 dp remove empty body from private member (hidden, not to be implemented)
//            VectorRep& VectorRep::operator = ( const VectorRep& );
//
// Special Notes:
//
// * To instantiate VectorOf<T>, class T must have
//   1) A default constructor
//   2) A copy constructor
//   3) An assignment operator
//   4) An == operator
//   5) A destructor
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
 
#ifndef _VECTOR_H_
#define _VECTOR_H_
 

#include "global.h"
#include "RefCounter.h"
#include "ObjectOf.h"
#include "Chars.h"
#include "ExtendedIPC.h"

template <class T> struct VectorRep : public RefCounter {
  
  public :
  
  // copy constructor does deep copy
  
  VectorRep( const VectorRep& vr ) {
	 
	 len = vr.last - vr.first;
	 first = 0;
	 last = len;
	 vec = new T[len];
#if ( SAFETY > 1 )
	 if( !vec )
	   error("Cannot allocate memory in VectorRep::VectorRep( const VectorRep& vr )");
#endif
	 for( int i = 0; i < len; i++ )
		vec[i] = vr.vec[vr.first + i];
	 fastExpansion = false;
  }
  
  VectorRep( int l ) {
	 len = l;
	 first = 0;
	 last = len;
	 vec = new T[len];
#if ( SAFETY > 1 )
	 if( !vec )
	   error("Cannot allocate memory in VectorRep::VectorRep(int)");
#endif
	 fastExpansion = false;
  }
  // creates an uninitialized vector of length l
  
  VectorRep( int l, bool e ) {
	 len = l;
	 first = 0;
	 last = len;
	 vec = new T[len];
#if ( SAFETY > 1 )
	 if( !vec )
	   error("Cannot allocate memory in VectorRep::VectorRep(int,bool)");
#endif
	 fastExpansion = e;
  }
  // creates an uninitialized vector of length l

  ~VectorRep( ) { delete [] vec; }
  
  // standard cloning operation for representations
  
  VectorRep* clone( ) { return new VectorRep( *this ); }

  int length() const { return last - first; }

  // for reference access
  T& ref(int i) {
    #if ( SAFETY > 0 )
	   if ( i < 0 || i >= last - first ) error("VectorOf index out of bounds"
			" in T& VectorRep::ref(int)");
	 #endif
	 return vec[first + i];
  }
  
  // for constant reference access
  const T& constref(int i) const {
    #if ( SAFETY > 0 )
	   if ( i < 0 || i >= last - first ) error("VectorOf index out of bounds"
			" in const T& VectorRep::constref(int)");
	 #endif
	 return vec[first + i];
  }

  // for value access
  T val(int i) const {
    #if ( SAFETY > 0 )
	   if ( i < 0 || i >= last - first ) error("VectorOf index out of bounds"
			" in T& VectorRep::val(int) const");
	 #endif
	 return vec[first + i];
  }

  void append( const T& t ) {
	 if ( last < len ) {
		vec[last++] = t;
	 }
	 else {
		if (fastExpansion && len) len *= 2; else len++;
		T* new_vec = new T[len];
#if ( SAFETY > 1 )
		if( !new_vec )
		  error("Cannot allocate memory in VectorRep::append()");
#endif
		int j = 0;
		for( int i = first; i < last; i++ )
		  new_vec[j++] = vec[i];
		delete [] vec;
		vec = new_vec;
		vec[j++] = t;
		last = j;
		first = 0;
	 }
  }

  void prepend( const T& t ) {
	 if ( first > 0 ) {
		vec[--first] = t;
	 }
	 else {
		if (fastExpansion && len) len *= 2; else len++;
		T* new_vec = new T[len];
#if ( SAFETY > 1 )
		if( !new_vec )
		  error("Cannot allocate memory in VectorRep::prepend()");
#endif
		int j = 0;
		new_vec[j++] = t;
		for( int i = first; i < last; i++ )
		  new_vec[j++] = vec[i];
		delete [] vec;
		vec = new_vec;
		last = j;
		first = 0;
	 }
  }

  void shrink( int start, int newlen ) {
    #if ( SAFETY > 0 )
      // if ( start < first || start >= last || newlen > last - first )
      //if ( start < 0 || first + start >= last || newlen > last - first )
      if ( start < 0 || newlen < 0 || newlen > last - first - start )
        error("argument to VectorRep::shrink out of bounds");
    #endif
    // The semantics are dangerous if we allow shrink to `expand' the VectorOf:
    // a copy construction may throw the `extra' stuff away in between
    // calls to shrink.
    first += start;
    last = first + newlen;
  }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void write( ostream& ostr ) const
  {
#ifdef DEBUG
    ostr < Chars("Vector<") < ' ';
#endif    

    ostr < fastExpansion < first < last < len;
    for( int i = 0; i < len; ++i ) {
       ostr < vec[i];
       ostr < '\n'; //@dp temp
      // temporary cludge to avoid gcc 2.6.3 template bugs
//      cludgeWrite(ostr, vec[i]);
    }
#ifdef DEBUG
    ostr < ' ' < Chars(">Vector");
#endif
  }

  void read( istream& istr )
  {
    delete [] vec;

#ifdef DEBUG
    {
      Chars header;
      istr > header;
      if( header != Chars("Vector<") ) {
	error("Cannot load Vector<T>: bad header of the data section.");
      }
    }
#endif

    istr > fastExpansion > first > last > len;
    vec = new T[len];
#if ( SAFETY > 1 )
    if( !vec )
      error("Cannot allocate memory in VectorRep::read()");
#endif
    for( int i = 0; i < len; ++i ) {
      istr > vec[i];
      // temporary cludge to avoid gcc 2.6.3 template bugs
//      cludgeRead(istr, t);
    }

#ifdef DEBUG
    {
      Chars footer;
      istr > footer;
      if( footer != Chars(">Vector") ) {
	error("Cannot load Vector<T>: bad footer of the data section.");
      }
    }
#endif
  }


//@@db
  
/*  inline void cludgeWrite( ostream& ostr, const T& t ) const { ostr < t; }

  inline void cludgeRead( istream& istr, T& t ) { istr > t; }*/

   
private :
  
  // assignment operator undesired : made private
  
  VectorRep& operator = ( const VectorRep& );//@dp { }//@rn
  // hidden, not to be implemented

  // data members
  
  bool fastExpansion; // true if expansion should be done by doubling space 
  unsigned int first; // index of first valid entry
  unsigned int last;  // index + 1 of last valid entry
  unsigned int len;   // actual length of storage, so last - first <= len
  
  T* vec;

};

/*
//@db temporary cludges to avoid gcc template bugs

inline void VectorRep<Chars>::cludgeWrite( ostream& ostr, const Chars& t )
       const { ostr < t; }

inline void VectorRep<Chars>::cludgeRead( istream& istr, Chars& t )
       { istr > t; }

inline void VectorRep<int>::cludgeWrite( ostream& ostr, const int& t ) const
       { ostr < t; }

inline void VectorRep<int>::cludgeRead( istream& istr, int& t ) { istr > t; }

//@db end of cludges
*/


// ------------------------------- VectorOf -------------------------------- //


//@db 2.91

template <class T> class VectorOf;

template<class T>
ostream& operator < ( ostream& ostr, const VectorOf<T>& v )
{
  v.look()->write(ostr);
  return ostr;
}

template<class T>
istream& operator > ( istream& istr, VectorOf<T>& v )
{
  v.change()->read(istr);
  return istr;
}

//@db end


template <class T> class VectorOf : public ObjectOf< VectorRep<T> > {
  
  typedef VectorRep< T > Tvr;
  typedef ObjectOf< Tvr > Rep;
  
public:
  
  // copy constructor, operator=, and destructor supplied by compiler.
  
  VectorOf( int len = 0 ) : Rep( new Tvr(len) ) { }

  VectorOf( int len, bool e ) : Rep( new Tvr(len,e) ) { }
  // When e is true, the vector length doubles when an append or prepend
  // needs more space (instead of increasing by 1).

  VectorOf( int len, const VectorOf& v ) : Rep( new Tvr(len) ) {

	for (int i = 0; i < min( len, v.length() ); i++)
		this->enhance()->ref(i) = v[i];
  }
  // to make a vector of given length, (partly) initialized with
  // (part of) another vector

  VectorOf( int len, bool e, const VectorOf& v ) : Rep( new Tvr(len,e) ) {
	for (int i = 0; i < min( len, v.length() ); i++)
		this->enhance()->ref(i) = v[i];
  }
  // See comment for VectorOf( int len, bool e ).

  int operator == ( const VectorOf& v ) const
  {
	 if (this->look() == v.look()) return 1;
	 if ( this->look()->length() != v.look()->length() ) return 0;
	 int i = this->look()->length();
	 while ( i-- ) if ( !(this->look()->val(i) == v.look()->val(i)) ) return 0;
	 return 1;
  }
  
  int operator != ( const VectorOf& v ) const { return !(*this == v); }

  T operator [] ( int i ) const { return this->look()->val(i); }

  T& operator [] ( int i ) { return this->change()->ref(i); }

  //@rn Temporary (perhaps should be permanent) additions of Sergei:

  T val( int i ) const { return this->look()->val(i); }
  T& ref( int i ) { return this->change()->ref(i); }
  const T& constref( int i ) const { return this->look()->constref(i); }

  //@rn
  
  int length( ) const { return this->look()->length(); }

  int hash() const { return this->look()->length(); }
  //@rn Replace this in specific template instances if you want
  //    any semblance of efficiency.
  
  int indexOf( const T& t ) const {
	 int i = length();
	 while ( i-- ) if ( this->look()->val(i) == t ) return i;
	 return -1;
  }
  // Returns the index of t in this VectorOf, or -1 if not here.
  
  void append( const T& t ) { this->change()->append(t); }
  // Appends t to this VectorOf.
  
  void prepend( const T& t ) { this->change()->prepend(t); }
  // Prepends t to this VectorOf.

  void shrink( int newLength ) { this->change()->shrink(0, newLength); }
  void shrink( int start, int newLength )
        { this->change()->shrink(start, newLength); }

// I/O :
 
    // @stc these should not be inlined here but its easier than
    // fighting with g++'s template shortcomings
    inline friend ostream& operator << ( ostream& o, const VectorOf& v ) {
 
        o << "<";
        if ( v.length() == 0 )
            o << " ";
        else
            o << v[0];
        for ( int i = 1; i < v.length(); i++ ) o << "," << v[i];
        o << ">";
        return o;
    }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < <T>( ostream& ostr, const VectorOf& v );
  
  friend istream& operator > <T>( istream& istr, VectorOf& v );
  
private:


};



// concatenate the given vectors

template <class T>
inline VectorOf<T> concatenate(const VectorOf<T>& v1, const VectorOf<T>& v2)
{
  VectorOf<T> res(v1.length()+v2.length());
  int pos = 0;
  for(int i = 0; i < v1.length(); i++)
    res[pos++] = v1[i];
  for(int j = 0; j < v2.length(); j++)
    res[pos++] = v2[j];
  return res;
}


#endif

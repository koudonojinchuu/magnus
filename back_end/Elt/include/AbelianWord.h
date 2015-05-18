// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the classes AbelianWord, AbelianWordRep
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Usage:
//
// Special Notes:
//
// Revision History:
//

#ifndef _AbelianWord_h_
#define _AbelianWord_h_

#include "Integer.h"
#include "Vector.h"
#include "PureRep.h"
#include "Word.h"


// --------------------------- AbelianWordRep ------------------------------ //


class AbelianWordRep : public PureRep
{

public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Constructors:                                                      //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // No default constructor
  // Copy constructor provided by compiler (does logical deep copy).

  AbelianWordRep( ) { }

  AbelianWordRep( int numOfGens, const Word& w );

  AbelianWordRep( const VectorOf<Integer>& v ) : thePowers( v ) { }
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Operators:                                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool operator == ( const AbelianWordRep& w ) const;
 
  Integer operator [] ( int i ) const;

  Integer& operator [] ( int i );

  AbelianWordRep* operator * ( const AbelianWordRep& aw ) const;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Accessors:                                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int numberOfGenerators( ) const { return thePowers.length(); }

  VectorOf<Integer> getPowers( ) const { return thePowers; }

  Word getWord( ) const;

  Integer fullLength( ) const;

  AbelianWordRep inverse( ) const;

  bool isTrivial( ) const;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  I/O                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
    
  void printOn( ostream& ) const;
  
  // Input operator is not yet implemented.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  IPC tools:                                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void write( ostream& ostr ) const;

  void read( istream& istr );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Representation methods:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  AbelianWordRep* clone( ) const { return new AbelianWordRep(*this); }
  // overrides PureRep::clone()
  

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Data members:                                                      //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  VectorOf<Integer> thePowers;
};


// ---------------------------- AbelianWord -------------------------------- //


class AbelianWord : public ObjectOf<AbelianWordRep>
{

public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Constructors:                                                      //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Copy constructor provided by compiler (does logical deep copy).

  AbelianWord( )
    : ObjectOf<AbelianWordRep>( new AbelianWordRep( ) ) { }

  AbelianWord( int numOfGens, const Word& w )
    : ObjectOf<AbelianWordRep>( new AbelianWordRep(numOfGens,w) ) { }

  AbelianWord( const VectorOf<Integer>& v )
    : ObjectOf<AbelianWordRep>( new AbelianWordRep(v) ) { }
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Operators:                                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Operator = provided by compiler.

  bool operator == ( const AbelianWord& w ) const {
    return ( (look() == w.look()) || (*look() == *w.look()) );
  }
  // Returns true if the word is equal to w.

  int operator != ( const AbelianWord& w ) const { return !( *this == w ); }
  // Returns true if the word is not equal to w.

  Integer operator [] ( int i ) const {
    return look() -> operator [](i);
  }
  // Returns the power of i-th generator in abelian form of the word.

  Integer& operator [] ( int i ) {
    return change() -> operator [](i);
  }
  // Returns the power of i-th generator in abelian form of the word.

  AbelianWord operator * ( const AbelianWord& w ) const {
	 return AbelianWord( *look() * *w.look() );
  }
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Accessors:                                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int numberOfGenerators( ) const {
    return look()->numberOfGenerators();
  }
  // Returns the number of different generators in the word presentation.

  VectorOf<Integer> getPowers( ) const {
    return look()->getPowers();
  }
  // Transforms an abelian word to a vector of generators powers

  Word getWord( ) const {
    return look()->getWord();
  }
  // Transforms an abelian word to an usual word. 

  Integer fullLength( ) const {
    return look()->fullLength();
  }
  // I.e. the sum of all powers.

  AbelianWord inverse( ) const {
    return new AbelianWordRep( look()->inverse() );
  }
  // Computes the inverse word.

  bool isTrivial( ) const {
    return look()->isTrivial();
  }
  // True if word is equal to 1.
///xxw this is only a test function testing Integer mapping
  Integer test(Integer orderofElt) const{
      return orderofElt;
  }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  I/O                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator << ( ostream& ostr, const AbelianWord& w )
  {
    w.look()->printOn(ostr);
    return ostr;
  }
  
  // Input operator is not yet implemented.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const AbelianWord& w )
  {
    w.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, AbelianWord& w )
  {
    w.change()->read(istr);
    return istr;
  }


protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Protected functions:                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AbelianWord( AbelianWordRep* newrep ) 
    : ObjectOf<AbelianWordRep>(newrep) { }

};

#endif

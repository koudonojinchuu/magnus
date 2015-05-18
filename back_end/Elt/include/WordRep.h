// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of WordRep class.
//
// Principal Author: Stephane Collart
//
// Status: Useable.
//
// Revision History:
//
// * 01/96 Dmitry B. implemented IPC tools.
//

#ifndef _WORD_REP_H_
#define _WORD_REP_H_


#include "EltRep.h"
#include "WordData.h"


struct WordRep : EltRep, WordData {

// constructors:
  
  WordRep( int len ) : WordData(len) { }

  WordRep( const Generator& x ) : WordData(x) { }

  WordRep( const Generator& x, const Generator& y ) : WordData(x, y) { }

  WordRep( const VectorOf<Generator>& v ) : WordData(v) { }

  WordRep( const ListOf<Generator>& l ) : WordData(l) { }

  WordRep( const GeneratorType *p, int len ) : WordData(p, len) { }

// Representation and type stuff:
  
  PureRep* clone( ) const { return new WordRep(*this); }

  static const Type theWordType;

  static Type type( ) { return theWordType; }

  Type actualType( ) const { return theWordType; }
  // overrides EltRep::actualType()

// :

  Bool operator == ( const EltRep& ) const;
  
  int hash() const;

  EltRep* operator * ( const EltRep& a ) const;

  EltRep* inverse() const;

  EltRep* rightMultBy( const Generator& x ) const;

  EltRep* leftMultBy( const Generator& x ) const;

  EltRep* conjugateBy( const EltRep* ep ) const;

  EltRep* commutatorWith( const EltRep* ep ) const;

  void printOn(ostream&) const;
  // overrides Elt::printOn();

  void debugPrint(ostream&) const;
  // overrides Elt::debugPrint();


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
    
  void write( ostream& ostr ) const
  {
    ostr < len;
    for( int i = 0; i < len; ++i )
      ostr < wrd[i];
  }

  void read( istream& istr )
  {
    delete [] wrd;
    istr > len;
    wrd = new GeneratorType[len];
    for( int i = 0; i < len; ++i ) {
      istr > wrd[i];
#ifdef DEBUG
      if( wrd[i] == 0 ) {
	error("WordRep::read(istream&): Zero generator was read");
      }
#endif
    }
  }

};

#endif

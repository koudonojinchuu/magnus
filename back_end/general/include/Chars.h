//   Copyright (C) 1994 The New York Group Theory Cooperative
//   See magnus/doc/COPYRIGHT for the full notice.
 
// Contents: Definition of Chars and CharsRep classes
//
//   Chars is a char* which knows its length, wrapped up as a user class.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
//
// * 6/23/95 Roger flushed the old version, which used the libg++
//           String class, when he discovered the hard way that a
//           String cannot be longer than 2^15.
//
// * 01/96 Dmitry B. implemented IPC tools.
//
// * 2/97 EP added: 
//           Chars(int num);  // constructor
//           Chars ordinalPostfix(int num); // global
//
// * 03/97 dp made destructor ~CharsRep() virtual.
//
// Next Implementation Steps:
//
// * Make more versions of Chars::op == and Chars::op !=
//   which do not require conversion of the arguments.
//
 
#ifndef _CHARS_H_
#define _CHARS_H_
 

#include <string.h>
#include "RefCounter.h"
#include "ObjectOf.h"


//------------------------------------------------------------------------//
//---------------------------- CharsRep ----------------------------------//
//------------------------------------------------------------------------//


class CharsRep : public RefCounter
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  CharsRep( );  // Gives empty string

  CharsRep( const char* s );

  CharsRep( char c );

  CharsRep( const CharsRep& CR );

  CharsRep( int num );

  virtual ~CharsRep( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int len( ) const { return length; }

  const char* getChars( ) const { return theChars; }
  
   // for reference access
  char& ref(int i);
  
  // for value access
  char val(int i) const;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Misc:                                                               //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  CharsRep* clone( ) const { return new CharsRep( *this ); }

  void concat( const char*, int len );

  void readFrom(istream& istr);


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  virtual void write( ostream& ostr ) const;
 
  virtual void read( istream& istr );
 

private:

  CharsRep operator = ( const CharsRep& );  // Hide this

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int length;

  char* theChars;

  char special[2];
  // To not use the heap even more in the special cases of the
  // empty string, and the one char string.

};


//------------------------------------------------------------------------//
//------------------------------ Chars -----------------------------------//
//------------------------------------------------------------------------//


class Chars : public ObjectOf<CharsRep>
{
public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  // copy constructor supplied by compiler.

  Chars( ) : ObjectOf<CharsRep>( new CharsRep() ) { }  // Gives empty string
  
  Chars( const char* s ) : ObjectOf<CharsRep>( new CharsRep(s) ) { }
  
  Chars( const char c ) : ObjectOf<CharsRep>( new CharsRep(c) ) { }
  
  Chars ( int num ) : ObjectOf<CharsRep>( new CharsRep(num) ) { }
  
  // destructor supplied by compiler.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  int length( ) const { return look()->len(); }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Operators:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  // assignment operator supplied by compiler.

  inline friend int operator == ( const Chars& l, const Chars& r ) {
    return ( ! strcmp(l.look()->getChars(), r.look()->getChars()) );
  }

  bool operator == ( const char * s) const {
    return ! strcmp( look()->getChars(), s );
  }
 
  inline friend int operator != ( const Chars& l, const Chars& r ) {
    return ( !(l == r) );
  }

  inline friend int operator != ( const Chars& l, const char* r ) {
    return ( !(l == r) );
  }
  // op == and op != global to permit symmetric promotion of arguments.
  
  Chars operator + (const Chars&) const;
  
  Chars operator + (const char*) const;
  
  Chars operator + (char) const;
  
  Chars& operator += (const Chars&);

  Chars& operator += (const char*);

  Chars& operator += (char);
  
  operator const char*( ) const { return look()->getChars(); }
  
  char operator [] ( int i ) const { return look()->val(i); }
  
  char& operator [] ( int i ) { return change()->ref(i); }

  friend ostream& operator << ( ostream& ostr, const Chars& C ) {
    ostr << C.look()->getChars();
    return ostr;
  }
  
  friend istream& operator >> ( istream& istr, Chars& C ) {
	 C.change()->readFrom(istr);
	 return istr;
  }
  // This reads a whitespace-delimited string which cannot be
  // longer than 2^15.

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const Chars& c )
  {
    c.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, Chars& c )
  {
    c.change()->read(istr);
    return istr;
  }

};

//========================================================================
//                    Global scope functions
//========================================================================

Chars ordinalPostfix(int num);
// returns ordinal postfix for given number, i.e. "st", "nd", "rd" or "th"

Chars encodeSpaces( const Chars& s );
// Encodes all spaces with escape char. The original escape char is encoded
// with double escape.

Chars decodeSpaces( const Chars& s );
// Decodes string restoring spaces.

#endif

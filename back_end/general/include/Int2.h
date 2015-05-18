// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition and implementation of class Int2
//
// Principal Author: Dmitry Bormotov
//
// Status: temporary
//
// Revision History:


#ifndef _Int2_H_
#define _Int2_H_


#include "global.h"


struct Int2 {

  Int2( ) : v(0) { }

  Int2( int x ) : v(x) { }

  int compare( Int2 x ) const {
    if( this->v < x.v ) return -1;
    else if( this->v > x.v ) return 1;
    else return 0;
  }

  friend ostream& operator << ( ostream& ostr, const Int2& i )
  {
    ostr << i.v;
    return ostr;
  }

  bool operator == ( const Int2& i ) const { return v == i.v; }

  bool operator != ( const Int2& i ) const { return !(*this == i); }

  Int2 operator - ( ) const { return -v; }

  Int2 operator + ( const Int2& i ) const { return v + i.v; }

  Int2& operator += ( const Int2& i ) { v += i.v; return *this; }

  Int2 operator * ( const Int2& i ) const { return v * i.v; }

  Int2& operator *= ( const Int2& i ) { v *= i.v; return *this; }

  bool operator < ( const Int2& i ) const { return v < i.v; }

  bool operator > ( const Int2& i ) const { return v > i.v; }

  int value( ) const { return v; }

private:

  int v;
};

#endif

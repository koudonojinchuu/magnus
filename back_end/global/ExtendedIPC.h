// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Global operators for Interprocess Communication (IPC).
//
// Principal Author: Dmitry Bormotov
//
// Status: Under development
//
// Special Notes:
//
//   This operators are for special cases, i.e. they are seldom useable. 
//

#ifndef _EXTENDED_IPC_H_
#define _EXTENDED_IPC_H_


#include "global.h"
#include "Integer.h"
#include "Rational.h"


// ------------------------- Output operators ------------------------------ //


inline ostream& operator < ( ostream& ostr, const Integer& n )
{
  ostr << n << ' ';
  return ostr;
}

inline ostream& operator < ( ostream& ostr, const Rational& r )
{
  ostr << r << ' ';
  return ostr;
}


// ------------------------- Input operators ------------------------------- //


inline istream& operator > ( istream& istr, Integer& n )
{
  istr >> n;
  return istr;
}

inline istream& operator > ( istream& istr, Rational& r )
{
  istr >> r;
  return istr;
}


#endif


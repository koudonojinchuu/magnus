// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:
//
// Principal Author: Dmitry Pechkin
//
// Status: under trial.
//
// Revision History:
//
// TO DO:
//
//

#include "SuperGen.h"
#include "CONDITION.h"

void operator < ( ostream& ostr, const SuperGen& g )
{
  ostr < g.generator();
  ostr < ' ';
  ostr < g.exponent();
}

void operator > ( istream& istr, SuperGen& g )
{
  Generator gen;
  int exp;
  istr > gen;
  istr > exp;
  g = SuperGen(gen, exp);
}

ostream& operator << ( ostream& ostr, const SuperGen& g )
{
  ostr << ord(g.generator()) << '^' << g.exponent();
  return ostr;
}

istream& operator >> ( istream& istr, SuperGen& g )
{
  int generator;
  istr >> generator;

  char ch;
  istr >> ch;
  
  CONDITION( WARNING, ch == '^' );

  int exponent;
  istr >> exponent;

  g = SuperGen( generator, exponent);

  return istr;
}


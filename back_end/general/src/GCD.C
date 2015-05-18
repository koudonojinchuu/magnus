
// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:
//
// Principal Authors: Eugene Paderin, Dmitry Pechkin
//
// Status: draft
//
// Revision History:
//
// Discussion:
//
// Bugs:
//
//
// Special Notes:
//
//

#include "GCD.h"
#include "List.h"

int GCD(int a, int b)
{
  if( a < b ) {
    int temp = a;
    a = b;
    b = temp;
  }
  while( b != 0 ) {
    int k = a % b;
    a = b;
    b = k;
  }
  return a;
}

Integer GCD(Integer a, Integer b)
{
  if( a < b ) {
    Integer temp = a;
    a = b;
    b = temp;
  }
  while( b != 0 ) {
    Integer k = a % b;
    a = b;
    b = k;
  }
  return a;
}

//for given a, b finds the greatest common divisor and p, q 
//such that p*a + q*b = gcd(a,b).

int GCD(int a, int b, int& p, int& q) 
{
  ListOf<int> quot;

  int a_old = a, b_old = b;

  while( b != 0 ) {
    int k = a % b;
    quot.prepend(a / b);
    a = b;
    b = k;
  }

  p = q = 1;
  ListIterator< ListOf<int> > iter(quot);

  for( ; ! iter.done(); iter.next() ) {
    int new_q = p - q * iter.value();
    p = q;
    q = new_q;
  }

  // validity check

  if( p * a_old + q * b_old != a)
    error("Bad p,q computed in GCD(int, int, int&, int&)");

  return a;
}

Integer GCD(Integer a, Integer b, Integer& p, Integer& q) 
{
  ListOf<Integer> quot;

  Integer a_old = a, b_old = b;
  while( b != 0 ) {
    Integer k = a % b;
    quot.prepend(a / b);
    a = b;
    b = k;
  }

  p = q = 1;
  ListIterator< ListOf<Integer> > iter(quot);

  for( ; ! iter.done(); iter.next() ) {
    Integer new_q = p - q * iter.value();
    p = q;
    q = new_q;
  }

  // validity check

  if( p * a_old + q * b_old != a)
    error("Bad p,q computed in GCD(Integer, Integer, Integer&, Integer&)");

  return a;
}

int LCM(int a, int b) { return a * b / GCD(a,b); }

Integer LCM(Integer a, Integer b) { return a * b / GCD(a,b); }



// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
// 
// Contents: Implementation of class Permutation
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
//


#include "Permutation.h"
#include "PermutationParser.h"


// ------------------------------ Permutation ------------------------------ //


Permutation::Permutation( VectorOf<int> v ) 
  : perm( v )
{ }

ostream& operator << ( ostream& ostr, const Permutation& p )
{
  //ostr << "Magnus" << endl;
  
  int pLen = p.perm.length();
  ostr << "{";
  for( int i = 0; i < pLen; ++i )
    ostr << " " << p.perm[i];
  ostr << " }";

  return ostr;
}

void Permutation::printInGAP( ostream& ostr ) const
{
  ostr << "GAP" << endl;
}

void Permutation::readInGAP( istream&, Chars& )
{
  printInGAP(cout);
}

IStreamPoll operator >> ( istream& istr, Permutation& p )
{
  /*
  cout << "Magnus" << endl;
  Chars errMsg;
  return IStreamPoll( istr, errMsg );
  */

  Chars errMsg;
  PermutationParser P(istr);
  p = P.parsePermutation(errMsg);
  return IStreamPoll( istr, errMsg );
}

ostream& operator << ( ostream& o, const Permutation::GAPPermutation& gp ) 
{
  gp.perm.printInGAP(o);
  return o;
}
 
IStreamPoll operator >> ( istream& i, const Permutation::GAPPermutation& gp ) 
{
  Chars errMsg;
  gp.perm.readInGAP(i, errMsg);
  return IStreamPoll(i, errMsg);
}
    

Permutation Permutation::inverse( ) const
{
  /*
  GAP gap;
  if( gap.happy() ) {
    gap << "p:=" << this->inGAP() << ";" << "Inverse
  } 
  else warn("Sorry! All copies of GAP are busy and it is not possible to run another one.");
  */
}

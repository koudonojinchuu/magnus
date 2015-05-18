// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class PermutationParser
//
// Principal Author: Dmitry Bormotov
//
// Status: Under development
//
// Usage:
//
// Discussion:
//
// Revision History:
//


#include "config.h"
//#ifndef BSD
//#include <values.h>
//#endif
//
#include "List.h"
#include "conversions.h"
#include "PermutationParser.h"


//--------------------------- PermutationParser -----------------------------//


bool PermutationParser::isSpace( char c )
{
  return c == ' ' || c == '\t' || c == '\n';
}

bool PermutationParser::isDigit( char c )
{
  return isdigit(c) && c != '0';
}

void PermutationParser::eatWhite( )
{
  while( isSpace(istr.peek()) ) {
    istr.get(); ++pos;
  }
}

Permutation PermutationParser::parsePermutation( Chars& errMesg ) 
{
  ListOf<int> l;
  int len;
  bool bSuccess = false;
  pos = 0;

  // reading permutation

  eatWhite();
  if( istr.eof() || istr.get() != '{' ) {
    ++pos;
    errMesg = "'\\{' expected";
    return VectorOf<int>();
  }
  ++pos;

  while ( !istr.eof() ) {

    eatWhite();
    char c = istr.peek();
    
    if( c == '}' ) { // reading is finished; go to correctness check
      istr.get(); ++pos;
      bSuccess = true;
      break; 
    }

    if( !isDigit(c) ) {
      //cout << " c = " << c << endl;
      errMesg = "Integer or '\\}' expected";
      return VectorOf<int>();
    }

    int n;
    istr >> n;
    
    Chars tmp(n);
    pos += tmp.length();

    l.append(n);
    
    c = istr.peek();
    if( !isSpace(c) &&  c != '}') {
       errMesg = "space or '\\}' expected";
       return VectorOf<int>();
    }

    eatWhite();
  }

  if( !bSuccess ) {
     errMesg = "'\\}' expected";
     return VectorOf<int>();
  }
   
  // correctness check
  
  if( l.length() == 0 ) {
    errMesg = Chars("");
    return VectorOf<int>(); 
  }
  
  int max = l.length();
  
  VectorOf<int> r(max);
  for( int i = 0; i < max; ++i )
    r[i] = 0;

  pos = 0;
  ListIterator< ListOf<int> > I(l);
  for( I.reset(); !I.done(); I.next() ) {
    int v = I.value();
    
    if( v < 1 || v > max ) {
      std::ostrstream ostr;
      ostr << v << " is greater that the length of the permutation" << ends;
      errMesg = Chars(ostr.str());
      return VectorOf<int>(); 
    } 

    if( r[v-1] == 0 ) 
      r[v-1] = 1;
    else {
      std::ostrstream ostr;
      ostr << v << " occurs more than once" << ends;
      errMesg = Chars(ostr.str());
      return VectorOf<int>(); 
    }
		       
  }

  // check for trivial permutation
  bool bEqual = true;
  int k = 0;
  for( I.reset(); !I.done(); I.next(), ++k ) {
    int v = I.value();
    if( k+1 != v ) {
      bEqual = false;
      break;
    }
  }
  
  errMesg == Chars("");

  if( bEqual )
    return VectorOf<int>(); 
  else
    return makeVectorOf(l);
}


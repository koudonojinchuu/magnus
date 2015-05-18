// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class RipsConstruction
//
// Principal Authors: Dmitry Bormotov
//
// Status: In progress
//
// Special Notes:
//
// Revision History:
//


#include "RipsConstruction.h"
#include "FreeGroup.h"


// -------------------------- RipsConstruction ----------------------------- //


FPGroup RipsConstruction::getRipsConstruction( ostream* out ) const
{
  int c = 80, n = 80, nr = 0;
  VectorOf<Chars> v = theGroup.namesOfGenerators();
  int vLen = v.length();
  VectorOf<Chars> gens(vLen+2);
  for( int i = 0; i < vLen; ++i )
    gens[i] = v[i];

  for( int j = vLen; j <= vLen+1; ++j ) {

    char c;
    for( c = 'a'; c <= 'z'; ++c ) {
      int i;
      for( i = 0; i < j; ++i )
	if( c == gens[i] ) 
	  break;
      if( i >= j ) {
	gens[j] = Chars(c);
	break;
      } 
    }
    if( c >= 'z' ) return FPGroup();
  }

  //cout << gens[vLen] << " " << gens[vLen+1] << endl;

  /*
  gens[vLen] = "a";
  gens[vLen+1] = "b";
  */

  FreeGroup F(gens);
  SetOf<Word> rels = theGroup.getRelators();
  Word a = Generator(vLen+1);
  Word b = Generator(vLen+2);
  for( int i = 0; i < vLen; ++i ) {
    rels |= a.conjugateBy( Word(Generator(i+1)) );
    rels |= a.conjugateBy( Word(Generator(-i-1)) );
    rels |= b.conjugateBy( Word(Generator(i+1)) );
    rels |= b.conjugateBy( Word(Generator(-i-1)) );
  }

  int numOfRels = rels.cardinality();
  SetOf<Word> R;
  SetIterator<Word> I(rels);

  for( ; !I.done(); I.next() ) {

    int rLen = c * ( 2 * n + c - 1 );
    VectorOf<Generator> r(rLen);
    int ri = -1;

    for( int i = 0; i < c; ++i ) {
      for( int j = 0; j < n; ++j )
	r[++ri] = Generator(vLen+1);
      for( int j = 0; j < n; ++j )
	r[++ri] = Generator(vLen+2);
      ++n;
    }
    
    Word w = (Word(I.value()).inverse() * Word(r)).freelyReduce();
    R |= w;
   
    if( out != 0 ) {
      *out << ++nr << ": ";
      F.printWord(*out, w); 
      *out << endl << endl;
    }
  }
  
  if( out != 0 )
    *out << end;
  
  return FPGroup(gens,R);
}

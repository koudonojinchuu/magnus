// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class FreeByCyclic
//
// Principal Author: Roger Needham
//
// Status: In progress.
//
// Revision History:
//
//  10/98 Dmitry Pechkin implemented:
//        FreeByCyclic& FreeByCyclic::operator=( const FreeByCyclic& G );
//        ostream& operator<<( ostream& ostr, const FreeByCyclic& G );
//        istream& operator>>( istream& istr, FreeByCyclic& G );
//

#include "FreeByCyclic.h"


FreeByCyclic::FreeByCyclic(const FreeGroup F, const Map aut) :
  theGroup(F),
  theAut(aut),
  theAutInv( Map(theGroup, theGroup,
		 theGroup.inverseAutomorphism(aut.generatingImages())
		 )
	     ),
  newGenerator( theGroup.numberOfGenerators() + 1 )
{
}

FreeByCyclic& FreeByCyclic::operator=( const FreeByCyclic& G )
{
  (FreeGroup&)theGroup  = G.theGroup;
  (Map&) theAut = G.theAut;
  (Map&) theAutInv = G.theAutInv;
  newGenerator      = G.newGenerator;

  return *this;
}



VectorOf<Chars> FreeByCyclic::namesOfGenerators( ) const
{
  //@rn  An rather clumsy way of guaranteeing a unique name
  //@rn  for the new generator:

  VectorOf<Chars> temp = theGroup.namesOfGenerators();
  Chars name("t");
  while ( temp.indexOf( name ) != -1 )
	 name += "t";

  temp.append( name );
  return temp;
}


SetOf<Word> FreeByCyclic::getRelators( ) const
{
  // Construct set of relators of the form t^-1 x t = theAut(x)
  // for each generator x, where t is the new generator.

  SetOf<Word> relators( newGenerator - 1 );

  VectorOf<Generator> conj(3);
  conj[0] = Generator(-newGenerator);
  conj[2] = Generator(newGenerator);
  for( int x = 1; x < newGenerator; ++x ) {
    conj[1] = Generator(-x);
    relators.adjoinElement( theAut.imageOf(Generator(x)) * Word( conj ) );
  }

  return relators;
}


Word FreeByCyclic::normalForm(const Word& w) const
{
  // Strategy: let X be the generators of the free group, a be the
  // automorphism, and t be the new generator representing a. Rewrite
  // any subword W(X) t == t t^-1 W(X) t == t a(W(X)), and any subword
  // W(X) t^-1 == t^-1 t W(X) t^-1 == t^-1 a^-1(W(X)), until w has the
  // form W(t) W(X). Freely reducing gives the normal form.

  // Tactics: we get t's exponent sum in w, thus giving W(t), then
  // scan through w for the max'l subwords W(X) between t's, and
  // cumulate the product of the a(W(X))'s and a^-1(W(X))'s.

  int i = 0;     // index of leftmost letter in max'l subword W(X)
  int j = 0;     // index of rightmost letter + 1
  Word result;
  int len = w.length();

  while ( i < len ) {
    while ( i < len && abs(ord(w[i])) == newGenerator ) ++i;
    
    if ( i < len ) {
      j = i + 1;
      while ( j < len && abs(ord(w[j])) != newGenerator ) ++j;
      
      result *= w.subword(i, j);		
      if ( j < len ) {
	if ( ord(w[j]) > 0 ) {
	  result = theAut.imageOf(result);
	} else {
	  result = theAutInv.imageOf(result);
	}
      }
      i = j + 1;
    }
  }
  
  result = result.freelyReduce();
  
  int tExp = w.exponentSum( Generator(newGenerator) );
  if ( tExp == 0 ) {
    
    return ( result );

  } else {
    
    VectorOf<Generator> theTs( abs(tExp) );
    for( i = 0; i < abs(tExp); ++i )
      theTs[i]
	= ( tExp > 0 ? Generator(newGenerator) : Generator(-newGenerator) );
    
    return ( Word(theTs) * result );
  }
}

ostream& operator<<( ostream& ostr, const FreeByCyclic& G )
{
  ostr << G.theGroup << ' ' << G.theAut;
}

istream& operator>>( istream& istr, FreeByCyclic& G )
{
  FreeGroup F;
  Chars errMesg = istr >> F;
  Map M(F,F);
  istr >> M;
  G = FreeByCyclic(F,M);
}


// to shut-up linker about 'undefined reference'
static FreeByCyclic *F;
static int foo( FreeByCyclic& G) 
{
  *F = G;
}


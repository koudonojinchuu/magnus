#include "Whitehead.h"
#include "AP-fixups.h"

//#define DEBUG_WHITEHEAD

ElementaryWhiteheadAuto::ElementaryWhiteheadAuto( 
  const Generator& x, const Generator& y, const WhiteheadAutoType& wat )
  : g1(x), g2(y), theType(wat)
{
#if SAFETY > 0
  if( x == y || x == ::inv(y) )
      //|| wat == WA_PERMUTATION && ord(x) * ord(y) < 0 )
    error("Bad parameters in calling "
	  "ElementaryWhiteheadAuto::ElementaryWhiteheadAuto(x,y,wat) "
	  " - not an elementary automorphism.");
#endif

  if( ord(x) < 0 ) {

    g1 = ::inv(x);
    g2 = ::inv(y);

    switch( theType ) {
    case WA_LEFT_MULT: 
      // x^-1 -> y x^-1  =>  x -> x y^-1, change the type of auto.
      theType = WA_RIGHT_MULT;
      break;
    case WA_RIGHT_MULT:
      // x^-1 -> x^-1 y  =>  x -> y^-1 x, change the type of auto.
      theType = WA_LEFT_MULT;
      break;
#if SAFETY > 0
    case WA_PERMUTATION:
      break;
    default:
      error("Illegal type of ElementaryWhiteheadAuto on construction "
	    "of an automorphism.");
#endif
    }
  }
}

ElementaryWhiteheadAuto::ElementaryWhiteheadAuto( const Generator& x )
  : g1(x), g2(1), theType(WA_INVERSE)
{
  if( ord(x) < 0 )
    g1 = ::inv(x);
}

Word ElementaryWhiteheadAuto::imageOf( const Word& w ) const
{
  if( theType == WA_PERMUTATION ) {

    int  maxGen = abs(ord(w.maxOccurringGenerator()));
    VectorOf<Word> images( maxGen );
    for(int i = 0 ; i < maxGen; ++i )
      images[i] = Generator(i+1);
    int ordg1 = abs(ord(g1)) - 1;
    if( ordg1 < maxGen ) 
      images[ ordg1 ] = ( ord(g1) > 0 ? g2 : inv(g2) );
    int ordg2 = abs(ord(g2)) - 1;
    if( ordg2 < maxGen )
      images[ ordg2 ] = ( ord(g2) > 0 ? g1 : inv(g1) );
    
    return w.replaceGenerators( images );      
  }

  Word subst;
  switch( theType ) {
  case WA_LEFT_MULT:
    subst = g2 * g1;
    break;
  case WA_RIGHT_MULT:
    subst = g1 * g2;
    break;
  case WA_INVERSE:
    subst = ::inv(g1);
    break;
  }

  return w.replaceGeneratorWithWord( g1, subst );
}

ElementaryWhiteheadAuto ElementaryWhiteheadAuto::inverse( ) const
{
  switch( theType ) {
  case WA_INVERSE:
  case WA_PERMUTATION:
    return *this;
  case WA_LEFT_MULT:
  case WA_RIGHT_MULT:
    return ElementaryWhiteheadAuto( g1, inv(g2), theType );
  }
}

void ElementaryWhiteheadAuto::printGenerator( 
  ostream& ostr, const Generator& g) const
{
  ostr << 'x' << abs(ord(g));
  if( ord(g) < 0 ) {
    ostr << "^-1";
  }
}

void ElementaryWhiteheadAuto::printGenerator( 
  ostream& ostr, const Generator& g, const VectorOf<Chars>& names) const
{
  ostr << names[ abs(ord(g))-1 ];
  if( ord(g) < 0 ) {
    ostr << "^-1";
  }
}

void ElementaryWhiteheadAuto::printOn( ostream& ostr ) const
{
  ostr << "{ ";
  printGenerator( ostr, g1 );
  ostr << " -> ";

  switch( theType ) {
  case WA_INVERSE:
    printGenerator( ostr, ::inv(g1) );
    break;
  case WA_PERMUTATION:
    printGenerator( ostr, g2 );
    ostr << ", ";
    printGenerator( ostr, g2 );
    ostr << " -> ";
    printGenerator( ostr, g1 );
    break;
  case WA_LEFT_MULT:
    printGenerator( ostr, g2 );
    ostr << " * ";
    printGenerator( ostr, g1 );
    break;
  case WA_RIGHT_MULT:
    printGenerator( ostr, g1 );
    ostr << " * ";
    printGenerator( ostr, g2 );
    break;
  }

  ostr << " }";
}

void ElementaryWhiteheadAuto::printOn( 
  ostream& ostr, const VectorOf<Chars>& names ) const
{
  ostr << "{ ";
  printGenerator( ostr, g1, names );
  ostr << " -> ";

  switch( theType ) {
  case WA_INVERSE:
    printGenerator( ostr, ::inv(g1), names );
    break;
  case WA_PERMUTATION:
    printGenerator( ostr, g2, names );
    ostr << ", ";
    printGenerator( ostr, g2, names );
    ostr << " -> ";
    printGenerator( ostr, g1, names );
    break;
  case WA_LEFT_MULT:
    printGenerator( ostr, g2, names );
    ostr << " * ";
    printGenerator( ostr, g1, names );
    break;
  case WA_RIGHT_MULT:
    printGenerator( ostr, g1, names );
    ostr << " * ";
    printGenerator( ostr, g2, names );
    break;
  }

  ostr << " }";
}



WhiteheadAuto WhiteheadAuto::inverse() const 
{
  ListOf<ElementaryWhiteheadAuto> invAutoList;
  ListIterator< ListOf<ElementaryWhiteheadAuto> > I(autoList);
  while( !I.done() ) {
    invAutoList.prepend( I.value().inverse() );
    I.next();
  }
  return WhiteheadAuto(invAutoList);
}

Word WhiteheadAuto::imageOf(const Word& w) const 
{
  Word u = w;
  ListIterator< ListOf<ElementaryWhiteheadAuto> > I(autoList);

  while( !I.done() ) {
    u = I.value().imageOf(u).freelyReduce();
    // the word is freely reduced to avoid
    // its overgrowth (exponential!) when
    // the autoList is long enough
    I.next();
  }
  return u;
}

/*
VectorOf<Word> WhiteheadAuto::imageOf( const VectorOf<Word>& vec ) const
{
  int len = vec.length();
  VectorOf<Word> result( len );
  for( int i = 0; i < len; ++i ) 
    result[i] = imageOf( vec[i] );

  return result;
}
*/

Automorphism WhiteheadAuto::makeAutomorphism(const FGGroup& G) const 
{
  Automorphism M(G);
  int numOfGens = G.numberOfGenerators();

  VectorOf<Word> images( numOfGens );
  for(int i=0; i<numOfGens; i++) 
    images[i] = imageOf( Generator(i+1) );

  M.setGeneratingImages( images );
  return M;
}


void WhiteheadAuto::printOn( ostream& ostr ) const
{
  ostr << autoList;
}


int totalLengthOf( const VectorOf<Word>& vec ) 
{
  int sum = 0;
  for( int i = vec.length()-1; i >= 0; --i ) 
    sum += vec[i].length();
  return sum;
}

bool doesShorten( VectorOf<Word>& V, const WhiteheadAuto& aut ) 
{
  VectorOf<Word> tmp( V.length() );

  for(int i = V.length()-1; i >= 0; --i )
    tmp[i] = aut.imageOf( V[i] ).freelyReduce();

  if( totalLengthOf(tmp) < totalLengthOf(V) ) {
    V = tmp;
    return true;
  }

  return false;
}

bool findShortenAuto( ElementaryWhiteheadAuto& aut, VectorOf<Word>& V ) 
{
  if( totalLengthOf(V) <= V.length() )
    return false;

  int numberOfGenerators = 0;
  for( int i = V.length()-1; i >= 0; --i ) 
    numberOfGenerators = max( numberOfGenerators, 
			      abs(ord(V[i].maxOccurringGenerator())));

  for( int i = 1; i <= numberOfGenerators ; ++i ) {
    for( int j = 1; j <= numberOfGenerators ; ++j ) {
      if( i != j ) {
	Generator g(i), h(j);
	if(doesShorten(V,(aut=ElementaryWhiteheadAuto(g,h,     WA_RIGHT_MULT)))
	|| doesShorten(V,(aut=ElementaryWhiteheadAuto(g,inv(h),WA_RIGHT_MULT)))
	|| doesShorten(V,(aut=ElementaryWhiteheadAuto(g,h,     WA_LEFT_MULT )))
	|| doesShorten(V,(aut=ElementaryWhiteheadAuto(g,inv(h),WA_LEFT_MULT )))
	) {
	  // `aut' shortens `V'.
	  return true;
	}
      }
    }    
  }

  return false;
}


WhiteheadAuto whiteheadDecomposition( const VectorOf<Word>& vec ) 
{
  VectorOf<Word> V( vec );
  WhiteheadAuto result;

  bool done = false;

  ElementaryWhiteheadAuto aut( Generator(1) ); 
  // proper value will be assigned in `findShortenAuto'.

  while( findShortenAuto(aut, V) ) {
    result = result | aut;
  }

  bool allOnes = true;
  for( int i = V.length() - 1; i >= 0; --i ) {
    if( V[i].length() != 1 ) {
      allOnes = false;
      break;
    }
  }

  if( allOnes ) {

    // make all inverses.
    
    for( int i = 0; i < V.length(); ++i ) {      
      Generator g = V[i][0];
      if( ord(g) < 0 ) {
	V[i][0] = inv(g);
	result = result | ElementaryWhiteheadAuto( g );
      }
    }
    // make all permutations of generators.

    for( int i = 0; i < V.length(); ++i ) {
      Generator g = V[i][0];
      
      if( ord(g) != i+1 ) {
	for( int j = i+1; j < V.length(); ++j ) {
	  Generator h = V[j][0];
	  if( ord(h) == i+1 ) {
	    V[i][0] = h;
	    V[j][0] = g;
	    result = result | ElementaryWhiteheadAuto( g, h, WA_PERMUTATION );
	    break;
	  }
	}
      }
      
    }
        
  }

  return result.inverse();
}

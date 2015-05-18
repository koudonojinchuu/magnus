// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: 
//
// Principal Author:
//
// Status: in progress
//
// Usage:
//
//
// Special Notes:
//
// Revision History:
//


#include "HNNExtension.h"
#include "conversions.h"
#include "CONDITION.h"

HNNExtensionRep::HNNExtensionRep( const FPGroup& G )
  : FGGroupRep( G.namesOfGenerators() ),
    theFPGroup( G )
    //    theNameOfStableLetter( nameOfStableLetter ),
    //    theStableLetter( G.numberOfGenerators() )
{
}


Trichotomy HNNExtensionRep::isTrivial( ) const
{
  if( theNumberOfGenerators == 0 )
    return yes;

  return no;
}

Trichotomy HNNExtensionRep::isFinite( ) const
{
  return isTrivial();
}

Trichotomy HNNExtensionRep::isInfinite( ) const
{
  return !isFinite();
}

Trichotomy HNNExtensionRep::isAbelian( ) const
{
  for( int i = 1; i < theNumberOfGenerators; ++i ) {
    Word gen_i = Generator(i);
    for( int j = i+1; j <= theNumberOfGenerators; ++j ) {
      Word gen_j = Generator(j);
      if( wordProblem( commutator( gen_i, gen_j ) ) == no )
	return no;
    }
  }

  return true;
}



// Performs the inverse operation: multiplies the components of
// given decomposition. The reduction is not performed, but if the
// decomposition is "proper" (made by reducedDecomposition or
// freeDecomposition) then the product IS freely reduced.

Word HNNExtensionRep::compose(const VectorOf<Word>& v)
{
  int length = 0;
  for(int i = 0; i < v.length(); i++)
    length += v[i].length();

  VectorOf<Generator> result(length);
  for(int i = 0, pos = 0; i < v.length(); i++) {
    Word w = v[i];
    for(int j = 0; j < w.length(); j++)
      result[ pos++ ] = w[j];
  }
  return Word(result);
}


VectorOf<Word> HNNExtensionRep::decompositionOf( const Word& w ) const
{
  int wLen = w.length();
  
  if( wLen == 0 )
    return VectorOf<Word>(1); // empty decomposition consist of empty word.
  
  ListOf<Word> deco;
  Word identity;

  Word component;
  int stable = ord( stableLetter() );
  int lastGen = abs(ord(w[0]));
  int start = 0;
  for( int i = 0; i < wLen; ++i ) {
    if( abs(ord(w[i])) == stable ) {
      deco.append( w.subword(start,i) ); // a basic group's element.
      deco.append( w.subword(i, i+1) );  // a power of the stable letter.
      start = i+1;
    }
  }
  if( start < wLen ) deco.append( w.subword(start, wLen) );

  // 
  // make sure that the decomposition is :   
  // 
  //     w_0 * t^e_1 * w_1* t^e_1* ... * w_n-1 * t^e_n * w_n
  //
  // where w_i belong to the basis group of HNN-extension.
  // 

  if( abs(ord(deco[deco.length()-1][0])) == stable ) // Last component 
    deco.append( Word() );

  return makeVectorOf( deco );
}

int HNNExtensionRep::powerOfStableGen( 
  int component, const VectorOf<Word>& deco ) const
{
  if( component < 0 && component >= deco.length() )
    return 0;

  const Word w = deco[component];

  CONDITION( ERROR, w.length() == 1 );

  const int g = ord(w[0]);
  const int stable = theNumberOfGenerators;

  CONDITION( ERROR, abs(g) == stable );

  return ( g > 0 ? 1 : -1 );
}

bool HNNExtensionRep::suspectPinch( int component, const VectorOf<Word>& deco )
  const
{
  if( powerOfStableGen(component-1, deco) 
      * powerOfStableGen(component+1,deco) == -1 )
    return true;
  return false;  
}

HNNExtensionRep::Pinch 
HNNExtensionRep::formPinch( int component, 
				    const VectorOf<Word>& deco ) const
{
  if( powerOfStableGen(component-1, deco) == -1 )
    return UP;

  CONDITION( WARNING,  powerOfStableGen(component+1, deco) == -1 );

  return DOWN;
}



VectorOf<Word> HNNExtensionRep::reducedDecompositionOf( const Word& w ) const
{
  VectorOf<Word> deco = decompositionOf( w.freelyReduce() );

  for( int component = 2; component < deco.length()-2; component += 2 ) {

    if( component > 1 && suspectPinch( component, deco ) ) {

      Pinch pinch = formPinch(component, deco);
      Word pinchWord = deco[component];

      // investigate the `pinch' to be a proper pinch.
      // pinch.word == deco[component]

      if( pinch == UP && subgroupContains(A, pinchWord) 
	  || pinch == DOWN && subgroupContains(B, pinchWord) ) {
	
	NumberOfSubgroup S = ( pinch == UP ? A : B );
	Word newPinch = mappingFromSubgroup( S, pinchWord /*InBasis*/ );
	
	// merging pinch with left and right components which exist always.
	
	newPinch = ( deco[component-2] * newPinch 
		     * deco[ component+2 ] ).freelyReduce();
	deco[ component-2 ] = newPinch;
	
	// remove 4 old  components from decomposition.
	int decoLen = deco.length();
	for( int pos = component + 3; pos < decoLen; ++pos ) 
	  deco[pos-4] = deco[pos];
	deco.shrink( decoLen - 4 );

	component -= 4;
      } // if the current pinch is proper.
    } // if( suspectPinch())
  } // for( component=..;..;..)

  return deco;
}

VectorOf<Word> HNNExtensionRep::normalDecompositionOf( const Word& w ) const
{

  // We start with reduced decomposition which already has a
  // minimal number of components.

  VectorOf<Word> decomposition = reducedDecompositionOf(w);

  // Shifting out trivial cases.

  if( decomposition.length() == 1 )
    return decomposition;

  CONDITION( WARNING, 
	     decomposition.length() > 2 && decomposition.length() % 2 == 1 );

  // 
  //  t^-1 a_i = b_i t^-1
  //  t b_i    = a_i t

  // The normal form of the word is: g[0] t^e1 g[1] t^e2 g[2] ... t^en g[n], 
  // where :
  //   g[0] is an element of the basic group, 
  //   g[1] ... g[n] are certain right representatives, 
  //   and any two successive g[i] g[i+1] belong to distinct factors.

  Word carry;

  for( int i = decomposition.length() - 1; i > 0; i -= 2 ) {

    Word component = decomposition[i];
    component = (component * carry).freelyReduce();
    NumberOfSubgroup S = ( powerOfStableGen(i-1,decomposition) == -1 ? A : B );
    Word rep = rightRepresentative( S, component );    
    Word subgroupElement =  component * rep.inverse();
    carry = mappingFromSubgroup( S, subgroupElement );
    decomposition[i] = rep;
  }

  decomposition[0] = ( decomposition[0] * carry ).freelyReduce();

  return decomposition;
}

VectorOf<Word> HNNExtensionRep::cyclicallyReducedDecompositionOf( 
  const Word& w, Word& conjugator ) const
{
  VectorOf<Word> deco = normalDecompositionOf( w );

  conjugator = Word();

  bool reduced = true;
  while( deco.length() > 2 && reduced ) {
    reduced = false;
    int decoLen = deco.length();
    Word elt = (deco[decoLen-1] * deco[0]).freelyReduce();

    deco[0] = elt;
    conjugator = deco[decoLen-1] * conjugator;
    deco.shrink( decoLen-1 );
    decoLen = deco.length();

    if( decoLen > 3 ) {

      // It means N>=1, where deco = (a_0, t^e0, ... , t^eN)
      VectorOf<Word> potencialPinch(3);
      potencialPinch[0] = deco[ decoLen-1 ];
      potencialPinch[1] = deco[0];
      potencialPinch[2] = deco[1];

      if( suspectPinch(1, potencialPinch) ) {
	Pinch pinch = formPinch( 1, potencialPinch );
	
	// investigate the `pinch' to be a proper pinch.

	if( pinch == UP && subgroupContains(A, elt) 
	    || pinch == DOWN && subgroupContains(B, elt) ) {	  
	  NumberOfSubgroup S = ( pinch == UP ? A : B );
	  conjugator = deco[ decoLen-1 ] * conjugator;
	  deco[2] = mappingFromSubgroup( S, elt ) * deco[2];

	  // Get out 2 heading and 1 tailing components.
	  deco.shrink( 2, decoLen-3 ); 

	  reduced = true;
	}	
      } // if( suspectPinch(1, potencialPinch) ) 
    }
  } // while();

  if( deco.length() == 1 ) {
    Word tmp = deco[0];
    conjugator = 
      tmp.terminalSegment( (tmp.length()-tmp.cyclicallyReduce().length()) / 2 )
      * conjugator;
    deco[0] = tmp.cyclicallyReduce();
  }

  return deco;
}



Trichotomy HNNExtensionRep::wordProblem( const Word& w ) const
{
  VectorOf<Word> deco = reducedDecompositionOf( w );

  if( deco.length() > 1 )
    return no;

  return getBasisGroup().wordProblem( deco[0] );
}

void HNNExtensionRep::printOn( ostream& ostr ) const 
{
  Chars theNameOfStableLetter =  theNamesOfGenerators[ ord(stableLetter())-1 ];
  ostr << "< " << getBasisGroup() << ", " << theNameOfStableLetter;
  int sgens = getNumberOfGeneratorsInSubgroup(A);
  if( sgens > 0 ) {
    ostr << " ; ";
    for( int i = 0; i < sgens; ++i ) {
      if( i ) ostr << ", ";
      ostr << theNameOfStableLetter << "^-1 ";
      printWord( ostr, getGeneratorOfSubgroup(A, i) );
      ostr << ' ' << theNameOfStableLetter << " = ";
      printWord( ostr, getGeneratorOfSubgroup(B, i) );
    }
  }
  ostr << " >";
}


bool HNNExtensionRep::isSubgroupTrivial( const VectorOf<Word>& vec ) const
{
  int len = vec.length();
  for( int i = 1; i < len; ++i )
    if( wordProblem( vec[i] ) == no )
	return false;
  return true;
}

bool HNNExtensionRep::isSubgroupAbelian( const VectorOf<Word>& vec ) const
{
  int len = vec.length();
  for( int i = 1; i < len-1; ++i )
    for( int j = i+1; j < len; ++j )
      if( wordProblem( commutator( vec[i], vec[j] ) ) == no )
	return false;
  return true;
}


void HNNExtensionRep::write( ostream& ostr ) const
{
  FGGroupRep::write( ostr );
  ostr < theFPGroup; 
}

void HNNExtensionRep::read( istream& istr ) 
{
  FGGroupRep::read( istr );
  istr > theFPGroup; 
}

//---------------------- HNNExtOfFreeGroupRep:: --------------------//
//------------------------- Static members -------------------------//
 
const Type HNNExtensionRep::theHNNExtensionType =  Type( Type::unique() );
 




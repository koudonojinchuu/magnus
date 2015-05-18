// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implemenentation of class MagnusBreakdown.
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

#include "MagnusBreakdown.h"
#include "Range.h"
#include "CONDITION.h"
#include "AP-fixups.h"
#include "stdio.h"

#define DEBUG_BREAKDOWN

#ifdef DEBUG_BREAKDOWN
#  include "Margin.h"
#endif

//
//    Miscleanous stuff.
//

Word raiseToPower( const Word& w, int power )
{
  if( power == 0 )
    return Word();

  Word u = w;

  if( power < 0 ) {
    u = u.inverse();
    power = -power;
  }

  Word result;
  for(int i = 0; i < power; ++i )
    result = result * u;

  return result;
}

Chars intToChars( int i ) 
{
  char buffer[100];
  sprintf(buffer, "%d", i);
  return Chars(buffer);
}

void initVectorOfInt( VectorOf<int>& vec )
{
  for( int i = vec.length()-1; i >= 0; --i )
    vec[i] = 0;
}


//    MagnusBreakdown class.

MagnusBreakdown::MagnusBreakdown( const OneRelatorGroup& G )
    // stable data members:
  : theORGroup( G ),
    theGeneratorsWithZeroExpSum(),
    theGeneratorsWithNonzeroExpSum(),
    exponentsOfOldGenerators( G.numberOfGenerators() ),
    theNumberOfOldGenerators( G.numberOfGenerators() ),

    //  volatile data members:
    haveHNNPresentation( false ),
    theHNNPresentation( G, Chars("stableGen"), 
			SubgroupOfOneRelatorGroup(G, VectorOf<Word>()),
			SubgroupOfOneRelatorGroup(G, VectorOf<Word>())
			), // @dp dummy ctor for HNN.
    theLargerORGroup( G ),
    theEmbeddingOfORGroups( G ),
    stable(),
    accomp(),
    theNumberOfNewGenerators(0),
    theNamesOfNewGenerators(),
    stableNewGen(1), // @dp dummy ctor.
    toNewGens(0),
    toOldGens(0),
    mapToHNN( G , theLargerORGroup ), // @dummy ctor
    mapToORGroup( G, theLargerORGroup ) //@dummy ctor
{ 
  // @dp We suppose all generators of the group occur in the relator.

  // Set  the `dummy' inserting map of given one-relator group into bigger one.

  VectorOf<Word> idImages( theNumberOfOldGenerators );
  for( int i = 0; i < theNumberOfOldGenerators; ++i )
    idImages[i] = Generator(i+1);
  theEmbeddingOfORGroups.setGeneratingImages( idImages );

  // Sort generators into two sets: zero and non-zero exp sums.

  AbelianWord abelianRelator( G.numberOfGenerators(), G.relator());
  for( int i = 0; i < theNumberOfOldGenerators; ++i )
    exponentsOfOldGenerators[i] = abelianRelator[i].as_long(); 

  for( int i = 0; i < theNumberOfOldGenerators; ++i ) {
    int exponent = exponentsOfOldGenerators[i];
    if( exponent == 0 )
      theGeneratorsWithZeroExpSum.append( i+1 );
    else
      theGeneratorsWithNonzeroExpSum.append( i+1 );
  }
  
}

template<class T>
inline int sign( const T& a )
{
  if( a > 0 )
    return 1;
  if( a < 0 ) return -1;
  return 0;
}

Word MagnusBreakdown::rewriteWordInOldGenerators( const Word& w ) const
{
  if( !haveHNNPresentation )
    error("MagnusBreakdown::findHNNPresentation should be called "
	  "before rewriting of a word");

  Word u =  mapToORGroup.imageOf(w) .freelyReduce();

#ifdef DEBUG_BREAKDOWN 
  Margin margin;
  margin.set( globalMargin );
  
  cerr << margin << "MagnusBreakdown::rewriteInOldGenerators: " << endl
       << margin << "w = " << w << endl
       << margin << "u = " << u << endl;
#endif

  if( stable.exponent() == 0 ) 
    return u;

  int lastPos = -1;
  Word v;

  // stable: (x,a)
  // accomp: (y,b)
  // d = GCD(a,b)
  // embedding mapping: x     ->  x^(b/d),   y    ->  y x^(-a/d)
  //                    x^-1  ->  x^(-b/d),  y^-1 ->  x^(a/d) y^-1

  
  const int a = stable.exponent();
  const int b = accomp.exponent();
  const int d = GCD2( a, b );
  int stablePower = 0;
  
  while( u.length() > 0 ) {
    Generator gen = u[0];
    u = u.terminalSegment( u.length()-1 );

    if( stable.generator() == abs(ord(gen)) ) {
      stablePower += ( ord(gen) > 0 ? 1 : -1 );
      gen = ord(gen) * sign(b/d);
      if( ord(gen) > 0 && stablePower == b/d 
	  || ord(gen) < 0  && stablePower == -b/d ) {
	stablePower = 0;
      }
      else 
	continue;
    }
    else if( accomp.generator() == abs(ord(gen)) ) {
      if( ord(gen) < 0 ) {
	stablePower -= a/d;
	if( stablePower % (b/d) != 0 ) {
	  error("MagnusBreakdown::rewriteWordInOldGenerators(): "
		"The word cannot be rewritten in old generators.");
	}
	v = v * raiseToPower( stable.generator(), stablePower / (b/d) );
	stablePower = 0;
      }
      else {
	if( stablePower != 0 ) {
	  error("MagnusBreakdown::rewriteWordInOldGenerators(): "
		"The word cannot be rewritten in old generators.");
	}
	stablePower = a/d;
      }
    }
    else if( stablePower != 0 ) {
      error("MagnusBreakdown::rewriteWordInOldGenerators(): "
	    "The word cannot be rewritten in old generators.");
    }
    v = v * gen;
  }

  if( stablePower % abs(b/d) != 0 )
    error("MagnusBreakdown::rewriteWordInOldGenerators(): "
	  "The word cannot be rewritten in old generators.");
  
  while( sign(stablePower) == sign(b/d) ) {
    v = v * stable.generator();
    stablePower -= b/d;
  }
  while( sign(stablePower) == -sign(b/d) ) {
    v = v * inv(stable.generator());
    stablePower += b/d;
  }

  return v; // sometimes it needs to be freely reduced.
}

Word MagnusBreakdown::rewriteWordInNewGenerators( const Word& w ) const
{
  if( !haveHNNPresentation )
    error("MagnusBreakdown::findHNNPresentation should be called "
	  "before rewriting of a word");
  
  Word u = w;
  int uLen = u.length();

  Word result;

#ifdef DEBUG_BREAKDOWN
  Margin margin;
  margin.set( globalMargin );
#endif

  Generator stableGen = stable.generator();
  int sumPowerConj = 0;

  for( int i = 0; i < uLen; ++i ) {

    Generator gen = u[i];

#ifdef DEBUG_BREAKDOWN
    //    cerr << margin << "u[" << i << "] = " << gen << endl
    //	 << margin << "result = " << result << endl;
#endif

    int expGen = 1;
    if( ord(gen) < 0 ) {
      expGen = -1;
      gen = inv(gen);
    }

    if( gen == stableGen ) 
      sumPowerConj += expGen;
    else {
      Range subscripts = subscriptsOfGenerator[ ord(gen) ];
      if( subscripts.low > sumPowerConj ) {
	result = result * raiseToPower( stableNewGen, 
					sumPowerConj-subscripts.low );
	sumPowerConj = subscripts.low;
      }
      else if( sumPowerConj > subscripts.high ) {
	result = result * raiseToPower( stableNewGen, 
					sumPowerConj-subscripts.high );
	sumPowerConj = subscripts.high;
      }
      if( expGen > 0 )
	result = result * adjoinSubscript( gen, sumPowerConj );
      else
	result = result * inv( adjoinSubscript( gen, sumPowerConj ) );
    }
  }

  if( sumPowerConj != 0 )
    result = result * raiseToPower( stableNewGen, sumPowerConj ); 

#ifdef DEBUG_BREAKDOWN
  //  cerr << margin << "final result = " << result << endl;
#endif

  return result;
}

void MagnusBreakdown::makeHNNPresentation( ) 
{
  haveHNNPresentation = true;

  makeEmbedding();
  makeSubscriptsTable();
  makeTranslationTables();

  // make the basis one-relator group for HNN extension.

  VectorOf<Chars> newGensOfORGroup( theNamesOfNewGenerators.length()-1, 
				    theNamesOfNewGenerators );
  // all new generators except last one which is a stable letter.

  OneRelatorGroup G( newGensOfORGroup, rewriteWordInNewGenerators( 
                       theEmbeddingOfORGroups.imageOf(theORGroup.relator()) )
		     );

  // make amalgamated subgroups.

  // @dp Now approach is quite unefficient. We can form mappings in
  // time of building translations tables and new generators names.
  // But then that function will be heavy overloaded (too many targets
  // for just one function).

  const int A = 0;
  const int B = 1;
  VectorOf<Word> subgroups[2];
  const int subGensCount = theNumberOfNewGenerators-theNumberOfOldGenerators; 
  subgroups[A] = VectorOf<Word>( subGensCount );
  subgroups[B] = VectorOf<Word>( subGensCount );
  int eltOf[2] = { 0, 0 };

  int subscript;
  Generator oldGen;
  for( int newGen = 1; newGen <= theNumberOfNewGenerators; ++newGen ) {
    oldGen = extractSubscript( newGen, subscript );
    Range subscripts = subscriptsOfGenerator[ ord(oldGen) ];
    if( subscript != subscripts.low )
      subgroups[B][ eltOf[B]++ ] = Generator( newGen );
    if( subscript != subscripts.high )
      subgroups[A][ eltOf[A]++ ] = Generator(newGen);
  }

  SubgroupOfOneRelatorGroup subgroupA( G, subgroups[A] );
  SubgroupOfOneRelatorGroup subgroupB( G, subgroups[B] );

  theHNNPresentation = 
    HNNExtOfORGroup( G, theNamesOfNewGenerators[ord(stableNewGen)-1],
		     subgroupA, subgroupB
		     );
  haveHNNPresentation = true;

  // make maps between the one-relator group and the HNN-presentation 
  // of last one:

  const Generator stableGen = stable.generator();

  VectorOf<Word> oldImages( theNumberOfNewGenerators );
  for( int i = 0; i < theNumberOfNewGenerators; ++i ) {
    int pwr;
    const Generator oldGen = extractSubscript( Generator(i+1), pwr );
    oldImages[i] = raiseToPower( stableGen, pwr )
      * oldGen * raiseToPower( stableGen, -pwr );
  }
  
  FPGroup H =  theHNNPresentation.getPresentation();

  mapToORGroup = Map( H, theORGroup, oldImages );

  VectorOf<Word> newImages( theNumberOfOldGenerators );
  for( int i = 0; i < theNumberOfOldGenerators; ++i )
    newImages[i] = rewriteWordInNewGenerators( Generator(i+1) );

  mapToHNN = Map( theORGroup, H, newImages );
}


// `Preview of new relator'. 
// Scans for powers of stable generator can be moved to right of the 
// new relator.

void MagnusBreakdown::makeSubscriptsTable( )
{
  subscriptsOfGenerator = VectorOf<Range>( theNumberOfOldGenerators+1 );
  int sumOfPowerConj = 0;
  Word relator = theLargerORGroup.relator();
  for(int i = 0; i < relator.length(); ++i ) {
    Generator gen = relator[i];
    int genOrd = abs(ord( gen ));
    int exponent = ( ord(gen) > 0 ? 1 : -1 );
    
    if( genOrd == stable.generator() )
      sumOfPowerConj += exponent;
    else {
      if( subscriptsOfGenerator[genOrd].Default ) 
	subscriptsOfGenerator[genOrd] = Range(sumOfPowerConj, sumOfPowerConj);
      else if( sumOfPowerConj < subscriptsOfGenerator[genOrd].low )
	subscriptsOfGenerator[genOrd].low = sumOfPowerConj;
      else if( sumOfPowerConj > subscriptsOfGenerator[genOrd].high )
	subscriptsOfGenerator[genOrd].high = sumOfPowerConj;
    }
  }
}

Generator MagnusBreakdown::adjoinSubscript(const Generator& oldGen, 
					   int subscript) const
{
  int old = abs(ord(oldGen));
  Range subscripts = subscriptsOfGenerator[old];

  CONDITION( ERROR, 
	     subscripts.low <= subscript && subscript <= subscripts.high );
  
  int newGen = toNewGens[old] + subscript - subscripts.low;
  
  return ( ord(oldGen) > 0 ? newGen : -newGen );
}

Generator MagnusBreakdown::extractSubscript( const Generator& newGen, 
					     int& subscript) const
{
  int newOrd = abs(ord(newGen));
  Generator oldGen = toOldGens[newOrd];
  Range subscripts = subscriptsOfGenerator[ord(oldGen)];
  subscript = newOrd - toNewGens[ord(oldGen)] + subscripts.low;
  return ( ord(newGen) > 0 ? oldGen : inv(oldGen) );
}


// Translation between old and new coding tables of generators.

void MagnusBreakdown::makeTranslationTables() 
{
  int stableGen = ord(stable.generator());
  
  // Determines the number of new generators;

  theNumberOfNewGenerators = 0;
  for(int oldGen = 1; oldGen <= theNumberOfOldGenerators; ++oldGen ) {
    Range subscript = subscriptsOfGenerator[oldGen];
    theNumberOfNewGenerators += subscript.high - subscript.low + 1;
  }
  theNamesOfNewGenerators = VectorOf<Chars>( theNumberOfNewGenerators );
  stableNewGen = Generator(theNumberOfNewGenerators);
  

  toNewGens = VectorOf<int>( theNumberOfOldGenerators+1 );
  toOldGens = VectorOf<int>( theNumberOfNewGenerators+1 );


  initVectorOfInt(toNewGens);
  initVectorOfInt(toOldGens);

  //@dp WARNING: toNewGens[0] && toOldGens[0] are undefined!

  Chars ul = "_"; 
  VectorOf<Chars> theNamesOfOldGenerators( theORGroup.namesOfGenerators() );
  bool doNames = ( theNamesOfOldGenerators.length() == 0 ? false : true );
    

  int newGen = 1;
  for( int oldGen = 1; oldGen < stableGen; ++oldGen ) {
    toNewGens[oldGen] = newGen;
    Range subscripts = subscriptsOfGenerator[oldGen];
    for(int subscript=subscripts.low; subscript<=subscripts.high; ++subscript){
      toOldGens[ newGen ] = oldGen;
      if( doNames ) 
	theNamesOfNewGenerators[ newGen-1 ] = 
	  theNamesOfOldGenerators[oldGen-1] + ul + intToChars(subscript);
      ++newGen;
    }
  }

  toNewGens[ stableGen ] = ord(stableNewGen); 
  toOldGens[ theNumberOfNewGenerators ] = stableGen;
  theNamesOfNewGenerators[ ord(stableNewGen)-1 ] = Chars("t");

  for(int oldGen = stableGen+1; oldGen <= theNumberOfOldGenerators; ++oldGen) {
    toNewGens[oldGen] = newGen;
    Range subscripts = subscriptsOfGenerator[oldGen];
    for(int subscript=subscripts.low; subscript<=subscripts.high; ++subscript){
      toOldGens[ newGen ] = oldGen;
      if( doNames )
	theNamesOfNewGenerators[ newGen-1 ] = 
	  theNamesOfOldGenerators[oldGen-1] + ul + intToChars(subscript);
      ++newGen;
    }
  }
}


void MagnusBreakdown::makeEmbedding()
{

  VectorOf<Word> images(theNumberOfOldGenerators);
  
  for( int i = 0; i < theNumberOfOldGenerators; ++i )
    images[i] = Generator(i+1);

  if( stable.exponent() != 0 ) {
    int s = stable.exponent();
    int a = accomp.exponent();
    int gcd_sa = GCD2( abs(s), abs(a) );
    images[ ord(stable.generator())-1 ] = 
      raiseToPower( stable.generator(), a / gcd_sa );
    images[ ord(accomp.generator())-1 ] = 
      accomp.generator() * raiseToPower(stable.generator(), -s / gcd_sa );
  }
   
  FreeGroup F( theNumberOfOldGenerators );
  theEmbeddingOfORGroups = Automorphism( F, images );

  theLargerORGroup = OneRelatorGroup( theORGroup.namesOfGenerators(), 
		       theEmbeddingOfORGroups.imageOf( theORGroup.relator() )
		     );
}

void MagnusBreakdown::debugPrint( ostream& ostr ) const
{
  ostr << endl 
       << "*** Debug printing of MagnusBreakdown: ***" << endl
       << "theORGroup  = " << theORGroup << endl
       << "theNumberOfOldGenerators = " << theNumberOfOldGenerators << endl
       << "theGeneratorsWithZeroExpSum = " << theGeneratorsWithZeroExpSum 
       << endl
       << "theGeneratorsWithNonzeroExpSum = " << theGeneratorsWithNonzeroExpSum
       << endl
       << "exponentsOfOldGenerators = " << exponentsOfOldGenerators << endl
       << "theLargerORGroup = " << theLargerORGroup << endl
       << "theHNNPresentation = " << theHNNPresentation << endl
       << "haveHNNPresentation = " << haveHNNPresentation << endl
       << "theEmbeddingOfORGroups = " << theEmbeddingOfORGroups << endl
       << "stable = " << stable << endl
       << "accomp = " << accomp << endl
       << "theNumberOfNewGenerators = " << theNumberOfNewGenerators << endl
       << "stableNewGen = " << stableNewGen << endl
       << "subscriptsOfGenerator = " << subscriptsOfGenerator << endl
       << "toNewGens = " << toNewGens << endl
       << "toOldGens = " << toOldGens << endl
       << "*** end of debug printing of MagnusBreakdown. *** " << endl << endl;
    ;
}

bool MagnusBreakdown::findHNNPresentation( )
{
  if( !chooseStableGenerator() )
    return false;
  makeHNNPresentation();
  return true;
}


bool MagnusBreakdown::findHNNPresentation( const Generator& stableGen )
{
#if SAFETY > 0
  if( !theGeneratorsWithZeroExpSum.contains( stableGen ) ) {
    haveHNNPresentation = false;
    return false;
  }
#endif
  
  stable = SuperGen( stableGen, 0 );
  makeHNNPresentation();
  return true;
}


bool MagnusBreakdown::findHNNPresentation( const Generator& stableGen, 
					   const Generator& accompGen )
{
#if SAFETY > 0
  // @dp NB: The user can specify stable generator with non-zero exponent sum 
  //         even zero sum generators is presented.

  if( ! ( theGeneratorsWithNonzeroExpSum.contains(stableGen) 
	  && theGeneratorsWithNonzeroExpSum.contains(accompGen)
	  && abs(ord(stableGen)) != abs(ord(accompGen))
	) ) {
    return haveHNNPresentation = false;
  }
#endif
  
  stable = SuperGen( stableGen, 
		     exponentsOfOldGenerators[ abs(ord(stableGen))-1 ] );
  accomp = SuperGen( accompGen, 
		     exponentsOfOldGenerators[ abs(ord(accompGen))-1 ] );
  makeHNNPresentation();
  return true;
}

ListOf<Generator> MagnusBreakdown::getDefaultBreakdownGenerators() const 
{
  ListOf<Generator> gens;
  if( theGeneratorsWithZeroExpSum.length() > 0 )
    gens.append( theGeneratorsWithZeroExpSum[0] );
  else if( theGeneratorsWithNonzeroExpSum.length() > 1 ) {
    gens.append( theGeneratorsWithNonzeroExpSum[0] );
    gens.append( theGeneratorsWithNonzeroExpSum[1] );
  }
  return gens;
}

bool MagnusBreakdown::chooseStableGenerator()
{
  if( theGeneratorsWithZeroExpSum.length() > 0 ) {
    Generator g = theGeneratorsWithZeroExpSum[0];
    stable = SuperGen( g, exponentsOfOldGenerators[ abs(ord(g))-1 ]);
  }
  else if( theGeneratorsWithNonzeroExpSum.length() > 1 ) {
    // choose stable generator with minimal non-zero exponent sum.
    Generator g = theGeneratorsWithNonzeroExpSum[0];
    int exp_g =  exponentsOfOldGenerators[ abs(ord(g))-1 ];
    for( int i = 1; i < theGeneratorsWithNonzeroExpSum.length(); ++i ) {
      Generator newg = theGeneratorsWithNonzeroExpSum[i];
      int exp_newg = exponentsOfOldGenerators[ abs(ord(newg))-1 ];
      if( exp_newg < exp_g ) {
	g = newg;
	exp_g = exp_newg;
      }
    }
    stable = SuperGen( g, exp_g );
    
    // choose some accompanying generator.
    Generator ga = theGeneratorsWithNonzeroExpSum[0]; 
    if( ga == g )
      ga = theGeneratorsWithNonzeroExpSum[1];
    accomp = SuperGen( ga, exponentsOfOldGenerators[ abs(ord(ga))-1 ]);
  }
  else
    return false;

  return true;
}


ProductOfRelatorConjugates liftUpProduct( 
  const ProductOfRelatorConjugates& mProduct, 
  const MagnusBreakdown& M, 
  const OneRelatorGroup& G )
{
  ProductOfRelatorConjugates gProduct(0,true);

  const Word hnnRelator = M.getHNNPresentation().basisGroup().relator();
  const Word hnnRelatorInv = hnnRelator.inverse();
  const Word gRelator = G.relator();
  const Word gRelatorInv = G.relator().inverse();

  for( int i = mProduct.length()-1; i >= 0; --i ) {
    if( mProduct[i].relator == hnnRelator ) {
      const Word conj = M.rewriteWordInOldGenerators( mProduct[i].conjugator );
      gProduct.prepend( RelatorConjugate( gRelator,conj ) );
    }
    else if( mProduct[i].relator == hnnRelatorInv ) {
      const Word conj = M.rewriteWordInOldGenerators( mProduct[i].conjugator );
      gProduct.prepend( RelatorConjugate( gRelatorInv,conj ) );
    }
    else {
      // This conjugation is eliminated by lifting up to the group G.
      // from the given HNN-extenension.
      // Do nothing.
    }
  }

#ifdef DEBUG_BREAKDOWN
  Margin margin;
  margin.set( globalMargin );
  
  cerr << margin << "LIFT UP:" << endl
       << margin << "H = " << M.getHNNPresentation() << endl
       << margin << "G = " << G << endl
       << margin << "mProduct = " << mProduct << endl 
       << margin << "gProduct = " << gProduct << endl;
#endif

  return gProduct;
}


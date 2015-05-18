// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Usage:
//
// Revision History:
//

#include "OneRelatorGroupWithTorsion.h"
#include "MagnusBreakdown.h"
#include "ShortenByRelators2.h"
#include "HNNExtOfORGroup.h"
#include "conversions.h"

#define DEBUG_MAXIMAL_ROOT

#ifdef DEBUG_MAXIMAL_ROOT
# include "Margin.h"
#endif


//
// Helper class Shorten
//

class ShortenInSGofORGWithTorsion : public ShortenByRelators2 
{
public:
  
  ShortenInSGofORGWithTorsion( const SubgroupOfORGroupWithTorsion& s );
  
private:
  
  int compare( const Word& w1, const Word& w2 ) const;
  // Override virtual ShortenByRelators2::compare().
  
  int numberOfSpecialLetters( const Word& w ) const;
  // Count letters in w from the relator which aren't subgroup's letters.
  
  // data members:
  SubgroupOfORGroupWithTorsion theSubgroup;
};

ShortenInSGofORGWithTorsion::ShortenInSGofORGWithTorsion( 
  const SubgroupOfORGroupWithTorsion& s )
  : ShortenByRelators2( s.parentGroup().relator() ), theSubgroup( s ) 
{ 
}

int ShortenInSGofORGWithTorsion::compare( 
  const Word& w1, const Word& w2 ) const
{
  int result;

  int w1index = numberOfSpecialLetters( w1 );
  int w2index = numberOfSpecialLetters( w2 );
  if( w1index < w2index )
    result = -1;
  else if( w1index > w2index )
    result = 1;
  else
    result = 0;

  return result;
}

int ShortenInSGofORGWithTorsion::numberOfSpecialLetters( const Word& w ) const
{
  // make set of generators (letters) of the subgroup and their inverses.
  SetOf<Generator> gens;
  for( int i = 0; i < theSubgroup.generators().length(); ++i ) {
    Generator g = theSubgroup.generators()[i][0];
    gens |= g;
    gens |= inv(g);
  }

  // Count special letters in w.
  int count = 0;
  for( int i = w.length()-1; i >= 0; --i ) {
    if( !gens.contains( w[i] ) ) 
      ++count;
  }

  return count;
}


//
//  Helper class:: DoubleCoset
//

HNNDoubleCoset::HNNDoubleCoset( const SubgroupOfORGroupWithTorsion& Sa,
			  const SubgroupOfORGroupWithTorsion& Sb )
  : A(Sa), B(Sb)
{
}

// Solve an equation f a g = b, where f,g are given words,
// a belongs the subgroup A, b belongs the subgroup B.
Trichotomy HNNDoubleCoset::solve( 
  const Word& f, Word& a, const Word& g, Word& b ) const 
{
  return oneRelatorWithTorsionSolution(f,a,g,b);
}

Trichotomy HNNDoubleCoset::oneRelatorWithTorsionSolution( 
  const Word& f, Word& a, const Word& g, Word& b ) const 
{
#ifdef DEBUG_MAXIMAL_ROOT
  cerr << "Solving equation for double coset: f a g = b" << endl
       << "f = " << f << ", g = " << g << endl;
#endif

  Word s;
  int N;
  maximalRootInFreeGroup( A.parentGroup().relator(), s, N );
  // r = s^N

  if( N > 2 ) {
    const Word f1 = minimalSpecialForm(f,A);
    const Word g1 = minimalSpecialForm(g,A);

#ifdef DEBUG_MAXIMAL_ROOT
    cerr << "special minimal form:" << endl
	 << f << " => " << f1 << endl
	 << g << " => " << g1 << endl;
#endif
    
    SetOf<Word> segments;
    // make initial and terminal segments of s 
    // which does not contain letter in b(H)\b(A)
    
    SetOf<Word> aGens = makeSetOf( A.generators() );

    for( int i = 0; i < s.length(); ++i ) {
      if( aGens.contains( Generator(s[i]) ) 
	  || aGens.contains( inv(Generator(s[i])) ) ) {
	segments |= s.initialSegment( i+1 );
	segments |= s.initialSegment( i+1 ).inverse();
      }
      else {
	break;
      }
    }

    for( int i = s.length()-1; i >= 0; --i ) {
      if( aGens.contains( Generator(s[i]) ) 
	  || aGens.contains( inv(Generator(s[i])) ) ) {
	segments |= s.terminalSegment( s.length()-i );
	segments |= s.initialSegment( s.length()-i ).inverse();
      }
      else {
	break;
      }
    }

    segments |= Word();
    
    SetIterator<Word> I(segments);
    for( I.reset(); !I.done(); I.next() ) {
      a = I.value();
      b = f1 * a * g1;
      if( B.contains( b ) ) {
	return yes;
      }
    }
  }
  else {
    // The case of r=s^2 is not actually developed for now.
    // There are some hacks for this case only.
    if( f == g.inverse() ) { 
      a = b = Word();
      return yes;
    }
    // To be impelemented.
    warn("Unimplemented: maximal root problem for one-relator group "
	 "with torsion G=< X | r^N=1 >, where N = 2.");
    return dontknow;
  }
  return no;
}

Word HNNDoubleCoset::minimalSpecialForm( 
  const Word& w, const SubgroupOfORGroupWithTorsion S ) const
{
  ShortenInSGofORGWithTorsion I( S );
  Word result = I.getShortenWord( w );

#ifdef DEBUG_MAXIMAL_ROOT
  cerr << "HNNDoubleCoset::minimalSpecialForm(" << w << ") = " 
       << result << endl;
#endif

  return result;
}



Trichotomy OneRelatorGroupWithTorsionRep::maximalRoot( 
  const Word& w, Word& maxRoot, int&  maxPower ) const
{

  // Get a minimal form of the given element.
  ProductOfRelatorConjugates cyclicProduct;
  Word cConjugator;
  Word u = cyclicallyShortenByRelators( w, theRelator, cConjugator, 
					cyclicProduct );

  if( u.length() == 0 ) {
    maxRoot = Word();
    maxPower = 1;
    return no;
  }

  //@dp Here we can decompose the given group into a free product,
  //    but nothing does now.

  Word s;
  int N;
  maximalRootInFreeGroup( theRelator, s, N );
  if( s.length() == 1 ) {
    // Trivial case  G = < X | s^N = 1 > = < X | x^N=1 >,  
    // where x is a generator of G.
    maximalRootInFreeGroup( u, maxRoot, maxPower );
    maxRoot = maxRoot.conjugateBy( cConjugator );
    maxRoot = maxRoot.freelyReduce();
    return ( maxPower > 1 ? yes : no );
  }

  // Find a Magnus breakdown for the group and bring the calculation 
  // into the  HNN presentation.

#ifdef DEBUG_MAXIMAL_ROOT
  Margin margin;
  margin.set( globalMargin );
  
  cerr << margin << "*** MAXIMAL ROOT in ONE-RELATOR WITH TORSION" << endl
       << margin << "ORGroup G = ";
  printOn( cerr );
  cerr << ";" << endl;
#endif

  MagnusBreakdown M( OneRelatorGroup(theNamesOfGenerators, theRelator ));
  bool done = M.findHNNPresentation();
  if( !done )
    error("Internal error in OneRelatorGroupWithTorsionRep::maximalRoot().");

#ifdef DEBUG_MAXIMAL_ROOT
  cerr << margin << "HNN presentation = " << M.getHNNPresentation() << endl
       << margin << "?? theRelator = " << theRelator << endl
       << margin << "?? M.stableGen = " << M.stableGenerator() 
       << ", has accomp " << M.hasAccompGenerator() << endl
       << margin << "?? M.toHNNPresentation() = " << M.toHNNPresentation() 
       << endl
       << margin << "?? M.embeddingOfORGroups() = " << M.embeddingOfORGroups() 
       << endl
       << margin << "?? rewrite word : old " << u;
#endif

  Word rewrittenWord = 
    M.rewriteWordInNewGenerators( 
       M.embeddingOfORGroups().imageOf( u ).freelyReduce() 
    );

#ifdef DEBUG_MAXIMAL_ROOT
  cerr << ", new = " << rewrittenWord << endl;
#endif

  const HNNExtOfORGroupGeneric::NumberOfSubgroup A = HNNExtOfORGroupGeneric::A;
  const HNNExtOfORGroupGeneric::NumberOfSubgroup B = HNNExtOfORGroupGeneric::B;
  HNNExtOfORGroup H_0 = M.getHNNPresentation();
  OneRelatorGroupWithTorsion G( H_0.basisGroup().namesOfGenerators(),
				H_0.basisGroup().relator() 
				);
  SubgroupOfORGroupWithTorsion SA( G, H_0.subgroup(A).generators() );
  SubgroupOfORGroupWithTorsion SB( G, H_0.subgroup(B).generators() );
  HNNExtOfORGroupWithTorsion H( G, H_0.nameOfStableGenerator(), SA, SB );
  
  Trichotomy answer = H.maximalRoot( rewrittenWord, maxRoot, maxPower );

  if( answer == dontknow ) {
    maxRoot = Word();
    maxPower = 1;
    return dontknow;
  }

#ifdef DEBUG_MAXIMAL_ROOT
  cerr << margin << "H(1Rwt) = " << H << endl;
  cerr << margin << "maxRoot in HNN = " << maxRoot << endl;
#endif

  maxRoot = M.rewriteWordInOldGenerators(maxRoot).freelyReduce();

#ifdef DEBUG_MAXIMAL_ROOT
  cerr << margin << "maxRoot in G (lift up) = " << maxRoot << endl;
#endif

  maxRoot = maxRoot.conjugateBy( cConjugator/*.inverse()*/ );
  maxRoot = maxRoot.freelyReduce();

#ifdef DEBUG_MAXIMAL_ROOT
  cerr << margin << "cConjugator = " << cConjugator << endl
       << margin << "maxRoot in G (conj) = " << maxRoot << endl;
#endif

  return answer;
}


Trichotomy HNNExtOfORGroupWithTorsion::maximalRoot( 
  const Word& w, Word& maxRoot, int& maxPower ) const
{
  if( w.freelyReduce().length() == 0 ) {
    maxRoot  = Word();
    maxPower = 1;
    return no;
  }

  Word u = w;

  // Start root problem solving.
  MaximalRootProblem RootProblem( u );
  RootProblem.solve( *this ); 

  maxRoot = RootProblem.root();
  maxPower = RootProblem.power();

  return RootProblem.answer();
}

//////////////////////////////////////////////////////////////////
//                                                              //
//   Helper class:  MaximalRoot for HNNExtOfORGroupWithTorsion  //
//                                                              //
//////////////////////////////////////////////////////////////////

HNNExtOfORGroupWithTorsion::MaximalRootProblem::MaximalRootProblem( 
  const Word& w )
  : theWord( w ), theAnswer( dontknow ), isSolved( false )
{
}

void  HNNExtOfORGroupWithTorsion::MaximalRootProblem::setAnswer( 
  const Word& maxRoot, const int maxPower )
{
  theRoot = maxRoot.freelyReduce();
  thePower = maxPower;
  theAnswer =  (thePower > 1) ? yes : no ;
}

void HNNExtOfORGroupWithTorsion::MaximalRootProblem::adjustRoot( ) 
{
  if( thePower < 0 ) {
    theRoot = theRoot.inverse();
    thePower = -thePower;
  }
}

void HNNExtOfORGroupWithTorsion::MaximalRootProblem::solve( 
  const HNNExtOfORGroupWithTorsion& group )
{
  if( isSolved ) return;

  Word wConj;
  const VectorOf<Word> wDeco = 
    group.cyclicallyReducedDecompositionOf( theWord, wConj );
  Word wRed = compose(wDeco);

  Word root;
  int power;
  switch(  wDeco.length() ) { 
  case 0: 
  case 2:
    setAnswer( wRed, 1 ); 
    break;
  case 1: 
    {
      Trichotomy answer = ((OneRelatorGroupWithTorsion&)group.basisGroup())
	.maximalRoot( wRed, root, power );
      setAnswer( root, power );
      theAnswer = answer;
    }
    break;
  default:
    lengthN( group, wDeco );
  }

#ifdef DEBUG_MAXIMAL_ROOT
  Margin margin;
  margin.set( globalMargin );

  cerr << margin << "MaximalRootProblem::BEFORE ADJUST" << endl 
       << margin << "theRoot = " << theRoot << endl
       << margin << "thePower = " << thePower << endl
       << margin << "wConj = " << wConj << endl;
#endif

  theRoot = theRoot.conjugateBy( wConj );
  theRoot = theRoot.freelyReduce();
  adjustRoot();
  isSolved = true;

#ifdef DEBUG_MAXIMAL_ROOT
  cerr << margin << "MaximalRootProblem::AFTER ADJUST" << endl 
       << margin << "theRoot = " << theRoot << endl
       << margin << "thePower = " << thePower << endl
       << margin << "wConj = " << wConj << endl
       << margin << "theAnswer = " << theAnswer << endl;
#endif

}

bool HNNExtOfORGroupWithTorsion::MaximalRootProblem::lexCheckOfStableLetters( 
  const VectorOf<Word>& wDeco, const int rootLen ) const
{
  const int wDecoLen = wDeco.length();
  for( int i = rootLen + 1; i < wDecoLen; i += 2 ) {
    if( wDeco[i-rootLen] != wDeco[i] ) 
      return false;
  }
  return true;
}


void HNNExtOfORGroupWithTorsion::MaximalRootProblem::lengthN( 
  const HNNExtOfORGroupWithTorsion& H,
  const VectorOf<Word>& wDeco )
{
  const Word w = compose( wDeco );

  // H = < G, t | A^t = B >  
  //  const HNNExtOfORGroupWithTorsionRep& H = group.presentation();
  const OneRelatorGroupWithTorsion& G = 
    (const OneRelatorGroupWithTorsion&)H.basisGroup();
  Generator stableLetter = H.stableLetter();
  // Normal cyclically reduced form of w = s1 t^e1 s2 t^e2 s3 .. t^eN.

  const int wDecoLen = wDeco.length();
  const int halfLen = wDeco.length() / 2; 

  for( int rootLen = 2; rootLen <= halfLen; rootLen += 2 ) {

    if( wDecoLen % rootLen || !lexCheckOfStableLetters( wDeco, rootLen ) )
      continue;

    const int power = wDecoLen / rootLen;

    // We find a maximal root x of g (x^p = g) in the form:
    //  x = a1 r1 t^f1 r2 ... rM t^fM, where M = N/p, r(i) = s(N-M+i)
    //  f(i) = e(N-M+i), where s(i), e(i) are from the reduced form of w.
    // a1 is an element of an amalgamated subgroup A1 
    // ( A1 = sign(fM)==1? A : B ).
    // All r(i) and f(i) are known and only a1 should be determined.
    
    // To find a1 we raise x to the power p, inverse the x^p and  multiply it
    // from left by w. Carry out free reduction of the product we have 
    // the word:
    //
    //      s1 t^e1 ... s(N-M) t^e(N-M) a1^-1 t^(-fM) rM^-1 ... a1^-1.
    //
    // The word is an identity and reducing can be done on the glue of the 
    // product. Replace t^e(N-M) a1^-1 t^(-fM) by corresponding element b1 
    // of other amalgamated subgroup we have the word:
    //
    //      s1 t^e1 ... t^e(N-M-1) s(N-M) b1 rM^-1 t^( -f(M-1) ) ....
    //
    // Here we solve the equation
    //
    //      s(N-M) b1 rM^-1 = c1,
    //
    // where b1,c1 are unknown elements of respective amalgamated subgroups.

    // B1  =  (   sign(fM)   == 1 )  ?  B  :  A  ;
    // C1  =  ( sign(f(M-1)) == 1 )  ?  A  :  B  ;

    VectorOf<Word> x(rootLen);
    for( int i = 0; i < rootLen; ++i ) {
      x[i] = wDeco[ wDecoLen-rootLen+i ];
    }
    
    const NumberOfSubgroup A1 = ( ord(wDeco[wDecoLen-1][0]) > 0  ? A : B );
    const NumberOfSubgroup B1 = ( ord(wDeco[wDecoLen-1][0]) > 0  ? B : A );
    const NumberOfSubgroup C1 = ( ord(wDeco[wDecoLen-3][0]) > 0  ? A : B );

    SubgroupOfORGroupWithTorsion 
      SB( (OneRelatorGroupWithTorsion&)H.basisGroup(), 
	  H.subgroup(B1).generators()
	  );
    SubgroupOfORGroupWithTorsion 
      SC( (OneRelatorGroupWithTorsion&)H.basisGroup(), 
	  H.subgroup(C1).generators()
	  );

    HNNDoubleCoset D( SB, SC );

    const Word s = wDeco[wDecoLen-rootLen-2];
    const Word r = wDeco[wDecoLen-2].inverse();
    Word b1, c1; // words expressed in generators of amalgamated subgroups.
    Trichotomy answer = D.solve( s, b1, r, c1 );
    if( answer == dontknow ) {
      setAnswer( w, 1 );
      theAnswer = dontknow;
      return;
    }
    else if( answer == yes ) {
      // There is a solution of the equation. The solution is unique
      // because B1 and A1 are malnormal subgroups ( B.B.Newmann, ????? )

      // Restore a1 from b1. 
      
      const Word a1 = SB.rewriteFromGroupToSubgroupGenerators(b1)
	.replaceGenerators( H.subgroup(A1).generators() );
      x[0] = a1 * x[0];
      const Word root = compose(x);
      if( G.areEqual( w, G.raiseToPower(root,power) ) == yes ) {
	// We'd checked the pretender by raising to maxPower.
	// The maximal root is FOUND.
	setAnswer( root, power );
	return;
      }
    }
  }
  setAnswer( w, 1 ); // the word w is prime in H.
}

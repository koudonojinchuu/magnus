// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implemenatation of class OneRelatorGroupWithTorsionRep.
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
// TO DO:
//



#include "OneRelatorGroupWithTorsion.h"
#include "SubgroupOfOneRelatorGroup.h"
#include "PresentationParser.h"
#include "ShortenByRelators2.h"
#include "MagnusBreakdown.h"
#include "HNNExtOfORGroup.h"
#include "conversions.h"
#include "FreeGroup.h"
#include "AP-fixups.h"
#include "CONDITION.h"
#include "Margin.h"

#define DEBUG_ORWTORSION
#define DEBUG_CONJUGACY_PROBLEM


//@db 2.91 this function is defined at the end of this file 
//    (template instantiation)

//@njz: removed
//int SetData<QuickAssociation<Generator, int> >::hashElement(
//  QuickAssociation<Generator, int> const & t) const;
#if GCC_VERSION < 29600
int SetData<QuickAssociation<Generator, int> >::hashElement(
  QuickAssociation<Generator, int> const & t) const;
#endif
//


OneRelatorGroupWithTorsionRep::OneRelatorGroupWithTorsionRep(
 const VectorOf<Chars>& gennames, const Word& relator )
  : OneRelatorGroupRep( gennames, relator )
{
}

OneRelatorGroupWithTorsionRep::OneRelatorGroupWithTorsionRep(
  int ngens, const Word& relator )
  : OneRelatorGroupRep( ngens, relator )
{
}

OneRelatorGroupWithTorsionRep::OneRelatorGroupWithTorsionRep( 
  const FPGroup& G )
  : OneRelatorGroupRep( G )
{
}

Trichotomy OneRelatorGroupWithTorsionRep::isFreeProduct( ) const  
{
  return dontknow;
}

Trichotomy OneRelatorGroupWithTorsionRep::wordProblem( const Word& w ) const
{
  ProductOfRelatorConjugates dummy;
  return wordProblem( w, dummy );
}

Trichotomy OneRelatorGroupWithTorsionRep::wordProblem( const Word& w,
  ProductOfRelatorConjugates& productOfRelatorConjugates ) const
{
  ShortenByRelators2 S( theRelator );

  Word elt = 
    S.expressWordInConjugatesOfRelators( w, productOfRelatorConjugates );

  if( elt.length() == 0 )
    return yes;

  productOfRelatorConjugates = ProductOfRelatorConjugates();
  return no;
}

Trichotomy OneRelatorGroupWithTorsionRep::conjugacyProblem( 
  const Word& u, const Word& v, Word&  conjugator ) const
{
  // Get minimal forms of the given elements.
  ProductOfRelatorConjugates uCyclicProduct;
  Word uConjugator;
  Word u1 = 
    cyclicallyShortenByRelators( u, theRelator, uConjugator, uCyclicProduct );

  ProductOfRelatorConjugates vCyclicProduct;
  Word vConjugator;
  Word v1 = 
    cyclicallyShortenByRelators( v, theRelator, vConjugator, vCyclicProduct );

#ifdef DEBUG_CONJUGACY_PROBLEM
  Margin margin;
  margin.set( globalMargin );
  
  cerr << margin << "u = " << u << ", u1 = " << u1 
       << ", uConj = " << uConjugator << endl
       << margin << "v = " << v << ", v1 = " << v1
       << ", vConj = " << vConjugator << endl;
#endif

  //@dp Here we can decompose the given group into a free product,
  //    but nothing does now.

  Word s;
  int N;
  maximalRootInFreeGroup( theRelator, s, N );
  if( s.length() == 1 ) {
    // Trivial case  G = < X | s^N = 1 > = < X | x^N=1 >, 
    // where x is a generator of G.
    // To solve the conjugacy problem like the one in a free group.
    FreeGroup F( theNumberOfGenerators );
    Word freeConj;
    bool answer = F.conjugacyProblem( u1, v1, freeConj);
    if( answer ) {
      // compose the entire conjugator.
      conjugator = 
	(uConjugator.inverse() * freeConj * vConjugator).freelyReduce();
    }
    else {
      conjugator = Word();
    }
    return answer;
  }

  // Find a Magnus breakdown for the group and bring the calculation 
  // into the  HNN presentation.

#ifdef DEBUG_CONJUGACY_PROBLEM
  cerr << margin << "*** CONJUGACY PROBLEM in ONE-RELATOR WITH TORSION" << endl
       << margin << "ORGroup G = ";
  printOn( cerr );
  cerr << ";" << endl;
#endif

  MagnusBreakdown M( OneRelatorGroup(theNamesOfGenerators, theRelator ));
  bool done = M.findHNNPresentation();
  if( !done )
    error("Internal error in OneRelatorGroupWithTorsionRep::"
	  "conjugacyProblem().");

#ifdef DEBUG_CONJUGACY_PROBLEM
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

  Word uh = M.rewriteWordInNewGenerators(
	       M.embeddingOfORGroups().imageOf( u1 ).freelyReduce() 
	    );

  Word vh = M.rewriteWordInNewGenerators( 
	       M.embeddingOfORGroups().imageOf( v1 ).freelyReduce() 
	    );

#ifdef DEBUG_CONJUGACY_PROBLEM
  cerr << ", uh = " << uh << endl << ", vh = " << vh << endl;
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
  
  Word conjh;
  Trichotomy answer = H.conjugacyProblem( uh, vh, conjh );

#ifdef DEBUG_CONJUGACY_PROBLEM
  cerr << margin << "H(1Rwt) = " << H << endl;
  cerr << margin << "conjugator in HNN = " << conjh << endl;
#endif

  conjh = M.rewriteWordInOldGenerators(conjh).freelyReduce();

#ifdef DEBUG_CONJUGACY_PROBLEM
  cerr << margin << "conjugator in G (lift up) = " << conjh << endl;
#endif

  conjugator = uConjugator.inverse() * conjh * vConjugator; 

#ifdef DEBUG_CONJUGACY_PROBLEM
  cerr << margin << "answer = " << (bool)answer << endl
       << margin << "full conjugator = " << conjugator << endl;
#endif

  conjugator = conjugator.freelyReduce();
  return answer;
}

int OneRelatorGroupWithTorsionRep::powerOfElt( 
  const Word& u, Word& st, Word& x,
  ProductOfRelatorConjugates& productOfRelatorConjugates ) const
{
  FreeGroup F( theNumberOfGenerators );
  Word s;
  int k;

  // G = < F | r = s^k = 1 >
  maximalRootInFreeGroup(theRelator, s, k );

  if( wordProblem( u, productOfRelatorConjugates ) == yes ) {
    st = x = Word();
    return 1;
  }

  Word cConj;
  Word w = cyclicallyShortenByRelators( u, theRelator, cConj, 
					productOfRelatorConjugates );

#ifdef DEBUG_ORWTORSION
  cerr << "w = " << w << ", cConj = " << cConj << endl;
#endif

  Word wk = F.raiseToPower( w, k );

  CONDITION( WARNING, wk.length() == wk.freelyReduce().length() );

  if( wordProblem( wk ) == yes ) {
    // The element is of finite order. Find the order.

    {
      // quick check.
      CONDITION( WARNING, w.length() % s.length() == 0 );
      int t = w.length() / s.length();
      st = F.raiseToPower( s, t );
      CONDITION( WARNING, F.conjugacyProblem( w, st, x ) == yes );
      F.conjugacyProblem( w, st, x );
      x = x.inverse() * cConj.inverse();
      return k / GCD2(k,t);
      // THAT'S ALL. GOOD ENOUGH ???
    }      

    Word wi = (w * w).freelyReduce();
    for( int i = 2; i < k/2; ++i ) {

      
      if( k % i == 0 && yes == wordProblem( wi ) ) {
	return i;
      }
      wi = (w * wi).freelyReduce();
    }
    return k;
  }
  if( k < 2 ) {
    error("OneRelatorGroupWithTorsionRep::powerOfElt(): cannot compute order "
	  "of element for the given one-relator group without torsion"
	  );
  }

  // The element has infinite order.
  st = w;
  x = cConj.inverse();
  return 0; 
}

VectorOf<Word> OneRelatorGroupWithTorsionRep::centralizerOfElt( const Word& w )
  const 
{
  Word root;
  int power;
  bool dummy = maximalRoot( w, root, power );
  VectorOf<Word> result(1);
  result[0] = root;
  return result;
}


bool OneRelatorGroupWithTorsionRep::doesMagnusSubgroupContainElt( 
  const VectorOf<Generator>& subgroup, 
  const Word& w, Word& wInSubgroupBasis ) const
{
#if SAFETY > 0
  if( !isProperMagnusSubgroup( subgroup ) )
    error("Called OneRelatorGroupWithTorsionRep::"
	  "doesMagnusSubgroupContainElt() with non-Magnus subgroup.");
#endif

  ShortenByRelators2 S( theRelator );

  Word u = S.getShortenWord( w );

  // check lexically containment of shortened word to the Magnus subgroup
  // (B.B.Newman ...

  QuickAssociationsOf<Generator,int> gens;
  for( int i = 0; i < subgroup.length(); ++i )
    gens.bind( subgroup[i], i+1 );

  wInSubgroupBasis = u;

  for( int i = u.length()-1; i >= 0; --i ) {
    int g = ord( u[i] );
    if( gens.bound( g ) ) {
      wInSubgroupBasis[ i ] = gens.valueOf(g);
    }
    else if( gens.bound( -g ) ) {
      wInSubgroupBasis[ i ] =  -gens.valueOf(g);
    }
    else
      return false;
  }

  return true;
}

bool OneRelatorGroupWithTorsionRep::isPowerOfEltInMagnusSubgroup(
  const VectorOf<Generator>& subgroup, const Word& w, 
  Word& subgroupElt, int& powerOfElt ) const
{
#if SAFETY > 0
  //if( !isProperMagnusSubgroup( subgroup ) )
  //    error("Called OneRelatorGroupWithTorsionRep::"
  //          "doesMagnusSubgroupContainElt() with non-Magnus subgroup.");
#endif
  
}


								  
bool  OneRelatorGroupWithTorsionRep::isProperMagnusSubgroup(
   const VectorOf<Generator>& subgroup ) const
{
  SetOf<Generator> relGens;
  for( int i = 0; i < theRelator.length(); ++i ) 
    relGens |= Generator(abs(ord(theRelator[i])));

  for( int i = subgroup.length()-1; i >= 0; --i ) {
    const Word gen = subgroup[i];

    if( gen.length() == 0 )
      continue;

    if( gen.length() > 1 )
      return false;
    
    relGens -= Generator(abs(ord(gen[0])));
  }

  if( relGens.cardinality() > 0 )
    return true;

  return false;
  
  return false;
}


GroupRep* OneRelatorGroupWithTorsionRep::readFrom( 
  istream& istr, Chars& errMesg ) const
{
  PresentationParser P( istr );
  FPGroupRep *rep = P.parseFPGroup( errMesg );
  if( errMesg.length() > 0 )
    return 0;

  if( rep->relators.cardinality() != 1 ) {
    delete rep;
    errMesg = "{0} { One relator expected only.} ";
    return 0;
  }

  Word relator = SetIterator<Word>( rep->relators ).value();
  VectorOf<Chars> names = rep->theNamesOfGenerators;
  delete rep;

  return new OneRelatorGroupWithTorsionRep( names, relator );
}


template <> int SetData<QuickAssociation<Generator, int> >::hashElement(
  QuickAssociation<Generator, int> const & t) const
{
  return t.hash();
}


// --------------------- OneRelatorGroupWithTorsionRep -------------------- //
// ---------------------------- Static members ---------------------------- //
 
const Type OneRelatorGroupWithTorsionRep::theOneRelatorGroupWithTorsionType = 
  Type( Type::unique() );


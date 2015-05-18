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


#include "HNNExtOfFreeGroup.h"
#include "HNNParser.h"
#include "CONDITION.h"
#include "APofFreeGroups.h"

#define EMPTY_BODY


// This is a special function to adjust maximal root.
// Changes root to min(root, root^-1), adjusting power.

static void adjustRoot(Word& root, int& power) {
  Word rootInv = root.inverse();
  if( rootInv < root ) {
    root = rootInv;
    power = -power;
  }
}

  
static FPGroup makeHNNPresentation( const FreeGroup& F,
				    const Chars& nameOfStableLetter,
				    const SGofFreeGroup& subgroupA,
				    const SGofFreeGroup& subgroupB )
{
  VectorOf<Chars> names( F.numberOfGenerators() + 1, F.namesOfGenerators() );
  names[ F.numberOfGenerators() ] = nameOfStableLetter;

  Generator stableLetter = F.numberOfGenerators() + 1;
  Generator stableLetterInv = inv( stableLetter );
  
  SetOf<Word> relators;
  for( int i = subgroupA.generators().length() - 1; i >= 0; --i )
    relators |= stableLetterInv * subgroupA.generators()[i] 
      * stableLetter * subgroupB.generators()[i].inverse();

  return FPGroup( names, relators );
}


HNNExtOfFreeGroupRep::HNNExtOfFreeGroupRep( const FreeGroup& F, 
					    const Chars& nameOfStableLetter,
					    const SGofFreeGroup& subgroupA,
					    const SGofFreeGroup& subgroupB )
  : HNNExtensionRep( makeHNNPresentation(F,nameOfStableLetter,subgroupA,subgroupB) ),
    theBasisFreeGroup( F ),
    theSubgroups( 2 ),
    mapNielsenGensToSubgroupGens( 2 )
{
  PRECONDITION( WARNING, 
		subgroupA.generators().length() == subgroupB.generators().length() );

  theSubgroups[A] = subgroupA;
  theSubgroups[B] = subgroupB;

  // make maps between given and nielsen basises of the subgroups;

  makeSubgroupsMappings();
}

void HNNExtOfFreeGroupRep::makeSubgroupsMappings() 
{
  SGofFreeGroup SA = theSubgroups[A];
  SGofFreeGroup SB = theSubgroups[B];

  int numOfSubgrpGens = SA.generators().length();
  VectorOf<Word> images( numOfSubgrpGens );

  VectorOf<Word> agens = SA.generators();
  for( int i = 0; i < numOfSubgrpGens; ++i ) 
    images[i] = SA.inNielsenWords( agens[i] );
  
  mapNielsenGensToSubgroupGens[A] = images;

  VectorOf<Word> bgens = SB.generators();
  for( int i = 0; i < numOfSubgrpGens; ++i ) 
    images[i] =  SB.inNielsenWords( bgens[i] );
  
  mapNielsenGensToSubgroupGens[B] = images;
}

Word HNNExtOfFreeGroupRep::mappingFromSubgroup( const NumberOfSubgroup S, 
						const Word& w ) const
{
  PRECONDITION( WARNING, ((SGofFreeGroup)theSubgroups[S]).contains( w ) );

  Word wInNielsenBasis = ((SGofFreeGroup)theSubgroups[S]).inNielsenWords( w );
  Word wInSubgroupGens =
    wInNielsenBasis.replaceGenerators( mapNielsenGensToSubgroupGens[S] ).freelyReduce();
  Word wInOtherSubgroup = 
    wInSubgroupGens.replaceGenerators( theSubgroups[1-S].generators() );

  return wInOtherSubgroup.freelyReduce();
}

Trichotomy HNNExtOfFreeGroupRep::isFree( ) const 
{
  // dummy stub.
  // return dontknow;

  // Let E be the given HNN-extension:
  // E = < F,t | U^t = V >, F = <f_1,..,f_r>, U=<u_1,..,u_m>, V=<v_1,..,v_m>
  // 
  // Now we make corresponding amalgamated product G of free groups.
  // G = < A*B | H=K >
  // A = < f_1,..,f_r, c >, H = <h_1,..,h_m>, h_i = c^-1 u_i f_i,
  // B = < g_1,..,g_r, d >, K = <k_1,..,k_m>, k_i are obtained from v_i by 
  // replacing each f_j that appears in v_i by equally indexed g_i and then
  // conjugating by d.
  //
  // Then E is free iff G is free.

  int numOfFreeGens = theBasisFreeGroup.numberOfGenerators();
  int numOfSubgrpGens = theSubgroups[0].generators().length();

  FreeGroup A( numOfFreeGens+1 );
  FreeGroup B( numOfFreeGens+1 );
  VectorOf<Word> H = theSubgroups[0].generators();
  VectorOf<Word> K = theSubgroups[1].generators();
  
  Generator cInv = inv( Generator( numOfFreeGens+1 ) );
  Generator d = Generator( numOfFreeGens+1 );
  for( int i = 0; i < numOfSubgrpGens; ++i ) {
    H[i] = cInv * H[i] * Generator(i+1);
    K[i] = K[i].conjugateBy( Word(d) );
  }

  AmalgProductOfFreeGroups G( A, B, H, K );

  return G.isFree();
}


static bool isProperPowerOfSecondInFreeGroup( const Word& w, 
					      const Word& u,
					      int&  power )
{
  Word t = u;
  Word wInv = w.inverse();
  power = 1;
  while( t.length() <= w.length() ) {
    if( (wInv * t).freelyReduce().length() == 0 )
      return true;
    if( (w * t).freelyReduce().length() == 0 ) {
      power = -power;
      return true;
    }
    t = (t * u).freelyReduce();
    ++power;
  }
  power = 0;
  return false;
}

// Returns `true' iff it exists a number k>1 such that w^k = u,
// `false' otherwise.
bool HNNExtOfFreeGroupRep::isProperPowerOfSecond( const Word& w, 
						  const Word& u,
						  int& power ) const
{
  power = 0;

  Word uConj;
  VectorOf<Word> uDeco = cyclicallyReducedDecompositionOf(u, uConj);
  
  Word w2 = w.conjugateBy( uConj.inverse() );
  VectorOf<Word> wDeco = normalDecompositionOf( w2 );

  int uLen = uDeco.length();
  int wLen = wDeco.length();
  
  if( uLen == 1 || wLen == 1 ) {
    // Both words must have the same length (0 in HNN-ext.)
    if( uLen != wLen )
      return false;
    return isProperPowerOfSecondInFreeGroup( wDeco[0], uDeco[0], power );
  }

  if( wDeco[ wLen-1 ].length() == 0 ) {
    wDeco.shrink( wLen-1 );
    --wLen;
  }

  // it's follow from cyclically reduced form of uDeco that wDeco must have
  // cyclically reduced form.
  if( wLen % 2 ) 
    return false;

  if( wLen % uLen )
    return false;

  power = wLen / uLen;

  if( areEqual( w, raiseToPower(u, power)) )
      return true;

  if( areEqual( w, raiseToPower(u, -power) ) ) {
    power = -power;
    return true;
  }

  power = 0;
  return false;

}

Trichotomy HNNExtOfFreeGroupRep::isProperPower( const Word& w ) const
{
  Word dummyRoot;
  int dummyPower;
  return maximalRoot( w, dummyRoot, dummyPower );  
}


void HNNExtOfFreeGroupRep::makeReducedCyclicPresentation( ) const
{
  Word aRelator = getGeneratorOfSubgroup( A, 0 );
  Word bRelator = getGeneratorOfSubgroup( B, 0 );
  
  Word aConj;
  aRelator = cyclicallyReduce( aRelator, aConj );
  Word aConjInv = aConj.inverse();

  int ordOfStableLetter = ord( stableLetter() ); 

  VectorOf<Word> image( ordOfStableLetter );
  for( int i = 0; i < ordOfStableLetter-1; ++i )
    image[i] = Word( Generator(i+1) ); 

  Word bConj;
  bRelator = cyclicallyReduce( bRelator, bConj );
  

  // Calculate roots of amalgamated subgroups' generators.
  Word aRoot,  bRoot;
  int  aPower, bPower;

  maximalRootInFreeGroup( aRelator, aRoot, aPower );
  adjustRoot( aRoot, aPower );

  maximalRootInFreeGroup( bRelator, bRoot, bPower );
  adjustRoot( bRoot, bPower );

  bool areAmalgSubgroupsConjugateSeparated = true;

  Word sConj;
  if( theBasisFreeGroup.conjugacyProblem( aRoot, bRoot, sConj ) ) {
    areAmalgSubgroupsConjugateSeparated = false;
    bRoot = aRoot;
    bRelator =  raiseToPower( aRoot, bPower );
  }
  else if( theBasisFreeGroup.conjugacyProblem( aRoot, bRoot.inverse(), sConj ) ) {
    areAmalgSubgroupsConjugateSeparated = false;
    bRoot  = aRoot;
    bPower = -bPower;
    bRelator = raiseToPower( aRoot, bPower );
  }

  VectorOf<Word> rootsOfSubgroupGens(2);
  rootsOfSubgroupGens[A] = aRoot;
  rootsOfSubgroupGens[B] = bRoot;

  VectorOf<int>  powersOfSubgroupGens(2);
  powersOfSubgroupGens[A] = aPower;
  powersOfSubgroupGens[B] = bPower;

  image[ ordOfStableLetter - 1 ] = aConjInv * stableLetter() * sConj * bConj;

  FreeGroup F( ordOfStableLetter );
  Automorphism M(F, image);

  // Old presentation is < x1,.., x_n, t | t^-1 a(x_i) t = b(x_i) >.
  // Now we have presentation is < x1,.., x_n, s | s^-1 a1(x_i) s = b1(x_i) >, 
  // where a1(x_i), b1(x_i) are cyclicallry reduced words.

  // make reduced presentation (rep-level onject).
  VectorOf<Word> aVec(1), bVec(1);
  aVec[0] = aRelator;
  bVec[0] = bRelator;

  HNNExtOfFreeGroupRep* This = (HNNExtOfFreeGroupRep*)this;
  // break physical constness under maintenence of logical constness

  This->reducedPresentation = SpecialHNNExtOfFreeGroup(
    M.inverse(),
    theBasisFreeGroup, 
    theNamesOfGenerators[ ord(stableLetter())-1 ],
    SGofFreeGroup( theBasisFreeGroup, aVec ),
    SGofFreeGroup( theBasisFreeGroup, bVec ),
    areAmalgSubgroupsConjugateSeparated,
    rootsOfSubgroupGens,
    powersOfSubgroupGens
    );
}

Trichotomy HNNExtOfFreeGroupRep::maximalRoot( const Word& w, 
					      Word& maxRoot, 
					      int& maxPower ) const
{
  if( w.freelyReduce().length() == 0 ) {
    maxRoot  = Word();
    maxPower = 1;
    return no;
  }

  int numSubgrpGens = getNumberOfGeneratorsInSubgroup( A );

  if( numSubgrpGens > 1 )
    return dontknow;

  if( numSubgrpGens == 0 ) {
    maximalRootInFreeGroup( w, maxRoot, maxPower );
    if( maxPower > 1 )
      return yes;
    else
      return no;
  }

  if( !reducedPresentation.doneInit() )
    makeReducedCyclicPresentation();

  // Translate w into the reduced presentation.
  Word u = reducedPresentation.toOriginalPresentation.inverse().imageOf(w).freelyReduce();

  // Start root problem solving.
  MaximalRootProblem RootProblem( u );
  RootProblem.solve( reducedPresentation );

  //  if( RootProblem.answer() == yes ) {
  maxRoot = RootProblem.root();
  maxPower = RootProblem.power();

  return RootProblem.answer();
}


GroupRep* HNNExtOfFreeGroupRep::readFrom( istream& istr, Chars& errMesg ) const
{
  HNNExtensionParser P(istr);
  return P.parseHNNExtensionOfFreeGroup( errMesg );
}

Trichotomy HNNExtOfFreeGroupRep::conjugacyProblem( const Word& x,
						   const Word& y,
						   Word& conjugator ) const
{
  FreeGroup F( theNumberOfGenerators );

  if(  getNumberOfGeneratorsInSubgroup(A) == 0 ) 
    return F.conjugacyProblem( x, y, conjugator );

  Word dummy;
  if( cyclicallyReducedDecompositionOf(x, dummy).length() != cyclicallyReducedDecompositionOf(y, dummy).length() ) 
    return no;

  if( getNumberOfGeneratorsInSubgroup(A) > 1 ) {
    
    // This algorithm can determine whether the elements are conjugated
    // only for cyclic subgroups case only.
    
    if( lengthOf( x ) == 1 && F.conjugacyProblem( x, y, conjugator ) == yes )
      return yes;
    
    return dontknow;
  }

  // main algorithm works only with cyclic subgroups.

  // Make reduced presentation.
  if( !reducedPresentation.doneInit() ) 
    makeReducedCyclicPresentation();

  //Automorphism M( FreeGroup(1) ); // dummy ctor.
  //HNNExtOfFreeGroupRep H = makeReducedCyclicPresentation( M );

  Automorphism M = reducedPresentation.toOriginalPresentation.inverse();

  // Translate x,y into the reduced presentation.
  Word w = M.imageOf(x).freelyReduce();
  Word u = M.imageOf(y).freelyReduce();
  
  const HNNExtOfFreeGroupRep& H = reducedPresentation.presentation();
  Trichotomy answer = H.conjugacyProblem_reduced( w, u, conjugator );

  if( answer == yes )
    // translate the found conjugator into original group presentation.
    conjugator = M.inverse().imageOf( conjugator ).freelyReduce(); 
  else
    conjugator = Word();

  return answer;
}

Trichotomy HNNExtOfFreeGroupRep::conjugacyProblem_reduced( const Word& u, 
  const Word& v, Word& conjugator ) const
{
  // Find cyclic decomposition of u and v.

  conjugator = Word();

  Word uConj, vConj;
  VectorOf<Word> uDec = cyclicallyReducedDecompositionOf( u, uConj );
  VectorOf<Word> vDec = cyclicallyReducedDecompositionOf( v, vConj );
  
  Word uRed = compose(uDec);
  Word vRed = compose(vDec);

  Trichotomy answer = dontknow;

  // The two non-trivial cases depending on words length.
  if(uDec.length() == 0)
    answer = yes;
  else if(uDec.length() == 1) 
    answer = conjugacyProblem_case1(uRed, vRed, conjugator);
  else 
    answer = conjugacyProblem_case2(uDec, vDec, conjugator);

  if( answer == yes)
    conjugator =  (uConj.inverse() * conjugator * vConj).freelyReduce();

  return answer;
}

// It determines whether the word `u' is conjugated (in HNN-extention) 
// with an element of the given amalgamated subgroup `S' and last one 
// is conjugated (in free group) with the word `v'.

Trichotomy  HNNExtOfFreeGroupRep::conjugateInSubgroups( NumberOfSubgroup S,
  const Word& u, const Word& v, Word& conjugator, bool oneIteration ) const
{
  Trichotomy answer = no;

  Word u_conj;
  Word u_elt = u;
  Word t = ( S == A ? stableLetter() : inv(stableLetter()) );

  while( theSubgroups[S].conjugateInSubgroup(u_elt, u_conj) ) {

    Word a_elt = u_elt.conjugateBy(u_conj);
    Word b_elt = mappingFromSubgroup( S, a_elt );
    conjugator = conjugator * t;
    Word b_conj;

    if( ::conjugacyProblem( theBasisFreeGroup, b_elt, v, b_conj ) ) {
      conjugator = conjugator * b_conj;
      return yes;
    }
    else
      u_elt = b_elt;

    if( oneIteration )
      break;
  }

  return no;
}

// Conjugacy problem for special case: amalgamated words are cyclically
// reduced, and every of u, v belongs to the basis group.

Trichotomy HNNExtOfFreeGroupRep::conjugacyProblem_case1( const Word& u,
  const Word& v, Word& conjugator ) const
{

  if( theBasisFreeGroup.conjugacyProblem( u, v, conjugator ) == yes )
    return yes;

  bool recursiveSubgroupSearch = false;

  Word a = getGeneratorOfSubgroup(A,0);
  Word b = getGeneratorOfSubgroup(B,0);

  int  aPower, bPower;
  Word aRoot, bRoot;
  maximalRootInFreeGroup( a, aRoot, aPower );
  adjustRoot( aRoot, aPower );
  maximalRootInFreeGroup( b, bRoot, bPower );
  adjustRoot( bRoot, bPower );

  Word dummy;
  bool assocSubgroupsAreConjugateSeparated = 
    ( theBasisFreeGroup.conjugacyProblem(aRoot,bRoot,dummy) == yes ? false : true );

  Trichotomy answer = no;

  if( assocSubgroupsAreConjugateSeparated ) {

    // case 1.2.1 
    answer = conjugateInSubgroups( A, u, v, conjugator, !recursiveSubgroupSearch );
    if( answer != yes )
      answer = conjugateInSubgroups( B, u, v, conjugator, !recursiveSubgroupSearch );

  }
  else {
    // Ga,b = < F, t | t^-1 a^r t = a^s >

    int r = ::maximalRoot( a );
    int s = ::maximalRoot( b );
    
    // case 1.2.2

    // case a. r = s
    if( r == s ) {
      // no more calculations required.
      answer = no;
    }

    // case b. r = -s
    else if( r == -s ) {
      answer = conjugateInSubgroups( A, u, v, conjugator, !recursiveSubgroupSearch );
      if( answer != yes )
	answer = conjugateInSubgroups( B, u, v, conjugator, !recursiveSubgroupSearch );
      
    }
    // all other cases:
    //   case c. r | s
    //   case d. s | r
    //   case e. otherwise cases like case 1.2.1.
    else { 
      answer = conjugateInSubgroups( A, u, v, conjugator, recursiveSubgroupSearch );
      if( answer != yes )
	answer = conjugateInSubgroups( B, u, v, conjugator, recursiveSubgroupSearch );
    }
    // end of case 1.2.2
  }

  return answer;
}

Trichotomy HNNExtOfFreeGroupRep::conjugacyProblem_case2(
  VectorOf<Word>& uDec, VectorOf<Word>& vDec, Word& conjugator ) const
{
  conjugator = Word();

  CONDITION( WARNING, vDec.length() % 2 == 0 && uDec.length() % 2 == 0 );

  int decLen = uDec.length();
  Word u = compose( uDec );

  for( int shift = 0; shift < decLen; shift += 2 ) {

    VectorOf<Word> vPerm = cyclicallyPermute( vDec, shift );

    // Check whether uDec and vPerm have the same powers of stable letters.

    bool sameStableLetters = true;
    for( int i = 1; i < decLen; i += 2 ) {
      if( uDec[i] != vPerm[i] ) {
	sameStableLetters = false;
	break;
      }
    }
    if( sameStableLetters ) {
      // Check whether uDec and vPerm are conjugated by element of
      // amalgamated subgroup.

      Generator t_first = vPerm[1][0];
      Generator t_last  = vPerm[decLen-1][0];

      NumberOfSubgroup cSubgrp = ( ord(t_last) > 0 ? B : A );
      NumberOfSubgroup dSubgrp = ( ord(t_first) > 0 ? A : B );

      Word uComp = uDec[0];
      Word vComp = vPerm[0];
      
      int mMax = uComp.length() + vComp.length() + 1;

      Word cWord = getGeneratorOfSubgroup( cSubgrp, 0 );
      Word c = raiseToPower( cWord, -mMax-1 );

      for( int m = -mMax; m <= mMax; ++m ) {

	c = (c * cWord).freelyReduce();
	
	if( theSubgroups[dSubgrp].contains( uComp.inverse() * c * vComp ) ) {
	  Word v1 = compose( vPerm ).conjugateBy( c.inverse() );
	  if( areEqual( u, v1 ) ) {
	    VectorOf<Word> vConjDec = vDec;
	    vConjDec.shrink( shift, decLen-shift );
	    Word vConj = compose( vConjDec );
	    conjugator = (c * vConj).freelyReduce();
	    return yes;
	  }
	}
      } // for( m = -mMax; m < mMax; ++m ) 
    }
  } // for( shift = 0; ... )
  return no;
}


void HNNExtOfFreeGroupRep::write( ostream& ostr ) const
{
  HNNExtensionRep::write( ostr );
  ostr 
    < theSubgroups[0].generators()
    < theSubgroups[1].generators();
}

void HNNExtOfFreeGroupRep::read( istream& istr ) 
{
  HNNExtensionRep::read( istr );
  
  // contsruct theBasisFreeGroup.
  VectorOf<Chars> baseGroupGens = theNamesOfGenerators;
  baseGroupGens.shrink( theNumberOfGenerators-1 );
  theBasisFreeGroup = FreeGroup( baseGroupGens );

  // construct theSubgroups;
  VectorOf<Word> theSubGens;
  istr > theSubGens;
  theSubgroups[0] = SGofFreeGroup( theBasisFreeGroup, theSubGens );
  istr > theSubGens;
  theSubgroups[1] = SGofFreeGroup( theBasisFreeGroup, theSubGens );

  makeSubgroupsMappings();
  
}

////////////////////////////////////////////////////////////////////////////
//
//  Helper class:  <HNNExtOfFreeGroup::SpecialHNNExtOfFreeGroupRep>.
//  
//  Implementation.
//
////////////////////////////////////////////////////////////////////////////

HNNExtOfFreeGroupRep::SpecialHNNExtOfFreeGroup::SpecialHNNExtOfFreeGroup( 
  const Automorphism& mapToOriginalPresentation,
  const FreeGroup& basisFreeGroup,
  const Chars& nameOfStableLetter,
  const SGofFreeGroup& A,
  const SGofFreeGroup& B,
  bool  conjugatedSeparated,
  const VectorOf<Word>& rootsOfSubgroupsGens,
  const VectorOf<int>&  powersOfSubgroupsGens
  )
  : toOriginalPresentation( mapToOriginalPresentation ),
    H( new HNNExtOfFreeGroupRep( basisFreeGroup, nameOfStableLetter, A, B ) ),
    areAmalgSubgroupsConjugateSeparated( conjugatedSeparated ),
    theRootsOfSubgroupsGens( rootsOfSubgroupsGens ),
    thePowersOfSubgroupsGens( powersOfSubgroupsGens )
{ } 


HNNExtOfFreeGroupRep::SpecialHNNExtOfFreeGroup::SpecialHNNExtOfFreeGroup( 
  const HNNExtOfFreeGroupRep::SpecialHNNExtOfFreeGroup& S )
  : toOriginalPresentation( S.toOriginalPresentation ),
    H( S.H ? new HNNExtOfFreeGroupRep(*S.H) : 0 ),
    areAmalgSubgroupsConjugateSeparated( S.areAmalgSubgroupsConjugateSeparated ),
    theRootsOfSubgroupsGens( S.theRootsOfSubgroupsGens ),
    thePowersOfSubgroupsGens( S.thePowersOfSubgroupsGens )
{ }

HNNExtOfFreeGroupRep::SpecialHNNExtOfFreeGroup& 
HNNExtOfFreeGroupRep::SpecialHNNExtOfFreeGroup::operator = ( 
  const HNNExtOfFreeGroupRep::SpecialHNNExtOfFreeGroup& S )
{
  if( &S != this ) {
    toOriginalPresentation = S.toOriginalPresentation;
    delete H;
    if( S.H )
      H = new HNNExtOfFreeGroupRep( *S.H );
    else
      H = 0;
    areAmalgSubgroupsConjugateSeparated = S.areAmalgSubgroupsConjugateSeparated;
    theRootsOfSubgroupsGens  = S.theRootsOfSubgroupsGens;
    thePowersOfSubgroupsGens = S.thePowersOfSubgroupsGens;
  }
  return *this;
}



const HNNExtOfFreeGroupRep& 
HNNExtOfFreeGroupRep::SpecialHNNExtOfFreeGroup::presentation() const
{
  if( !H )
    error("Called SpecialHNNExtOfFreeGroup::presentation() "
	  "when no HNN-extension was specified.");
  return *H;
}

 
////////////////////////////////////////////////////////////////////////////
//
//  Helper class:  <HNNExtOfFreeGroup::Maximal Root Problem>.
//  
//  Implementation.
//
////////////////////////////////////////////////////////////////////////////

HNNExtOfFreeGroupRep::MaximalRootProblem::MaximalRootProblem( const Word& w )
  : theGroup(), theWord( w ), theRoot(), thePower(1), theAnswer( dontknow ), 
    isSolved( false )
{ }


void HNNExtOfFreeGroupRep::MaximalRootProblem::solve( 
  const SpecialHNNExtOfFreeGroup& group )
{
  if( isSolved ) return;

  theGroup = group;

  const HNNExtOfFreeGroupRep& H = theGroup.presentation();

  int numOfGens = H.theNumberOfGenerators;

  Word wConj;
  VectorOf<Word> wDeco = H.cyclicallyReducedDecompositionOf( theWord, wConj );
  
  Word wRed = compose(wDeco);

  stableLetter = H.stableLetter(); 

  // The two non-trivial cases depending on words length.
  if( wDeco.length(  ) == 0 ) 
    setAnswer( Word(), 1 );
  else if( wDeco.length() == 1 ) 
    length0( wRed ); 
  else 
    lengthN( wDeco );

  if( theAnswer == yes )
    theRoot = theRoot.conjugateBy( wConj );

  isSolved = true;
}

void HNNExtOfFreeGroupRep::MaximalRootProblem::length0( const Word& w ) 
{
  const HNNExtOfFreeGroupRep& H = theGroup.presentation();
  // H = < F, t | t^-1 a^k t = b^l >
  const FreeGroup F = (const FreeGroup&)H.getBasisGroup(); 
  Word a = theGroup.theRootsOfSubgroupsGens[A];
  Word b = theGroup.theRootsOfSubgroupsGens[B];
  int  k = theGroup.thePowersOfSubgroupsGens[A];
  int  l = theGroup.thePowersOfSubgroupsGens[B];

  // w = w(x,y,..,z), where F = <x,y,..,z>
  // w is a cyclically reduced word.
  Word wRoot;
  int wPower;
  maximalRootInFreeGroup( w, wRoot, wPower );
  adjustRoot( wRoot, wPower );


  if( !theGroup.areAmalgSubgroupsConjugateSeparated ) {
    // G = < F, t | t^-1 a^k t = a^l >,  a - root word, A=<a^k>, B=<a^l>

    if( abs(k) == abs(l) ) {
      // Maximal root of w is computed as in a free group.
      setAnswer( wRoot, wPower );
      return;
    }

    // Let us define d=gcd(k,l), C=< a^d >.
    // Check whether w is conjugated with some element of C.

    int d = GCD2(k,l);
    Word ad = F.raiseToPower( a, d ); 

    VectorOf<Word> gen(1);
    gen[0] = ad;
    SGofFreeGroup S( F, gen );
    
    /* Bug in SGofFreeGroup::conjugateInSubgroup():
       if( !S.conjugateInSubgroup( w, wConjugator ) ) { 
       setAnswer( wRoot, wPower );
       return;
       } 
     */

    if( w.length() % ad.length() ) { // w, ad are cyclically reduced words.
      setAnswer( wRoot, wPower );
      return;
    }   

    Word wRoot2 = w.initialSegment( a.length() );    

    // Calculate a power r of element w in subgroup C: w = (a^r)^x. 
    int r = w.length() / a.length();
    Word wc;
    Word wConjugator;

    if( F.conjugacyProblem( wRoot2.inverse(), a, wConjugator ) ) {
      r = -r;
      wc = wc.inverse();
    }
    else if( F.conjugacyProblem( wRoot2, a, wConjugator ) ) {
      wc = w;
    }
    else {
      // w is not conjugate with C. Compute maximal root as in a free group.
      setAnswer( wRoot, wPower );
      return;
    }
	  
    wc = Word( wc.conjugateBy( wConjugator ) ).freelyReduce();
    
    wRoot = wc.initialSegment( a.length() );
    adjustRoot( wRoot, r );

    CONDITION( WARNING, 
	       wc.length() == w.length() && wc.length() % wRoot.length() == 0 );

    // Make sure |k| < |l|. 
    Word tconj;
    if( abs(k) < abs(l) ) {
      tconj = inv( H.stableLetter() );
    }
    else {
      tconj = H.stableLetter();
      swap( k, l );
    }

    if( k < 0 ) {
      wRoot = wRoot.inverse();
      k = -k;
      l = -l;
      r = -r;
    }

    // Reduces k,l,r by gcd(k,l,r).
    int k1 = k / d;
    int l1 = l / d;
    int r1 = r / d;   

    if( k1 == 1 ) {
      // G = < a, t | t^-1 a t = a^l1 >, w = a^r1
      // Maximal root of w does not exist!
      setAnswer( w, 0 );
      return;
    }
    else {
      // Let r1 = k1^alpha * l1^beta * r2, note that gcd(k1,l1) = 1
      // Then a power of maximal root is r1' = l1^(alpha+beta) * r2.
      // and the maximal root is  ( a^r1' ) ^ (t^alpha).
      int alpha = 0;
      while( r1 % k1 == 0 ) {
	r1 = (r1 / k1) * l1;
	++alpha;
      }
      
      r = d * r1;
      wRoot = wRoot.conjugateBy( F.raiseToPower(tconj,alpha) * wConjugator.inverse() );
      setAnswer( wRoot, r );
    }
  } 
  else {
    // G = < F, t | t^-1 a^k t = b^l >
    // Amalgamated subgroups are conjugate separated.

    VectorOf<Word> gen(1);
    
    gen[0] = a;
    SGofFreeGroup SA( F, gen );

    gen[0] = b;
    SGofFreeGroup SB( F, gen );

    Word wConjugator;

    if( SA.conjugateInSubgroup( wRoot, wConjugator ) 
	&& wPower % k == 0 
	&& abs(k) < abs(l) ) {
      
      wRoot = Word(wRoot.conjugateBy( wConjugator )).freelyReduce();
      adjustRoot( wRoot, wPower );
      CONDITION( WARNING, wRoot == a );

      int r = (wPower / k) * l;
      wRoot = b.conjugateBy( stableLetter * wConjugator.inverse() );
      setAnswer( wRoot, r );
      return;
    }
    else if( SB.conjugateInSubgroup( wRoot, wConjugator ) 
	     && wPower % l == 0 
	     && abs(l) < abs(k) ) {
      
      wRoot = Word(wRoot.conjugateBy( wConjugator )).freelyReduce();
      adjustRoot( wRoot, wPower );
      CONDITION( WARNING, wRoot == b );
      
      int r = (wPower/ l) * k;
      wRoot = a.conjugateBy( stableLetter * wConjugator.inverse() );
      setAnswer( wRoot, r );
    }
    else {
      // Extract maximal root as in free group.
      setAnswer( wRoot, wPower );
    }
  }

}


void HNNExtOfFreeGroupRep::MaximalRootProblem::lengthN( const VectorOf<Word> wDeco )
{

  Word w = compose( wDeco );
  
  if( wDeco.length() == 2 ) {
    setAnswer( w, 1 );
    return;
  }

  // H = < F, t | t^-1 a^k t = b^l >  
  const HNNExtOfFreeGroupRep& H = theGroup.presentation();
  const FreeGroup& F = H.theBasisFreeGroup;
  Generator stableLetter = H.stableLetter();
  // Normal cyclically reduced form of w = a1 t^e1 s1 t^e2 s2 .. t^eN.

  int wDecoLen = wDeco.length();
  int halfLen = wDeco.length() / 2; 

  // find maximal terminal segment of normal decomposition:
  // all components commute with a or b.
  
  int commutedComponents = 0;

  for( int component = wDecoLen-2; component >= halfLen; component -= 2 ) {
    
    if( component > 0 && wDeco[component-1] == wDeco[component+1] )
      break;
    
    int S = ( wDeco[component+1][0] == stableLetter ? A : B );
    Word g = theGroup.theRootsOfSubgroupsGens[S];

    if( !F.wordProblem( F.commutator(g, wDeco[component]) ) )
      break;

    commutedComponents += 2;
  }

  for( int rootLen = 2; rootLen <= halfLen; rootLen += 2 ) {

    if( wDecoLen % rootLen )
      continue;

    // Check stable letters.
    
    int lexCheckStableLetters = true;
    for( int i = rootLen + 1; i < wDecoLen; i += 2 ) {
      if( wDeco[i-rootLen] != wDeco[i] ) {
	lexCheckStableLetters = false;
	break;
      }
    }

    if( !lexCheckStableLetters ) 
      continue;

    int power = wDecoLen / rootLen;

    if( rootLen <= commutedComponents ) {

      // all components of cyclically reduced normal form of g 
      // commute with generators of amalgamated subgroups.
      // Commutative case.
      NumberOfSubgroup S = ( wDeco[1][0] == stableLetter ? A : B );
      Word rightRep = H.rightRepresentative( S, wDeco[0] );
      Word subgroupElt = (wDeco[0] * rightRep.inverse()).freelyReduce();
      int abskt = H.theSubgroups[S].inNielsenWords( subgroupElt ).length();

      if( abskt % power == 0 ) {

	// make a pretender to be a maximal root of w.
	VectorOf<Word> rootDeco( wDeco );
	rootDeco.shrink( wDecoLen-rootLen, rootLen );       
	Word root = ( subgroupElt.initialSegment( subgroupElt.length()/power) 
		      * H.compose(rootDeco) ).freelyReduce();
	
	// final check for the pretender.
	if( H.areEqual( w, H.raiseToPower( root, power ) ) ) {
	  setAnswer( root, power );
	  return;
	}      
      }
    }
    else {
      // some component of root decomposition does not commute with a generator 
      // of some amalgamated subgroup generator.
      bool lexCheckCommutedPartOfRoot = true;
      for( int i = wDecoLen-commutedComponents; i < wDecoLen; i += 2 ) { 
	if( wDeco[ i ] != wDeco[ i-rootLen ] ) {
	  lexCheckCommutedPartOfRoot = false;
	  break;
	}
      }
      if( !lexCheckCommutedPartOfRoot )
	continue;

      // Check the equation:    g1^r x g2^q = y
      // x, y -- components of normal decomposition of w,
      // g1, g2 -- generators of amalgamated subgroups ( g1 != g2 ).

      int component = wDecoLen - commutedComponents - 2; //!!
      Word y = wDeco[ component - rootLen ];
      Word x = wDeco[ component ];

      Generator rightT = wDeco[ component + 1 ][0];
      NumberOfSubgroup S2 = ( rightT == stableLetter ? A : B );
      Word g2 = H.getGeneratorOfSubgroup( S2, 0 );

      Generator leftT;
      if( commutedComponents == rootLen-2 ) // ????
	leftT = wDeco[ wDecoLen - 1 ][0];
      else 
	leftT =  wDeco[ component - 1 ][0];
      NumberOfSubgroup S1 = ( leftT == stableLetter ? B : A );
      Word g1 = H.getGeneratorOfSubgroup( S1, 0 );

      bool xcommute = false;
      bool ycommute = false;
      Word u;
      if( F.wordProblem( F.commutator(g1, x) ) ) {
	xcommute = true;
	u = x.inverse() * y;
      }
      else if( F.wordProblem( F.commutator(g2, x) ) ) {
	xcommute = true;
	u = y * x.inverse();
      }
      else if( F.wordProblem( F.commutator(g1.inverse(), y) ) ) {
	ycommute = true;
	u = y.inverse() * x;
      }
      else if( F.wordProblem( F.commutator(g2.inverse(), y) ) ) {
	ycommute = true;
	u = x * y.inverse();
      }
      
      if( !xcommute && !ycommute ) {
	// general case:  g1^p x g2^q = y,  where g1,g2,x,y are given.

	VectorOf<Word> rootDeco( wDeco );
	rootDeco.shrink( wDecoLen-rootLen, rootLen );	
	Word root = H.compose( rootDeco );
	NumberOfSubgroup S = ( wDeco[wDecoLen-1][0] == stableLetter ? B : A );
	Word g = H.getGeneratorOfSubgroup( S, 0 );

	// Let us rewrite the equation: g2^-q = y^-1 g1^p x.
	// g1, g2 are cyclically reduced
	// Free group property: |p| + |q| < (|x| + |y|)/min(|a|,|b|) + 8.
	Word yInv = y.inverse();
	int maxPower = (x.length()+y.length()) / min(g1.length(),g2.length()) + 8;
	for( int p = -maxPower; p <= maxPower; ++p ) {
	  Word tmp = yInv * H.raiseToPower( g1, p ) * x;
	  if( H.subgroupContains( S2, tmp ) ) {
	    Word fullRoot = Word(H.raiseToPower( g, p ) * root).freelyReduce();
	    if( H.areEqual( w, H.raiseToPower(fullRoot, power) ) ) {
	      setAnswer( fullRoot, power );
	      return;
	    } 
	  }
	}
      }
      else {
	// g1^p g2^q = u.
	VectorOf<Word> sg(2);
	sg[0] = g1;
	sg[1] = g2;	  
	SGofFreeGroup SG( F, sg );
	
	if( !SG.contains( u ) )
	  continue;
	
	Word unielsen = SG.inNielsenWords( u );
	
	// determine p.
	Generator g1nielsen = SG.inNielsenWords( g1 )[0];
	int p = 0;
	for( p = 0; 
	     p < unielsen.length() && abs(ord(g1nielsen)) == abs(ord(unielsen[p]));
	     ++p )
	  EMPTY_BODY;
	
	// determine q.
	int q = 0;
	Generator g2nielsen = SG.inNielsenWords( g2 )[0];
	for( int i = p; 
	     i < unielsen.length() && abs(ord(g2nielsen)) == abs(ord(unielsen[i])); 
	     ++i, ++q )
	  EMPTY_BODY;
	
	if( p+q != unielsen.length() ) // u cannot be represented as g1^p * g2^q.
	  continue;
	
	// Corrects sign of p,q in decomposition u. 
	if( q > 0 && ord(unielsen[p]) != ord(g2nielsen) )
	  q = -q;
	
	if( p > 0 && ord(unielsen[0]) != ord(g1nielsen) )
	  p = -p;	    
	
	// Try to construct a maximal root of w.
	
	VectorOf<Word> rootDeco( wDeco );
	rootDeco.shrink( wDecoLen-rootLen, rootLen );
	Word g;
	if( wDeco[ wDecoLen-1 ][0] == stableLetter )
	  g = H.getGeneratorOfSubgroup( B, 0 );
	else 
	  g = H.getGeneratorOfSubgroup( A, 0 );
	
	Word root = H.raiseToPower( g, q );
	if( ycommute )
	  root = root.inverse();
	root = root * H.compose( rootDeco );
	if( H.areEqual( w, H.raiseToPower( root, power ) ) ) {
	  setAnswer( root, power );
	  return;
	}
      }	
    }
  }

  setAnswer( w, 1 ); // the word w is prime in H.
}	


void HNNExtOfFreeGroupRep::MaximalRootProblem::setAnswer( const Word& root, int power )
{
  theRoot = root.freelyReduce();
  if( power < 0 ) {
    thePower = -power;
    theRoot  = theRoot.inverse();
  }
  else 
    thePower = power;

  theRoot = theGroup.toOriginalPresentation.imageOf( theRoot ).freelyReduce();

  theAnswer = ( thePower > 1 ? yes : no );
}

//---------------------- HNNExtOfFreeGroupRep:: --------------------//
//------------------------- Static members -------------------------//
 
const Type HNNExtOfFreeGroupRep::theHNNExtOfFreeGroupType = Type( Type::unique() );
 
 

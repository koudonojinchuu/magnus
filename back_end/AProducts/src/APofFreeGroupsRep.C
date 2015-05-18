// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the AmalgProductOfFreeGroupsRep class.
//           The name is abbreviated to fit in a library.
//
// Principal Authors: Eugene Paderin, Dmitry Pechkin
//
// Status: draft
//
// Revision History:
//
// Discussion:
//
// Bugs:
//
//
// Special Notes:
//
//


#include "APofFreeGroupsRep.h"
#include "AmalgamatedProductParser.h"
#include "Automorphism.h"
#include "Associations.h"
#include "APofFreeGroups.h"
#include "Stack.h"
#include <stdio.h>
#include "GeneralWhitehead.h"
#include "CONDITION.h"

//#define DEBUG_APGroup

//------------- Class LocalWord ------------------------------------

// multiplication for local words
// the result is always freely reduced

LocalWord operator * (const LocalWord& w1, const LocalWord& w2)
{
  // only empty words have INTERSECTION flag
  if(w2.theFactor == INTERSECTION) return w1.freelyReduce();
  if(w1.theFactor == INTERSECTION) return w2.freelyReduce();
  
#if SAFETY > 0
  if(w1.theFactor != w2.theFactor)
    error("Attempt to multiply words of different groups");
#endif

  LocalWord res;
  res.theWord = w1.theWord * w2.theWord;
  res = res.freelyReduce();
  if(res.theWord.length() == 0)
    res.theFactor = INTERSECTION;
  else res.theFactor = w1.theFactor;

  return res;
}

//------------ Class AmalgProductOfFreeGroupsRep -------------------

//
// constructors
//

AmalgProductOfFreeGroupsRep::AmalgProductOfFreeGroupsRep(
  const FreeGroup& g1, const FreeGroup& g2,
  const VectorOf<Word>& gen1, const VectorOf<Word>& gen2 )
: FPGroupRep( concatenate(g1.namesOfGenerators(), g2.namesOfGenerators()) ),
  factor(2), assocSubgroup(2), subgroupMapping(2), nielsenBasisToGensOfSubgroup(2),
  numerationShift(g1.rank())
{
  // finish constructing of FPGroupRep part
  
  SetOf<Word> relators;
  int numberOfSubgroupGenerators = gen1.length();
  for(int i=0; i<numberOfSubgroupGenerators; i++) {
    Word rel=gen1[i]*localToGlobal(LocalWord(gen2[i],RIGHT_FACTOR)).inverse();
    relators |= rel.freelyReduce();
  }
  setRelators(relators);
  
  // initialize vector data

  factor[0] = g1;
  factor[1] = g2;
  
  assocSubgroup[0] = SGofFreeGroup(g1, gen1);
  assocSubgroup[1] = SGofFreeGroup(g2, gen2);
  rankOfSubgroups = ((SGofFreeGroup)assocSubgroup[0]).nielsenBasis().length();

  makeSubgroupMapping(gen1, gen2);
  fixGeneratorsNames();
}

AmalgProductOfFreeGroupsRep::AmalgProductOfFreeGroupsRep(
  const SGofFreeGroup& sg1, const SGofFreeGroup& sg2)
: FPGroupRep( concatenate(sg1.parentGroup().namesOfGenerators(),
			  sg2.parentGroup().namesOfGenerators()) ),
  factor(2), assocSubgroup(2), subgroupMapping(2), 
  nielsenBasisToGensOfSubgroup(2),
  rankOfSubgroups( SGofFreeGroup(sg1).nielsenBasis().length() ), 
  // Use copy of sg1 to call non-const method 'nielsenBasis()'.
  numerationShift(sg1.parentGroup().rank())
{

  // initialize vector data

  assocSubgroup[0] = sg1;
  assocSubgroup[1] = sg2;

  factor[0] = sg1.parentGroup();
  factor[1] = sg2.parentGroup();

  // Finish constructing of FPGroupRep part.

  SetOf<Word> relators;
  int numberOfSubgroupGenerators = sg1.generators().length();
  for( int i = 0; i < numberOfSubgroupGenerators; ++i ) {
    Word rel = sg1.generators()[i] * 
      localToGlobal(LocalWord(sg2.generators()[i], RIGHT_FACTOR)).inverse();
    relators |= rel.freelyReduce();
  }
  setRelators(relators);

  makeSubgroupMapping(sg1.generators(), sg2.generators());
  fixGeneratorsNames();
}


void
AmalgProductOfFreeGroupsRep::makeSubgroupMapping(const VectorOf<Word>& gen1,
						 const VectorOf<Word>& gen2)
{
  // Check whether the subgroups have the same rank.
  if( ((SGofFreeGroup)assocSubgroup[0]).nielsenBasis().length() != 
      ((SGofFreeGroup)assocSubgroup[1]).nielsenBasis().length() ) {
    error("Cannot construct an amalgamated product: "
	  "associated subgroups have different rank.");
  }

  // Find a free basis of subgroup and express it in generators of old one. 
  // Do this for both associated subgroups.
  VectorOf<Word> freeGens1, nielsenInGens1;
  VectorOf<Word> freeGens2, nielsenInGens2;
  FreeGroup::NielsenBasis nbasis1( gen1, factor.ref(0).namesOfGenerators(), false );
  freeGens1 = nbasis1.newGenerators();
  nielsenInGens1 = nbasis1.expressNewGensInOldGens();
  FreeGroup::NielsenBasis nbasis2( gen2, factor.ref(1).namesOfGenerators(), false );
  freeGens2 = nbasis2.newGenerators();
  nielsenInGens2 = nbasis2.expressNewGensInOldGens();

#ifdef DEBUG_APGroup
  cerr << "gen1 = " << gen1 << ",  gen1nielsen = " << freeGens1
       << ", nielsenInGens1 = " << nielsenInGens1 << endl
       << "gen2 = " << gen2 << ",  gen2nielsen = " << freeGens2
       << ", nielsenInGens2 = " << nielsenInGens2 << endl;
#endif

  CONDITION( ERROR, rankOfSubgroups == freeGens2.length() );

  for( int i = 0; i < rankOfSubgroups; ++i ) {
    freeGens1[i] = nielsenInGens1[i].replaceGenerators( gen1 ).freelyReduce();
    freeGens2[i] = nielsenInGens1[i].replaceGenerators( gen2 ).freelyReduce();
  }

  // Now we work only with free basises of associated subgroups. 
  // Forget the old (given) basises!

#ifdef DEBUG_APGroup
  cerr << "express free basis in group generators: " << endl
       << " freeGens1 = " << freeGens1 << endl
       << " freeGens2 = " << freeGens2 << endl;
#endif

  // Construct subgroupMapping.

  FreeGroup fg(rankOfSubgroups);
  // Free group generated by Nielsen words of subgroup basis.

  VectorOf<Word> images(rankOfSubgroups);

  for(int i = 0; i < rankOfSubgroups; i++)
    images[i] = assocSubgroup.ref(0).inNielsenWords(freeGens1[i]);

  Automorphism mapGen1toNielsen1(fg, images);
  // Rewrites a word from the first subgroup in terms of its Nielsen basis.

  nielsenBasisToGensOfSubgroup[0] = mapGen1toNielsen1.inverse();

  for(int i = 0; i < rankOfSubgroups; i++)
    images[i] = assocSubgroup.ref(1).inNielsenWords(freeGens2[i]);

  Automorphism mapGen2toNielsen2(fg, images);
  // Rewrites a word from the second subgroup in terms of its Nielsen basis.

  nielsenBasisToGensOfSubgroup[1] = mapGen2toNielsen2.inverse();

  Automorphism nielsenBridge = nielsenBasisToGensOfSubgroup.ref(0) | mapGen2toNielsen2;
  nielsenBridge.reduceGenImages();
  // Maps Nielsen basis of the first subgroup to Nielsen basis of the second.

  Map map = nielsenBridge | Map(fg, (FreeGroup)factor[1],
				assocSubgroup.ref(1).nielsenBasis());
  //@dp Not implemented in `Map': map.reducedGenImages().
  subgroupMapping[0] = map;

  map = nielsenBridge.inverse() | Map(fg, (FreeGroup)factor[0],
				      assocSubgroup.ref(0).nielsenBasis());
  //@dp it will be useful to implement Map::reducedGenImages();
  subgroupMapping[1] = map;
}

static VectorOf<Chars> standardGeneratorsNames(const char *prefix, int maxGen)
{
  VectorOf<Chars> names(maxGen);
  char *tmpName = new char[strlen(prefix)+20]; // reserves enough memory 

  // Generates standard names.
  for(int i = 0; i<maxGen; ++i) {
    sprintf(tmpName, "%s%u", prefix, i);
    names[i] = Chars(tmpName);
  }

  delete tmpName;
  return names;
}



void AmalgProductOfFreeGroupsRep::fixGeneratorsNames()
{
  int rankOfFirstFactor  = factor.ref(0).rank();
  int rankOfSecondFactor = factor.ref(1).rank();
  
  // First perfoms cases when a factor has nameless generators.
  // We should put standard generators names for these ones.

  VectorOf<Chars> firstNames;
  int numberOfFirstNames = factor.ref(0).namesOfGenerators().length();
  if( numberOfFirstNames == 0 ) 
    firstNames = standardGeneratorsNames("x", rankOfFirstFactor);
  else
    firstNames = factor.ref(0).namesOfGenerators();
  
  VectorOf<Chars> secondNames;
  int numberOfSecondNames = factor.ref(1).namesOfGenerators().length();
  if( numberOfSecondNames == 0 ) {
    if( numberOfFirstNames == 0 )
      secondNames = standardGeneratorsNames("y", rankOfSecondFactor);
    else
      secondNames = standardGeneratorsNames("x", rankOfSecondFactor);
  }
  else
    secondNames = factor.ref(1).namesOfGenerators();
  
  theNamesOfGenerators = concatenate(firstNames, secondNames);
  theNumberOfGenerators = theNamesOfGenerators.length();
  
  if( numberOfFirstNames+numberOfSecondNames == 0 )
    return;
  
  bool haveDuplicates = false;
  
  SetOf<Chars> gens1;
  for(int i = 0; i < rankOfFirstFactor; ++i)
    gens1 |= theNamesOfGenerators[i];
  
  for(int i = 0; i < rankOfSecondFactor; ++i) {
    Chars genName = theNamesOfGenerators[numerationShift+i];
    if( gens1.contains(genName) )
      haveDuplicates = true;
  }
  
  if( !haveDuplicates ) return;
  
  VectorOf<Chars> newNames(rankOfSecondFactor);
  
  // Generates suffix to append to names of second factor and checks 
  // for duplicates once again.
  // First trying one-digit sufficies 1,2,0,3,4,5,6,7,9 (in this order).
  // If failed, then trying sufficies 11, 111, 1111, etc. 
  // The last ones are looked badly but it's only way to differ generators of
  // different factors of product due the definition of generator's grammar.

  bool badSuffix;
  Chars suffix;

  StackOf<Chars> suffixList;
  suffixList.push( "1" );
  suffixList.push( "9" );
  suffixList.push( "8" );
  suffixList.push( "7" );
  suffixList.push( "6" );
  suffixList.push( "5" );
  suffixList.push( "4" );
  suffixList.push( "3" );
  suffixList.push( "0" );
  suffixList.push( "2" );
  suffixList.push( "1" );

  do {
    badSuffix = false;

    if( !suffixList.isEmpty() ) 
      suffix = suffixList.pop();
    else
      suffix = suffix + "1";

    for(int i = 0; i < rankOfSecondFactor; i++) {
      Chars name = theNamesOfGenerators[numerationShift+i];
      name = name + suffix;
      if( gens1.contains(name) ) {
	badSuffix = true;
	break;
      }
      newNames[i] = name;
    }
  } while ( badSuffix );
  
  // Replace old names of second factor by new generated ones.
  for(int i = 0; i < rankOfSecondFactor; ++i)
    theNamesOfGenerators[numerationShift+i] = newNames[i];
}


//
// Methods dealing with the properties of the group:
//

int AmalgProductOfFreeGroupsRep::order( ) const
{
  return isTrivial() ? 1 : 0;
}

Trichotomy AmalgProductOfFreeGroupsRep::isTrivial( ) const
{
  return factor[0].isTrivial() && factor[1].isTrivial();
}

Trichotomy AmalgProductOfFreeGroupsRep::isFinite( ) const
{
  return isTrivial();
}

Trichotomy AmalgProductOfFreeGroupsRep::isInfinite( ) const
{
  return !isFinite();
}

Trichotomy AmalgProductOfFreeGroupsRep::isAbelian( ) const
{
  return factor[0].isAbelian() == yes && factor[1].isAbelian() == yes &&
    ( assocSubgroup.val(0).nielsenWord(0).length() == 1 ||
     assocSubgroup.val(1).nielsenWord(0).length() == 1 ) ;
}


// Determines whether the given generator of the subgroup is a part of
// a free basis for parent group of H. If yes, H_1 is a presentation
// of the subgroup in the found basis.
static bool isPartOfGroupBasis( int subgroupGenerator, const SGofFreeGroup& H,
				SGofFreeGroup& H_1 )
{
  int m =  H.generators().length();
  Word gen = H.generators() [subgroupGenerator];
  FreeGroup A = H.parentGroup();
  GeneralWhitehead G( A );
  G.startComputation( gen );

#ifdef DEBUG_APGroup
  cerr << "Check generator " << subgroupGenerator << " of subgroup " << H << endl;
#endif

  while( !G.done() )
    G.continueComputation();
  bool extend = G.extendsToFreeBasis();

#ifdef DEBUG_APGroup
  cerr << "Done. ";
  if( extend ) 
    cerr << " Extends to free basis of the group.";
  cerr << endl;
#endif
  
  if( extend ) {
    Map toNewBasis = G.getAutomorphism();
    
    CONDITION( ERROR, toNewBasis.imageOf( gen ).length() == 1 );
    
    int newGen = ord( toNewBasis.imageOf( gen )[0] );
    if( abs(newGen) != 1 ) {
      // makes subgroupGenerator to be first generator 
      // in new basis of the group.
      VectorOf<Word> images(m);
      if( newGen > 0 )
	images[ abs(newGen)-1 ] = Generator(-1);
      else
	images[ newGen-1 ] = Generator(1);
      newGen = abs(newGen);
      for( int j = 1, k = 1; j <= m; ++j ) {
	  if( j != newGen )
	    images[ k++ ] = Generator(j);
      }
      Map map( A, A, images );
      toNewBasis = toNewBasis | map;
      
#ifdef DEBUG_APGroup
      cerr << "Interbasis mapping is: " << toNewBasis << endl;
#endif
    }
    
    VectorOf<Word> newSubgroupGens( m );
    for( int j = 0, k = 0; j < m; ++j ) 
      newSubgroupGens[ k++ ] = toNewBasis.imageOf( H.generators()[j] );
    H_1 = SGofFreeGroup( H.parentGroup(), newSubgroupGens );
  }
  
  return extend;
}

// Determines whether one of generators of H is part of a basis for A.
// If yes then basis for A is changed to new one, generators of H are rewrotten
// in new basis, the generator and corresponding one in subgroup K are removed.
static bool checkSubgroup( FreeGroup& A, SGofFreeGroup& H, 
			   FreeGroup& B, SGofFreeGroup& K )
{
  int m = H.generators().length();
  SGofFreeGroup H1( A, VectorOf<Word>(0) ); // dummy ctor.
  for( int i = 0; i < m; ++i ) {
    if( isPartOfGroupBasis( i, H, H1 ) ) {

#ifdef DEBUG_APGroup
      cerr << " Old  presentation is : " << endl
	   << "   A = " << A << endl
	   << "   H = " << H << endl
	   << "   B = " << B << endl
	   << "   K = " << K << endl;
#endif

      // make reduced presentation of amalgamated product.
      // A = FreeGroup( A.numberOfGenerators()-1 );
      VectorOf<Word> newHGens( m-1 );
      VectorOf<Word> newKGens( m-1 );
      for( int j = 0, k = 0; j < m; ++j ) {
	if( j != i ) {
	  newHGens[ k ] = H1.generators()[ j ];
	  newKGens[ k ] = K.generators()[ j ];
	  ++k;
	}
      }
      H = SGofFreeGroup( A, newHGens );
      K = SGofFreeGroup( B, newKGens );

#ifdef DEBUG_APGroup
      cerr << " New presentation is : " << endl
	   << "   A_1 = " << A << endl
	   << "   H_1 = " << H << endl
	   << "   B_1 = " << B << endl
	   << "   K_1 = " << K << endl;
#endif
      return true;
    }
  }
  
  return false;
}


Trichotomy AmalgProductOfFreeGroupsRep::isFree( ) const
{
  //@dp Old implementation.
  if( assocSubgroup[0].isAFreeFactor() ||
     assocSubgroup[1].isAFreeFactor() ) return yes;

  return dontknow;

  //@dp New implementation. Based on a letter from G.Baumslag.
  //    Incomplete.
  FreeGroup A = factor[0];
  FreeGroup B = factor[1];
  SGofFreeGroup H = assocSubgroup[0];
  SGofFreeGroup K = assocSubgroup[1];

  int r = A.rank();
  int s = B.rank();
  int m = H.generators().length();
  
  if( m > r || m > s )
    return no;

  while( m > 0 ) {
    
    if( !checkSubgroup( A,H, B,K ) && !checkSubgroup( B,K, A,H ) )
      return no;
    --m;
  }

  return yes;
}

Trichotomy AmalgProductOfFreeGroupsRep::isHyperbolic( ) const
{
  //@dp 96/10/12 new implementation

  Word dummy;
  if( assocSubgroup[0].isMalnormal(dummy) || assocSubgroup[1].isMalnormal(dummy) )
    return yes;

  return dontknow;

}


//
// Methods dealing with group elements:
//


// Determines the group the given formal (and non-reduced) word belongs to.
// Returns INTERSECTION only if the word is zero-length.

NumberOfFactor AmalgProductOfFreeGroupsRep::factorOfFormalWord(const Word& w) const
{
  if(w.length() == 0) return INTERSECTION;

  NumberOfFactor fac = factorOfGenerator(w[0]);
  for(int i=1; i<w.length(); i++)
    if( factorOfGenerator(w[i]) != fac ) return PRODUCT;

  return fac;
}

// Determines the group the given word belongs to.
// Returns INTERSECTION iff the word belongs to the associated subgroup.

NumberOfFactor AmalgProductOfFreeGroupsRep::factorOfElement(const Word& w) const
{
  Word u = w.freelyReduce();
  if( u.length() == 0 ) return INTERSECTION;

  VectorOf<Word> decomposition = reducedDecomposition(u);
  if( decomposition.length() > 1 ) return PRODUCT;

  LocalWord x = globalToLocal(decomposition[0]);

  if( isElementOfSubgroup(x) ) 
    return INTERSECTION;
  else 
    return x.theFactor;
}

// Decomposes the given word to the product of words d_1 d_2 ....
// such that every d_i belongs to one of the factors and any
// two successive words belong to distinct factors.
// If the word is not freely reduced then components are non-reduced too.

VectorOf<Word> AmalgProductOfFreeGroupsRep::decompose(const Word& w) const
{
  VectorOf<Word> res(w.length());
  // It's better to reserve some extra space than to resize the vector
  // any time we add a new element.

  if( w.length() == 0 ) return res;

  int start, end;
  int component = 0;

  for(start=0; start < w.length(); start=end) {
    int fac = factorOfGenerator( w[start] );
    for(end = start+1; end<w.length() && factorOfGenerator(w[end])==fac; end++)
      {/* empty `for' */}
    res[component++] = w.subword( start, end );
  }
  res.shrink(component); // Stripping extra space.

  return res;
}

//
//   Methods dealing with "local words"
//

// Convert local coding word to global coding one.
// The result is freely reduced iff the source is freely reduced.

Word AmalgProductOfFreeGroupsRep::localToGlobal(const Word& theWord,
  NumberOfFactor theFactor) const
{
  if( theFactor == LEFT_FACTOR )
    return theWord;

  Word res = theWord;
  for(int i=0; i<res.length(); i++) {
    int Ord = ord(res[i]);
    if( Ord > 0)
      res[i] = Generator(Ord + numerationShift);
    else
      res[i] = Generator(Ord - numerationShift);
  }

  return res;
}

// Convert local coding word to global codeing one.
// The result is freely reduced iff the source is freely reduced.

LocalWord AmalgProductOfFreeGroupsRep::globalToLocal(const Word& w) const
{
  LocalWord res;
  res.theWord = w;
  res.theFactor = factorOfFormalWord(w);
  if(res.theFactor == LEFT_FACTOR) return res;

  for(int i = res.theWord.length()-1; i >= 0; i--) {
    int Ord = ord(res.theWord[i]);
    if( Ord > 0)
      res.theWord[i] = Generator(Ord - numerationShift);
    else res.theWord[i] = Generator(Ord + numerationShift);
  }

  return res;
}

// w is an element of associated subgroup (written in generators
// of one factor). The function rewrites it in generators of another
// factor.

LocalWord 
AmalgProductOfFreeGroupsRep::mapFromSubgroup(const LocalWord& w) const
{
  LocalWord res = w;

  if(res.theWord.length() == 0) return res;

  res.theFactor = (NumberOfFactor) (1 - w.theFactor);
  Word r = assocSubgroup[w.theFactor].inNielsenWords(w.theWord);
  res.theWord = subgroupMapping[w.theFactor].imageOf(r).freelyReduce();

  return res;
}

// Determines whether the given word is an element of associated subgroup.
bool AmalgProductOfFreeGroupsRep::isElementOfSubgroup(const LocalWord& w) const
{
  return assocSubgroup[w.theFactor].contains(w);
}

// Finds right Schreier Representative of given word.
LocalWord AmalgProductOfFreeGroupsRep::rightSchreierRepresentativeOf(
  const LocalWord& w) const
{
  LocalWord res = w.freelyReduce();
  res.theWord = assocSubgroup[ w.theFactor ]
                .rightSchreierRepresentative( w.theWord );
  return res;
}


// Finds the minimal (in the sense of number of components) decomposition
// of the given word. Decomposition components are freely reduced.

VectorOf<Word>
AmalgProductOfFreeGroupsRep::reducedDecomposition(const Word& w) const
{
  Word u = w.freelyReduce();
  VectorOf<Word> decomposition = decompose(u);

  if( decomposition.length() < 2 ) return decomposition;

  for(int i=0; i < decomposition.length() && decomposition.length() > 1; ) {

    LocalWord component = globalToLocal(decomposition[i]);

    if( isElementOfSubgroup(component) ) {
      component = mapFromSubgroup(component);
      Word res = localToGlobal(component);

      do {
	if(i != 0)  {
	  res = (Word)decomposition[--i] * res;
	  excludeFrom(decomposition, i);
	}
	if( i+1 < decomposition.length() )  {
	  res *= decomposition[i+1];
	  excludeFrom(decomposition, i+1);
	}
	res = res.freelyReduce();
      } while( res.length() == 0 && decomposition.length() > 1 );

      decomposition[i] = res;
    }
    else ++i;

  } // for(int i=0; i < decomposition.length() && decomposition.length() > 1;)

  if( decomposition.length() == 1 && decomposition.ref(0).length() == 0 )
    return VectorOf<Word>(0);

  return decomposition;
}


// Finds the normal decomposition of the given word which is a reduced 
// decomposition with second and later components are some right Schreier 
// representatives, freely reduced.

VectorOf<Word>
AmalgProductOfFreeGroupsRep::normalDecomposition(const Word& w) const
{
  // We start with reduced decomposition which already has a
  // minimal number of components.

  VectorOf<Word> decomposition = reducedDecomposition(w);

  // Sifting out trivial cases.

  if( decomposition.length() == 1 )
    if( Word(decomposition[0]).length() == 0 )
      return VectorOf<Word>(0);
    else return decomposition;

  // The normal form of the word is: g[0] g[1] g[2] ... g[n], where
  // g[0] is an element of the group, g[1] ... g[n] are
  // certain right (Schreier) representatives, and any two successive
  // g[i] g[i+1] belong to distinct factors.

  LocalWord subgroupElement;

  for(int i = decomposition.length() - 1; i >= 0; i--) {
    LocalWord component = globalToLocal(decomposition[i]);
    component = (component * subgroupElement).freelyReduce();
    LocalWord rep = rightSchreierRepresentativeOf(component);
    subgroupElement = mapFromSubgroup(component * rep.inverse());

    decomposition[i] = localToGlobal(rep);
    if( Word(decomposition[i]).length() == 0 )
      error("bad representation in normal decomposition");
  }

  subgroupElement = mapFromSubgroup(subgroupElement);
  Word first = localToGlobal(subgroupElement);
  if( decomposition.length() == 0 )
    decomposition.prepend(first);
  else {
    if( Word(decomposition[0]).length() == 0 && first.length() > 0 )
      error("bad representative (identity) in normal decomposition "
	    "with a non-identity element of assocciated subgroup");
    decomposition[0] = (first * (Word)decomposition[0]).freelyReduce();
  }

  if( decomposition.length() == 1 && Word(decomposition[0]).length() == 0 )
    return VectorOf<Word>(0);

  return decomposition;
};


void AmalgProductOfFreeGroupsRep::maximalRoot(const Word& w, Word& root, 
  int& power) const
{
  error("The method AmalgProductOfFreeGroupsRep::maximalRoot is not implemented.");
}


void
AmalgProductOfFreeGroupsRep::cyclicDecomposition(const Word& w,
  VectorOf<Word>& vec, Word& conjugator) const
// Cyclic decomposition is normal one satisfied the condition: all cyclic 
// permutations are normal decompositions.
// Let a normal decomposition be g1 * g2 * ... * gN, then the one satisfies
// the condition above iff  N > 1 && factor(g1) != factor(gN)  or N <= 1.
{
  VectorOf<Word> normalDeco = normalDecomposition(w);
  conjugator = Word();

  Word s1, s2;
  bool reduced = true;
  int decoLen = normalDeco.length();

  while( reduced && decoLen > 1 && decoLen % 2 == 1 ) {

    reduced = false;

    if( factorOfFormalWord(normalDeco[0]) == factorOfFormalWord(normalDeco[1]))
      error("two elements in normal decomposition belong to one factor.");

    //    printDecomposition(cerr, normalDeco);
    //    cerr << endl;

    s1 = normalDeco[0];
    s2 = normalDeco[decoLen-1];
    conjugator = s2 * conjugator;
    s1 = (s2 * s1).freelyReduce();

    if( factorOfElement(s1) == INTERSECTION ) {

      s1 = mapFromSubgroup( s1 );
      normalDeco[1] = (s1 * normalDeco[1]).freelyReduce();
      normalDeco.shrink(1, decoLen-2);
      reduced = true;

    }
    else {
      // Reduction process is stopped on this step.
      normalDeco[0] = s1;
      normalDeco.shrink(0, decoLen-1);
    }

    decoLen = normalDeco.length();
  }

  if( normalDeco.length() == 1 ) {

    if( Word(normalDeco[0]).length() == 0 )
      vec = VectorOf<Word>(0);
    else {
      // If there is only one component then reduce it.
      Word conj;
      normalDeco[0] = cyclicallyReduce(normalDeco[0], conj);
      conjugator = (conj * conjugator).freelyReduce();
      vec = normalDeco;
    }

  }
  else {
    if( normalDeco.length() > 0 )
      normalDeco[0] = Word(normalDeco[0]).freelyReduce();
    vec = normalDeco;
  }

}

void AmalgProductOfFreeGroupsRep::cyclicReduction(const Word& w,
  Word& redo, Word& conjugator) const
{
  VectorOf<Word> redo_vec;
  cyclicDecomposition(w, redo_vec, conjugator);
  redo = compose(redo_vec);
}


bool AmalgProductOfFreeGroupsRep::isProperPower(const Word& w) const
{
  Word root;
  int power;
  maximalRoot(w, root, power);

  return ( power > 1 ? true : false );
}

bool AmalgProductOfFreeGroupsRep::isProperPowerOfSecond(const Word& u, 
  const Word& w, int& power ) const
{
  Word uRoot, wRoot;
  int uPower, wPower;
  maximalRoot(u, uRoot, uPower);
  maximalRoot(w, wRoot, wPower);

  if( uRoot == wRoot  && uPower % wPower == 0 )
    power = uPower / wPower;
  else if( areEqual(wRoot.inverse(), uRoot) == yes &&  uPower % wPower == 0 )
    power = - uPower / wPower;
  else {
    power = 0;
    return false;
  }

  return true;
}


int AmalgProductOfFreeGroupsRep::numberOfSubstitutions( const Word& w ) const
{
  Word u = w.freelyReduce();

  int numOfSubsts = 0;

  if( wordProblem(u) != yes ) 
    return 0;

  VectorOf<Word> decomposition = decompose(u);

  int i = 0;
  while( decomposition.length() > 1 ) {

    LocalWord component = globalToLocal(decomposition[i]);

    if( isElementOfSubgroup(component) ) {
      
      int numOfFactor = component.theFactor;
      Word wordInGensOfSubgroup = nielsenBasisToGensOfSubgroup[ numOfFactor ]
	.imageOf( assocSubgroup[numOfFactor].inNielsenWords( (Word)component ) )
	.freelyReduce();

      numOfSubsts += wordInGensOfSubgroup.length();

      component = mapFromSubgroup(component);
      Word res = localToGlobal(component);

      do {
	if(i != 0)  {
	  res = (Word)decomposition[--i] * res;
	  excludeFrom(decomposition, i);
	}
	if( i+1 < decomposition.length() )  {
	  res *= decomposition[i+1];
	  excludeFrom(decomposition, i+1);
	}
	res = res.freelyReduce();
      } while( res.length() == 0 && decomposition.length() > 1 );

      decomposition[i] = res;
    }
    else ++i;

  }

  return numOfSubsts;
}


bool AmalgProductOfFreeGroupsRep::commute(const Word& u, const Word& w) const
{
  return ( wordProblem(commutator(u,w)) == yes ? true : false );
}


bool 
AmalgProductOfFreeGroupsRep::isSubgroupTrivial(const VectorOf<Word>& vec) const
{
  for(int i = 0; i < vec.length(); ++i)
    if( wordProblem(vec[i]) == no )
      return false;

  return true;
}


// Computes whether the subgroup defined by the given set of words
// is abelian.

bool 
AmalgProductOfFreeGroupsRep::isSubgroupAbelian(const VectorOf<Word>& vec) const
{
  // Brute force approach.
  int n = vec.length();
  for(int i = 0; i < n; i++)
    for(int j = i+1; j < n; j++)
      if( !commute(vec[i], vec[j]) )
	return false;

  return true;
}

//@ep This is now valid only for one relator case with its own
// maximalRoot. In general case, causes an error in maximalRoot().

bool 
AmalgProductOfFreeGroupsRep::isSubgroupCyclic(const VectorOf<Word>& vec) const
{
  bool isFirst = true;
  Word generalRoot;

  for(int i = 1; i < vec.length(); ++i) {
    Word root;
    int dummyPower;
    maximalRoot(vec[i], root, dummyPower);
    if( isFirst ) {
      generalRoot = root;
      isFirst = false;
    }
    else if( !areEqual( root, generalRoot ) )
      //    else if( root != generalRoot ) 
      // @dp 96/10/12 should we use word problem?
      return false;
  }
  return true;
}

//
// I/O:
//

void AmalgProductOfFreeGroupsRep::printOn(ostream& o) const
{
  o << "< " << factor[0] << " * ";
  o << " < ";
  int doComma = 0;
  for(int i = factor[0].numberOfGenerators(); i<theNumberOfGenerators; i++) {
    if( doComma ) o << ", ";
    doComma = 1;
    o << theNamesOfGenerators[i];
  }
  o << " > ; ";
  printRelators(o);
  o << " >";
}

GroupRep*
AmalgProductOfFreeGroupsRep::readFrom(istream& istr, Chars& errMesg) const
{
  AmalgamatedProductParser P(istr);
  return P.parseAPofFreeGroups(errMesg);
}

void AmalgProductOfFreeGroupsRep::printRelators(ostream& o) const
{
  int doComma = 0;
  VectorOf<Word> left  = assocSubgroup[0].generators();
  VectorOf<Word> right = assocSubgroup[1].generators();
  for(int i = 0; i < left.length(); i++ ) {
    if( doComma ) o << ", ";
    factor[0].printWord(o, (Word)left[i]);
    o << " = ";
    printWord(o, localToGlobal( LocalWord((Word)right[i], RIGHT_FACTOR) ) );
    doComma = 1;
  }
}

void AmalgProductOfFreeGroupsRep::printDecomposition(ostream& ostr, 
  const VectorOf<Word>& deco) const
{
  int len = deco.length();
  for( int i = 0; i < len; ++i ) {
    printWord(ostr, deco[i]);
    if ( i < len - 1 ) ostr << " . ";
  }
}


const Type AmalgProductOfFreeGroupsRep::theAmalgProductOfFreeGroupsType = Type::unique();




// ------------------------------------------------------------------- //
//                                                                     //
//                      APofFreeGroup class                            //
//                                                                     //
// ------------------------------------------------------------------- //

FreeGroup AmalgProductOfFreeGroups::factor( const NumberOfFactor& t ) const 
{
  switch( t ) {
  case LEFT_FACTOR:
  case RIGHT_FACTOR:
    return look()->factor[t];
  default:
    error("Neither left nor right factor is specified in APofFreeGroup::factor().");
  }
}


SGofFreeGroup AmalgProductOfFreeGroups::subgroup( const NumberOfFactor& t ) const 
{
  switch( t ) {
  case LEFT_FACTOR:
  case RIGHT_FACTOR:
    return look()->assocSubgroup[t];
  default:
    error("Neither left nor right factor is specified in APofFreeGroup::factor().");
  }
}
    

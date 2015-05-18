// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class HNNExtOfORGroup.
//
// Principal Author:  Dmitry Pechkin
//
// Status:
//
// Revision History:
//

#include "CONDITION.h"
#include "OneRelatorGroup.h"
#include "HNNExtOfORGroup.h"
#include "conversions.h"
#include "AbelianWord.h"

#define DEBUG_HNN_ORGROUP

#ifdef DEBUG_HNN_ORGROUP
# include "Margin.h"
#endif

inline bool isOdd( int i ) { return i & 1; }


HNNExtOfORGroupGeneric::HNNExtOfORGroupGeneric( 
  const OneRelatorGroup& G, const Chars& stableGenName,
  const ORGSubgroup& subgroupA, const ORGSubgroup& subgroupB
  )
{
#if SAFETY > 0
  if( subgroupA.parentGroup() != subgroupB.parentGroup() )
    error("HNNExtOfORGroupGeneric: amalgamated subgroups have different "
	  "parents.");
  if( subgroupA.parentGroup() != G ) 
    error("HNNExtOfORGroupGeneric: a basis group is different from "
	  "the parent group of the amalgamated subgroups.");
#endif

  theBasisGroup = new OneRelatorGroup( G );
  theNameOfStableGenerator = stableGenName;
  subgroups[A] = subgroupA.copy();
  subgroups[B] = subgroupB.copy();
}


HNNExtOfORGroupGeneric::HNNExtOfORGroupGeneric( 
  const HNNExtOfORGroupGeneric& H )
{
  init( H );
}

HNNExtOfORGroupGeneric::~HNNExtOfORGroupGeneric( )
{
  delete theBasisGroup;
  delete subgroups[A];
  delete subgroups[B];
}


HNNExtOfORGroupGeneric& HNNExtOfORGroupGeneric::operator=( 
  const HNNExtOfORGroupGeneric& H )
{
  if( this != &H ) {
    delete theBasisGroup;
    delete subgroups[A];
    delete subgroups[B];
    init( H );
  }
  return *this;
}

void HNNExtOfORGroupGeneric::init( const HNNExtOfORGroupGeneric& H )
{
  theBasisGroup = new OneRelatorGroup( *H.theBasisGroup );
  theNameOfStableGenerator = H.theNameOfStableGenerator;
  subgroups[A] = H.subgroups[A]->copy();
  subgroups[B] = H.subgroups[B]->copy();
}


FPGroup HNNExtOfORGroupGeneric::getPresentation() const 
{
  int numOfGens =  theBasisGroup->numberOfGenerators();

  // make generators names.
  VectorOf<Chars> gensNames( numOfGens+1 );  
  for( int i = 0; i < numOfGens; ++i )
    gensNames[i] = theBasisGroup->namesOfGenerators()[i];
  gensNames[numOfGens] = theNameOfStableGenerator;

  // make relators:
  
  SetOf<Word> relators;

  VectorOf<Word> aGens = subgroups[0]->generators();
  VectorOf<Word> bGens = subgroups[1]->generators();
  int numOfSubgrpsGens = aGens.length();
  Generator stableGen = numOfGens + 1;
  Generator invStableGen = inv(stableGen);
  
  for( int i = 0; i < numOfSubgrpsGens; ++i )
    relators |= stableGen * aGens[i] * invStableGen * bGens[i].inverse();
      
  // from one-relator basis group:
  relators |= theBasisGroup->relator();
  
  return FPGroup( gensNames, relators );
}


void HNNExtOfORGroupGeneric::printOn( ostream& ostr ) const 
{
  ostr << getPresentation();
}


bool HNNExtOfORGroupGeneric::operator == ( 
  const HNNExtOfORGroupGeneric& G ) const 
{
  if( *theBasisGroup == *G.theBasisGroup 
      && *subgroups[A] == *G.subgroups[A]
      && *subgroups[B] == *G.subgroups[B]
      )
    return true;
  return false;
}

void HNNExtOfORGroupGeneric::printDecomposition( 
  ostream& ostr, const VectorOf<Word>& deco ) const
{
  if( deco.length() == 0 ) {
    ostr << "1";
  }
  else {
    FPGroup G = getPresentation();
    int decoLen = deco.length();
    for( int i = 0; i < decoLen; ++i ) {
      if( i ) ostr << " * ";
      G.printWord( ostr, deco[i] );
    }
  }
}

void HNNExtOfORGroupGeneric::debugPrint( ostream& ostr ) const 
{
  ostr << endl
       << "*** Debug printing of HNNExtOfORGroup ***" << endl
       << getPresentation() << endl
       << "*** End of Debug printing of HNNExtOfORGroup ***" << endl 
    ;
}

int HNNExtOfORGroupGeneric::lengthOf( const VectorOf<Word>& deco ) const
{ 
  return ( deco.length() - 1 ) / 2 ;
}

int HNNExtOfORGroupGeneric::lengthOf( const Word& w ) const 
{
  return lengthOf( decompositionOf(w) );
}


VectorOf<Word> HNNExtOfORGroupGeneric::decompositionOf( const Word& w ) const
{
  if( w.length() == 0 )
    return VectorOf<Word>(1); // empty decomposition consist of empty word.
  
  ListOf<Word> deco;

  Word component;
  int stable = theBasisGroup->numberOfGenerators()+1;
  int wLen = w.length();
  int lastGen = abs(ord(w[0]));
  int start = 0;
  for( int i = 0; i < wLen; ++i ) {
    int gen = abs(ord(w[i]));
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

  if( abs(ord(deco[deco.length()-1][0])) == stable )
    deco.append( Word() );

  return makeVectorOf( deco );
}

VectorOf<Word> HNNExtOfORGroupGeneric::reducedDecompositionOf( 
  const Word& w ) const
{
  VectorOf<Word> deco = decompositionOf( w.freelyReduce() );

#ifdef DEBUG_HNN_ORGROUP
  Margin margin;
  margin.set( globalMargin );
  cerr << margin << "w = " << w << ", (deco) = " << deco << endl;
#endif

  int numOfGroupGens =  theBasisGroup->numberOfGenerators();

  //@dp: we do not use abelianization test now.
  //
  //  AbelianWord ab(numOfGroupGens, theBasisGroup->relator() );
  //  VectorOf<int> abelianRelator( numOfGroupGens );
  //  for(int i = ab.numberOfGenerators()-1; i >= 0; --i )
  //    abelianRelator[i] = ab[i].as_long();

  VectorOf<bool> falseVector( numOfGroupGens );
  for( int i = numOfGroupGens-1; i >= 0; --i )
    falseVector[i] = false;

  VectorOf<bool> subgroupGens[2];
  for( int S = 0; S < 2; ++S ) {
    subgroupGens[S] = falseVector;
    const VectorOf<Word> subWords =  subgroups[S]->generators();
    for( int i = subWords.length()-1; i >= 0; --i ) {
      int gen = abs(ord(subWords[i][0]));
      subgroupGens[S][gen-1] = true;
    }
  }

  ListOf<RelatorConjugate> listOfRelatorConjugates;

  for( int component = 2; component < deco.length()-2; component += 2 ) {

    if( component > 1 && suspectPinch( component, deco ) ) {

#ifdef DEBUG_HNN_ORGROUP
      cerr << margin << "* GENERIC: Trying subgroup containment for : " << endl
	   << margin << "  HNN = ";
      DEBUG_PRINT( cerr, *this );
      cerr << endl
	   << margin << "  deco = " << deco << endl
	   << margin << "  deco[current] = " << deco[component-1] << " . "
	   << deco[component] << " . " << deco[component+1] << endl;
#endif
      
      PinchStruct pinch = formPinch(component, deco);

      NumberOfSubgroup S = ( pinch.type == UP
			     ? HNNExtOfORGroupGeneric::A 
			     : HNNExtOfORGroupGeneric::B 
			     );

/*
  if( !abelianizationTest(pinch, abelianRelator, subgroupGens[S]) ) {
    #ifdef DEBUG_HNN_ORGROUP
    cerr << margin << "   AbTest says NO for this pinch." << endl;
    #endif
    continue;
  }

*/
      if( subgroup(S).contains( pinch.word ) ) {
	
	Word pinchInBasis = 
	  subgroup(S).rewriteFromGroupToSubgroupGenerators( pinch.word );
	
#ifdef DEBUG_HNN_ORGROUP
	cerr << margin << "Subgroup[" << S << "] = " 
	     << subgroup(S).generators() << endl
	     << margin << " deco[component] in subgroup basis is " 
	     << pinchInBasis << endl;
#endif

	Word newPinch = mappingFromSubgroup( S, pinchInBasis );

#ifdef DEBUG_HNN_ORGROUP
	cerr << margin << "  YES! new word is " << newPinch << endl;
#endif	

	// merging pinch with left and right components which exist always.
	
	newPinch = 
	  (deco[component-2] * newPinch * deco[ component+2 ]).freelyReduce();
	deco[ component-2 ] = newPinch;

#ifdef DEBUG_HNN_ORGROUP
	cerr << margin 
	     << "  merging with left and right compinents gives us "
	     << newPinch << endl;
#endif
	
	// remove 4 old  components from decomposition.
	int decoLen = deco.length();
	for( int pos = component + 3; pos < decoLen; ++pos ) 
	  deco[pos-4] = deco[pos];
	deco.shrink( decoLen - 4 );

	component -= 4;
      } // if the current pinch is proper.
      else {
#ifdef DEBUG_HNN_ORGROUP
	cerr << margin
	     << "   Containment problem says NO for this pinch." << endl;
#endif
      }
    } // if( suspectPinch())
  } // for( component=..;..;..)

  return deco;
}

int HNNExtOfORGroupGeneric::powerOfStableGen( 
  int component, const VectorOf<Word>& deco ) const 
{
  if( component < 0 && component >= deco.length() )
    return 0;

  const Word w = deco[component];

  CONDITION( ERROR, w.length() == 1 );

  const int g = ord(w[0]);
  //  const stable = theBasisGroup->numberOfGenerators() + 1;

  CONDITION( ERROR, abs(g) == stableLetter() );

  return ( g > 0 ? 1 : -1 );
}


bool HNNExtOfORGroupGeneric::suspectPinch( 
  int component, const VectorOf<Word>& deco ) const
{
  if( powerOfStableGen(component-1, deco) 
      * powerOfStableGen(component+1,deco) == -1 )
    return true;
  return false;
}

bool HNNExtOfORGroupGeneric::abelianizationTest( const PinchStruct& pinch, 
  const VectorOf<int>& abelianRelator,
  const VectorOf<bool>& subgroupGens ) const 
{
  // the abelianized word problem. Test whether
  // the exponentsum of each generator in test and not in gensTargetSubgp
  // is divisible by the exponentsum of the same generator in relator.

  AbelianWord abelianWord( theBasisGroup->numberOfGenerators(), pinch.word );

  if( !abelianWord.isTrivial() ) {

    bool ratioDefined = false;
    int ratio;
    
    SetOf<Generator> foundGens;

    for(int i = theBasisGroup->numberOfGenerators()-1; i >= 0; --i ) {
      if( subgroupGens[i] ) continue;
      foundGens |= Generator(i+1);
      int ai = abelianWord[i].as_long();
      int pi = abelianRelator[i];

      if( pi == 0 && ai != 0 
	  || pi != 0 && (ai % pi != 0)
	  || pi != 0 && ai == 0 && ratioDefined )
	return false;

      if( ai != 0 && pi != 0 ) {
	if( ratioDefined ) {
	  if( ratio != ai/pi ) return false;
	}
	else {
	  ratio = ai / pi;
	  ratioDefined = true;
	  // do second pass from the beginning.
	  i = theBasisGroup->numberOfGenerators(); 
	}
      }
    }
    if( ratioDefined ) {
      // search for a generator with nonzero exponent sum in the
      // relator, not appearing in the target subgroup, and
      // not appearing in test
      Word relator = theBasisGroup->relator();
      for( int i = relator.length()-1; i >= 0; --i ) {
	int gen = abs(ord(relator[i]));
	if( abelianRelator[gen-1] != 0 && !foundGens.contains(Generator(gen)) 
	    && !subgroupGens[gen-1] )
	  return false;
      }
    }
  }
  return true;
}

HNNExtOfORGroupGeneric::PinchStruct HNNExtOfORGroupGeneric::formPinch( 
  int component, const VectorOf<Word>& deco) const
{
  PinchStruct pinch;
  pinch.number = component;
  if( powerOfStableGen( component-1, deco ) == 1
      && powerOfStableGen( component+1, deco ) ==  -1 ) {
    pinch.type = UP;
  }
  else {
    CONDITION( WARNING, 
	       powerOfStableGen( component-1, deco ) == -1 
	       && powerOfStableGen( component+1, deco ) == 1 
	       );
    pinch.type = DOWN;
  }
  pinch.word = deco[component]; // ???

  return pinch;
}

/*@db
static inline int sign(const int i ) 
{
  return ( i > 0 ? 1 : -1 );
}
*/

Word HNNExtOfORGroupGeneric::mappingFromSubgroup( 
  const NumberOfSubgroup S, const Word& w ) const
{
  PRECONDITION( ERROR, S == A || S == B );

  Word result = w;
  NumberOfSubgroup Q = NumberOfSubgroup(1-S);
  for( int i = w.length()-1; i >= 0; --i ) {
    int agen = abs(ord(w[i]));
    CONDITION( ERROR, subgroups[Q]->generators()[ agen-1 ].length() == 1 );
    int gen = ord( subgroups[Q]->generators()[ agen-1 ][0] );
    result[i] = sign(ord(w[i])) * gen; 
  }
#ifdef DEBUG_HNN_ORGROUP
  Margin margin;
  margin.set( globalMargin );

  cerr << margin << "rewrote w=" << w << " into " << result << endl;
#endif
  return result;
}


VectorOf<Word> HNNExtOfORGroupGeneric::cyclicallyReducedDecompositionOf( 
  const Word& w, Word& conjugator ) const
{
  //@dp const bool keepDetails = true;
  //@dp ProductOfRelatorConjugates product;

  VectorOf<Word> deco = reducedDecompositionOf( w );

  conjugator = Word();

#ifdef DEBUG_HNN_ORGROUP
  Margin margin;
  margin.set( globalMargin );

  cerr << margin << "HHNORG::CyclicallyReduced Problem" << endl;
#endif

  bool reduced = true;
  while( deco.length() > 2 && reduced ) {

#ifdef DEBUG_HNN_ORGROUP
    cerr << margin << "deco = " << deco << endl;
#endif

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
	PinchStruct pinch = formPinch( 1, potencialPinch );
	
	// investigate the `pinch' to be a proper pinch.

	NumberOfSubgroup S = ( pinch.type == UP ? A : B );

	if(  subgroup(S).contains(elt) ) {
	  conjugator = deco[ decoLen-1 ] * conjugator;
	  Word pinchInBasis = 
	    subgroup(S).rewriteFromGroupToSubgroupGenerators( elt );
	  deco[2] = mappingFromSubgroup( S, pinchInBasis ) * deco[2];

	  // Get out 2 heading and 1 tailing components.
	  deco.shrink( 2, decoLen-3 );

	  reduced = true;
	}	
      } // if( suspectPinch(1, potencialPinch) ) 
    }
  } // while();

#ifdef DEBUG_HNN_ORGROUP
  cerr << margin << "deco after while= " << deco << endl;
#endif

  if( deco.length() == 1 ) {
    Word tmp = deco[0];
    conjugator = 
      tmp.terminalSegment((tmp.length() - tmp.cyclicallyReduce().length()) / 2)
      * conjugator;
    deco[0] = tmp.cyclicallyReduce();
  }

#ifdef DEBUG_HNN_ORGROUP
  cerr << margin << "deco final= " << deco << endl;
#endif
  return deco;
}


void HNNExtOfORGroupGeneric::write( ostream& ostr ) const
{
  ostr 
    < *theBasisGroup < theNameOfStableGenerator 
    < *subgroups[A] < *subgroups[B];
}

void HNNExtOfORGroupGeneric::read( istream& istr )
{
  istr 
    > *theBasisGroup > theNameOfStableGenerator 
    > *subgroups[A] > *subgroups[B];
}


HNNExtOfORGroup::HNNExtOfORGroup( const OneRelatorGroup& G,
				  const Chars& stableGenName,
				  const SubgroupOfOneRelatorGroup& subgroupA,
				  const SubgroupOfOneRelatorGroup& subgroupB
				  )
: HNNExtOfORGroupGeneric( G, stableGenName, subgroupA, subgroupB )
{
}



int HNNExtOfORGroup::unusedGenerators( const Word& test, 
				       VectorOf<Word>& toNewGens,
				       VectorOf<Word>& toOldGens ) const
{
  SetOf<Generator> realGens;
  
  Word relator = theBasisGroup->relator();
  for( int i = relator.length()-1; i >= 0; --i )
    realGens |= Generator(abs(ord(relator[i])));
  
  for( int S = A; S <= B; ++S ) {
    VectorOf<Word> gens = subgroups[S]->generators();
    for(int i = gens.length()-1; i >= 0; --i ) {
      Word wrd = gens[i];
      for( int j = wrd.length()-1; j >= 0; --j ) 
	realGens |= Generator(abs(ord(wrd[j])));
    }
  }

  for( int i = test.length()-1; i >= 0; --i )
    realGens |= Generator(abs(ord(test[i])));

  int allGensCount = theBasisGroup->numberOfGenerators() + 1;

  realGens |= Generator( allGensCount ); // add stableLetter also.

  if( realGens.cardinality() == allGensCount )
    return 0;

  toNewGens = VectorOf<Word>( allGensCount );
  toOldGens = VectorOf<Word>( realGens.cardinality() );
  int gen = 0;
  for( int i = 0; i < allGensCount; ++i ) {
    if( realGens.contains( Generator(i+1)) ) {
      toOldGens[gen] = Word(Generator(i+1));
      toNewGens[i] =  Word(Generator( ++gen ));
    }
  }
  return  allGensCount - realGens.cardinality();
}

Trichotomy HNNExtOfORGroup::wordProblem( const Word& w ) const
{
  ProductOfRelatorConjugates dummy;
  const bool keepDetails = true;
  return wordProblem( w, !keepDetails, dummy );
} 

Trichotomy HNNExtOfORGroup::wordProblem( const Word& test, bool keepDetails,
  ProductOfRelatorConjugates& productOfRelatorConjugates ) const
{
  Word w = test.freelyReduce();

  if( w.length() == 0 ) {
    productOfRelatorConjugates = ProductOfRelatorConjugates();
    return yes;
  }

#ifdef DEBUG_HNN_ORGROUP
  Margin margin;
  margin.set( globalMargin );
#endif

  VectorOf<Word> toNewGens, toOldGens;
  
  if( unusedGenerators( w, toNewGens, toOldGens ) > 0 ) {

    VectorOf<Chars> oldNames = theBasisGroup->namesOfGenerators();
    VectorOf<Chars> newNames;
    for(int i = 0; i < theBasisGroup->numberOfGenerators(); ++i ) {
      if( toNewGens[i].length() > 0 )
	newNames.append( oldNames[i] );
    }

    Word newRelator = theBasisGroup->relator().replaceGenerators( toNewGens );
    VectorOf<Word> gensA, gensB;    
    for( int i = 0; i < subgroups[A]->generators().length(); ++i ) {
      gensA.append( 
	subgroups[A]->generators()[i].replaceGenerators( toNewGens ) 
      );
    }

    for( int i = 0; i < subgroups[B]->generators().length(); ++i ) {
      gensB.append( 
        subgroups[B]->generators()[i].replaceGenerators( toNewGens ) 
      );
    }
    OneRelatorGroup G( newNames, newRelator );
    SubgroupOfOneRelatorGroup newA( G, gensA ), newB( G, gensB );
    HNNExtOfORGroup H_reduced( G, theNameOfStableGenerator, newA, newB );
    Word u = w.replaceGenerators( toNewGens );

#ifdef DEBUG_HNN_ORGROUP
    cerr << margin << " make reduced HHN-presentation with "
	 << toNewGens << endl
	 << margin << " old pres = ";
    DEBUG_PRINT( cerr, *this );
    cerr << endl << margin << " new pres = ";
    DEBUG_PRINT( cerr, H_reduced ); 
    cerr << endl
	 << margin << " old test = " << w << endl
	 << margin << " new test = " << u << endl;
#endif
    
    ProductOfRelatorConjugates prodDeco;
    Trichotomy answer = H_reduced.wordProblem( u, keepDetails, prodDeco );
    if( answer != yes ) { 
      productOfRelatorConjugates = ProductOfRelatorConjugates();
      return answer;
    }

    // Rewrite product decomposition into original generators of the group.
    for( int i = prodDeco.length()-1; i >= 0; --i ) {
      prodDeco[i].relator = 
	prodDeco[i].relator.replaceGenerators( toOldGens );
      prodDeco[i].conjugator = 
	prodDeco[i].conjugator.replaceGenerators( toOldGens );
    }
    productOfRelatorConjugates = prodDeco;
    return yes;
  }

#ifdef DEBUG_HNN_ORGROUP
  static int level = 0;
  cerr << margin << ++level << "**** decompose a word " << w << endl;
#endif

  ProductOfRelatorConjugates prodDeco;
  VectorOf<Word> reducedDeco = 
    reducedDecompositionOf( w, keepDetails, prodDeco );

#ifdef DEBUG_HNN_ORGROUP
  FPGroup G = getPresentation();
  cerr << margin << level 
       << " **** HNNExtOfORGroup::wordProblem( const Word& test ): " << endl
       << margin << "   HNN= ";
  DEBUG_PRINT( cerr, *this );
  cerr << endl << margin << ", test (w)  = "; 
  DEBUG_PRINT_WORD( cerr, G, test ); 
  cerr << endl << margin << ", reducedDeco = ";
  DEBUG_PRINT_VECTOR_WORDS( cerr, G, reducedDeco );
  cerr << endl << endl;
#endif

  if( reducedDeco.length() > 1 ) {

#ifdef DEBUG_HNN_ORGROUP
    --level;
#endif
    productOfRelatorConjugates = ProductOfRelatorConjugates();
    return no;
  }

  ProductOfRelatorConjugates bprod;
  Trichotomy answer = 
    theBasisGroup->wordProblem( reducedDeco[0], keepDetails, bprod );

#ifdef DEBUG_HNN_ORGROUP
  --level;
#endif

  if( answer == no ) {
    productOfRelatorConjugates = ProductOfRelatorConjugates();
    return answer;
  }

  productOfRelatorConjugates = concatenate(bprod,prodDeco);

#ifdef DEBUG_HNN_ORGROUP
  cerr << margin << "wordProblem: productOfRelatorConjugates: " << endl
       << margin << productOfRelatorConjugates << endl;
#endif
  return answer;
}


ProductOfRelatorConjugates HNNExtOfORGroupGeneric::mappingDecompositionOf( 
  const NumberOfSubgroup& S, const Word& w, 
  const Word& wInSBasis, const Word& ctail ) const
{
  Word tail = ctail;
  ListOf<RelatorConjugate> listOfRelatorConjugates;

  ProductOfRelatorConjugates id = 
    ((SubgroupOfOneRelatorGroup&)subgroup(S))
     .getIdentityProductDecomposition( w );

  Word te = ( S == B ? stableLetter() : inv(stableLetter()) );
  for( int i = id.length()-1; i >= 0; --i ) {
    listOfRelatorConjugates.prepend( 
       RelatorConjugate( id[i].relator, id[i].conjugator * te * tail)
    );
  }
  
  Generator stableGen = stableLetter();
  for( int k = wInSBasis.length()-1; k >= 0; --k ) {
    int sgen_ord = ord(wInSBasis[k]); 
    int sgen = abs(sgen_ord)-1;
    Word agen = subgroup(A).generators()[sgen];
    Word bgen = subgroup(B).generators()[sgen];
    Word relator = stableLetter() * agen * inv(stableLetter())* bgen.inverse();
    RelatorConjugate rc;
    if( S == A ) {
      if( sgen > 0 ) {
	// r = t a T B 
	//   t a T (tail) => t a T B b (tail) =>
	//  => r b (tail) => b (tail) r^(b tail)
	rc.relator = relator;
	tail = bgen * tail;
	rc.conjugator = tail;
      }
      else {
	// R = b t A T
	//   t A T (tail) => B b t A T (tail) => 
	//  => B R (tail) => B (tail) R^(tail)
	rc.relator = relator.inverse();
	rc.conjugator = tail;
	tail = bgen.inverse() * tail;
      }
    }
    else if( S == B ) {
      if( sgen > 0 ) {
	// R = b t A T
	//   T b t (tail) => T (b t A T) t a (tail) =>
	// => T R t a (tail) => a (tail) R^(t a tail)
	rc.relator = relator.inverse();
	tail = agen * tail;
	rc.conjugator = stableLetter() * tail;
      }
      else {
	// r = t a T B
	//   T B t (tail) => A T (t a T B) t (tail) =>
	// => A T r t (tail) => A (tail) r^(t tail)
	rc.relator = relator;
	rc.conjugator = stableLetter() * tail;
	tail = agen.inverse() * tail;
      }
    }
    
    listOfRelatorConjugates.prepend( rc );

  }

#ifdef DEBUG_HNN_ORGROUP
  Margin margin;
  margin.set( globalMargin );
  cerr << margin << "Made mappingFromSubgroup:" << S << " -> " << 1-S << endl
       << margin << "wInSBasis = " << wInSBasis << endl
       << margin << "ctail = " << ctail << endl
       << margin << "RCs = " << listOfRelatorConjugates << endl;
#endif

  return makeVectorOf( listOfRelatorConjugates );
}



VectorOf<Word> HNNExtOfORGroup::reducedDecompositionOf( const Word& w,
  bool keepDetails, ProductOfRelatorConjugates& productOfRelatorConjugates ) 
  const
{
#ifdef DEBUG_HNN_ORGROUP
  Margin margin;
  margin.set( globalMargin );
#endif

  VectorOf<Word> deco = decompositionOf( w.freelyReduce() );
  int numOfGroupGens =  theBasisGroup->numberOfGenerators();
  AbelianWord ab(numOfGroupGens, theBasisGroup->relator() );
  VectorOf<int> abelianRelator( numOfGroupGens );
  for(int i = ab.numberOfGenerators()-1; i >= 0; --i )
    abelianRelator[i] = ab[i].as_long();

  VectorOf<bool> falseVector( numOfGroupGens );
  for( int i = numOfGroupGens-1; i >= 0; --i )
    falseVector[i] = false;

  VectorOf<bool> subgroupGens[2];
  for( int S = 0; S < 2; ++S ) {
    subgroupGens[S] = falseVector;
    const VectorOf<Word> subWords =  subgroups[S]->generators();
    for( int i = subWords.length()-1; i >= 0; --i ) {
      int gen = abs(ord(subWords[i][0]));
      subgroupGens[S][gen-1] = true;
    }
  }

  //  for( int component = 2; component < deco.length()-2; component += 2 ) {
  int lastEvenIndex =  deco.length() - 1;
  for( int component = lastEvenIndex - 2; component > 1; component -= 2 ) {

    lastEvenIndex =  deco.length() - 1;
    if( component < lastEvenIndex && suspectPinch( component, deco ) ) {

#ifdef DEBUG_HNN_ORGROUP
      cerr << margin << "* Trying subgroup containment for : " << endl
	   << margin << "  HNN = ";
      DEBUG_PRINT( cerr, *this );
      cerr << endl
	   << margin << "  deco = " << deco << endl
	   << margin << "  deco[current] = " << deco[component-1] << " . "
	   << deco[component] << " . " << deco[component+1] << endl;
#endif
      
      PinchStruct pinch = formPinch(component, deco);

      NumberOfSubgroup S = ( pinch.type == UP ? A : B );

      if( subgroup(S).contains( pinch.word ) ) {
	
	Word pinchInBasis = 
	  subgroup(S).rewriteFromGroupToSubgroupGenerators( pinch.word );
	
#ifdef DEBUG_HNN_ORGROUP
	cerr << margin << "Subgroup[" << S << "] = " 
	     << subgroup(S).generators() << endl
	     << margin << " deco[component] in subgroup basis is "
	     << pinchInBasis << endl;
#endif

	Word newPinch = mappingFromSubgroup( S, pinchInBasis );

	if( keepDetails ) {
	  VectorOf<Word> c = deco;
	  c.shrink( component+2, deco.length()-component-2 );
	  Word tail = compose(c);
	  
	  productOfRelatorConjugates = 
	   concatenate(mappingDecompositionOf(S,pinch.word,pinchInBasis,tail), 
		       productOfRelatorConjugates 
		       );

#ifdef DEBUG_HNN_ORGROUP
	  cerr << margin << "Deco id = " << productOfRelatorConjugates << endl;
#endif	
	}

#ifdef DEBUG_HNN_ORGROUP
	cerr << margin << "  YES! new word is " << newPinch << endl;
#endif	

	// merging pinch with left and right components which exist always.
	
	newPinch = 
	  (deco[component-2] * newPinch * deco[ component+2 ]).freelyReduce();
	deco[ component-2 ] = newPinch;

#ifdef DEBUG_HNN_ORGROUP
	cerr << margin
	     << "  merging with left and right compinents gives us "
	     << newPinch << endl;
#endif
	
	// remove 4 old  components from decomposition.
	int decoLen = deco.length();
	for( int pos = component + 3; pos < decoLen; ++pos ) 
	  deco[pos-4] = deco[pos];
	deco.shrink( decoLen - 4 );

	//component -= 4;
      } // if the current pinch is proper.
      else {
#ifdef DEBUG_HNN_ORGROUP
	cerr << margin << "   Containment problem says NO for this pinch."
	     << endl;
#endif
      }
    } // if( suspectPinch())
  } // for( component=..;..;..)

  return deco;
}


////////////////////////////////////////////////////////////////////
//
//
// Class HNNExtOfORGroupWithTrosion
//
//
////////////////////////////////////////////////////////////////////


HNNExtOfORGroupWithTorsion::HNNExtOfORGroupWithTorsion( 
  const OneRelatorGroupWithTorsion& G,
  const Chars& stableGenName,
  const SubgroupOfORGroupWithTorsion& A,
  const SubgroupOfORGroupWithTorsion& B )
  : HNNExtOfORGroupGeneric( G, stableGenName, A, B )
{
}

Trichotomy HNNExtOfORGroupWithTorsion::wordProblem( const Word& w ) const 
{
  VectorOf<Word> deco = reducedDecompositionOf( w );

  if( deco.length() > 1 )
    return no;

  return basisGroup().wordProblem( deco[0] );
}


Trichotomy HNNExtOfORGroupWithTorsion::conjugacyProblem( 
  const Word& u, const Word& v, Word& conjugator ) const
{
  conjugator = Word();

  OneRelatorGroupWithTorsion& G = (OneRelatorGroupWithTorsion&)basisGroup();

  if(  subgroup(A).generators().length() == 0 ) 
    return G.conjugacyProblem( u, v, conjugator );

  // First checks the length equality of cyclically reduced forms 
  // of the elements.

  Word uConj, vConj;
  const VectorOf<Word> uDeco = cyclicallyReducedDecompositionOf(u, uConj);
  const VectorOf<Word> vDeco = cyclicallyReducedDecompositionOf(v, vConj);

  if( uDeco.length() != vDeco.length() ) {
    return no;
  }

  Trichotomy answer = dontknow;

  // The two non-trivial cases depending on words length.
  switch( uDeco.length() ) {
  case 0:
    answer = yes;
    break;
  case 1:
    answer = G.conjugacyProblem( uDeco[0], vDeco[0], conjugator);
    break;
  default:
    answer = conjugacyProblem_cyclicallyReduced( uDeco, vDeco, conjugator);
  }

  if( answer == yes ) {
    conjugator = (uConj.inverse() * conjugator * vConj).freelyReduce();
    // 98/07/13 conjugator =  (uConj * conjugator * vConj.inverse()).freelyReduce();
  }

  return answer;
}

Trichotomy HNNExtOfORGroupWithTorsion::conjugacyProblem_cyclicallyReduced(
  const VectorOf<Word>& uDeco, const VectorOf<Word>& vDeco, 
  Word& conjugator ) const
{

#ifdef DEBUG_HNN_ORGROUP
  Margin margin;
  margin.set( globalMargin );

  cerr << margin << "Solve the reduced conjugacy problem. " << endl
       << margin << "G = " << *this << endl
       << margin << "uDeco = " << uDeco << endl
       << margin << "vDeco = " << vDeco << endl;
#endif
    
  
  conjugator = Word();

  CONDITION( WARNING, vDeco.length() % 2 == 0 && uDeco.length() % 2 == 0 );

  int decLen = uDeco.length();
  Word u = compose( uDeco );

  for( int shift = 0; shift < decLen; shift += 2 ) {

    VectorOf<Word> vPerm = cyclicallyPermute( vDeco, shift );

    // Check whether uDec and vPerm have the same powers of stable letters.

    bool sameStableLetters = true;
    for( int i = 1; i < decLen; i += 2 ) {
      if( uDeco[i] != vPerm[i] ) {
	sameStableLetters = false;
	break;
      }
    }
    if( sameStableLetters ) {
      // Check whether uDeco and vPerm are conjugated by element of
      // amalgamated subgroup.

      // u = u1 t^e1 ... uK t^eK
      // v' = v1 t^e1 ... vK t^eK
      // u^c = v', where c belongs to an amalgamated subgroup S
      //   c^-1 u c v^-1 = 1
      //
      // c^-1 u1 t^e1 ... uK t^eK c t^-eK vK^-1 .. t^-e1 v1^-1 = 1
      //
      // c1 = t^eK c t^-eK
      //
      // therefore   uK c1 vK^-1 = d1
      // where 
      //   SC = ( sign(eK) == -1 ? A : B )
      //   SD = ( sign(eK-1) == 1 ? A : B )


      const NumberOfSubgroup C  = ( ord(uDeco[decLen-1][0]) > 0 ? A : B );
      const NumberOfSubgroup C1 = ( ord(uDeco[decLen-1][0]) < 0 ? A : B );
      const NumberOfSubgroup D1 = ( decLen <= 2 ? 
				    NumberOfSubgroup(1-C1) :
				    ( ord(uDeco[decLen-3][0]) > 0 ? A : B )
				  );

      SubgroupOfORGroupWithTorsion
	SC1( (OneRelatorGroupWithTorsion&)basisGroup(), 
	     subgroup(C1).generators()
	     );

      SubgroupOfORGroupWithTorsion
	SD1( (OneRelatorGroupWithTorsion&)basisGroup(), 
	     subgroup(D1).generators()
	     );
      
      const Word uK = uDeco[ decLen-2 ];
      const Word vKinv = vPerm[ decLen - 2].inverse();

      Word c1, d1;
      HNNDoubleCoset D( SC1, SD1 );
      if( D.solve( uK, c1, vKinv, d1 ) == yes ) {
	// There is a solution of the equation. The solution is unique
	// because SC1 and SD1 are malnormal subgroups ( B.B.Newmann, ????? )
	
	// Restore c from c1. 
	Word c = 
	  SC1.rewriteFromGroupToSubgroupGenerators(c1).replaceGenerators(
              subgroup(C).generators() 
	  ); //.inverse();

	Word u1 = compose( uDeco ).conjugateBy( c );
	Word v1 = compose( vPerm );

#ifdef DEBUG_HNN_ORGROUP
	cerr << margin << " c1 = " << c1 << ", u1 = " << u1 << uDeco 
	     << ", v1 = " << v1 << endl
	     << margin << " u1 *v1^-1 = " << u1 * v1.inverse() << endl;
#endif

	if( wordProblem( u1 * v1.inverse() ) == yes ) {
	  VectorOf<Word> vConjDeco = vDeco;
	  vConjDeco.shrink( 0, shift) ; // , decLen-shift );
	  Word vConj = compose( vConjDeco );
	  conjugator = ( c * vConj.inverse() ).freelyReduce();
	  return yes;
	}
      }
    }
  } // for( shift = 0; ... )
  return no;
}

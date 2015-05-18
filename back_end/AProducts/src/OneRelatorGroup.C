// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implemenatation of class OneRelatorGroup.
//
// Principal Author: Dmitry Pechkin
//
// Status:
//
// Revision History:
//
// TO DO:
//


#include "OneRelatorGroup.h"
#include "SubgroupOfOneRelatorGroup.h"
#include "HNNExtOfORGroup.h"
#include "MagnusBreakdown.h"
#include "ShortenByRelators2.h"
#include "PresentationParser.h"
#include "GeneralWhitehead.h"
#include <stdio.h>
#include "TupleEnumerator.h"
#include "AP-fixups.h"

#define DEBUG_ORGROUP

#ifdef DEBUG_ORGROUP
#include "Margin.h"
#endif

OneRelatorGroupRep::OneRelatorGroupRep(const VectorOf<Chars>& gennames, 
				       const Word& relator )
  : FGGroupRep( gennames ),
    theRelator( relator )
{
}

OneRelatorGroupRep::OneRelatorGroupRep( int ngens, const Word& relator )
  : FGGroupRep( ngens ),
    theRelator( relator )
{
  Chars x = "x";
  char number[20];

  theNamesOfGenerators = VectorOf<Chars>( ngens );
  for( int i = 0; i < ngens; ++i ) {
    sprintf(number, "%d", i+1);
    theNamesOfGenerators[i] = x + number; 
  }
}

OneRelatorGroupRep::OneRelatorGroupRep( const FPGroup& G )
  : FGGroupRep( G.namesOfGenerators() )
{
  SetOf<Word> relators = G.getRelators();

#if SAFETY > 0
  if( relators.cardinality() != 1 ) 
    error("Constructing OneRelatorGroupRepRep(FPGroup& G) with "
	  "non one-relator FP group.");
#endif

  theRelator = SetIterator<Word>(relators).value();  
}


int OneRelatorGroupRep::order( ) const
{
  if( theNumberOfGenerators == 0 )
    return 1;
  
  if( theNumberOfGenerators == 1 ) 
    return theRelator.length();

  return -1; // infinite order.
}

Trichotomy OneRelatorGroupRep::isTrivial( ) const
{
  return order() == 1;
}

Trichotomy OneRelatorGroupRep::isFinite( ) const
{
  return order() != -1;
}

Trichotomy OneRelatorGroupRep::isInfinite( ) const
{
  return order() == -1;
}

Trichotomy OneRelatorGroupRep::isAbelian( ) const
{
  if( theNumberOfGenerators <= 1 )
    return true;
  if( theNumberOfGenerators > 2 )
    return false;

  // In case of 2 generators a and b, the group is abelian iff [a,b] = 1.
  return wordProblem( commutator(Word(Generator(1)),Word(Generator(2))) );
}

Trichotomy OneRelatorGroupRep::isFree( ) const
{
  if( theNumberOfGenerators == 0 || theRelator.length() == 0 )
    return true;
  GeneralWhitehead GW( theNumberOfGenerators );
  GW.startComputation(theRelator);
  while( !GW.continueComputation() );
  return GW.extendsToFreeBasis();
}

		     
void OneRelatorGroupRep::write( ostream& ostr ) const 
{
  FGGroupRep::write( ostr );
  ostr < ' ' < theRelator; 
}

void OneRelatorGroupRep::read( istream& istr ) 
{ 
  FGGroupRep::read( istr );
  istr > theRelator;
}

extern void  maximalRootInFreeGroup( const Word& w, Word& root, int& power );

Trichotomy OneRelatorGroupRep::wordProblem( const Word& w ) const  
{
  ProductOfRelatorConjugates dummy;
  const bool keepDetails = false;
  return wordProblem( w, keepDetails, dummy );
}


Trichotomy OneRelatorGroupRep::wordProblem( const Word& w, bool keepDetails, 
  ProductOfRelatorConjugates& productOfRelatorConjugates ) const
{
  
#ifdef DEBUG_ORGROUP
  Margin margin;
  margin.set( globalMargin );

  cerr << margin << " * OneRelatorGroupRep::wordProblem(w): " << endl
       << margin << "   theORGroup = ";
  DEBUG_PRINT( cerr, *this );
  cerr << ", w = ";
  DEBUG_PRINT_WORD( cerr, *this, w );
  cerr << endl << endl;
#endif

  Word root;
  int pwr;
  maximalRootInFreeGroup( theRelator, root, pwr );

  if( pwr > 1 || theNumberOfGenerators == 1 ) {
    // Easy case: r = (a b .. c)^k. Use Dehn's algorithm.
    ShortenByRelators2 S( theRelator );
    Word elt = 
      S.expressWordInConjugatesOfRelators( w, productOfRelatorConjugates );
    if( elt.length() == 0 ) 
      return yes;
    //productOfRelatorConjugates = ProductOfRelatorConjugates();
    return no;
  }

  Word uConj1;
  // u_old = u_new^uConj1
  Word u = cyclicallyReduce( w, uConj1);

  //  cerr << "w = " << w << endl 
  //       << "u = " << u << endl 
  //       << "uConj = " << uConj1 << endl;

  ProductOfRelatorConjugates cyclicProduct;
  Word cConjugator;
  const Word v = 
    cyclicallyShortenByRelators( u, theRelator, cConjugator, cyclicProduct );

  cerr << "v = " << v << endl 
       << "cConjugator = " << cConjugator << endl
       << "cyclicProduct = " << cyclicProduct << endl;

  const Word uConj2 = cConjugator * uConj1;

  // Make cyclically reduce once again.
  // u_old = u_new^uConjugator
  const Word y = cyclicallyReduce( v, cConjugator);
  const Word uConj3 = cConjugator * uConj2;

  if( y.length() == 0 ) {
    productOfRelatorConjugates = ::conjugateBy( cyclicProduct, uConj1 );
    return yes;
  }

  //@dp Here we can decompose the given group into a free product,
  //    but nothing does now.

  OneRelatorGroup G(theNamesOfGenerators, theRelator);
  MagnusBreakdown M( G );

  bool done = M.findHNNPresentation();
  
  if( !done )
    error("Internal error in OneRelatorGroupRep::wordProblem().");

#ifdef DEBUG_ORGROUP
  cerr << margin << "?? theRelator = " << theRelator << endl
       << margin << "?? M.stableGen = " << M.stableGenerator() 
       << ", has accomp " << M.hasAccompGenerator() << endl
       << margin << "?? M.toHNNPresentation() = " << M.toHNNPresentation()
       << endl
       << margin << "?? M.embeddingOfORGroups() = " << M.embeddingOfORGroups()
       << endl
       << margin << "?? rewrite word : old " << y;
#endif

  Word rewrittenWord = 
    M.rewriteWordInNewGenerators( 
       M.embeddingOfORGroups().imageOf( y ).freelyReduce() 
    );

#ifdef DEBUG_ORGROUP
  cerr << ", new = " << rewrittenWord << endl;
#endif

  HNNExtOfORGroup H = M.getHNNPresentation(); 
  ProductOfRelatorConjugates hnnDeco;
  Trichotomy answer = H.wordProblem( rewrittenWord, keepDetails, hnnDeco );
  hnnDeco = liftUpProduct( hnnDeco, M, G );

  productOfRelatorConjugates =  
    concatenate(
		::conjugateBy( hnnDeco, uConj3 ),
		::conjugateBy( cyclicProduct, uConj1) 
		);

  return answer;
}


void OneRelatorGroupRep::printOn( ostream& ostr ) const 
{
  ostr << "< ";
  printGenerators( ostr );
  ostr << " ; ";
  printWord( ostr, theRelator );
  ostr << " >";
}

GroupRep* OneRelatorGroupRep::readFrom( istream& istr, Chars& errMesg ) const
{
  PresentationParser P( istr );
  FPGroupRep *rep = P.parseFPGroup( errMesg );
  if( errMesg.length() > 0 )
    return 0;

  if( rep->relators.cardinality() != 1 ) {
    delete rep;
    errMesg = "{0} { Only one relator expected.} ";
    return 0;
  }

  Word relator = SetIterator<Word>( rep->relators ).value();
  VectorOf<Chars> names = rep->theNamesOfGenerators;
  delete rep;

  return new OneRelatorGroupRep( names, relator );
}


//------------------------ OneRelatorGroupRep ----------------------//
//------------------------- Static members -------------------------//
 
const Type OneRelatorGroupRep::theOneRelatorGroupType = Type( Type::unique() );

EnumeratorOfConsequences::EnumeratorOfConsequences( const OneRelatorGroup& G )
  : theCurrentWordNumber( 0 ),
    theLastComputedWordNumber( 0 ),
    theCurrentWord( ),
    theRelator( G.relator() ),
    theGroup( FreeGroup( G.namesOfGenerators() ) )
{
}

void EnumeratorOfConsequences::generate( ) const 
{
  // The state of the object EnumeratorOfConsequences is determined by 
  // the condition theCurrentWordNumber == theLastComputedNumber;

  if( theLastComputedWordNumber != theCurrentWordNumber ) {

    EnumeratorOfConsequences* This = (EnumeratorOfConsequences *)this;
    // break phisical constness under maintenance of logical one.
    
    Word element;
    VectorOf<Integer> tuple;
    ProductOfRelatorConjugates wtuple;
    bool repeated = false;

    // generate some consequence until this one is not trivial in free group.
    do {
      if( repeated )
	++This->theCurrentWordNumber;
      tuple = CantorEnumeration::decodeTuple( theCurrentWordNumber );
      int n = tuple.length();
      wtuple = ProductOfRelatorConjugates( n );
      for( int i = 0; i < n; ++i ) {
	Word conjugator = theGroup.getN_thElement( tuple[i].as_long() );
	wtuple[i].relator = ( rand() % 2 ? theRelator : theRelator.inverse() );
	wtuple[i].conjugator = conjugator;
	element *= wtuple[i].relator.conjugateBy( conjugator );
      } 
      repeated = true;
      element = element.freelyReduce();
    } while( element.length() == 0 );

    // store generated element for futher quick access.
    This->theCurrentProduct = wtuple;
    This->theCurrentWord = element;
    This->theLastComputedWordNumber = theCurrentWordNumber;

  }

}

ProductOfRelatorConjugates EnumeratorOfConsequences::tuple( ) const
{
  generate();
  return theCurrentProduct;
}

Word EnumeratorOfConsequences::word( ) const
{
  generate();
  return theCurrentWord;
}

bool EnumeratorOfConsequences::done( ) const
{ 
  return false;
}

void EnumeratorOfConsequences::advance( )
{
  ++theCurrentWordNumber;
}



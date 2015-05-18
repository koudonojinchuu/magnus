// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:
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

#include "AP-fixups.h"
#include "Associations.h"
#include "ShortenByRelators2.h"
#include "conversions.h"

//#define DEBUG_AP_FIXUPS

void maximalRootInFreeGroup(const Word& w, Word& root, int& power)
{
  Word u = w.freelyReduce();
  root = u;
  power = 1;
 
  if( u.length() == 0 ) return;

  int i, j;
  for(i=0, j=u.length()-1; i < j && u[i] == inv(u[j]); ++i, --j)
    /* empty body*/;

  Word uConj = u.terminalSegment(i);
  u = u.subword(i, j+1);
 
  power = maximalRoot(u);
  root = u.initialSegment( u.length() / power ).conjugateBy(uConj);
}

Trichotomy conjugacyProblem( const FreeGroup& G, const Word& u,
  const Word& v, Word& conjugator )
{
  conjugator = Word();
  
  Word uConj, vConj;
  Word u1 = cyclicallyReduce(u, uConj);
  Word v1 = cyclicallyReduce(v, vConj);
  
  int len = u1.length();
  
  if( len != v1.length() ) return no;
  
  if( len == 0) return yes;
  
  for(int i = 0; i < len; i++) {
    if(u1 == v1.cyclicallyPermute(i)) {
      // Select shortest form.
      if( i > len-i )
	conjugator = u1.initialSegment(len-i);
      else
	conjugator = u1.terminalSegment(i).inverse();
      conjugator = (uConj.inverse() * conjugator * vConj).freelyReduce();
      return yes;
    }
  }
  return no;
}


VectorOf<int> exponentSum(const Word& w)
{
  Generator maxgen = w.maxOccurringGenerator();
  VectorOf<int> exponents(ord(maxgen));
  for(int i = ord(maxgen); i > 0; --i)
    exponents[i-1] = w.exponentSum(Generator(i));
  return exponents;
}

// Returns cyclically reduced form of w such that w = answer^conjugator.
// For better performance, one should rewrite Word::cyclicallyReduce
Word cyclicallyReduce(const Word& w, Word& conjugator) 
{
  Word wFreelyReduced = w.freelyReduce();
  Word wCyclicallyReduced = wFreelyReduced.cyclicallyReduce();
  int conjLen = (wFreelyReduced.length() - wCyclicallyReduced.length())/2;
  conjugator = wFreelyReduced.terminalSegment(conjLen);
  return wCyclicallyReduced;
}

int GCD2(int a, int b)
{
  a = abs(a);
  b = abs(b);

  if( a < b ) {
    int temp = a;
    a = b;
    b = temp;
  }
  while( b != 0 ) {
    int k = a % b;
    a = b;
    b = k;
  }
  return a;
}

// Performs the multiplication of the components of given vector
// and returns the product.

Word compose(const VectorOf<Word>& v)
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


// u = w^conjugator * r1^t1 * .. * rk^tk
// Where u is the given word, w -- returned value, 
// ri are some relators or their negatives,
// ti are some words.

Word cyclicallyShortenByRelators( 
  const Word& u, const SetOf<Word>& givenRelators, Word& conjugator,
  ProductOfRelatorConjugates& productOfRelatorConjugates )
{
  ListOf<RelatorConjugate> listOfRelatorConjugates;

  //  cerr << "Given relators = " << givenRelators << endl;

  ShortenByRelators2 S(givenRelators);
  Word w = u;
  bool wasShorten;
  do {
    wasShorten = false;
    Word t = w;
    for( int i = w.length(); i > 0; --i ) {
      ProductOfRelatorConjugates tmpRelatorConjugates;
      Word z = S.expressWordInConjugatesOfRelators( t, tmpRelatorConjugates );

#if defined(DEBUG_AP_FIXUPS)
      cerr << "t = " << t << ", z = " << z << endl
	   << "tmpRelatorConjugates = " << tmpRelatorConjugates << endl;
#endif

      if( z.length() != t.length() ) {
	// w = t^x, by shortening we obtain t = z * r^t1 * r^t2 * ... * r^tk.
	// Therefore w = u^x * r^(t1*x) * ... * r^(tk * x).
	int decompositionLength = tmpRelatorConjugates.length();
	if( i == w.length() ) {
	  // No cyclic permutation was done.
	  // This is a case of x=1  -->  w = t = z * r^t1 * r^t2 * ... * r^tk.
	  w = z;
	}
	else {
	  Word x = w.initialSegment( w.length() - i );
	  conjugator =  x.inverse() * conjugator;
	  w = z;
	  for( int k = 0; k < decompositionLength; k ++ ) {
	    Word tmp = tmpRelatorConjugates[k].conjugator * conjugator;
	    tmpRelatorConjugates[k].conjugator = tmp.freelyReduce();
	  }
	}
	for( int k = decompositionLength-1; k >= 0; --k ) {
	  listOfRelatorConjugates.prepend( tmpRelatorConjugates[k] );
	}
	wasShorten = true;

#if defined(DEBUG_AP_FIXUPS)
      cerr << "lst = " << listOfRelatorConjugates << endl;
#endif

	break;
      }
      t = t.cyclicallyPermute(1);
    }
  } while( wasShorten );

  productOfRelatorConjugates = makeVectorOf( listOfRelatorConjugates );
  return w;
}

ProductOfRelatorConjugates conjugateBy( 
  const ProductOfRelatorConjugates& product, const Word& conjugator )
{
  ProductOfRelatorConjugates result( product.length() );
  for( int i = 0; i < result.length(); ++i ) {
    result[i].relator = product[i].relator;
    result[i].conjugator = product[i].conjugator * conjugator;
  }
  return result;
}



////////////////////////////////////////////////////////////////////////////
//                                                                        //
//                                                                        //
//  Helper class:  DetailedReport                                         //
//                                                                        //
//                                                                        //
///////////////////////////////////////////////////////////////////////////

DetailedReport::DetailedReport( const FPGroup& group, const Chars fileName )
  : G(group), H(), theFile( fileName ), builtRelators(false), bHaveDetails(false)
{
  cerr << "debug point 5-1" << endl;
}

DetailedReport::DetailedReport( 
  const FPGroup& group, const VectorOf<Word>& subgroup, const Chars fileName )
  : G(group), H(subgroup), theFile( fileName ), builtRelators(false), 
    bHaveDetails(false)
{
}


void DetailedReport::printDehnTransformationDetails( const Word& w )
{
  ShortenByRelators2 S( G.getRelators() );
  ProductOfRelatorConjugates prodDeco;
  Word trivial = S.expressWordInConjugatesOfRelators( w, prodDeco );
  Word u = w;

  for( int i = prodDeco.length() - 1; i >= 0; --i ) {
    Word relator = prodDeco[i].relator;
    Word conjugator = prodDeco[i].conjugator;

    int shift = relator.agreementLength( conjugator );
    conjugator = conjugator.subword( shift, conjugator.length() );
    
    int tail = tailAgreementLength( u, conjugator );
    int bLen = conjugator.length() - tail;
    int aLen = relator.length() - bLen;

    theFile << "Using the relator ";
    theFile.setColor( titleColor );
    G.printWord( theFile, relator );
    theFile.setColor( mainColor );
    theFile << " : " << endl;
    if( u.length()-tail-aLen > 0 ) 
      G.printWord( theFile, u.initialSegment(u.length()-tail-aLen) );
    theFile << ' ';
    theFile.setColor( titleColor );
    G.printWord( theFile, u.subword(u.length()-tail-aLen, u.length()-tail) );
    theFile.setColor( mainColor );
    theFile << ' ';
    if( tail > 0 )
      G.printWord( theFile, u.terminalSegment( tail ) );
    theFile << " --> ";
    if( u.length()-tail-aLen > 0 ) 
      G.printWord( theFile, u.initialSegment(u.length()-tail-aLen) );
    if( bLen > 0 ) {
      theFile.setColor( titleColor );
      theFile << ' ';
      G.printWord( theFile, conjugator.initialSegment( bLen ) );
      theFile.setColor( mainColor );
    }
    theFile << ' ';
    if( tail )
      G.printWord( theFile, u.terminalSegment( tail ) );
    theFile << endl << endl;

    u =  u.initialSegment(u.length()-tail-aLen) 
      * conjugator.initialSegment( bLen )
      * u.terminalSegment( tail );

    if( u.freelyReduce().length() != u.length() ) {
      theFile << "Freely reducing :" << endl;
      G.printWord( theFile, u );
      theFile << " --> ";
      u = u.freelyReduce();
      G.printWord( theFile, u );
      theFile << endl << endl;
    }
  }

  if( u.length() == 0 ) 
    theFile << "Got the empty word.";
  else 
    theFile << "Cannot reduce the given word.";
  theFile << endl;

  theFile << end;
}


void DetailedReport::printHeader( const bool header )
{
  if( !header )
    return;

  theFile << "The group is given by the presentation: " << G << "\n";
  
  SetIterator<Word> I( G.getRelators() );
  for( int i = 0; !I.done(); I.next() ) {
    theFile << "\nr" << ++i << " = ";
    G.printWord( theFile, I.value() );
  }
  theFile << "\n\n";

  if( H.length() > 0 ) {

    theFile << "\nThe given subgroup generators are:\n";

    for( int s = 0; s < H.length(); ++s ) {
      theFile << "\nh" << s+1 << " = ";
      G.printWord( theFile, H[s] );
    }
    theFile << "\n\n";
  }
}


void DetailedReport::printTrivialWordDecomposition(  
  const ProductOfRelatorConjugates& deco )
{
  bHaveDetails = true;

  bool star = false;
  long pos = theFile.tellp();

  for( int i = 0; i < deco.length(); ++i ) {

    Word word = deco[i].relator;
    Word conjugator = deco[i].conjugator;

    theFile << ( star ? " * " : " " );
    star = true;

    if( theRelators.bound( word ) ) {
      theFile << "r" << theRelators.valueOf(word) + 1;
    }
    else if( theRelators.bound(word.inverse()) ) {
      theFile << "R" << theRelators.valueOf(word.inverse()) + 1;
    }
    else if( theSGenerators.bound( word ) ) {
      theFile << "h" << theSGenerators.valueOf(word) + 1;
    }
    else if( theSGenerators.bound(word.inverse()) ) {
      theFile << "H" << theSGenerators.valueOf(word.inverse()) + 1;
    }
    else {
      if( conjugator.length() )	theFile << "(";
      G.printWord( theFile, word );
      if( conjugator.length() )	theFile << ")";
    }

    if( conjugator.length() ) {
      theFile << "^(";
      G.printWord( theFile, conjugator );
      theFile << ")";
    }

    const bool OneConjugatorPerLine = true;
    if( OneConjugatorPerLine ) {
      theFile << '\n';
    }
    else {
      // Wrap lines which are longer than 75 characters.
      if( theFile.tellp() > pos + 75 ) {
	theFile << '\n';
	pos = theFile.tellp();
      }
    }

  }
  theFile << '\n' << endl;
}

void DetailedReport::buildRelators( ) 
{
  SetIterator<Word> I( G.getRelators() );
  for( int i = 0; !I.done(); I.next() ) {
    theRelators.bind( I.value(), i++ );
  }

  for( int i = 0; i < H.length(); ++i) {
    theSGenerators.bind( H[i], i+1 );
  }

  builtRelators = true;
}

void DetailedReport::printTrivialWordDetails(  
  const Word& w, const ProductOfRelatorConjugates& deco, bool header )
{
  printHeader( header );

  if( !builtRelators )
    buildRelators();

  theFile << "A word is given as ";
  G.printWord( theFile, w );
  theFile << "is trivial in the group. It can be expresses\n"
    "as a product of conjugates of the given defining relator(s)\n"
    "and its inverse(s) as follows, where each such conjugate is written\n" 
    "on a separate line:\n";

  printTrivialWordDecomposition( deco );
}


void DetailedReport::printNonTrivialWordDetails( 
  const Word& w, const Chars& expl, bool header )
{
  printHeader( header );

  theFile << "A word given as ";
  G.printWord( theFile, w );
  theFile << ".\nThe word is not trivial -- " << expl << '\n';
  theFile << flush;

  bHaveDetails = true;
}


void DetailedReport::printTrivialGeneratorDetails( 
  const Generator& g, const ProductOfRelatorConjugates& deco, 
  const bool header )
{
  printHeader( header );

  if( !builtRelators )
    buildRelators();

  theFile << "A generator ";
  G.printWord( theFile, g );
  theFile << " is trivial in the group. It can  be expressed\n"
    "as a product of conjugates of the given defining relator(s)\n"
    "and its inverse as follows, where each such conjugate is written\n"
    "on a separate line:\n";

  printTrivialWordDecomposition( deco );
}



void DetailedReport::printNonTrivialGeneratorDetails( 
  const Generator& g, const Chars& expl, bool header )
{
  printHeader( header );

  theFile << "A generator ";
  G.printWord( theFile, g );
  theFile << " is not trivial in the group.\n";
  theFile << flush;

  bHaveDetails = true;
}


void DetailedReport::printTrivialCommutatorDetails( 
  const Chars& comm, const ProductOfRelatorConjugates& deco, const bool header)
{
  printHeader( header );

  if( !builtRelators ) 
    buildRelators();

  theFile<< "A commutator " << comm << " is trivial. It can be expressed\n"
    "as a product of conjugates of the given defining relator(s)\n"
    "and its inverse(s)  as follows, where each such conjugate is written\n"
    "on a separate line:\n";

  printTrivialWordDecomposition( deco );
}

void DetailedReport::printNonTrivialCommutatorDetails( 
  const Chars& /* comm */, const Chars& explanation, const bool header )
{
  printHeader( header );

  theFile << explanation << end;
}


void DetailedReport::printSubgroupElement( 
  const Word& w, const ProductOfRelatorConjugates& deco, const bool header ) 
{
  printHeader( header );

  if( !builtRelators )
    buildRelators();

  theFile << "A word is given as ";
  G.printWord( theFile, w );
  theFile << " lies in the subgroup. It can be expressed\n"
    "as a product of subgroup generator(s) and a product of conjugates \n"
    "of the given defining relator(s) and its inverse(s) as follows,\n"
    "where each such conjugate is written on a separate line:\n";

  printTrivialWordDecomposition( deco );
}

// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the ShortenByRelators2 class.
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
// Special Notes:
//
//


#include "ShortenByRelators2.h"
#include "conversions.h"

#define DEBUG_SHORTEN2

#ifdef DEBUG_SHORTEN2
#  include "Margin.h"
#endif


//@db 2.91 these functions are defined at the end of this file 
//    (template instantiation)

template <> int SetData<QuickAssociation<Word, int> >::hashElement(const QuickAssociation<Word, int>& t) const;

template <> int SetData<long>::hashElement( const long& t ) const;

//#if GCC_VERSION < 29600 || defined(BSD)
//int SetData<QuickAssociation<Word, int> >::hashElement( 
//  const QuickAssociation<Word, int>& t) const;
//
//int SetData<long>::hashElement( const long& t ) const;
//#endif
//

//@db 


int cmpInt(const void* ptr1, const void* ptr2);

ShortenByRelators2::ShortenByRelators2(const SetOf<Word>& relators) 
  : theRelators( makeVectorOf(relators) ), base( relators.cardinality()+1 )
{
  int numberOfRelators = relators.cardinality();
  SetOf<long> relLens;

  for( int curRel = 0; curRel < numberOfRelators; ++curRel ) {
    Word relator = theRelators[ curRel ];
    relLens |= relator.length();
    int halfRelatorLen = relator.length()/2 + 1;
    for( int power = 0; power < 2; ++power ) { 
      Word longRelator = relator * relator;
      for( int position = 0; position < relator.length(); ++position ) {
	Word piece = longRelator.subword(position, position+halfRelatorLen);
	if( !relatorsPieces.bound( piece ) ) {
	  // index is encoded pair (position, currentRelator)
	  int index = position * base + (curRel+1);
	  if( power == 1 ) 
	    index = -index; 
	  relatorsPieces.bind( piece, index );
	}
      }
      relator = relator.inverse();
    }    
  }

  int N = relLens.cardinality();
  int *tmpbuf = new int[N];
  SetIterator< long > I(relLens);
  for( int i = 0; i < N; ++i ) {
    tmpbuf[i] = (int)I.value();
    I.next();
  }
  qsort( tmpbuf, N, sizeof(tmpbuf[0]), cmpInt );
  
  relatorsLengths = VectorOf<int>( N );
  for( int i = 0; i < N;++i ) {
    relatorsLengths[i] = tmpbuf[i];
  }
}

Word ShortenByRelators2::getShortenWord(const Word& w) const 
{
  ProductOfRelatorConjugates dummy;
  return expressWordInConjugatesOfRelators( w, dummy );
}

int ShortenByRelators2::compare( const Word& w1, const Word& w2 ) const
{
  int result;

  if( w1.length() > w2.length() )
    result = 1;
  else if( w1.length() < w2.length() )
    result = -1;
  else 
    result = 0;

  return result;
}


Word ShortenByRelators2::expressWordInConjugatesOfRelators(const Word& v, 
    ProductOfRelatorConjugates& productOfRelatorConjugates
) const
{
  Word w = v.freelyReduce();
  if( w.length() == 0 || theRelators.length() == 0 ) {
    productOfRelatorConjugates = ProductOfRelatorConjugates();
    return w;
  }

  // Reserve some extra space for relsults. On exit these will be truncated.
  productOfRelatorConjugates = ProductOfRelatorConjugates( w.length() );

  // We fill the decomposition vectors (relators,conjugates) from the ends.
  int right = w.length();
  int left = 0;

  int maxRelLenCount = relatorsLengths.length() - 1;
  bool wasShortened = true;
  while( wasShortened ) {
    wasShortened = false;

#ifdef DEBUG_SHORTEN2
    Margin margin;
    margin.set( globalMargin );

    cerr << margin << "Shortening the word w = " << w << endl;
#endif

    // shrinks the set of acceptable relators which length is not greater
    // than twice length of the word `result'

    int wLen = w.length();
    while( maxRelLenCount > 0 && relatorsLengths[maxRelLenCount] > 2 * wLen ) {
      --maxRelLenCount;
    }

    // Check for shortening only pieces of w with the length of half length 
    // of some relator.
    for( int relLenCount = maxRelLenCount; relLenCount >= 0 && !wasShortened; 
	 --relLenCount ) {
      int relatorLength = relatorsLengths[ relLenCount ];
      int halfRelLen = relatorLength / 2 + 1;

      for( int pos = halfRelLen; pos <= w.length() && !wasShortened ; ++pos ) {
	Word piece = w.subword( pos-halfRelLen, pos );

	if( relatorsPieces.bound( piece ) ) {
	  // we found a subword that has long common piece with some cyclically
	  // permutation of a relator R. Let C is the conjugator of R.
	  
	  // extract info for the shifted relator.
	  int index = relatorsPieces.valueOf( piece );
	  int relNumber = abs(index) % base - 1;
	  int relShift = abs(index) / base; //it's a shift of origial rel.
	  Word r = theRelators[relNumber];
	  if( index < 0 ) r = r.inverse();
	  Word rx = r.cyclicallyPermute( relShift ); // shifted relator r'.

	  int pieceLen = // length of maximal common piece of r' and w.
	    rx.agreementLength( w.subword(pos-halfRelLen, w.length() ) );

	  // this is maximal common piece. The `piece' above can be not
	  // maximal but only a half of relator length.
	  piece = w.subword( pos-halfRelLen, pos-halfRelLen+pieceLen);

#ifdef DEBUG_SHORTEN2
	  cerr << margin << "Found: piece = " << piece 
	       << ", index = " << index << ", relNumber = " << relNumber 
	       << ", relShift = " << relShift << endl;
#endif

	  // w = a u b, where u is initial segment of r' and |u| >= |r'|/2.
	  Word a = w.initialSegment( pos-halfRelLen );
	  Word b = w.subword( pos-halfRelLen+pieceLen, w.length() );
	  //   u = `piece';

	  Word x, y, z, wShortened, conjugator;
	  // There are 2 cases:
	  if( pieceLen  <= r.length() - relShift ) {
	    // Case 1. The maximal common piece does not intersect with C.
	    // r = xyz, r' = r^x = yzx, w = ayb, where |x|+|z| <= |r|/2 <= |y|
	    // w = a y b = aX (xyz) Zb = aX r Zb = aXZb r^(Zb)
	    x = r.initialSegment( relShift );
	    y = rx.initialSegment( pieceLen );
	    z = r.terminalSegment( r.length() - x.length() - y.length() );
	    wShortened = a * x.inverse() * z.inverse() * b;
	    conjugator = (z.inverse() * b).freelyReduce();
	  }
	  else { 
	    // Case 2. The maximal common piece intersects with C.
	    // r = xyz, r' = r^xy = zxy, w = azxb, where |y| <= |r|/2 <= |z|+|x|
	    // w = a zx b = az x b = az (xyz) ZY b = az r ZYb = 
	    //   = azZYb r^(ZYb) = aYb r^(ZYb) 
	     y = rx.terminalSegment( r.length() - pieceLen );
	     z = r.subword( relShift, r.length() );
	     x = r.initialSegment( pieceLen - z.length() );
	     wShortened = a * y.inverse() * b;
	     conjugator = (z.inverse() * y.inverse() * b).freelyReduce();
	  }

	  // The shortened word is.
	  Word wx = wShortened.freelyReduce();

#ifdef DEBUG_SHORTEN2
	  cerr << margin << "Found word can be shortened: w = " << w << '\n'
	       << margin 
	       << "a = " << a << ", u = " << piece << ", b = " << b << '\n'
	       << margin << "r = " << r << ", r' = " << rx << '\n'
	       << margin 
	       << "x = " << x << ", y = " << y << ", z = " << z << '\n'
	       << margin << "Shortened w' = " << wx << endl;
#endif

	  if( compare( wx, w ) < 0 ) { // check minimality
	    // NOW always move conjugator to right.
	    w = wx;

	    if( right == 0 ) {
	      RelatorConjugate elt( r, conjugator );
	      productOfRelatorConjugates.prepend( elt );
	    }
	    else {
	      --right;
	      productOfRelatorConjugates[ right ].relator = r;
	      productOfRelatorConjugates[ right ].conjugator = conjugator;
	    }
	    wasShortened = true;

#ifdef DEBUG_SHORTEN2
	    {
	      Word v_check = w;
	      for(int j=right; j < productOfRelatorConjugates.length(); ++j) {
		v_check *= productOfRelatorConjugates[j].relator.conjugateBy( 
                             productOfRelatorConjugates[j].conjugator );
	      }
	      if( v_check.freelyReduce() != v.freelyReduce() )
		error("Internal error in ShortenByRelators2::"
		      "expressWordInConjugatesOfRelators");
	    }
#endif
	    
	  } // if minimal
	}
      }
    }
  }
  
  int numberOfConjugates = left + productOfRelatorConjugates.length() - right;
  int gap = right - left;
  for( int i = right; i < productOfRelatorConjugates.length(); ++i ) {
    productOfRelatorConjugates[ i-gap ]   = productOfRelatorConjugates[ i ];
  }

  productOfRelatorConjugates.shrink( numberOfConjugates );
  
  return w;
}

int cmpInt(const void* ptr1, const void* ptr2)
{
  int a = *(int *)ptr1;
  int b = *(int *)ptr2;
  if( a > b )
    return 1;
  else if( a < b )
    return -1;
  return 0;
}


//
// @dp  Some (temporary?!) hacks since gcc 2.7.2 doesn't do templates right:
//
template <> int SetData<QuickAssociation<Word, int> >::hashElement( 
  const QuickAssociation<Word, int>& t) const
{
  return t.hash();
}

template <> int SetData<long>::hashElement( const long& t ) const
{ return t; }

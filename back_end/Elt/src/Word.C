// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the Word class.
//
// Principal Author: Stephane Collart, Roger Needham
//
// Status: Useable.
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//

#include "Word.h"


Bool Word::operator < ( const Word& w ) const
{
  if ( length() < w.length() ) return 1;
  if ( length() > w.length() ) return 0;
  cGeneratorPtrType wp1 = look()->cFirst();
  cGeneratorPtrType wp2 = w.look()->cFirst();
  for (int i = 0; i < length(); i++)
	 if ( *wp1 != *wp2 ) {
		if ( *wp1 == -*wp2 ) return ( *wp2 < *wp1 );
		return ( abs(*wp1) < abs(*wp2) );
	 } else {
		wp1++;
		wp2++;
	 }
  return 0;
}


Word Word::nextInShortLex(int numberOfGens) const
{
  #if SAFETY > 0
  if ( numberOfGens <= 0 )
	 error("called Word::nextInShortLex with param <= 0");
  #endif
  // Need to increase length?
  cGeneratorPtrType tp = look()->cFirst();
  cGeneratorPtrType stop = look()->cLast();
  while ( tp <= stop && *tp == -numberOfGens ) ++tp;
  if ( tp > stop ) {
	 Word u( length() + 1 );
	 GeneratorPtrType up = u.change()->first();
	 stop = u.look()->cLast();
	 while ( up <= stop ) *up++ = 1;
	 return u;
  }

  Word w = *this;
  GeneratorPtrType wp = w.change()->last();
  stop = w.look()->cFirst();
  while ( wp >= stop ) {
	 if ( *wp == -numberOfGens )
		*wp-- = 1;
	 else {
		if ( *wp > 0 )
		  *wp = -*wp;
		else *wp = 1 - *wp;
		return w;
	 }
  }

  error("internal bug: fell out of Word::nextInShortLex");
}


Word Word::nextFreelyReduced(int numberOfGens) const
{
  #if SAFETY > 0
  if ( numberOfGens <= 0 )
    error("called Word::nextInShortLex with param <= 0");
  #endif

  cGeneratorPtrType tp = look()->cLast();
  cGeneratorPtrType fp = look()->cFirst();

  #if SAFETY > 1
  {
    cGeneratorPtrType temp = tp;
    while ( temp-- > fp )
      if ( *temp == -*(temp + 1) )
        error("invoked Word::nextFreelyReduced for a non-freely reduced word");
  }
  #endif

  // We increment words `odometer-style', where 1 -> -1 -> 2 -> -2, etc.
  // Let x be the highest generator, X it's inverse. Then incrementing
  // either goes from  X X . . . X, (n times) to 1 1 . . . 1 (n+1 times),
  // or from . . . y z X X . . . X, n >= 0 X's, z != X, to
  // . . . y ++z 1 1 . . . 1, n 1's.
  // If we start with a freely reduced word, then by incrementing it
  // we may create adjacent inverse generators: y and ++z, or ++z and 1.
  // After each increment we scan left-to-right for these, starting at y.
  // Now suppose we have . . . u v . . ., u == -v, and this is the leftmost.
  // Then if v != X the next word is . . . u ++v 1 1 . . . 1 if v != 1 and
  // v was not the rightmost letter, else . . . u -1 -1 . . . -1. Otherwise
  // v == X and we replace u with X to get . . . X 1 1 . . . 1, then scan
  // again just to the left of where u was.

  while ( tp >= fp && *tp == -numberOfGens ) --tp;
  if ( tp < fp ) {
    Word w( length() + 1 );
    GeneratorPtrType wp = w.change()->last();
    fp = w.look()->cFirst();
    while ( wp >= fp ) *wp-- = 1;
    return w;
  }

  // tp points to z. Increment z.
  Word w = *this;
  GeneratorPtrType wp = w.change()->first() + ( tp - fp ); // Note this *must*
  fp = w.look()->cFirst();                                 //  come before
  cGeneratorPtrType lp = w.look()->cLast();                //  these!
  if ( *wp > 0 ) *wp = -*wp; else *wp = 1 - *wp;
  GeneratorPtrType temp = wp;
  while ( ++temp <= lp ) *temp = 1;

  // Now scan for inverse pairs, left-to-right,
  // starting one to the left of *wp.
  if ( wp > fp ) --wp;
  while ( ((wp != lp) && (*wp == -*(wp + 1))) ||
          ((++wp != lp) && (*wp == -*(wp + 1))) ) {
    if ( *(wp + 1) != -numberOfGens ) {
      if ( (*++wp == 1) && (wp < lp) && (*(wp + 1) == 1) ) {
        while ( wp <= lp ) *wp++ = -1;
      } else {
        if ( *wp > 0 ) *wp = -*wp; else *wp = 1 - *wp;
        while ( ++wp <= lp ) *wp = 1;
      }
      break;
    } else {
      *wp = -numberOfGens;
      temp = wp;
      while ( ++temp <= lp ) *temp = 1;
      if ( wp > fp ) --wp;
    }
  }
  return w;
}


Word Word::nextCyclicallyReduced(int numberOfGens) const
{
  Word w = nextFreelyReduced(numberOfGens);
  if ( *(w.look()->cFirst()) == -*(w.look()->cLast()) )
    return w.nextFreelyReduced(numberOfGens);
  else
    return w;
}


Word Word::subword(const int i, const int j) const
{
  #if ( SAFETY > 0 )
    if ((j < i) || (i < 0) || (j > length()))
	   error("subword indices out of bounds");
  #endif

  Word w(j - i);
  GeneratorPtrType wp1 = w.change()->first();
  cGeneratorPtrType wp2 = look()->cFirst() + i;
  for (int k = i; k < j; k++) *wp1++ = *wp2++;
  return w;
}


Word Word::findAgreement(const Word& w) const
{
  int min_len = ( length() < w.length() ? length() : w.length() );
  int i = 0;
  cGeneratorPtrType wp1 = look()->cFirst();
  cGeneratorPtrType wp2 = w.look()->cFirst();
  while ( ( i < min_len ) && ( *wp1++ == *wp2++ ) ) i++;
  Word result(i);
  GeneratorPtrType wp = result.change()->first();
  wp1 = look()->cFirst();
  for (int j = 0; j < i; j++) *wp++ = *wp1++;
  return result;
}


int Word::agreementLength( const Word& w ) const
{ // @stc provisional implementation
  int res = 0;
  while (res < min(length(),w.length()) && (*this)[res] == w[res]) ++res;
  return res;
}


Word Word::shortenByRelator(const Word& r) const
{
  int halflen = (r.length() >> 1) + 1;
  cGeneratorPtrType start = look()->cFirst();
  cGeneratorPtrType stop = start + length() - halflen;
  cGeneratorPtrType first_r = r.look()->cFirst();
  cGeneratorPtrType last_r = r.look()->cLast();
  cGeneratorPtrType last_t = look()->cLast();

  // We match the largest prefix of r as possible with the subword of
  // *this beginning at start. Of course, start can't go past stop.
  // If the match has length at least halflen, we can replace and return.

  while ( start <= stop ) {
	 cGeneratorPtrType compare_t = start;
	 cGeneratorPtrType compare_r = first_r;
	 while ( compare_r <= last_r && compare_t <= last_t &&
				*compare_r == *compare_t ) {
		++compare_r;
		++compare_t;
	 }
	 // compare_[rt] now point just past the common subwords

	 if ( compare_r - first_r >=  halflen ) {
		GeneratorType *buffer = new GeneratorType[length()]; // Big enough
		GeneratorType *tp = buffer;
		cGeneratorPtrType copy = look()->cFirst();
		while ( copy < start ) *tp++ = *copy++;
		while ( last_r >= compare_r ) *tp++ = -*last_r--;
		while ( compare_t <= last_t ) *tp++ = *compare_t++;
		Word result(buffer, tp - buffer);
		delete buffer;
		return result;
	 }
	 else start++;
  }
  return *this;
}


int Word::numberOfOccurrences(const Generator& g) const
{
  int count = 0;
  cGeneratorPtrType wp = look()->cFirst();
  int ago = abs(ord(g));
  for (int j = 0; j < length(); j++)
	 if ( abs(*wp++) == ago ) count++;
  return count;
}


int Word::exponentSum(const Generator& g) const
{
  int count = 0;
  cGeneratorPtrType wp = look()->cFirst();
  int go = ord(g);
  for (int i = 0; i < length(); i++) {
	 if ( *wp == go )
		count++;
	 else if ( *wp == -go )
		count--;
	 wp++;
  }
  return count;
}


bool Word::allExponentSumsZero( ) const
{
  int count_vec_len = abs( ord( maxOccurringGenerator() ) );
  int count_vec[count_vec_len];
  for( int i = 0; i < count_vec_len; ++i ) count_vec[i] = 0;

  cGeneratorPtrType wp = look()->cFirst();
  for( int i = length(); i > 0; --i ) {
	 if ( *wp > 0 )
		++count_vec[ord( *wp ) - 1];
	 else
		--count_vec[abs( ord( *wp ) ) - 1];
	 wp++;
  }

  for( int i = 0; i < count_vec_len; ++i )
    if ( count_vec[i] != 0 ) return false;

  return true;
}
//@rn There is no test for this in test-Word.C.


bool Word::isProperPower( ) const
// by Dmitry Pechkin, Eugeny Paderin
// @stc temporary implementation, rescued from the Omsk randomMSCGroup
// stuff.
{
    Word w = this->cyclicallyReduce();
    int wLen = w.length();
    Word ww = w * w;
    for(int rootLen = 1; rootLen<wLen; rootLen++)
    {
        if( wLen % rootLen != 0 ) continue;
 
        for(int shift = 0; shift<rootLen; shift++)
        {
            Word root = ww.subword(shift, rootLen);
            Bool piecesAreEqual = TRUE;
            for(int pos = shift+rootLen; pos<wLen; pos += rootLen)
                if( root != ww.subword(pos, pos+rootLen) )
                {
                    piecesAreEqual = FALSE;
                    break;
                }
            if( piecesAreEqual ) return TRUE;
        }
    }
    return FALSE;
}


Generator Word::maxOccurringGenerator( ) const
{
  #if SAFETY > 0
    if (length() == 0) error("attempt to take Generator"
		" Word::maxOccurringGenerator() on trivial word.");
  #endif

  GeneratorType g = 0;
  cGeneratorPtrType wp = look()->cFirst();
  for (int i = 0; i < length(); i++) {
	 if ( abs(*wp) > g )
		g = abs(*wp);
	 wp++;
  }
  return g;
}


Word Word::replaceGeneratorWithWord(const Generator& g, const Word& w) const
{
  int count_g = numberOfOccurrences(g);
  if ( count_g == 0 ) return *this;

  Word result( length() + count_g * ( w.length() - 1 ) );

  int k;
  cGeneratorPtrType tp = look()->cFirst();
  GeneratorPtrType rp = result.change()->first();
  for (int i = 0; i < length(); i++) {
	 if ( abs(*tp) != g )
		*rp++ = *tp++;
	 else if ( *tp++ > 0 ) {
		cGeneratorPtrType wp = w.look()->cFirst();
		for (k = 0; k < w.length(); k++)
		  *rp++ = *wp++;
	 }
	 else {
		cGeneratorPtrType wp = w.look()->cLast();
		for (k = 0; k < w.length(); k++)
		  *rp++ = -*wp--;
	 }
  }
  return result;
}

Elt Word::replaceGenerators( const VectorOf<Elt>& eltvec ) const {
// @stc provisional implementation

  if (length() == 0) return *this;

  #if SAFETY > 0
	if (maxOccurringGenerator() > eltvec.length())
		error("not enough replacement Elts in"
			" Word::replaceGenerators(const VectorOf<Elt>&);");
  #endif

  int o = ord((*this)[0]);
  Elt res = eltvec[ abs(o) - 1 ];
  if (o < 0) res = res.inverse();

  int i = 0;
  while (++i < length()) {
	o = ord((*this)[i]);
	if (o < 0)
		res *= ::inverse(eltvec[-o - 1]);
		// @stc need to check why global inverse isn't found alone
	else
		res *= eltvec[o - 1];
  }

  return res;
}

/* @rn 12/18/94

Elt Word::replaceGenerators( const VectorOf<Word>& wrdvec ) const {
// @stc provisional implementation; see Word.h

  if (length() == 0) return *this;

  #if SAFETY > 0
	if (maxOccurringGenerator() > wrdvec.length())
		error("not enough replacement Elts in"
			" Word::replaceGenerators(const VectorOf<Word>&);");
  #endif

  int o = ord((*this)[0]);
  Elt res = wrdvec[ abs(o) - 1 ];
  if (o < 0) res = res.inverse();

  int i = 0;
  while (++i < length()) {
	o = ord((*this)[i]);
	if (o < 0)
		res *= ::inverse(wrdvec[-o - 1]);
		// @stc need to check why global inverse isn't found alone
	else
		res *= wrdvec[o - 1];
  }

  return res;
}
*/


Word Word::replaceGenerators( const VectorOf<Word>& wrdvec ) const
{
  // First find out how long the result will be.
  int len = 0;
  int i = length();
  while ( i-- ) len += wrdvec[ abs(ord((*this)[i])) - 1 ].length();

  VectorOf<Generator> temp(len); // Fill this in with the result.
  i = length();
  while ( i-- ) {
	 int o = ord((*this)[i]);
	 Word w = wrdvec[ abs(o) - 1 ];
	 //@rn Making this a const ref invokes the same 2.6.0 dtor bug
	 int j = w.length();
	 len -= j;
	 if ( o > 0 ) {
		while ( j-- ) temp[len + j] = w[j];
	 } else {
		int k = len;
		while ( j-- ) temp[k++] = -ord(w[j]);
	 }
  }
  return Word(temp);
}

Word Word::replaceSubword(const int i, const int j, const Word& w) const
{
  #if ( SAFETY > 0 )
    if ((j < i) || (i < 0) || (j > length()))
	   error("replaceSubword indices out of bounds");
  #endif

  Word result( length() - (j - i) + w.length() );
  GeneratorPtrType rp = result.change()->first();

  cGeneratorPtrType wp = look()->cFirst();
  for (int k = 0; k < i; k++) *rp++ = *wp++;

  wp = w.look()->cFirst();
  for( int k = 0; k < w.length(); k++ ) *rp++ = *wp++;

  wp = look()->cFirst() + j;
  for( int k = j; k < length(); k++ ) *rp++ = *wp++;

  return result;
}


Word Word::freelyReduce( ) const
{
	if ( length() < 2 ) return *this;

	GeneratorType *buffer = new GeneratorType[length()];
	GeneratorType *tp = buffer - 1;
	cGeneratorPtrType sp = look()->cFirst();
	cGeneratorPtrType stop = look()->cLast();
	while ( sp <= stop ) {
	  if ( tp < buffer ) *(++tp) = *sp++;
	  else if ( *tp == -*sp ) {
		 tp--;
		 sp++;
	  } else *(++tp) = *sp++;
	}

	// Now make word of length (tp - buffer + 1) from buffer.
	Word result(buffer, tp - buffer + 1);
	delete buffer;
	return result;
}


Word Word::cyclicallyReduce( ) const
{
	if ( length() < 2 ) return *this;

	// First freely reduce. Method call too expensive; do it directly.

	GeneratorType *buffer = new GeneratorType[length()];
	GeneratorType *tp = buffer - 1;
	cGeneratorPtrType sp = look()->cFirst();
	cGeneratorPtrType stop = look()->cLast();
	while ( sp <= stop ) {
	  if ( tp < buffer ) *(++tp) = *sp++;
	  else if ( *tp == -*sp ) {
		 tp--;
		 sp++;
	  } else *(++tp) = *sp++;
	}

	// Now burn candle at both ends.

	GeneratorType *b = buffer;
	GeneratorType *e = tp;
	while ( ( b < e ) && ( *b == -*e ) ) { b++; e--; }

	// Now make word of length (e - b + 1) from buffer starting at b.
	Word result(b, e - b + 1);
	delete buffer;
	return result;
}


Word Word::cyclicallyPermute(const int j) const
{
  int i = j;
  while ( i >= length() ) i = i - length();
  while ( i < 0 ) i = i + length();
  if ( i == 0 ) return *this;

  Word w(length());
  GeneratorPtrType tp = w.change()->first();
  cGeneratorPtrType sp = look()->cFirst() + i;
  cGeneratorPtrType stop = look()->cLast();
  while ( sp <= stop ) *tp++ = *sp++;
  sp = look()->cFirst();
  stop = look()->cFirst() + i;
  while ( sp < stop ) *tp++ = *sp++;
  return w;
}

//--------------------------- Word -----------------------------------//
//---------------- static member implementations ---------------------//


Word Word::wordByLexRank( int numGens, int lexRank ) {
// @stc provisional implementation

  int twiceNumGens = 2 * numGens;
  #if SAFETY > 0
	if (lexRank < 0) error(   "Negative short lex rank in "
	  "Word::wordByLexRank( int numGens, int lexRank )");
  #endif
  // now construct the list of generators corresponding to the
  // word with short lex rank lexRank recursively (significance growing
  // from left to right)
  ListOf<Generator> lg;
  int r;
  while (lexRank) {
	  lexRank--;
	  r = lexRank % twiceNumGens - numGens;
	  if (r < 0)
		  lg.append( inv(Generator( -r )) );
	  else
		  lg.append( Generator( r + 1 ) );
	  lexRank = lexRank / twiceNumGens;
  }
  return Word(lg); //@rn make new constr
}

//-------------- Word: associated global functions ----------------//


// @rn 12/16/94 This is here temporarily, until we can sort things out.
// The return value is the largest n such that w is syntactically equal
// to some prefix u, repeated n times, without free reduction.

int maximalRoot(const Word& w)
{
  int len = w.length();
  int halfLen = len / 2;
  
  for( int subwordLen = 1; subwordLen <= halfLen; ++subwordLen )
    if ( len % subwordLen == 0 ) {
      int i = subwordLen;
      while ( i < len ) {
        if ( w[i - subwordLen] != w[i] ) break;
        ++i;
      }
      if ( i == len ) return len / subwordLen;
    }
  
  return 1;
}


SetOf<Word>& closeUnderInverses(SetOf<Word>& S)
{
    SetIterator<Word> I(S);
    while ( !I.done() )
    {
        S |= I.value().inverse();
        I.next();
    }
	return S;
}
// by @db & @dp


SetOf<Word>& closeUnderCyclicPermutations(SetOf<Word>& S)
{
    SetIterator<Word> I(S);
    while ( !I.done() )
    {
        Word w = I.value();
        int i = w.length();
        while ( i-- )
        {
            w = w.cyclicallyPermute(1);
            S |= w;
        }
        I.next();
    }
	return S;
}
// by @db & @dp


int cancellationLambda( const SetOf<Word>& ss )
{
	int lambda = 0;
    SetIterator<Word> I(ss);
    while( !I.done() )
    {
        Word firstWord = I.value();
        SetIterator<Word> J = I;
        J.next();
 
        // compare firstWord with rest of the ss set.
 
        while( !J.done() )
        {
            Word secondWord = J.value();
            int pieceLen = firstWord.agreementLength(secondWord);
            if ( pieceLen != 0 )
				  {
                int minLen = min(firstWord.length(), secondWord.length());
					 if ( pieceLen == minLen ) return 1;
                if( lambda == 0 || minLen <= lambda * pieceLen )
						lambda = (minLen - 1) / pieceLen;
					 if ( lambda == 1 ) return 1;
					 // @stc the 0 test on lambda makes the value lambda == 0
					 // equivalent to infinity; the test slows the loop
					 // marginally, but guarantees semantic correctness;
				  }
            J.next();
        }
        I.next();
    }
	return lambda;
}
// by @db & @dp


Trichotomy hasMetricSmallCancellation( const SetOf<Word>& S, const int lambda )

{
    SetIterator<Word> I(S);
    while(!I.done())
    {
        Word firstWord = I.value();
        int firstLen = firstWord.length();
 
        // K iterates the rest of the relators set.
        SetIterator<Word> K = I;
        K.next();
 
        // compare firstWord with rest of the relators set.
        while(!K.done())
        {   Word secondWord = K.value();
            int secondLen = secondWord.length();
 
            // calculate the length of common piece of two relators
 
            int pieceLen = firstWord.agreementLength(secondWord);
 
            // check the C'(1/lambda) condition
 
            if( pieceLen * lambda >= min(firstLen, secondLen) )
                return NO;
            K.next();
        }
        I.next();
    }
    return YES;
}
// by @db & @dp


//-------------------------------------------------------------------//
//--------------------------- WordRep -------------------------------//
//------------------------ static members ---------------------------//

 
const Type WordRep::theWordType = Type( Type::unique() );

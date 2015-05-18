// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
// 
// Contents: Implementation of GAWord class
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
//


#include "GAWord.h"
#include "Roulette.h"


// ------------------------------- GAWord ---------------------------------- //


GAWord GAWord::mutate( ) const
{
  Word w(theWord);
  int wLen = theWord.length();
  int num = int(r.rand() * wLen);
  
  float op = r.rand();
  
  if( op <= 0.1 ) { // insert a new letter to the word (10% chance)
    num = int(r.rand() * (wLen+1));
    Word a,b;
    if( num > 0 )
      a = w.initialSegment(num);
    if( num < wLen ) 
      b =  w.terminalSegment(wLen-num);
    w =  a * Word(Generator(randomGen())) * b;
  }

  else if( op <= 0.2 ) { // delete one letter (10% chance)
    if( wLen > 0 ) {
      Word a,b;
      if( num > 0 )
	a = w.initialSegment(num);
      if( num < wLen - 1 ) 
	b =  w.terminalSegment(wLen-1-num);
      w = a * b;
    }
  }

  else { // mutate one letter (80% chance)
    if( wLen > 0 )
      w[num] = Generator(randomGen());
  }
  
  return GAWord(numOfGens, w.freelyReduce());
}


GAWord GAWord::crossover( const GAWord& w ) const
{
  int len1 = theWord.length();
  int len2 = w.theWord.length();
  Word a,b;
  if( len1 > 0 ) {
   int num1 = 1+int(r.rand() * len1);
   a = theWord.initialSegment(num1);
  }
  if( len2 > 0 ) {
    int num2 = 1+int(r.rand() * len2);
    b = w.theWord.terminalSegment(num2);
  }
  
  Word res = a * b;
  return GAWord(numOfGens, res.freelyReduce());
}  


GAWord GAWord::randomWord( ) const
{
  Word w;
  do { 
    int vLen =  int(r.rand() * maxWordLen) + 1;
    //int vLen = maxWordLen/2;
    VectorOf<Generator> v(vLen);
    for( int i = 0; i < vLen; ++i )
      v[i] = randomGen();
    w = Word(v).freelyReduce();
  } while( w.length() == 0 );
  
  return GAWord(numOfGens, w);
}


int GAWord::randomGen( ) const
{
  int gen = int(r.rand() * numOfGens) + 1;
  return (r.rand() <= 0.5) ? gen : -gen;
}


UniformRandom GAWord::r;
int GAWord::maxWordLen = 10;



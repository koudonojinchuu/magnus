// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
// 
// Contents: Implementation of GASubgroup class
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
//


#include "GASubgroup.h"
#include "Roulette.h"


//----------------------------- GASubgroup ----------------------------------//

GASubgroup::GASubgroup( ) { }


int GASubgroup::fitness( const GASubgroup& S ) const
{
  SubgroupGraph A(numOfGens, gens);
  SubgroupGraph B(numOfGens, S.gens);
  SubgroupGraph I = A.intersection(B);
  return (I.rank() - 1) - (A.rank() - 1) * (B.rank() - 1);
}


GASubgroup GASubgroup::mutate( ) const
{
  SetOf<Word> S(gens);
  int card = S.cardinality();
  for( int ii = 0; ii < card; ++ii ) {

    int num = int(r.rand() * S.cardinality());
    SetIterator<Word> I(S);
    for( int i = 0; i < num; ++i ) I.next();
    
    float op = r.rand();
    
    if( op <= 0.05 ) { // add a new generator to the subgroup (5% chance)
      S |= randomWord();
    }
    
    else if( op <= 0.1 ) { 
      // delete one generator from the subgroup (5% chance)
      S.removeElement( I.value() );
    }
    
    else if( op <= 0.4 ) { // mutate one letter (30% chance)
      Word w = I.value();
      w[int(r.rand() * w.length())] = randomGen();
      w = w.freelyReduce();
      if( w.length() != 0 ) {
	S.removeElement( I.value() );
	S |= w;
      }
    }
    else if( op <= 0.7 ) { // insert one letter (30% chance)
      Word w = I.value();
      int num = int(r.rand() * w.length()) + 1;
      int vLen = w.length() + 1;
      VectorOf<Generator> v(vLen);
      for( int i = 0; i < num; ++i ) v[i] = w[i];
      v[num] = randomGen();
      for( int i = num + 1; i < vLen; ++i ) v[i] = w[i-1];
      w = Word(v).freelyReduce();
      if( w.length() != 0 ) {
	S.removeElement( I.value() );
	S |= w;
      }
    }
    
    else { // delete one letter (30% chance)
      Word w = I.value();
      int num = int(r.rand() * w.length());
      int vLen = w.length() - 1;
      VectorOf<Generator> v(vLen);
      for( int i = 0; i < num; ++i ) v[i] = w[i];
      for( int i = num; i < vLen; ++i ) v[i] = w[i+1];
      w = Word(v).freelyReduce();
      if( w.length() != 0 ) {
	S.removeElement( I.value() );
	S |= w;
      }
    }
    
  }
  
  card = S.cardinality();
  if( card <= 1 || card > maxCard ) return gens;
  SubgroupGraph A(numOfGens, S);
  if( A.rank() <= 1 ) return gens;
  return S;
}


GASubgroup GASubgroup::crossover( const GASubgroup& S ) const
{
  SetOf<Word> result, U = setUnion(gens, S.gens);
  SetIterator<Word> I(U);
  for( ; !I.done(); I.next() )
    if( r.rand() <= 0.5)
      result |= I.value();
  if( result.cardinality() > 1)
    return result;
  else
    return *this;
}


GASubgroup GASubgroup::randomSubgroup( ) const
{
  SetOf<Word> S;
  int numOfSGens = maxCard/2;//int(r.rand() * (maxCard-1)) + 2;
  for( int g = 0; g < numOfSGens; ++g )
    S |= randomWord();

  SubgroupGraph A(numOfGens, S);
  if( A.rank() <= 1 ) return randomSubgroup( );
  return S;
}


Word GASubgroup::randomWord( ) const
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
  
  return w;
}


int GASubgroup::randomGen( ) const
{
  int gen = int(r.rand() * numOfGens) + 1;
  return (r.rand() <= 0.5) ? gen : -gen;
}


UniformRandom GASubgroup::r;
int GASubgroup::maxWordLen = 15;
int GASubgroup::maxCard = 30;
int GASubgroup::numOfGens = 5;



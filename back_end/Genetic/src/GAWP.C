// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
// 
// Contents: Implementation of GAWP class
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
//

#include "GAWP.h"

//#ifndef BSD
//#include "values.h"
//#endif
//

#include "Roulette.h"
#include "File.h"


// ------------------------------- GAWP ------------------------------------ //
  

Trichotomy GAWP::isTrivial( const Word& u, ostream& out )
{
  w = u;
  wLen = w.length();
  int popSize = cfg.populationSize();
  Word pop[popSize],newPop[popSize];
  int fit[popSize];

  // the main loop

  int numOfGens = cfg.numOfGenerations();
  float crossRate = cfg.chanceOfCrossover();
  float mutRate = cfg.chanceOfMutation();
  int max, min, maxInd, g;
 
  for( g = 0; g < numOfGens; ++g ) {

    min = MAXINT; max = -MAXINT;  maxInd = -1;
    
    // compute fitness values
    for( int i = 0; i < popSize; ++i ) {
	
      fit[i] = fitness(pop[i]);
     
      if( fit[i] < min ) {
	min = fit[i];
      }
      if( fit[i] > max ) {
	max = fit[i];
	maxInd = i;
      }
    }
    
    // print current results
    out << "Generation: " << g << "   Fitness: " << max << endl << ready;
    // << "Index: " << maxInd << endl;
   
    /*
    //if( g % 100 == 0 ) {
      for( int i = 0; i < popSize; ++i ) {
	out << "x" << i << " = ";
	theGroup.printWord(cout, pop[i]);
	out << endl;
      }
      out << endl;
      //}
    */

    // exit if found a solution
    if( max == wLen ) {
      out << "The word is trivial." << endl << end;
      return yes;
    }
    
    // make fitness values suitable for Roulette wheel selection
    int base = abs(min) + 1;
    for( int i = 0; i < popSize; ++i )
      fit[i] += base;
    
    // fitness scaling
    if( cfg.haveFitnessScaling() )
      for( int i = 0; i < popSize; ++i )
	fit[i] = fit[i] * fit[i];
    
    /*
    // crossover
    RouletteWheel<int> wheel(popSize,fit);
    for( int i = 0; i < popSize; ++i ) {
      if( devgen.rand() <= crossRate ) {
	int i1 = wheel.GetIndex();
	int i2 = wheel.GetIndex();
	newPop[i] = pop[i1].crossover(pop[i2]);
      }
      else {
	newPop[i] = pop[i];
      }
    }
    */


    // Roulette Wheel selection
    RouletteWheel<int> wheel(popSize,fit);
   
    for( int i = 0; i < popSize; ++i ) {
      newPop[i] = pop[wheel.GetIndex()];
    }  


    // mutation
    for( int i = 0; i < popSize; ++i ) {
      if( r.rand() <= mutRate )
	newPop[i] = mutate(newPop[i]);
    }
     
    // elitist selection
    if( cfg.haveElitistSelection() ) {
      newPop[0] = pop[maxInd];
    }
    
    // prepare for the next iteration
    for( int i = 0; i < popSize; ++i ) {
      pop[i] = newPop[i];
    }    
  }
  return dontknow;
}


int GAWP::fitness( const Word& u ) const
{
  int uLen = u.length();
  int minLen = min(wLen, uLen);
  int fit = -abs(uLen - wLen);
  for( int i = 0; i < minLen; ++i )
    if( w[i] == u[i] ) ++fit;
    else --fit;
  return fit;
}


Word GAWP::mutate( const Word& u )
{
  Word v = u;
  int vLen = v.length();

  if( r.rand() < 0.3 ) { // conjugate u by a random generator (30% chance)

    if( vLen == 0 ) return v;
    Word gen;
    if( r.rand() < 0.5 )
      gen = Word(v[0]);
    else
      gen = Word(v[vLen-1]).inverse();
    return (gen.inverse() * u * gen).cyclicallyReduce();
  }

  else  { // insert a new relator (70% chance)
    
    int num = int(r.rand() * (vLen+1));
    Word a,b;
    if( num > 0 )
      a = v.initialSegment(num);
    if( num < vLen ) 
      b =  v.terminalSegment(vLen-num);
    
    I.reset();
    num = int(r.rand() * theGroup.getRelators().cardinality());
    for( int i = 0; i < num; ++i) I.next();

    Word rel = I.value();
    if( r.rand() < 0.5 ) rel = rel.inverse();
    return (a * rel * b).cyclicallyReduce();
  }
}


// ------------------------------- GAWP2 ----------------------------------- //


GAWP2::GAWP2( const FPGroup& G, const GHNConfig& config ) 
  : theGroup( G ), 
    cfg( config ), 
    I(G.getRelators()),
    relators( G.getRelators() )
{ 
  symmetrise(relators);
  I = SetIterator<Word>(relators);
}

Trichotomy GAWP2::isTrivial( const Word& u, ostream* out )
{
  /*
  if( out != 0 ) {
    (*out) << "The algorithm tries to reduce length of the word genetically. "
      "The fitness value below is the lowest length produced on the "
      "current generation. " << endl << endl << ready;
  }
  */

  w = u;
  wLen = w.length();
  int popSize = cfg.populationSize();
  Word pop[popSize],newPop[popSize];
  int fit[popSize];

  // create the original population

  for( int i = 0; i < popSize; ++i )
    pop[i] = w;

  // the main loop

  int numOfGens = cfg.numOfGenerations();
  float crossRate = cfg.chanceOfCrossover();
  float mutRate = cfg.chanceOfMutation();
  int max, min, minInd, g;
 
  for( g = 0; g < numOfGens || numOfGens == -1; ++g ) {
    
    min = MAXINT; max = -MAXINT;  minInd = -1;
    
    // compute fitness values
    for( int i = 0; i < popSize; ++i ) {
	
      fit[i] = fitness(pop[i]);
     
      if( fit[i] < min ) {
	min = fit[i];
	minInd = i;
      }
      if( fit[i] > max ) {
	max = fit[i];
      }
    }
    
    // print current results
    if( g < 100 || (g <1000 && g % 10 == 0) || ( g % 100 == 0 ) ) {
      if( out ) {
	*out << "Generation: " << g << "   Best Fitness: " << min << endl
	     << "The Fittest Word: ";
	theGroup.printWord(*out, pop[minInd]);
	*out << endl << endl << ready;
      }
    }
   
    /*
    //if( g % 100 == 0 ) {
      for( int i = 0; i < popSize; ++i ) {
	out << "x" << i << " = ";
	theGroup.printWord(out, pop[i]);
	out << endl;
      }
      out << endl;
      //}
    */

    // exit if found a solution
    if( min == 0 ) {
      if( out )
	*out << "The word is trivial." << endl << endl << end;
      return yes;
    }
    
    // make fitness values suitable for Roulette wheel selection
    int base = max + 1;
    for( int i = 0; i < popSize; ++i )
      fit[i] = base - fit[i];
    
    // fitness scaling
    if( cfg.haveFitnessScaling() )
      for( int i = 0; i < popSize; ++i )
	fit[i] = fit[i] * fit[i];
    
    /*
    // crossover
    RouletteWheel<int> wheel(popSize,fit);
    for( int i = 0; i < popSize; ++i ) {
      if( devgen.rand() <= crossRate ) {
	int i1 = wheel.GetIndex();
	int i2 = wheel.GetIndex();
	newPop[i] = pop[i1].crossover(pop[i2]);
      }
      else {
	newPop[i] = pop[i];
      }
    }
    */


    // Roulette Wheel selection
    RouletteWheel<int> wheel(popSize,fit);
   
    for( int i = 0; i < popSize; ++i ) {
      newPop[i] = pop[wheel.GetIndex()];
    }  


    // mutation
    for( int i = 0; i < popSize; ++i ) {
      if( r.rand() <= mutRate )
	newPop[i] = mutate(newPop[i]);
    }
     
    // elitist selection
    if( cfg.haveElitistSelection() ) {
      newPop[0] = pop[minInd];
    }
    
    // prepare for the next iteration
    for( int i = 0; i < popSize; ++i ) {
      pop[i] = newPop[i];
    }    
  }
  return dontknow;
}


int GAWP2::fitness( const Word& u ) const
{
  return u.length();
}


Word GAWP2::mutate( const Word& u )
{
  Word res(u);
  int resLen = res.length();
  if( resLen == 0 ) return res;

  while( true ) {
    
    int relCard = relators.cardinality();
    int n = int(r.rand() * relCard);
    I.reset();
    for( int i = 0; !I.done() && i < n; ++i ) I.next();
    Word v = I.value();
    int vLen = v.length();
    do {  n = 1 + int(r.rand() * vLen); } while( n > resLen );
    Word vSeg = v.initialSegment(n);
    
    for( int i = 0; i < resLen; ++i )
      for( int j = i+1; j <= resLen; ++j ) {
	if( vSeg == res.subword(i,j) ) {

	  Word a,b;
	  if( i > 0 ) 
	    a = res.subword(0,i);
	  if( j < resLen )
	    b =  res.subword(j,resLen);

	  Word res1 = ( a *  v.subword(n,vLen).inverse() * b ).freelyReduce();
	  /*
	  out << "In " << res << " replacing " << vSeg << " by " 
	       << v.subword(n,vLen).inverse() << " we'll get "
	       << res1 << endl;
	  */
	  return res1;
	}
      }
  }
}


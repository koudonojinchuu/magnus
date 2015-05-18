// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Informal test.
//
// Principal Author: Dmitry Bormotov
//
// Status:
//
// Revision History:
//


#include <values.h>
#include "global.h"
#include "Map.h"
#include "GHNConfig.h"
#include "RandomNumbers.h"
#include "Roulette.h"
#include "GAWord.h"

int main ( )
{
  Chars errMsg;
  GHNConfig c;
  //cout << c << endl;

  ifstream in("../wp.in");
  in >> c;
  cout << c << endl;

  FreeGroup G;
  cout << "Enter a free group: ";
  errMsg = cin >> G;
  if( errMsg.length() > 0 ) {
    cout << errMsg;
    exit(0);
  }
  cout << endl;
  
  VectorOf<Chars> v = G.namesOfGenerators();
  int rLen = v.length() + 1;
  VectorOf<Chars> r(rLen);
  for( int i = 0; i < rLen - 1; ++i )
    if( v[i] != "x" && v[i] != "X" )
      r[i] = v[i];
    else
      error("x is reserved for variables\n");
  r[rLen-1] = "x";

  FreeGroup F(r);
  cout << "Enter an equation (a word) with one variable x:";
  Word w;
 
  w = F.readWord(cin,errMsg);
  if( errMsg.length() > 0 ) {
    cout << errMsg;
    exit(0);
  }
  cout << endl;

  VectorOf<Word> im(rLen);
  for( int i = 0; i < rLen; ++i )
    im[i] = Word(Generator(i+1));
  Map M(F,F,im);

  int popSize = c.populationSize();
  GAWord pop[popSize],newPop[popSize];
  for( int i = 0; i < popSize; ++i ) {
    pop[i] = GAWord(rLen-1,Word());
  }

  int fit[popSize];

  // the main loop

  int numOfGens = c.numOfGenerations();
  //bool bHaveFitnessScaling = c.haveFitnessScaling();
  float crossRate = c.chanceOfCrossover();
  float mutRate = c.chanceOfMutation();
  UniformRandom devgen;
  int max, min, minInd, g;
 
  // create the original random populations
  for( int i = 0; i < popSize; ++i ) {
    pop[i] = pop[i].randomWord();
  }
  
  for( g = 0; g < numOfGens; ++g ) {
    
    min = MAXINT; max = 0;  minInd = -1;
    
    // compute fitness values
    for( int i = 0; i < popSize; ++i ) {
	
      M.setGeneratingImages(rLen-1,(pop[i]).getWord());
      fit[i] = M.imageOf(w).freelyReduce().length();

      if( fit[i] < min ) {
	min = fit[i];
	minInd = i;
      }
      else if( fit[i] > max )
	max = fit[i];
    }
    
    // print current results
    cout << "Generation: " << g << "   Fitness: " << min << endl;
    /*
    if( g % 100 == 0 ) {
      for( int i = 0; i < popSize; ++i ) {
	cout << "x" << i << " = ";
	F.printWord(cout, (pop[i]).getWord());
	cout << endl;
      }
      cout << endl;
    }
    */
    // exit if found a solution
    if( min == 0 ) {
      cout << "x = ";
      F.printWord(cout, (pop[minInd]).getWord());
      cout << endl;
      return 0;
    }
    
    // make fitness values suitable for Roulette wheel selection
    int base = max + 1;
    for( int i = 0; i < popSize; ++i )
      fit[i] = base - fit[i];
    
    // fitness scaling
    if( c.haveFitnessScaling() )
      for( int i = 0; i < popSize; ++i )
	fit[i] = fit[i] * fit[i];
    
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
    
    
    // mutation
    for( int i = 0; i < popSize; ++i ) {
      if( devgen.rand() <= mutRate ) {
	newPop[i] = newPop[i].mutate();
      }
    }
    
    
    // elitist selection
    if( c.haveElitistSelection() ) {
      newPop[0] = pop[minInd];
    }
    
    // prepare for the next iteration
    for( int i = 0; i < popSize; ++i ) {
      pop[i] = newPop[i];
    }    
  }
}

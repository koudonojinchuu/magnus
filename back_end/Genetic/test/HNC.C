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
#include "GHNConfig.h"
#include "RandomNumbers.h"
#include "Roulette.h"
#include "GASubgroup.h"

int main ( )
{
  GHNConfig c;
  //cout << c << endl;

  ifstream in("test/in");
  if(! in)
    {
      cerr << "error opening file ../in, aborting" << endl;
      exit(1);
    }
  cout << in << endl;
  cout << "No error" << endl;
  in >> c;
  cout << "File successfully downloaded" << endl;
  cout << c << endl;

  // create the original random populations
  
  int popSize = c.populationSize();
  GASubgroup pop1[popSize],pop2[popSize],newPop1[popSize],newPop2[popSize];
  for( int i = 0; i < popSize; ++i ) {
    pop1[i] = pop1[i].randomSubgroup();
    pop2[i] = pop2[i].randomSubgroup();
  }
  int fit[popSize];

  // the main loop

  int numOfGens = c.numOfGenerations();
  //bool bHaveFitnessScaling = c.haveFitnessScaling();
  float crossRate = c.chanceOfCrossover();
  float mutRate = c.chanceOfMutation();
  UniformRandom devgen;
    
  for( int g = 0; g < numOfGens; ++g ) {
    
    int best = -MAXINT, worst = MAXINT, bestInd = -1;
    
    // compute fitness values
    for( int i = 0; i < popSize; ++i ) {

      fit[i] = pop1[i].fitness(pop2[i]);
      if( fit[i] > best ) {
	best = fit[i];
	bestInd = i;
      }
      else if( fit[i] < worst )
	worst = fit[i];
    }

    // print current results
    cout << "Generation: " << g << "   Fitness: " << best << endl
	 << "S1: " << pop1[bestInd] << endl
	 << "S2: " << pop2[bestInd] << endl
	 << endl;
    
    // make fitness values positive with minimum value equal to 1
    int base = abs(worst) + 1;
    for( int i = 0; i < popSize; ++i )
      fit[i] += base;
    
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
	newPop1[i] = pop1[i1].crossover(pop1[i2]);
	newPop2[i] = pop2[i1].crossover(pop2[i2]);
      }
      else {
	newPop1[i] = pop1[i];
	newPop2[i] = pop2[i];
      }
    }

    
    // mutation
    for( int i = 0; i < popSize; ++i ) {
      if( devgen.rand() <= mutRate )
	newPop1[i] = newPop1[i].mutate();
      if( devgen.rand() <= mutRate )
	newPop2[i] = newPop2[i].mutate();
    }
    
   
    // elitist selection
    if( c.haveElitistSelection() ) {
      newPop1[0] = pop1[bestInd];
      newPop2[0] = pop2[bestInd];
    }

    // prepare for the next iteration
    for( int i = 0; i < popSize; ++i ) {
      pop1[i] = newPop1[i];
      pop2[i] = newPop2[i];
    }    
  }
}

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
#include "Timer.h"

int main ( )
{
  const int numOfSecs = 40;
  const int timeValue = numOfSecs*1000;

  cout << "The goal of this program is to provided some statistics " << endl
       << "for genetic algorithm that tries to find a counter example to "<<endl
       << "the Hanna Neumann conjecture. The programs runs the genetic "<<endl
       << "algorithm and then the random algorithm giving them the same "<<endl 
       << "amount of time. Whoever produces a pair of subgroups with bigger "<<endl
       << "fitness value wins. The winner gains 1 point, the looser gains "<<endl
       << "0 points. If there is a tie, everybody gets 0.5 points. The best "<<endl
       << "subgroups, fitness values, the number of iterations(the number "<<endl
       << "of intersections taken) and the total score are printed. The "<<endl
       << "following is the current set of parameters for genetic algorithm:"<<endl
       << endl;

  float genPoints = 0, randPoints = 0;

  GHNConfig c;
  //cout << c << endl;

  ifstream in("../in");
  in >> c;
  cout << c << endl;

  int popSize = c.populationSize();
  GASubgroup pop1[popSize],pop2[popSize],newPop1[popSize],newPop2[popSize];
  int fit[popSize];

  // the main loop

  int numOfGens = c.numOfGenerations();
  //bool bHaveFitnessScaling = c.haveFitnessScaling();
  float crossRate = c.chanceOfCrossover();
  float mutRate = c.chanceOfMutation();
  UniformRandom devgen;
  int best, worst, bestInd, g;
  Timer t(0);

  while( true ) {

    cout << "Genetic algorithm ...   " << endl << endl;
    t.reset(timeValue);

    // create the original random populations
    for( int i = 0; i < popSize; ++i ) {
      pop1[i] = pop1[i].randomSubgroup();
      pop2[i] = pop2[i].randomSubgroup();
    }
   
    for( g = 0; g < numOfGens, !t.expired(); ++g ) {
      
      best = -MAXINT; worst = MAXINT;  bestInd = -1;
      
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
      
      /*
	bestCount = 0;
	for( int i = 0; i < popSize; ++i )
	if( fit[i] == best )
	++bestCount;
      */
      
      // print current results
      //cout << "Generation: " << g << "   Fitness: " << best << endl;
      if( best > 0 )  
	cout << "S1: " << pop1[bestInd] << endl
	     << "S2: " << pop2[bestInd] << endl;
      //cout << endl;

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
   
    cout << "Best subgroups: " << endl;
    cout << "S1: " << pop1[bestInd] << endl
	 << "S2: " << pop2[bestInd] << endl;
   
    cout << "Random Algorithm ...  " << endl << endl;
    t.reset(timeValue);

    GASubgroup S1, S2;
    int randFit, bestRandFit = -MAXINT;
    long randIter = 0;
    while( !t.expired() ) {
      S1 = S1.randomSubgroup();
      S2 = S2.randomSubgroup();
      randFit = S1.fitness(S2);
      //cout << " (" << randFit << ")";
      if( randFit > bestRandFit )
	bestRandFit = randFit;
      if( randFit > 0 )  
	cout << "S1: " << S1 << endl
	     << "S2: " << S2 << endl;
      ++randIter;
    }

    cout << "Best subgroups: " << endl;
    cout << "S1: " << S1 << endl
	 << "S2: " << S2 << endl;


    cout << "Genetic algorithm.   " 
	 << "Iterations: " << g*c.populationSize() 
	 << "   Best Fitness: " << best << endl << endl;
 
    cout << "Random Algorithm.   Iterations: " << randIter 
	 << "   Best Fitness: " << bestRandFit << endl << endl;

    cout << "Result: ";
    if( best > bestRandFit ) {
      cout << "Genetic algorithm wins";
      ++genPoints;
    }
    else if( best < bestRandFit ) {
      cout << "Random algorithm wins";
      ++randPoints;
    }
    else {
      cout << "Tie";
      genPoints += 0.5;
      randPoints += 0.5;
    }
    cout << endl << endl;

    cout << "Total Score:   Genetic Algorithms: " << genPoints 
	 << "     Random Algorithm: " << randPoints << endl << endl << endl;

  }
}

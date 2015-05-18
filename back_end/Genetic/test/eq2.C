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

  int numOfVars;
  cout << "Enter the number of variables: ";
  cin >> numOfVars;
  cout << endl;
    
  FreeGroup G;
  cout << "Enter a free group: ";
  errMsg = cin >> G;
  if( errMsg.length() > 0 ) {
    cout << errMsg;
    exit(0);
  }
  cout << endl;
  
  VectorOf<Chars> v = G.namesOfGenerators();
  int numOfConst = v.length();
  int rLen = v.length() + numOfVars;
  VectorOf<Chars> r(rLen);
  int j;
  for( j = 0; j < v.length(); ++j )
    if( v[j][0] != 'x' && v[j][0] != 'X' )
      r[j] = v[j];
    else
      error("x is reserved for variables\n");

  char s[3] = {0,0,0};
  s[0] = 'x';
  
  for( int i = j; i < rLen; ++i ) {
    s[1] = i-j+1+48;
    r[i] = s;
  }

  FreeGroup F(r);
  cout << "Enter an equation (a word) with variables x1 ... x" 
       << numOfVars << ": ";
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
  GAWord pop[numOfVars][popSize],newPop[numOfVars][popSize];
  
  for( int k = 0; k < numOfVars; ++k )
    for( int i = 0; i < popSize; ++i ) {
      pop[k][i] = GAWord(numOfConst,Word());
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
  for( int k = 0; k < numOfVars; ++k )
    for( int i = 0; i < popSize; ++i ) {
      pop[k][i] = pop[k][i].randomWord();
    }
  
  for( g = 0; g < numOfGens || numOfGens == -1; ++g ) {
    
    min = MAXINT; max = 0;  minInd = -1;
    
    // compute fitness values

    for( int i = 0; i < popSize; ++i ) {

      for( int k = 0; k < numOfVars; ++k )
	M.setGeneratingImages(numOfConst+k,(pop[k][i]).getWord());
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
      for( int k = 0; k < numOfVars; ++k ) {
	cout << "x" << k+1 << " = ";
	F.printWord(cout, (pop[k][minInd]).getWord());
	cout << endl;
      }
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

    for( int k = 0; k < numOfVars; ++k )
      for( int i = 0; i < popSize; ++i ) {
	if( devgen.rand() <= crossRate ) {
	  int i1 = wheel.GetIndex();
	  int i2 = wheel.GetIndex();
	  newPop[k][i] = pop[k][i1].crossover(pop[k][i2]);
	}
	else {
	  newPop[k][i] = pop[k][i];
	}
      }
    
    
    // mutation
    for( int k = 0; k < numOfVars; ++k )
      for( int i = 0; i < popSize; ++i ) {
	if( devgen.rand() <= mutRate ) {
	  newPop[k][i] = newPop[k][i].mutate();
	}
      }
    
    
    // elitist selection
    for( int k = 0; k < numOfVars; ++k )
      if( c.haveElitistSelection() ) {
	newPop[k][0] = pop[k][minInd];
      }
    
    // prepare for the next iteration
    for( int k = 0; k < numOfVars; ++k )
      for( int i = 0; i < popSize; ++i ) {
	pop[k][i] = newPop[k][i];
      }    
  }
}

/*
 *	Author: Xinmao Wang
 */

#ifndef _PREABELIANREPPROBLEM_H_
#define _PREABELIANREPPROBLEM_H_
#include "SMFPGroup.h"
#include "File.h"
#include "fastProblems.h"

class PreAbelianRepProblem : public FastComputation
{
public:

  PreAbelianRepProblem(class SMFPGroup& theGroup) 
    { 
      G = theGroup.getFPGroup(); 
      m = G.numberOfGenerators(); //number of generators
      n = G.getRelators().cardinality();//number of relators
      g = VectorOf<Word>(m); //generator words
      x = VectorOf<Word>(m); //new generators
      y = VectorOf<Word>(n); //new relators
      z = VectorOf<Word>(m); //old generators in new generators
      a = new int[m*n]; //matrix of exponent sums
      c = 0; //counter
    }

  void takeControl( );

private:

  class FPGroup G;

  File f;

  int *a, c, m, n;

  VectorOf<Word> g, x, y, z; 

  int findmin(int);

  int plus(int, int, int, char);

  void show(void);

  void swap(int, int, char);

  Word pow(const Word &, int);

};

#endif

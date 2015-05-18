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
#include "GAEquationSolver.h"
#include "EquationParser.h"
#include "File.h"
#include "GAMatrixBurau.h"

int main ( )
{
  Chars errMsg;
  GHNConfig c;
  //cout << c << endl;
 
  ifstream in("../eqs.cfg");
  in >> c;
  cout << c << endl;
 
  FreeGroup F;
  cout << "Enter a free group: ";
  errMsg = cin >> F;
  if( errMsg.length() > 0 ) {
    cout << errMsg;
    exit(0);
  }
  cout << endl;

  VectorOf<Chars> names; 
  EquationParser p(cin);
  cout << "Enter an equation: ";
  Word w = p.parseEquation(F.namesOfGenerators(), names, errMsg);
  if( errMsg.length() > 0 ) {
    cout << errMsg;
    exit(0);
  }
  cout << endl;
  
  FreeGroup G(names);
  int numOfVars = names.length() - F.numberOfGenerators();
 
  /*
  cout << "Equation: ";
  G.printWord(cout,w);
  cout << endl;
  cout << "w = " << w << endl;
  cout << "Names: " << names << endl;
  cout << "NumOfVars: " << numOfVars << endl;
  */

  GAEquationSolver solver(G,numOfVars,c);
  Map solution = solver.getSolution(w,&cout);

  cout << "The solution is " << solution << endl;

  GAConfig c1(50,-1,100,100,GAConfig::STRONG,true);
  GAMatrixBurau solverT(c1, F); 
}


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
#include "DGESolver.h"
#include "EquationParser.h"
#include <sys/time.h>
#include <unistd.h>

double diffTime(timeval t2, timeval t1)
{
  double result;
  long s = t2.tv_sec - t1.tv_sec;
  result = (t2.tv_usec - t1.tv_usec) * 0.000001 + s;
  //if( t2.tv_usec < t1.tv_usec ) --result;
  return result;
}

int main ( )
{
  Chars errMsg;

  FreeGroup F;
  cout << "Enter a free group: ";
  errMsg = cin >> F;
  if( errMsg.length() > 0 ) {
    cout << errMsg;
    exit(0);
  }
  cout << endl;

  VectorOf<Chars> empty, varNames; 
  EquationParser p(cin);
  cout << "Enter a graphic equation: ";
  Word equation = p.parseEquation(empty, varNames, errMsg);
  if( errMsg.length() > 0 ) {
    cout << errMsg;
    exit(0);
  }
  cout << endl;

  cout << "Enter a word in F: ";
  WordParser P(cin);
  Word w = P.parseWordVerbatim( F.namesOfGenerators(), errMsg);  
  if( errMsg.length() > 0 ) {
    cout << errMsg;
    exit(0);
  }
  cout << endl;

  w = w.cyclicallyReduce();
  FreeGroup G(varNames);
  cout << "The group: " << F << endl;
  cout << "The variables: " << varNames << endl;
  cout << "Equation: ";
  G.printWord(cout, equation);
  cout << endl;
  cout << "Constant of length " << w.length() << ": ";
  F.printWord(cout, w);
  cout << endl;
 
  DGESolver solver(F,varNames,equation);
  Map solution;
  int wLen = w.length();

  timeval t1,t2;
  gettimeofday(&t1, NULL);

  for( int i = 0; i < wLen; ++i ) {

    Word u = w.cyclicallyPermute(i);
    cout << i << ": " << endl;
    if( solver.getSolution(u, solution) ) {
      cout << "The solution is: " << endl;
      for( int j = 0; j < varNames.length(); ++j ) {
	cout << varNames[j] << " = ";
	F.printWord(cout, solution.generatingImages(j));
	cout << endl;
      }
      cout << endl;
      break;
    }
    else
      cout << "The equation has no solutions" << endl;
  }
  gettimeofday(&t2, NULL);
  cout << "Time elapsed: " << diffTime(t2,t1) << endl << endl;
}






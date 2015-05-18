// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
// 
// Contents: Implementation of TwoCommSolver
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
//

#include "TwoCommSolver.h"

//#ifndef BSD
//#include "values.h"
//#endif
//


#include "Roulette.h"
#include "File.h"
#include "EquationParser.h"
#include "QEqnSolutions.h"


// ---------------------------- TwoCommSolver ------------------------------ //


//@njz
// const char* input = 
// "  
// t p q R P s Q r S T u x Y U z X y Z,
// p q R P u x Y U t s Q r S z X y Z T,
// p q R P u x Y U t z X y Z s Q r S T,
// p q R P t y s Q r S Z x T u z Y X U,
// p q R P t y Z x T u z s Q r S Y X U,
// s x y z S p t X Z U Q P R u Y T q r, 
// s x y z S p q u Y T P R Q t X Z U r,  
// s x y z S R u Y T q r p t X Z U Q P,  
// p q r X U Z Q y S R z T Y P x s t u,  
// ";
const char* input = 
 "  \n t p q R P s Q r S T u x Y U z X y Z,\n p q R P u x Y U t s Q r S z X y Z T,\n p q R P u x Y U t z X y Z s Q r S T,\n p q R P t y s Q r S Z x T u z Y X U,\n p q R P t y Z x T u z s Q r S Y X U,\n s x y z S p t X Z U Q P R u Y T q r, \n s x y z S p q u Y T P R Q t X Z U r,  \n s x y z S R u Y T q r p t X Z U Q P,  \n p q r X U Z Q y S R z T Y P x s t u, ";
//

const char* comms[9] = { 
  "pqux",
  "pqux",
  "pqux",
  "pqty",
  "pqty",
  "sxpt",
  "sxpq",
  "sxru",
  "qrys"
};
  
TwoCommSolver::TwoCommSolver( FreeGroup F )
  : theGroup( F ),
    numOfEq( 9 ),
    cfg(50,-1,95,95,2,1),
    numOfConsts( F.numberOfGenerators() ),
    maxWordLen( 10 ),
    r(1024)
{

  //@njz 
  //  istrstream in(input);
  std::istrstream in(input);
  //
  varNames = new VectorOf<Chars>[numOfEq];
  equation = new Word[numOfEq];
  VectorOf<Chars> empty;
  Chars errMsg;

  for( int l = 0; l < numOfEq; ++l ) {
    EquationParser p(in);
    equation[l] = p.parseEquation(empty, varNames[l], errMsg);
    if( errMsg.length() > 0 ) {
      cout << errMsg << endl;
      exit(0);
    }
  }
  numOfVars = varNames[0].length();
}


TwoCommSolver::~TwoCommSolver( )
{
  delete [] varNames;
  delete [] equation;
}


Word TwoCommSolver::crossover( const Word& u, const Word& v )
{
  int len1 = u.length();
  int len2 = v.length();
  Word a,b;
  if( len1 > 0 ) {
   int num1 = 1+int(r.rand() * len1);
   a = u.initialSegment(num1);
  }
  if( len2 > 0 ) {
    int num2 = 1+int(r.rand() * len2);
    b = v.terminalSegment(num2);
  }
  
  Word res = (a * b).freelyReduce();
  return res;
}  


Word TwoCommSolver::mutate( const Word& u )
{
  Word w = u;
  int wLen = w.length();
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
  
  return w.freelyReduce();
}


int TwoCommSolver::randomGen( )
{
  int gen = int(r.rand() * numOfConsts) + 1;
  return (r.rand() <= 0.5) ? gen : -gen;
}


Word TwoCommSolver::randomWord( )
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

//@njz
//Map TwoCommSolver::getSolution( const Word& u, int& eqInd, int& conjInd,
//				ostream* out = NULL )
Map TwoCommSolver::getSolution( const Word& u, int& eqInd, int& conjInd,
				ostream* out)
//
{
  Word w(u.cyclicallyReduce());
  maxWordLen = w.length()/numOfVars;
  int lmin = MAXINT;

  if( out != 0 ) {
    (*out) <<
      "This algorithm is trying to check whether a given word w is a product"
      " of two commutators: w = [x1,x2][x3,x4] ? The words x1,x2,x3,x4 are"
      " generated genetically and each of the fitness values below is the"
      " Hamming distance between the best substitution of [x1,x2][x3,x4] on"
      " current generation and w. When the distance is zero, the algorithm"
      " stops and reports found solution. If w is not a product of two"
      " commutators, the algorithm works forever."
	   << endl << endl << ready;
  }
  
  VectorOf<Word> *im = new VectorOf<Word>[numOfEq];
  Map *M = new Map[numOfEq];
  for( int l = 0; l < numOfEq; ++l ) {
    im[l] = VectorOf<Word>(numOfVars);
    FreeGroup domain(varNames[l]);
    M[l] = Map(domain,theGroup,im[l]);
  }

  int popSize = cfg.populationSize();
  Word pop[numOfEq][numOfVars][popSize],newPop[numOfEq][numOfVars][popSize];
  int fit[numOfEq][popSize];
  
  float crossRate = cfg.chanceOfCrossover();
  float mutRate = cfg.chanceOfMutation();
  int max, min, minInd, g, fitInd[numOfEq][popSize];
 
  // create the original random populations
  for( int l = 0; l < numOfEq; ++l )
    for( int k = 0; k < numOfVars; ++k )
      for( int i = 0; i < popSize; ++i ) {
	pop[l][k][i] = randomWord();
      }

  // the main loop
 
  for( g = 0; true; ++g ) {

    for( int l = 0; l < numOfEq; ++l ) {
    
      min = MAXINT; max = 0;  minInd = -1;
      
      // compute fitness values
      
      for( int i = 0; i < popSize; ++i ) {
	
	if( g == 0 || !cfg.haveStrongElitistSelection() ) {
	  for( int k = 0; k < numOfVars; ++k )
	    M[l].setGeneratingImages(k,pop[l][k][i]);
	  fit[l][i] = fitness( w, M[l].imageOf(equation[l]),fitInd[l][i]);
	}
	
	if( fit[l][i] < min ) {
	  min = fit[l][i];
	  minInd = i;
	}
	if( fit[l][i] > max )
	  max = fit[l][i];
      }
      
      // print current results
      if( out && g % 1 == 0 ) {
	if( min < lmin ) lmin = min;
	if( l == numOfEq - 1 )
	  *out << "Generation: " << g << "   Fitness: " << lmin << endl << ready;
      }
      // exit if found a solution
      if( min == 0 ) {
	/*
	  for( int k = 0; k < numOfVars; ++k ) {
	    if( out ) {
	    *out << "x" << k+1 << " = ";
	    theGroup.printWord(*out, pop[k][minInd]);
	    *out << endl << end;
	    }
	    }
	*/
	// prepare and return found solution

	Map S(M[l]);
	
	for( int k = 0; k < numOfVars; ++k )
	  S.setGeneratingImages(k,pop[l][k][minInd]);
	
	if( out ) {
	  //*out << "Solution: " << S << endl << end;
	}
	eqInd = l;
	conjInd = fitInd[l][minInd];
	delete [] im;
	delete [] M;

	return S;
      }
      
      // save results to a file
      /*
      if( g % 1000 == 0 && popFile != "" ) { 
	char sNum[10];
	sprintf(sNum, "%d", int(double(g) / 1000));
	
	//cout << "sNum = " << sNum << endl;
	
	ofstream out(popFile+sNum, ios::out | ios::trunc );
	if( !out ) 
	  error("Cannot open an output file.");
	
	for( int k = 0; k < numOfVars; ++k ) {
	  out << "# " << k << "   ";
	  theGroup.printWord(out, pop[l][k][minInd]);
	  out << endl;
	  for( int i = 0; i < popSize; ++i ) {
	    theGroup.printWord(out, pop[l][k][i]);
	    out << endl;
	  }
	  out << endl;
	}
	
	out << "# Fitness" << endl;
	for( int i = 0; i < popSize; ++i )
	  out << fit[l][i] << " ";
	out << endl;
	
	out.flush();
      }
      */
      
      // make fitness values suitable for Roulette wheel selection
      int f[popSize];
      int base = max + 1;
      for( int i = 0; i < popSize; ++i )
	f[i] = base - fit[l][i];
      
      // fitness scaling
      if( cfg.haveFitnessScaling() )
	for( int i = 0; i < popSize; ++i )
	  f[i] = f[i] * f[i];
      
      
      // crossover
      RouletteWheel<int> wheel(popSize,f,335);
      
      for( int k = 0; k < numOfVars; ++k )
	//if( r.rand() <= 0.5 ) 
	for( int i = 0; i < popSize; ++i ) {
	  if( r.rand() <= crossRate ) {
	    int i1 = wheel.GetIndex();
	    int i2 = wheel.GetIndex();
	    newPop[l][k][i] = crossover(pop[l][k][i1],pop[l][k][i2]);
	  }
	  else {
	    newPop[l][k][i] = pop[l][k][i];
	  }
	}
      
      // mutation
      for( int k = 0; k < numOfVars; ++k )
	for( int i = 0; i < popSize; ++i ) {
	  if( r.rand() <= mutRate ) {
	    newPop[l][k][i] = mutate(newPop[l][k][i]);
	  }
	}
      
      
      // strong elitist selection
      
      if( cfg.haveStrongElitistSelection() ) {
	
	
	// compute fitness values for the new population
	
	int newFit[popSize], newFitInd[popSize];
	
	for( int i = 0; i < popSize; ++i ) {
	  for( int k = 0; k < numOfVars; ++k )
	    M[l].setGeneratingImages(k,newPop[l][k][i]);
	  newFit[i] = fitness( w, M[l].imageOf(equation[l]), newFitInd[i] );
	}
	/*
	  cout << "Fitness: " << endl;
	  for( int i = 0; i < popSize; ++i ) 
	    cout << fit[i] << " ";
	  cout << endl;

	  cout << "New fitness: " << endl;
	  for( int i = 0; i < popSize; ++i ) 
	    cout << newFit[i] << " ";
	  cout << endl;
	  */
	for( int i = 0; i < popSize; ++i ) 
	  for( int j = 0; j < popSize; ++j ) 
	    if( newFit[i] < fit[l][j] ) {
	      fit[l][j] = newFit[i];
	      fitInd[l][j] = newFitInd[i];
	      for( int k = 0; k < numOfVars; ++k )
		pop[l][k][j] = newPop[l][k][i];
	      break;
	    }
	/*
	  cout << "Combined fitness: " << endl;
	  for( int i = 0; i < popSize; ++i ) 
	    cout << fit[i] << " ";
	  cout << endl;
	*/
      } else {
	
	// elitist selection
	for( int k = 0; k < numOfVars; ++k )
	  if( cfg.haveElitistSelection() ) {
	    newPop[l][k][0] = pop[l][k][minInd];
	  }
	
	// prepare for the next iteration
	for( int k = 0; k < numOfVars; ++k )
	  for( int i = 0; i < popSize; ++i ) {
	    pop[l][k][i] = newPop[l][k][i];
	  }    
      }
    }
  }
}

/*
int TwoCommSolver::fitness( Word u, Word v ) const
{
  int uLen = u.length();
  int vLen = v.length();
  int minLen = min(uLen, vLen);
  int fit = abs(uLen - vLen);
  for( int i = 0; i < minLen; ++i )
    if( u[i] != v[i] ) ++fit;
  return fit;
}
*/

int TwoCommSolver::fitness( Word u, Word v, int& ind ) const
{
  int uLen = u.length();
  int vLen = v.length();
  int minLen = min(uLen, vLen);
  int minFit = uLen + vLen;
  
  for( int i = 0; i < uLen; ++i ) {
    Word w = u.cyclicallyPermute(i);
    int fit = abs(uLen - vLen);
    for( int j = 0; j < minLen; ++j )
      if( w[j] != v[j] ) ++fit;
    if( fit < minFit ) { minFit = fit; ind = i; }
  }

  return minFit;
}

Word TwoCommSolver::getImage( Map M, Map solution, int ind, int num )
{
  VectorOf<Chars> names = M.domain().namesOfGenerators();
  Word t;
  char a = comms[ind][num];

  for( int i = 0; i < numOfVars; ++i )

    if( names[i] == a ) {
      Word im = M.generatingImages(i);
      t = solution.imageOf(im);
      break;
    }

  return t;
}


//@njz
//bool TwoCommSolver::isProductOfTwoComms( Word r, Word& x1, Word& x2,
//					 Word& y1, Word& y2,
//					 ostream* out = NULL )
bool TwoCommSolver::isProductOfTwoComms( Word r, Word& x1, Word& x2,
					 Word& y1, Word& y2,
					 ostream* out )
//
{
  Word cr;
  Word tt = r.freelyReduce();
  Word t = tt.cyclicallyReduce();
  int dif = (tt.length() - t.length()) / 2;
  if( dif != 0 )
    cr = tt.initialSegment(dif);

  int ind, conjInd;
  Map solution = getSolution(t,ind,conjInd,out);

  /*
  *out << "The solution of " << ind + 1 << "th equation (conj = "
       << conjInd << " ) is "
       << solution << endl << end << flush;
  */

  FreeGroup F(varNames[ind]);
  QEqnSolutionsInFreeGroup solver( F, equation[ind],numOfVars);
  Map S = solver.toSurfaceForm();
  Map M = F.inverseAutomorphism(S);

  Word conj = (cr * t.initialSegment(conjInd)).inverse();

  x1 = Word(getImage(M, solution, ind, 0).conjugateBy(conj)).freelyReduce();
  x2 = Word(getImage(M, solution, ind, 1).conjugateBy(conj)).freelyReduce();
  y1 = Word(getImage(M, solution, ind, 2).conjugateBy(conj)).freelyReduce();
  y2 = Word(getImage(M, solution, ind, 3).conjugateBy(conj)).freelyReduce();

  *out << endl << "It is a product of two commutators:" << endl
       << "[ ";
  theGroup.printWord(*out, x1);
  *out << ", ";
  theGroup.printWord(*out, x2);
  *out << " ] and " << endl << "[ ";
  theGroup.printWord(*out, y1);
  *out << ", ";
  theGroup.printWord(*out, y2);
  *out << " ]" << endl;
  
  return true;
}




// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
// 
// Contents: Implementation of GAEquationSolver, GraphicEquationSolver
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
//

#include "GAEquationSolver.h"

//#ifndef BSD
//#include "values.h"
//#endif
//

#include "Roulette.h"
#include "File.h"


// -------------------------- GAEquationSolver ----------------------------- //
  

GAEquationSolver::GAEquationSolver( const FreeGroup& G, int NumOfVars, 
				    const GHNConfig& config )
  : theGroup( G ), 
    numOfVars( NumOfVars ), 
    cfg( config ),
    numOfGens( G.numberOfGenerators() ),
    numOfConsts( G.numberOfGenerators() - NumOfVars ),
    maxWordLen( 10 )
{ 
  VectorOf<Chars> names = theGroup.namesOfGenerators();
  VectorOf<Chars> v(numOfGens);
  for( int i = 0; i < numOfConsts; ++i )
    v[i] = names[numOfVars+i];
  for( int i = 0; i < numOfVars; ++i )
    v[i+numOfConsts] = names[i];
  theGroup = FreeGroup(v);
}


Word GAEquationSolver::crossover( const Word& u, const Word& v )
{
  int len1 = u.length();
  int len2 = v.length();
  Word a,b;
  if( len1 > 0 ) {
   int num1 = min(len1,int(r.rand() * (len1+1)));
   a = u.initialSegment(num1);
  }
  if( len2 > 0 ) {
    int num2 = min(len2,int(r.rand() * (len2+1)));
    b = v.terminalSegment(num2);
  }
  
  Word res = (a * b).freelyReduce();
  return res;
}  


Word GAEquationSolver::mutate( const Word& u )
{
  Word w = u;
  int wLen = w.length();
  int num = min(wLen-1, int(r.rand() * wLen) );
  
  float op = r.rand();
  
  if( op <= 0.1 ) { // insert a new letter to the word (10% chance)
    num = min(wLen, int(r.rand() * (wLen+1)));
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


int GAEquationSolver::randomGen( )
{
  int gen = min(numOfConsts-1, int(r.rand() * numOfConsts)) + 1;
  return (r.rand() <= 0.5) ? gen : -gen;
}


Word GAEquationSolver::randomWord( )
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
//Map GAEquationSolver::getSolution( const Word& u, ostream* out = NULL, int& g )
Map GAEquationSolver::getSolution( const Word& u, ostream* out, int& g )
//
{
  Word w(u);
  int wLen = w.length();
  for( int i = 0; i < wLen; ++i ) {
    int p = power(w[i]);
    int a = abs(w[i]); 
    if( a <= numOfVars )
      w[i] = Generator(p * (a + numOfConsts));
    else
      w[i] = Generator(p * (a - numOfVars));
  }

  if( out != 0 ) {
    (*out) << "The algorithm tries to substitute variables with genetically "
      "produced words to reduce the length of an equation. "
      "The fitness value below is the lowest length of the equation produced "
      "on the current generation. " << endl << endl << ready;
  }
  
  VectorOf<Word> im(numOfGens);
  for( int i = 0; i < numOfGens; ++i )
    im[i] = Word(Generator(i+1));
  Map M(theGroup,theGroup,im);

  int popSize = cfg.populationSize();
  Word pop[numOfVars][popSize],newPop[numOfVars][popSize];
  int fit[popSize];
  
  float crossRate = cfg.chanceOfCrossover();
  float mutRate = cfg.chanceOfMutation();
  int max, min, minInd;
 
  // create the original random populations
  for( int k = 0; k < numOfVars; ++k )
    for( int i = 0; i < popSize; ++i ) {
      pop[k][i] = randomWord();
    }

  // the main loop
 
  for( g = 0; true; ++g ) {
    
    min = MAXINT; max = 0;  minInd = -1;
    
    // compute fitness values

    for( int i = 0; i < popSize; ++i ) {

      for( int k = 0; k < numOfVars; ++k )
	M.setGeneratingImages(numOfConsts+k,pop[k][i]);
      fit[i] = M.imageOf(w).freelyReduce().length();
      
      if( fit[i] < min ) {
	min = fit[i];
	minInd = i;
      }
      if( fit[i] > max )
	max = fit[i];
    }
    
    // print current results
    if( out && g%100 == 0) {
      *out << "Generation: " << g << "   Fitness: " << min << endl << ready;
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
      VectorOf<Chars> names = theGroup.namesOfGenerators();
      VectorOf<Chars> ret(numOfVars);
      for( int i = 0; i < numOfVars; ++i )
	ret[i] = names[i+numOfConsts];
      FreeGroup F(ret);
      VectorOf<Word> v(numOfVars);
      for( int i = 0; i < numOfVars; ++i )
	v[i] =  pop[i][minInd];
      
      VectorOf<Chars> c(numOfConsts);
      for( int i = 0; i < numOfConsts; ++i )
	c[i] = names[i];
      FreeGroup F2(c);
      
      Map res(F,F2,v);
      if( out ) {
	*out << "Solution: " << res << endl << end;
      }
      return res;
    }
    // make fitness values suitable for Roulette wheel selection
    int base = max + 1;
    for( int i = 0; i < popSize; ++i )
      fit[i] = base - fit[i];
    
    // fitness scaling
    if( cfg.haveFitnessScaling() )
      for( int i = 0; i < popSize; ++i )
	fit[i] = fit[i] * fit[i];
    

    // crossover
    RouletteWheel<int> wheel(popSize,fit);

    for( int k = 0; k < numOfVars; ++k )
      for( int i = 0; i < popSize; ++i ) {
	if( r.rand() <= crossRate ) {
	  int i1 = wheel.GetIndex();
	  int i2 = wheel.GetIndex();
	  newPop[k][i] = crossover(pop[k][i1],pop[k][i2]);
	}
	else {
	  newPop[k][i] = pop[k][i];
	}
      }
    
    
    // mutation
    for( int k = 0; k < numOfVars; ++k )
      for( int i = 0; i < popSize; ++i ) {
	if( r.rand() <= mutRate ) {
	  newPop[k][i] = mutate(newPop[k][i]);
	}
      }
    
    
    // elitist selection
    for( int k = 0; k < numOfVars; ++k )
      if( cfg.haveElitistSelection() ) {
	newPop[k][0] = pop[k][minInd];
      }
    
    // prepare for the next iteration
    for( int k = 0; k < numOfVars; ++k )
      for( int i = 0; i < popSize; ++i ) {
	pop[k][i] = newPop[k][i];
      }    
  }
}


int GAEquationSolver::dummy_g = 0;


// ------------------------ GraphicEquationSolver -------------------------- //
  

GraphicEquationSolver::GraphicEquationSolver( FreeGroup F,
					      VectorOf<Chars> vNames, 
					      Word eq, 
					      const GHNConfig& config,
					      Chars fn )
  : theGroup( F ), 
    varNames( vNames ),
    numOfVars( vNames.length() ),
    cfg( config ),
    numOfConsts( F.numberOfGenerators() ),
    maxWordLen( 10 ),
    equation( eq ),
    r(1024),
    popFile( fn )
{ }


Word GraphicEquationSolver::crossover( const Word& u, const Word& v )
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

char GraphicEquationSolver::crossoverChar( char u, char v )
{
  return (int(u)+int(v))/2;
/*	
  char mask = 255;
  int n = int(r.rand() * 9);
  mask = mask << n;
  return (u & mask) | (v & (~mask) );
*/
}  


Word GraphicEquationSolver::mutate( const Word& u )
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

char GraphicEquationSolver::mutateChar( char u )
{
  char mask = 1;
  int n = int(r.rand() * 8);
  mask = mask << n;
  return u ^ mask;
}


int GraphicEquationSolver::randomGen( )
{
  int gen = int(r.rand() * numOfConsts) + 1;
  return (r.rand() <= 0.5) ? gen : -gen;
}


Word GraphicEquationSolver::randomWord( )
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
//Map GraphicEquationSolver::getSolution( const Word& u, ostream* out = NULL )
Map GraphicEquationSolver::getSolution( const Word& u, ostream* out )
//
{
  Word w(u.cyclicallyReduce());
  maxWordLen = w.length()/numOfVars;

  /*
  if( out != 0 ) {
    (*out) << "The algorithm tries to substitute variables with genetically "
      "produced words to reduce the length of an equation. "
      "The fitness value below is the lowest length of the equation produced "
      "on the current generation. " << endl << endl << ready;
  }
  */

  VectorOf<Word> im(numOfVars);
  /*
  for( int i = 0; i < numOfVars; ++i )
    im[i] = Word(Generator(i+1));
  */
  FreeGroup domain(varNames);
  Map M(domain,theGroup,im);

  int popSize = cfg.populationSize();
  Word pop[numOfVars][popSize],newPop[numOfVars][popSize];
  int fit[popSize];
  unsigned char mut[numOfVars][popSize];
  unsigned char newMut[numOfVars][popSize];
  
  float crossRate = cfg.chanceOfCrossover();
  float mutRate = cfg.chanceOfMutation();
  int max, min, minInd, g;
 
  // create the original random populations
  for( int k = 0; k < numOfVars; ++k )
    for( int i = 0; i < popSize; ++i ) {
      pop[k][i] = randomWord();
      mut[k][i] = char(r.rand() * 256);
    }

  // the main loop
 
  for( g = 0; true; ++g ) {
    
    min = MAXINT; max = 0;  minInd = -1;
    
    // compute fitness values

    for( int i = 0; i < popSize; ++i ) {

      if( g == 0 || !cfg.haveStrongElitistSelection() ) {
	for( int k = 0; k < numOfVars; ++k )
	  M.setGeneratingImages(k,pop[k][i]);
	fit[i] = fitness( w, M.imageOf(equation) );
      }
      
      if( fit[i] < min ) {
	min = fit[i];
	minInd = i;
      }
      if( fit[i] > max )
	max = fit[i];
    }
    
    // print current results
    if( out && g % 1 == 0 ) {
      *out << "Generation: " << g << "   Fitness: " << min << endl << ready;
    }
    if( out && g % 10 == 0 ) {
      *out << "Mutations: " << endl;
      for( int k = 0; k < numOfVars; ++k )
	*out << double(mut[k][minInd])/255.0 << "  ";
      *out << endl << ready;
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

      for( int k = 0; k < numOfVars; ++k )
	M.setGeneratingImages(k,pop[k][minInd]);

      if( out ) {
	*out << "Solution: " << M << endl << end;
      }
      return M;
    }

    // save results to a file

    if( g % 1000 == 0 && popFile != Chars("") ) { 
      char sNum[10];
      sprintf(sNum, "%d", int(double(g) / 1000));

      //cout << "sNum = " << sNum << endl;

      ofstream out(popFile+sNum, ios::out | ios::trunc );
      if( !out ) 
	error("Cannot open an output file.");
      
      for( int k = 0; k < numOfVars; ++k ) {
	out << "# " << k << "   ";
	theGroup.printWord(out, pop[k][minInd]);
	out << endl;
	for( int i = 0; i < popSize; ++i ) {
	  theGroup.printWord(out, pop[k][i]);
	  out << endl;
	}
	out << endl;
      }
      
      out << "# Fitness" << endl;
      for( int i = 0; i < popSize; ++i )
	out << fit[i] << " ";
      out << endl;

      out.flush();
    }
      

    // make fitness values suitable for Roulette wheel selection
    int f[popSize];
    int base = max + 1;
    for( int i = 0; i < popSize; ++i )
      f[i] = base - fit[i];
    
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
	    newPop[k][i] = crossover(pop[k][i1],pop[k][i2]);
	    newMut[k][i] = crossoverChar(mut[k][i1],mut[k][i2]);
	  }
	  else {
	    newPop[k][i] = pop[k][i];
	    newMut[k][i] = mut[k][i];
	  }
	}
    
    // mutation
    for( int k = 0; k < numOfVars; ++k )
      for( int i = 0; i < popSize; ++i ) {
	if( r.rand() <= double(newMut[k][i])/255.0 ) {
	  newPop[k][i] = mutate(newPop[k][i]);
	  newMut[k][i] = mutateChar(newMut[k][i]);
	}
      }
    

    // strong elitist selection

    if( cfg.haveStrongElitistSelection() ) {

     
      // compute fitness values for the new population
      
      int newFit[popSize];
      
      for( int i = 0; i < popSize; ++i ) {
	for( int k = 0; k < numOfVars; ++k )
	  M.setGeneratingImages(k,newPop[k][i]);
	newFit[i] = fitness( w, M.imageOf(equation) );
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
	  if( newFit[i] < fit[j] ) {
	    fit[j] = newFit[i];
	    for( int k = 0; k < numOfVars; ++k ) {
	      pop[k][j] = newPop[k][i];
	      mut[k][j] = newMut[k][i];
	    }
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
	  newPop[k][0] = pop[k][minInd];
	  newMut[k][0] = mut[k][minInd];
	}
    
      // prepare for the next iteration
      for( int k = 0; k < numOfVars; ++k )
	for( int i = 0; i < popSize; ++i ) {
	  pop[k][i] = newPop[k][i];
	  mut[k][i] = newMut[k][i];
	}    
    }
  }
}

/*
int GraphicEquationSolver::fitness( Word u, Word v ) const
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

int GraphicEquationSolver::fitness( Word u, Word v ) const
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
    if( fit < minFit ) minFit = fit;
  }

  return minFit;
}


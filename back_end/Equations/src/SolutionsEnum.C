// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class QEqnSolutionsEnumerator
//
// Principal Authors: Dmitry Pechkin, Eugeny Paderin
//
// Status: Under trial
//
// Revision History:
//
// Special Notes:
//

#include "SolutionsEnum.h"
#include "NormalRandomWord.h"
#include "MagnusHome.h"

//////////////////// constructor //////////////////////////////////

QEqnSolutionsEnumerator::QEqnSolutionsEnumerator(const QEqnSolutionsInFreeGroup& equation)
  :
  theNumberOfGenerators(equation.numberOfGenerators()),
  theNumberOfVariables(equation.numberOfVariables()),
  theGroup(equation.group()),
  theRegStabGenerators(equation.regStabGenerators()),
  theRegStabInvGenerators(equation.regStabGenerators().length()),
  theBasicSolutions(equation.basicSolutions()),
  prefixAuto(equation.prefixMap()),
  allSolutionsAreEnumerated(false),
  numberOfSolutionsIsFinite(dontknow),
  // `dummy' values:
  solution(theGroup),
  specEndo(theGroup),
  stabAuto(theGroup),
  tuplesEnumerator(0,0),
  invGensComputed(equation.regStabGenerators().length())
{
  if( theBasicSolutions.length() == 0 ) {
    // equation has no solutions -- enumeration finished.
    allSolutionsAreEnumerated = true;
    return;
  }

  // check whether number of solutions is finite
	
  if( !areBasicSolutionsParametrized() )
    if(theRegStabGenerators.length() == 0)
      numberOfSolutionsIsFinite = yes;
    else
      numberOfSolutionsIsFinite = dontknow;
  else
    numberOfSolutionsIsFinite = no;
	
  currentBasicSolution = 0;
  // finite set of basic solutions is enumerated by counting...

  // depending on result of checking form an enumerator of
  // stabilizer and specializing endomorphism -- one for both.
  // We define specializer by n-tuple of words and stabilizer
  // by a formal word of generators - so we enumerate (n+1)-tuples.

  if( numberOfSolutionsIsFinite == no ) {
    int tupleSize = theNumberOfVariables;
    if( theRegStabGenerators.length() > 0 )	tupleSize++;
    VectorOf<int> gen(tupleSize);
    int i = 0;
    if( theRegStabGenerators.length() > 0 )
      gen[i++] = theRegStabGenerators.length();
    for(int j = 0; j < theNumberOfVariables; j++ )
      gen[i++] = theNumberOfGenerators;
    tuplesEnumerator = EnumeratorOfWordTuples(gen);
  }
  else if( numberOfSolutionsIsFinite == dontknow ) {
    // if there is no parametrized basic solutions then
    // there's no need in specializing endomorphism --
    // enumerate single words for stabilizer
    tuplesEnumerator = EnumeratorOfWordTuples(1,theRegStabGenerators.length());
  }

  for(int i = 0; i<invGensComputed.length(); i++)
    invGensComputed[i] = false;

  // build the first solution

  solution = prefixAuto | Endomorphism(theBasicSolutions[0]);
  solution.reduceGenImages();
}


bool QEqnSolutionsEnumerator::nextSolution( )
{
  if( done() ) return false;

  // for given stabilizer and specializer enumerate basic solutions
  // until exhaust

  if( ++currentBasicSolution == theBasicSolutions.length() ) {
    currentBasicSolution = 0;

    if( numberOfSolutionsIsFinite == yes) {
      allSolutionsAreEnumerated = true;
      return false;
    }

    // generate the next tuple

    Word oldRegStabWord;
    if( theRegStabGenerators.length() > 0 )
      oldRegStabWord = tuplesEnumerator.value()[0];

    tuplesEnumerator.next();
    VectorOf<Word> tuple = tuplesEnumerator.value();

    // extract and build a stabilizing automorphism

    int endoPos = 0;

    if( theRegStabGenerators.length() > 0 ) {
      Word w = tuple[0];
      endoPos = 1;
      if( oldRegStabWord != w ) {
	stabAuto = Automorphism(theGroup);
	for(int i=0; i<w.length(); i++) {
	  int index = abs(ord(w[i])) - 1;
	  if( ord(w[i]) > 0 )
	    stabAuto = stabAuto | (Automorphism)theRegStabGenerators[index];
	  else {
	    if( !invGensComputed[index] ) {
	      VectorOf<Word> images = Automorphism(theRegStabGenerators[index]).generatingImages();
	      VectorOf<Word> invImages = theGroup.inverseAutomorphism(images);
	      theRegStabInvGenerators[index] = Automorphism(theGroup, invImages);
	      invGensComputed[index] = true;
	    }
	    stabAuto = stabAuto | (Automorphism)theRegStabInvGenerators[index];
	  }
	  stabAuto.reduceGenImages();
	}
      }
    }

    // extract and build a specializer

    specEndo = Endomorphism(theGroup);
    specEndo.makeIdentity();
    for(int i = endoPos; i < tuple.length(); i++ )
      specEndo.setGeneratingImages(i-endoPos, tuple[i]);
  }

  // build new solution

  solution = (Endomorphism)prefixAuto | stabAuto
    | (Endomorphism)theBasicSolutions[currentBasicSolution]
    | specEndo;
  solution.reduceGenImages();
}

// checks whether basic solutions are parametrized, i.e. there is
// a solution mapping theEquation to the word containing variables

// Note: some solutions are parametrized and some aren't. We could
// separate them for better enumeration; now we didn't.

// a helper function; also used in GeneratorOfRandomSolutions
bool isParametrized(const Endomorphism& endo, int theNumberOfVariables) {
  for(int j = 0; j < theNumberOfVariables; j++) {
    Word gen_image = endo.generatingImages(j);
    for(int k = 0; k < gen_image.length(); k++)
      if( abs(ord(gen_image[k])) <= theNumberOfVariables )
	return true;
  }
  return false;
}

bool QEqnSolutionsEnumerator::areBasicSolutionsParametrized()
{
  for(int i = 0; i < theBasicSolutions.length(); i++) {
    if(isParametrized(theBasicSolutions[i], theNumberOfVariables) )
      return true;
  }
  return false;
}

//                                                                          //
// ---------------------- GeneratorOfRandomSolutions ---------------------- //
//                                                                          //

GeneratorOfRandomSolutions::GeneratorOfRandomSolutions( const FreeGroup& group, 
							const Word& equation, 
							int numberOfVariables )
  : theGroup(group), numOfVar(numberOfVariables), 
    numOfGen( group.numberOfGenerators()-numberOfVariables ),
    solution(group), variablesEliminator(group),
    threshold(randomSolutionsThreshold), rnd(1996)
{
  int numOfGens = group.numberOfGenerators();
  for( int i = numOfVar; i < numOfGens; ++i )
    variablesEliminator.setGeneratingImages( i, Generator(i+1) );
}
//@ep Should provide more sophisticated control of random solutions

bool GeneratorOfRandomSolutions::hasSolutions() const 
{
  return basicSolutions.length();
}


// This function generates a random solution made of the currently found
// basic solutions and RegStab generators.
// Returns true iff the solution is not registered before.

bool GeneratorOfRandomSolutions::generateSolution() 
{
  if( basicSolutions.length() == 0 ) {
    error("Attempt to generate random solution of quadratic equation "
          "when no basic solutions were found");
  }

  // trying 10 times to generate new solution
  for(int cnt=0; cnt<10; cnt++) {
    
    generateSomeSolution();
    // generates a solution and stores it to the this->solution field

    // if solution is not in set, register it

    if( !solutionSet.contains(solution) ) {

      // if the set is overfilled, empty it
      if( solutionSet.cardinality() > threshold ) {
	solutionSet.removeAllElements();
      }

      solutionSet.adjoinElement(solution);
      return true;
    }
  }
  // if we cannot invent new solution, take last found
  return false;
}


// helper function checking maximal length of generating images
// checked images from 0 to maxImage-1

static int imageLength(const Endomorphism& endo, int maxImage) {
  int maxLength = 0;
  for(int i=0; i< maxImage; i++)
    maxLength = max( maxLength, endo.generatingImages(i).length() );
  return maxLength;
}

// helper function generating some solution

void GeneratorOfRandomSolutions::generateSomeSolution() 
{

  // take one basic solution

  Endomorphism basic = basicSolutions[ rnd.rand(0, basicSolutions.length()-1) ];

  if( isParametrized(basic, numOfVar) ) {

    // so we must build a specializer

    NormalRandomWord rndSpec( 
			   numOfGen,
			   6,
			   4,
			   rnd.rand(100, 10000),      // length seed
			   rnd.rand(100, 10000)       // generator seed
			   );

    Endomorphism specEndo(theGroup);
    specEndo.makeIdentity();
    for(int i = 0; i < numOfVar; i++ ) {
      Word spec = rndSpec.word();
      for( int j = 0; j < spec.length(); ++j ) {
	int ordgen = ord(spec[j]); 
	spec[j] = (  ordgen > 0 ? ordgen+numOfVar : ordgen-numOfVar );
      }
      specEndo.setGeneratingImages( i, spec );
    }

    basic = basic | specEndo | variablesEliminator;
    basic.reduceGenImages();
    
  }

  // now generate stabilizing automorphism

  int numberOfStabGenerators = regStabGenerators.length();

  if(numberOfStabGenerators > 0) {
    int lastIndex = numberOfStabGenerators;
    int maxCount = min( max(10, numberOfStabGenerators), 20);
    int maxGen = rnd.rand(3, maxCount);

    for(int count = 0; count < maxGen; count++) {
      
      // take random automorphism

      int index = rnd.rand(0, numberOfStabGenerators - 1);
      int sign = rnd.rand(0,1);

      if(sign > 0) {
	if(index == -lastIndex) continue;
	lastIndex = index;
	basic = (Endomorphism)( (Automorphism) regStabGenerators[index] ) | basic ;
      }
      else {
	if(index == lastIndex) continue;
	lastIndex = -index;      
	basic = (Endomorphism) regStabGenerators.ref(index).inverse() | basic;
      }      
      basic.reduceGenImages();

      // now we must check length of generating images
      // do not allow them grow too large
      if( imageLength( basic, numOfVar) > 200 ) break;
    }
  }

  if( imageLength( basic, numOfVar) < 250 )
    solution = basic;

}





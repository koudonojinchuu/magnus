// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
// 
// Contents: 
//
// Principal Author: Alexei Miasnikov
//
// Status: in progress
//
// Revision History:
//
//

#include "GAIsPartOfBasis.h"

//#ifndef BSD
//#include "values.h"
//#endif
//

#include "Roulette.h"
#include "File.h"
#include "conversions.h"

// --------------------------- GAIsPartOfBasis ---------------------------- //

GAIsPartOfBasis::GAIsPartOfBasis( const FreeGroup& G, const ACConfig& config,
				  const Word& w)
    : theGroup( G ), 
      cfg( config ),
      numberOfGens( G.numberOfGenerators() ),
      theWords( 1 ),
      tmpWords( 1 ),
      bestWords( 1 ),
      threshold( 1 ),
      sav_first_max( MAXINT ),
      print_best_each_time( false ),
      g( 0 )
{
  theWords[0] = w;
  tmpWords[0] = w;
  bestWords[0] = w;
  initialize();
}

GAIsPartOfBasis::GAIsPartOfBasis( const FreeGroup& G, const ACConfig& config,
				  const VectorOf<Word>& v)
    : theGroup( G ), 
      cfg( config ),
      numberOfGens( G.numberOfGenerators() ),
      theWords( v ),
      tmpWords( v ),
      bestWords( v ),
      threshold( v.length() ),
      sav_first_max( MAXINT ),
      print_best_each_time( false ),
      g( 0 )
{
  initialize();
}

GAIsPartOfBasis::GAIsPartOfBasis( const FreeGroup& G, const ACConfig& config,
				  const VectorOf<Word>& v1,
				  const VectorOf<Word>& v2)
  : theGroup( G ), 
    cfg( config ),
    numberOfGens( G.numberOfGenerators() ),
    threshold( 0 ),
    sav_first_max( MAXINT ),
    print_best_each_time( false ),
    g( 0 )
{
  if ( v1.length() != v2.length() )
    error("Number of words in the tuples must be equal");
  int sumOfv1=0, sumOfv2=0;
  for (int i=0;i<v1.length();i++){
    sumOfv1 += v1[i].length();
    sumOfv2 += v2[i].length();
  }
  if (sumOfv1 > sumOfv2 ) {
    theWords = v1;
    tmpWords = v1;
    bestWords = v1;
    fixedWords = v2;
  } else {
    theWords =  v2;
    tmpWords = v2;
    bestWords = v2;
    fixedWords = v1;
  }
  initialize();
}
bool GAIsPartOfBasis::isPartOfBasis(ostream& out1, const Chars& out2_f_name)
{

  best_out_name = out2_f_name;
  //@njz: seems 'o' doesn't get initialized properly.
  //  ostream o;
  std::ostrstream o;
  //
  print_best_each_time = true;
  bool ret = isPartOfBasis( out1, o );

  return ret;
}

bool GAIsPartOfBasis::isPartOfBasis(ostream& out1, ostream& out2)
{
  total_tr = 0;
  swithc_tr = 0;
  Word res;
  VectorOf<Word> sav_w = theWords;
  out1 << "The algorithm tries to find Whitehead reduction genetically. ";
  // if only one word
  if (theWords.length() == 1){
    out1  << "The fitness value below is the length of the minimal"
	  << " word obtained on the current generation." 
	  << endl << endl <<  endl; 
  } else {
    out1  << "The fitness value below is the minimum of total lengths of tuples"
	  << " in the current generation." 
	  << endl << endl <<  endl;   
  }
  while (true){
    Trichotomy answer = transform( out1, 5 );
    if (answer == yes){
      // if the optimal is foun
      res = res * bestTransformation;
      theWords = sav_w;
      if (!print_best_each_time)
	fitness(res,out2,true);
      else {
	ofstream o(best_out_name);
	fitness(res,o,true);
	o.close();
      }
      theAuto = computeAuto( res );
      return true;
    } else if (answer == dontknow ){
      // start with shorter tuple 
      res = res * bestTransformation;
      theWords = sav_w;
      theAuto = computeAuto( res );
      if (print_best_each_time){
	ofstream o(best_out_name);
	fitness(res,o,true);
	o.close();
      }
      theWords = bestWords;
    } else{
      // exceeded the number of generations
      // res = res * bestTransformation;
      //theAuto = computeAuto( res );
      return false;
    }
  }
  return false;
}

Trichotomy GAIsPartOfBasis::transform( ostream& out, int num_of_occur )
{
  int max_count = 1;
  int popSize = cfg.populationSize();
  Word pop[popSize],newPop[popSize];
  int fit[popSize];
  bool finished = false;
  bool first_run = true;
  // for (int i = 0; i < popSize; i++ )
  //  pop[i] = randomWord();
  
  //  for (int i = 0; i < popSize; i++ )
  //    fit[i] = fitness( pop[i] );
  
  // the main loop
  
  float crossRate = cfg.chanceOfCrossover();
  float mutRate = cfg.chanceOfMutation();
  int max, min, maxInd, g_sav = g;
  int sav_max = -MAXINT;

  min = MAXINT; max = -MAXINT;  maxInd = -1;

  for( ; g < cfg.numOfGenerations(); ++g ) {
    
    min = MAXINT; max = -MAXINT;  maxInd = -1;
    
    
    // compute fitness values
    for( int i = 0; i < popSize; ++i ) {
      
      fit[i] = fitness(pop[i]);
      
      if( fit[i] < min ) {
	min = fit[i];
      }
      if( fit[i] > max ) {
	max = fit[i];
	
	bestWords = tmpWords;
	bestTransformation = pop[i];
	

	if ( abs(max) == threshold)
	  finished = true;
	maxInd = i;
	
      }
      
    }
    
    if ( sav_max < max){
      sav_max = max;
      if ( first_run){
	first_run = false;
	sav_first_max = max;
      }
      max_count = 0;
    } else if ( max == sav_first_max ){
      max_count = 0;
    } else if ( max_count <  num_of_occur ){
      max_count++;
    }else
      return dontknow;
      
    
    
    // print current results
    int divisor = 0;
    if ( cfg.penaltyRate() )
      divisor = 
	(pop[maxInd].length() > cfg.penaltyRate()) ? int(pop[maxInd].length()/cfg.penaltyRate() ) : 0;
    
    if ( finished ){
      out << "Finished at generation: " << g+1 
	  <<  "   Best fitness: " << -max << endl
	  << "The fittest word: " << endl;
      printWords( out, bestWords );
      
      return yes;
    }    
    
    if ( g >= g_sav ){
      out << "Generation: " << g+1
	  <<  "   Best fitness: " << -max << endl
	  << "The fittest word: " << endl;
      
      printWords( out, bestWords );
      //      out << "Other = " << total_tr << ", swithch = " <<  swithc_tr << endl;
      g_sav = g + 100;
    }
    
    // make fitness values suitable for Roulette wheel selection
    int base = abs(min) + 1;
    for( int i = 0; i < popSize; ++i )
      fit[i] += base;
    
    // fitness scaling
    if( cfg.haveFitnessScaling() )
      for( int i = 0; i < popSize; ++i )
	fit[i] = fit[i] * fit[i];
    

    // crossover
    RouletteWheel<int> wheel(popSize,fit);
    for( int i = 0; i < popSize; ++i ) {
      if( r.rand() <= crossRate ) {
	int i1 = wheel.GetIndex();
	int i2 = wheel.GetIndex();
	if ( cfg.crossoverType() == 1 ){
	  Word w = crossover1(pop[i1],pop[i2]);
	  if (w.length() > 30 )
	    newPop[i] = w.initialSegment(10);
	  else
	    newPop[i] = w;
	}
	else
	  error(Chars("Unknown crossover type: ") + Chars(cfg.crossoverType()));
      } 
      else {
	newPop[i] = pop[i];
      } 
    } 
    
    // mutation
    for( int i = 0; i < popSize; ++i ) {
      if( r.rand() <= mutRate ){
	Word w = mutate(newPop[i]);
	if (w.length() > 30 )
	  newPop[i] = w.initialSegment(10);
	else
	  newPop[i] = w;
      }
    }
    
    //    out << "." << flush;
    //    Roulette Wheel selection
    RouletteWheel<int> wheel1(popSize,fit);
    // elitist selection
    if( cfg.numOfElitistSelection() ) {
      newPop[0] = pop[maxInd];
      
      for( int i = 1; i < cfg.numOfElitistSelection(); ++i ) 
     	newPop[i] = pop[wheel1.GetIndex()];
      
    }
    
    //prepare for the next iteration
    for( int i = 0; i < popSize; ++i ) {
      pop[i] = newPop[i];
    } 
    
  }
  return no;
}

VectorOf<Word> GAIsPartOfBasis::computeAuto(const Word& u )
{
  Word v = u;
  int vLen = v.length();
  // make an image of transformations 
  transferImage = VectorOf<Word>(  numberOfGens  );
  VectorOf<Word> retAuto( numberOfGens );

  // add generators
  for ( int i = 0; i < numberOfGens; i++ ){
    transferImage[i] = Generator( i+1 );
    retAuto[i] = Generator( i+1 );
  }
  
  // apply transformations
  for ( int i = 0; i < vLen; i++ ){
    
    // get current transformation
    int trInd = abs(v[i])-1;
    Word tr = transformations[trInd];
    
    // get which generator to transfer
    int indexOfTransformed = int(trInd / (numberOfTrans / numberOfGens));
      
    // compute image
    transferImage[indexOfTransformed] = tr;
    for ( int n = 0;n<numberOfGens;n++){
      retAuto[n] = retAuto[n].replaceGenerators( transferImage ).freelyReduce();
    }
    transferImage[indexOfTransformed] = Generator(indexOfTransformed+1);
 }
  return retAuto;
}

int GAIsPartOfBasis::fitness( const Word& u, ostream& out, bool print ) 
{
  Word v = u;
  int vLen = v.length();
  tmpWords = theWords;
  VectorOf<Word> printSaveWords;
  
  int fit = 0;
  
  if (print){
    out << "Original words:\n";
    printWords( out,tmpWords );
  } 
  // make an image of transformations ti
  transferImage = VectorOf<Word>(  numberOfGens );
  // add generators
  for ( int i = 0; i < numberOfGens; i++ )
    transferImage[i] = Generator( i+1 );
  
  // apply transformations
  for ( int i = 0; i < vLen; i++ ){

    // @am tmp counters
    total_tr++;
    
    // get current transformation
    int trInd = abs(v[i])-1;
    Word tr = transformations[trInd];
    
    // get which generator to transfer
    int indexOfTransformed = int(trInd / (numberOfTrans / numberOfGens));
      
    // compute image
    transferImage[indexOfTransformed] = tr;
    bool switchGens = false;

    //swith generators transformation
    if ( (tr.length() == 1) && ( tr[0] != Generator(indexOfTransformed+1)) &&
	 ( tr[0] != inv(Generator(indexOfTransformed+1)))){
      if (power(tr[0])>0)
	transferImage[abs(tr[0])-1] = Generator(indexOfTransformed+1);
      else
	transferImage[abs(tr[0])-1] = inv(Generator(indexOfTransformed+1));

      total_tr--;
      swithc_tr++;
      switchGens = true;
    }
    VectorOf<Word> tmpImages( tmpWords.length() );
    for (int ii=0;ii<tmpWords.length(); ii++ )
      tmpImages[ii] = tmpWords[ii].replaceGenerators( transferImage ).freelyReduce();
    transferImage[indexOfTransformed] = Generator(indexOfTransformed+1);

    if (switchGens)
      transferImage[abs(tr[0])-1] = Word(Generator(abs(tr[0])));


      // print transformation if needed
    if ( print ) {
      if (printSaveWords != tmpImages ){

	out << "Applying Whitehead automorphism:\n  ";
	for ( int n=0; n < numberOfGens;n++){
	  if ( n != indexOfTransformed){
	    theGroup.printWord(out, Generator(n+1) );
	    out << " -> ";
	    theGroup.printWord(out, Generator(n+1) );
	  } else {
	    theGroup.printWord(out, Generator(n+1) );
	    out << " -> ";
	    theGroup.printWord(out, tr );	    
	  }
	  if ( n < numberOfGens-1)
	    out << ", ";
	}
	out << "\n\nResult:\n";
	printWords( out,tmpImages );
	printSaveWords = tmpImages;
      }
    }
    
    tmpWords = tmpImages;    
  }
    
  //tmpWord = tmpWord.cyclicallyReduce();
  if (fixedWords.length() == 0)
    fit = computeFitness1( tmpWords );
  else
    fit = computeFitness( tmpWords );
  
  
  // apply penalty
  if ( cfg.penaltyRate() ){
    int penalty = 
      (vLen > cfg.penaltyRate() ) ? int(vLen / cfg.penaltyRate() ) : 0;
    
    fit += penalty;
  }
  
  return -fit;
}

int GAIsPartOfBasis::computeFitness1( const VectorOf<Word>& v ) const
{
  int fit=0;
  for (int ii=0;ii < v.length();ii++)
    fit += v[ii].length();
  return fit;
}

int GAIsPartOfBasis::computeFitness( const VectorOf<Word>& v ) const
{

  VectorOf<Word> v1 = fixedWords;
  VectorOf<Word> v2 = v;

  int numberOfWords = v.length();
  int fit_tmp=0;
  int fit=MAXINT;

  for (int i=0;i<numberOfWords;i++){
    for (int j=i;j<numberOfWords;j++){
      fit_tmp = tupleHamingDistance(v1,v2);
      if (fit > fit_tmp)
	fit = fit_tmp;
      permuteVector(v1,j);
    }
  }
  return fit;
}

void GAIsPartOfBasis::permuteVector(VectorOf<Word>& v1, int move_to) const
{
  if (move_to >= v1.length() ||
      move_to < 0 )
    error("GAIsPartOfBasis::permuteVector(VectorOf<Word>& v1, int move_to)");
  if (move_to == v1.length()-1)
    return;
  Word w = v1[move_to];
  for (int i = move_to+1; i<v1.length(); i++)
    v1[i-1] = v1[i];
  v1[v1.length()-1] = w;
  return;
}

int GAIsPartOfBasis::tupleHamingDistance(const VectorOf<Word>& v1,
					 const VectorOf<Word>& v2) const
{
  if (v1.length() != v2.length() )
    error("GAIsPartOfBasis::tupleHamingDistance");
  int d = 0;
  
  for (int i=0;i<v1.length();i++)
    d+=hamingDistance(v1[i],v2[i]);
  
  return d;
}


int GAIsPartOfBasis::hamingDistance(const Word& w1,const Word& w2) const
{
  int minmalLength = min(w1.length(),w2.length());

  int distance = w1.length()-minmalLength+w2.length()-minmalLength;
  for (int i=0;i<minmalLength;i++)
    if (w1[i] != w2[i])
      distance++;
  return distance;
}

Word GAIsPartOfBasis::mutate( const Word& u )
{

  
  Word w = u;
  int wLen = w.length();
 
  int num = int(r.rand() * (wLen-1) );
 
  float op = r.rand();
  float mutation_segment = 0;
  if ( op > mutation_segment && 
       op <= cfg.chanceOfMutationAppend()+mutation_segment ){ 
    // append letter
    if ( wLen > 0 )
      w =  w * Word(Generator(genToInsertAfter( w[wLen-1])));
    else
      w =  w * Word( Generator(randomGen()) );
    return w;
  } 

  mutation_segment += cfg.chanceOfMutationAppend();
  
  if(  op > mutation_segment && 
       op <= cfg.chanceOfMutationInsert()+mutation_segment ) { 
    if (wLen == 0)
      return w;

   // insert a new letter to the word
    num = int(r.rand() * (wLen-1));
    Word a,b;
    if( num > 0 )
      a = w.initialSegment(num);
    if( num < wLen ) 
      b =  w.terminalSegment(wLen-num);
    
    //w =  a * Word( Generator(randomGen()) ) * b;
    
    if ( a.length() == 0 )
      w =  Word(Generator(genToInsertBefore(b[0]))) * b;
    else if ( b.length() == 0)
      w =  a * Word(Generator(genToInsertAfter(a[a.length()-1])));
    else 
      w =  a * Word(Generator(genToInsert(a[a.length()-1],b[0]))) * b;
    
    return w;
  }
  
  mutation_segment += cfg.chanceOfMutationInsert();
  
  if( op >  mutation_segment &&
      op <= cfg.chanceOfMutationDelete()+mutation_segment ) { 
    if (wLen == 0)
      return w;
    
    // delete one letter
    if( wLen > 0 ) {
      Word a,b;
      if( num > 0 )
	a = w.initialSegment(num);
      if( num < wLen - 1 ) 
	b =  w.terminalSegment(wLen-1-num);
      w = (a * b);

    }
    return w;
  }

  mutation_segment += cfg.chanceOfMutationDelete();
 
  if( op >  mutation_segment &&
      op <= cfg.chanceOfMutationChange()+mutation_segment )  { 
    // mutate one letter 
    
    if (wLen == 0)
      return w;

    //w[num] = Generator( randomGen() );
    
    if ( num > 0 && num < wLen - 1 )
      w[num] = Generator(genToInsert(w[num-1],w[num+1]));
    else if ( num == 0 )
      w[num] = Generator(genToInsertAfter(w[num]));
    else if ( num == wLen - 1)
      w[num] = Generator(genToInsertBefore(w[w.length()-1]));
    
   return  w;
   
  }

  return  w;
}



Word GAIsPartOfBasis::crossover1( const Word& w1,const Word& w2 )
{
  int len1 = w1.length();
  int len2 = w2.length();

  Word a,b;
  if( len1 > 0 ) {
   int num1 = 1+int(r.rand() * (len1-1));
   a = w1.initialSegment(num1);
  }
  if( len2 > 0 ) {
    int num2 = 1+int(r.rand() * (len2-1));
    b = w2.terminalSegment(num2);
  }
  
  Word res = a * b;
  return res;
}  



int GAIsPartOfBasis::randomGen( ) 
{
  int gen = int(r.rand() * (numberOfTrans-1)) + 1;
  return gen;
  //return (r.rand() <= 0.5) ? gen : -gen;
}

int GAIsPartOfBasis::genToInsertAfter( Generator g )
{
  while ( true ){
    int gen = randomGen( );
    if ( !reduces.bound( abs(g) - 1 ) )
      return gen;
    if ( reduces.valueOf(abs(g) - 1) != gen - 1 )
      return gen;
  }
}

int  GAIsPartOfBasis::genToInsertBefore( Generator g )
{
  while ( true ){
    int gen = randomGen( );
    if ( !reduces.bound( gen - 1 ) )
      return gen;
    if ( reduces.valueOf(gen - 1) != abs(g) - 1 )
      return gen;
  }
}

int  GAIsPartOfBasis::genToInsert( Generator g1, Generator g2 )
{
  while ( true ){
    int gen = genToInsertAfter( g1 );

    if ( !reduces.bound( gen - 1 ) )
      return gen;

    if ( reduces.valueOf(gen - 1) != abs(g2) - 1 )
      return gen;
  }  
}



Word GAIsPartOfBasis::randomWord( )
{
  Word w;
  do { 
    int vLen =  int(r.rand() * 50) + 1;
    //int vLen = maxWordLen/2;
    if ( vLen > 0 ) {
      VectorOf<Generator> v(vLen);
      v[0] = randomGen();
      for( int i = 1; i < vLen; ++i ){
	v[i] = genToInsertAfter( v[i-1]);
      }
      w = Word(v);
    }
  } while( w.length() == 0 );
  
  return  w;
}

void GAIsPartOfBasis::initialize()
{
 // 
  // transformations in F:
  // F = < x1, ... , xn >
  // t1: xi -> xi^-1,
  // t2: xi -> xi xj,
  // t3: xi -> xi xj,
  // t4: xi -> xi xj,
  // t5: xi -> xi xj,
  // t6: xi -> xj^-1 xi xj

  // numberOfTrans = n + 5n*(n-1), where n - number of generators
  numberOfTrans = numberOfGens + 5*numberOfGens*(numberOfGens - 1);
       
  transformations = VectorOf<Word>( numberOfTrans );
  
  // define names ( only for output )
  int counter = 0;

  //add generators transformation ( xi-> ... )
  for (int i = 0; i < numberOfGens; i++ ){
    Generator theGenerator( i+1 ); 
    
    // add x -> x^-1
    transformations[counter] = inv( theGenerator );
    counter++;
    // we want to avoid chains of type x -> x^-1 -> (x^-1)^-1 as it equal 
    // to x again, so we can reuce the second transformation
    reduces.bind( counter, counter);
    
    for ( int j = 0; j < numberOfGens; j++ )
      if ( i != j ){
	// add xi -> xi*xj, i != j
	transformations[counter] = theGenerator * Generator( j+1 );
	counter++;	
 	// add xi -> xi*xj^-1, i != j
	transformations[counter] = theGenerator * inv(Generator( j+1 ));
	counter++;
 	// add xi -> xj*xi, i != j
	transformations[counter] = Generator( j+1 ) * theGenerator;
	counter++;

	// try arrange transformations according to the process
	if (fixedWords.length() == 0){
	  // add xi -> xj^-1*xi, i != j
	  transformations[counter] = inv( Generator( j+1 )) * theGenerator;
	  counter++;
	} else {
	  // add xi -> xj, xj -> xi, i != j
	  transformations[counter] = Generator( j+1 );
	  counter++;	
	}
	// add xi -> xj, xj -> xi, i != j
	//transformations[counter] = Generator( j+1 );
	//counter++;

	// add xi -> xj^-1, xj -> xi^-1, i != j
	//	transformations[counter] = inv(Generator( j+1 ));
	//	counter++;	
   }
    
    //  add xi -> xj^-1 xi xj
    
    for ( int j = 0; j < numberOfGens; j++ )
      if ( i != j ){
	Generator g2(j+1);
	
	transformations[counter] = inv(g2) * theGenerator * g2;
	counter++;	
      }     
  } 
  /*  
  cout << numberOfTrans << ": " << endl;
  for ( int i = 0; i < counter; i++ ){
    cout << "t" << i + 1 << ": ";
    theGroup.printWord( cout, transformations[i] );
    cout << endl;
  }
    
  */  
}

void GAIsPartOfBasis::printWords(ostream& out, const VectorOf<Word>& v) const
{
  if (v.length() == 1){
    out << "  u" << " = ";
    theGroup.printWord(out, v[0] );
    out << "\n";
  }else{
    for (int kk=0;kk<v.length();kk++){
      out << "  u"<< kk+1 << " = ";
      theGroup.printWord(out, v[kk] );
      out << "\n";
    }
  }
  out << "\n";
  out << flush;
}

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

#include "ACGA.h"

//#ifndef BSD
//#include "values.h"
//#endif
//

#include "Roulette.h"
#include "File.h"
#include "conversions.h"
#include "GAIsPartOfBasis.h"
// ------------------------------- ACGA ------------------------------------ //

ACGA::ACGA( const FPGroup& G, const ACConfig& config )
    : theGroup( G ), 
      cfg( config ),
      numberOfRelators( G.getRelators().cardinality() ),
      numberOfGens( G.numberOfGenerators() ),
      relatorsVector( makeVectorOf( G.getRelators() ) ),
      useWhitehead( config.useWhitehead() )
{
  numberOfGenTransf = 
    useWhitehead ? 
    numberOfGens + 
    4*(numberOfGens*numberOfGens - numberOfGens) : 0; 
  
  numberOfRelTransf = numberOfRelators +
    numberOfRelators*numberOfRelators - numberOfRelators + 
    2*numberOfRelators*numberOfGens;
  
  

  numberOfTrans =  numberOfRelTransf + numberOfGenTransf;
  

  // transformations are words of special type in 
  // F = <t1, ... , t<numberOfTrans> >
  // and ti -> <one of the transformations>
  transformations = VectorOf<Word>( numberOfTrans );
  
  // define names ( only for output )
  trNames = VectorOf<Chars>( numberOfRelators + numberOfGens );
  for (int i = 0;i < numberOfGens; i++ )
    trNames[i] = theGroup.namesOfGenerators()[i];

  for (int i = numberOfGens;i < numberOfRelators+numberOfGens; i++ )
    trNames[i] = Chars("r") + Chars( i-numberOfGens);

  int counter = 0;
  

  // add relators transformations ( ri -> ... )
  for (int i = 0; i < numberOfRelators; i++ ){
    Generator theRelator( numberOfGens+i+1 ); 
    // add r -> r^-1
    transformations[counter] = inv( theRelator );
    counter++;
    // we want to avoid chains of type r -> r^-1 -> r^-1 as it equal 
    // to r again so we can reuce the second transformation
    reduces.bind( counter, counter);
    
    // add ri -> ri*rj, i != j
    for ( int j = 0; j < numberOfRelators; j++ )
      if ( i != j ){
	transformations[counter] = theRelator * Generator( numberOfGens+j+1 );
	counter++;	
      }
    
    // add r -> w^-1 * r * w
    for ( int j = 0; j < numberOfGens; j++ ){
      Generator g( j+1 );
      transformations[counter] = g * theRelator * inv( g );
      counter++;	    
      transformations[counter] = inv( g ) * theRelator * g;
      counter++;	    
      // again r -> w * r * w^-1 -> w^-1 * r * w -> r 
      // we don't want this two transformations to follow each other
      reduces.bind( counter, counter - 1);
      reduces.bind( counter - 1, counter);
    }
   
  }

  if ( useWhitehead ) 
    //add generators transformation ( xi-> ... )
    for (int i = 0; i < numberOfGens; i++ ){
      Generator theGenerator( i+1 ); 
      // add x -> x^-1
      transformations[counter] = inv( theGenerator );
      counter++;
      // we want to avoid chains of type x -> x^-1 -> x^-1 as it equal 
      // to x again so we can reuce the second transformation
      reduces.bind( counter, counter);
      
      for ( int j = 0; j < numberOfGens; j++ )
	if ( i != j ){
	  // add xi -> xi*xj, i != j
	  transformations[counter] = theGenerator * Generator( j+1 );
	  counter++;	
	  // add xi -> xj*xi, i != j
	  transformations[counter] = Generator( j+1 )*theGenerator;
	  counter++;	
	  // add xi -> xi*xj^-1, i != j
	  transformations[counter] = theGenerator * inv(Generator( j+1 ));
	  counter++;	
	  // add xi -> xj^-1*xi, i != j
	  //	  transformations[counter] = inv(Generator( j+1 ))*theGenerator;
	  //	  counter++;	
	}
      
      // add xi -> xj^-1*xi*xj, i != j
      for ( int j = 0; j < numberOfGens; j++ )
	if ( i != j ){
	  transformations[counter] = 
	    inv(Generator( j+1 )) * 
	    theGenerator * 
	    Generator( j+1 );
	  
	  counter++;	
	}
      
    } 
  
  
  /*
  cout << numberOfTrans << ": " << endl;
  FPGroup t( trNames ); 
  for ( int i = 0; i < numberOfTrans; i++ ){
    cout << i << ": ";
    t.printWord( cout, transformations[i] );
    cout << endl;
  }
    
  */
}


bool ACGA::transform( ostream& out, ostream& best_out  )
{
  bool best_printed = false;
  int popSize = cfg.populationSize();
  Word pop[popSize],newPop[popSize];
  int fit[50];
  ListOf<Word> random_conj[50];
  bool finished = false;
  int left_for_whitehead = cfg.numMinmizeAfterOf();
 
  out << "The algorithm tries to find an Andrews-Curtis reduction genetically. ";
  out << "The fitness value below is the minimum of total lengths of tuples"
      << " in the current generation."
      << endl << endl <<  endl; 
  

  best_out << "Starting with the tuple:" << endl;
  for (int i = 0;i < numberOfRelators; i++ ){
    best_out << "r" << i << " = ";
    theGroup.printWord(best_out,relatorsVector[i] );
    best_out << endl;
  }
  best_out << endl;
  
  int best_fit = -MAXINT;


  for (int i = 0; i < popSize; i++ )
    pop[i] = randomWord();
  
  //  for (int i = 0; i < popSize; i++ )
  //    fit[i] = fitness( pop[i] );
  
  // the main loop
  
  float crossRate = cfg.chanceOfCrossover();
  float mutRate = cfg.chanceOfMutation();
  int max, min, maxInd, g_sav = 0;
  min = MAXINT; max = -MAXINT;  maxInd = -1;
  
  for( g = 0; g < cfg.numOfGenerations(); ++g ) {
    /*
    if ( !left_for_whitehead ){
      applyWhitehead(out);
      left_for_whitehead = cfg.numMinmizeAfterOf();
    } else {
      left_for_whitehead--;
    }
    */
    min = MAXINT; 
    VectorOf<Word> nielsenRel(numberOfRelators);
    
    max = -MAXINT;  maxInd = -1;
      
    // compute fitness values
    for( int i = 0; i < popSize; ++i ) {
      
      fit[i] = fitness(pop[i],random_conj[i]);
      
      if( fit[i] < min ) {
	min = fit[i];
      }
      if( fit[i] > max ) {
	//	cout << max << " <- " << fit[i] << endl;
	max = fit[i];
	currentBest = transferImage;
	best_printed = false;
	if ( best_fit < max ){
	  best_fit = max;
	  best_out << endl << "Generation : " << g << "    ";
	  best_out << "New Best Fitness :" << -max << endl;
	  //	  best_out << "Best transformation :" << pop[i] << endl << endl;
	  print( pop[i], random_conj[i], best_out );
	  best_printed = true;
	}

	// apply Nielsen transformations
	FreeGroup F( numberOfGens );
	VectorOf<Word> newRel(numberOfRelators);
	
	for (int k=0;k<numberOfRelators;k++) 
	  newRel[k] = currentBest[k+numberOfGens];
	
	nielsenRel =  F.nielsenBasis(newRel);

	int lSum = 0;
	// check the lengths of Nielsen basis
	for (int k=0;k<numberOfRelators;k++) 
	  lSum += (nielsenRel[k]).length();
	//  lSum += (newRel[k]).length();

	int penalty = 0;
	if ( cfg.penaltyRate() )
	  penalty  = 
	    (pop[i].length() > cfg.penaltyRate()) ? int(pop[i].length()/cfg.penaltyRate() ) : 0;
	
	maxInd = i;

	//	if ( cfg.useSumFitness() ) {
	  // if equal to the number of generators we have an answer
	  if (lSum == numberOfGens){
	    finished = true;
	    break;
	  }
// 	} else
// 	  // if one of relators is equal to 1
// 	  if (max == -1 - penalty){
// 	    finished = true;
// 	    break;
// 	  }
      }
    }
    
    // print current results

    int divisor = 0;
    if ( cfg.penaltyRate() )
      divisor = 
	(pop[maxInd].length() > cfg.penaltyRate()) ? int(pop[maxInd].length()/cfg.penaltyRate() ) : 0;


    //    if ( max == -numberOfGens - divisor ){
    if ( finished ){
      out << endl << endl << "Finished at generation: " << g <<  "   Fitness: " 
	//	  << max << ", penalty = " << divisor << "." << endl
	  << -max <<  "." << endl;
      //	  << "The relations are :" << endl;
      
      //      out << "Curent best transformation :" << pop[maxInd] << endl;
      
      finalResult = pop[maxInd];
      
      /*      print( pop[maxInd],random_conj[maxInd], out );
      for ( int j = 0;j<numberOfRelators;j++){
	theGroup.printWord(out,currentBest[j+numberOfGens]);
	out << " -> ";
	theGroup.printWord(out,nielsenRel[j]);
	out << "       ( Nielsen transformations )" << endl;
      }
      
      
      print( pop[maxInd],random_conj[maxInd], best_out);
      */
      if ( !best_printed ) {
	best_out << endl << "Generation : " << g << "    ";
	best_out << "New Best Fitness :" << -max << endl;
	print( pop[maxInd], random_conj[maxInd], best_out );
      }
      return true;
    }    
    
    if ( g > g_sav ){
      out << endl << endl;
      out << endl <<  "Generation: " << g << "   Fitness: " 
	//	  << max << ", penalty = " << divisor
	  << -max << endl << ready;

      out << "Current best relations are :" << endl;
      
      for ( int j = 0;j<numberOfRelators;j++){
	out << "r" << j << " -> ";
	theGroup.printWord(out,currentBest[j+numberOfGens]);
	out << endl;
      }
      //      out << "Curent best transformation :" << pop[maxInd] << endl
      //	  <<  endl << endl;

      g_sav = g + 100;
    }
    // << "Index: " << maxInd << endl;
    
    
    
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
	if ( cfg.crossoverType() == 1 )
	  newPop[i] = crossover1(pop[i1],pop[i2]);
	else if ( cfg.crossoverType() == 2 )
  	  newPop[i] = crossover2(pop[i1],pop[i2]);
	else
	  error(Chars("Unknown crossover type: ") + Chars(cfg.crossoverType()));
    } 
      else {
	newPop[i] = pop[i];
      } 
    } 

    // mutation
    for( int i = 0; i < popSize; ++i ) {
      if( r.rand() <= mutRate )
	newPop[i] = mutate(newPop[i]);
    }

    // Strong elittest selection
    //    int newFit[popSize];
    
    //  for( int i = 0; i < popSize; ++i ) {
    //   newFit[i] = fitness( newPop[i] );
      //  newFit[i] *= newFit[i];
    // }
    
    //  for( int i = 0; i < popSize; ++i ) 
    //  for( int j = 0; j < popSize; ++j ) 
    //	if( newFit[i] > fit[j] ) {
    //	  fit[j] = newFit[i];
    //	  pop[j] = newPop[i];
    //  break;
    //}
    
    //    out << "." << flush;
    //    Roulette Wheel selection
    RouletteWheel<int> wheel1(popSize,fit);
    // elitist selection
    if( cfg.numOfElitistSelection() > 0 ) {
      pop[0] = pop[maxInd];
      random_conj[0] = random_conj[maxInd];
      
      /*     for( int i = 1; i < cfg.numOfElitistSelection(); ++i ) 
	     newPop[i] = pop[wheel1.GetIndex()];
      */
      
    }
    
    //prepare for the next iteration
    for( int i = 1; i < popSize; ++i ) {
      pop[i] = newPop[i];
      random_conj[i] = ListOf<Word>();
    } 
    
  }
  return false;
}




int ACGA::fitness( Word& u,ListOf<Word>& conjl ) 
{
  Word v = u;
  int vLen = v.length();
  FPGroup t( trNames ); 
  Word transformed;
  int indexOfTransformed = 0;
  bool updateGens = false;
  Chars randomTransfName;
  int best_fit = MAXINT;;
  int best_tr_index = 0;
  int fit = cfg.useSumFitness() ? 0 : MAXINT;

  // make an image of transformations ( ti -> w(r,x) )
  transferImage = VectorOf<Word>( numberOfRelators + numberOfGens );
  // image for Whitehesad transformations
  VectorOf<Word> gensTransformed( numberOfGens );
  for (int k=0;k<numberOfGens;k++)
    gensTransformed[k] = Generator( k+1 );

  // add generators
  for ( int i = 0; i < numberOfGens; i++ )
    transferImage[i] = Generator( i+1 );
  // add relators
  for ( int i = 0; i < numberOfRelators; i++ )
    transferImage[i+numberOfGens] = relatorsVector[i];
  
  // apply transformations
  for ( int i = 0; i < vLen; i++ ){
    int trInd = abs(v[i])-1;
    updateGens = false; 
    indexOfTransformed = 0;
    Word tr;

    if ( trInd > numberOfTrans - 1) {
      // make addidional random transformations
      // transformation index [0,numberOfRelators*2)
      int trans_index = trInd - numberOfTrans;

      // compute index of the relator to be changed
      indexOfTransformed = int((trans_index)/numberOfRelators);

      // transformation type [0,1]
      // 0 - random permutation
      // 1 - conjugate by a random word
      int transf_type = trans_index - indexOfTransformed*2;
      indexOfTransformed += numberOfGens;
     if ( transf_type ){
	Word conj = randomWordInGroup();
	conjl.append(conj);
	transformed = 
	  (conj * 
	   transferImage[ indexOfTransformed ] * 
	   conj.inverse()).freelyReduce();
	randomTransfName = "Conjugate by a word";
      } else {
	Word theRelator = transferImage[ indexOfTransformed ];
	int numOfPer =  int(r.rand() * (theRelator.length() - 2) )+1;
	
	Word conj = (theRelator.initialSegment(numOfPer).inverse());
	conjl.append(conj);

	transformed = 
	  (conj * 
	   transferImage[ indexOfTransformed ] * 
	   conj.inverse()).freelyReduce();
	randomTransfName = "Make a permutation";
      }
      
    } else {
      if (power(v[i]) > 0 )
	tr = transformations[trInd];
      else
	tr = transformations[trInd].inverse();
      
      // get what we will transfer generator or a relator
      if ( trInd < numberOfRelTransf ){
	// transfer relator
	int relInd = ( trInd ) / 
	  ( (numberOfRelTransf) / numberOfRelators );
	
	transformed = tr.replaceGenerators( transferImage );
	indexOfTransformed = relInd + numberOfGens;
      } else {
	// transfer generator
	int genInd = ( trInd-numberOfRelTransf ) / 
	  ( (numberOfGenTransf) / numberOfGens );
	
	updateGens = true; 
	transformed = tr.replaceGenerators( gensTransformed );
	//      transformed = tr;
	indexOfTransformed = genInd;     
      }
    } // end else
    
    if (updateGens){
      gensTransformed[ indexOfTransformed ] = transformed.freelyReduce();
    }  
    else
      transferImage[ indexOfTransformed ] = transformed.freelyReduce();


  }
  
  fit = cfg.useSumFitness() ? 0 : MAXINT;
  
  for ( int i = 0;i<numberOfRelators;i++){
    // substitute images of the generators into relators
    if (useWhitehead)
      transferImage[i+numberOfGens] = 
	(transferImage[i+numberOfGens].replaceGenerators( gensTransformed )).
	freelyReduce();
  }  

  for ( int i = 0;i<numberOfRelators;i++){
    
    if ( cfg.useSumFitness() ){
      int update = 0;
      update = (transferImage[i+numberOfGens]).length();
      fit += update;
    }else{
      int update = (transferImage[i+numberOfGens]).length();
      /*      for ( int k = 0; k < numberOfRelators; k++){
	      int update = MAXINT;
	      if ( i != k )
	      update = (transferImage[i+numberOfGens]).length() +  
	      (transferImage[k+numberOfGens]).length();
      */	
      if ( fit > update )
	fit = update;
      //}
    }
    
  }

  // apply penalty
  if ( cfg.penaltyRate() ){
    int penalty = 
      (vLen > cfg.penaltyRate() ) ? int(vLen / cfg.penaltyRate() ) : 0;
    
    fit += penalty;
  }
  
  
  return -fit;
}


Word ACGA::mutate( const Word& u )
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

 mutation_segment += cfg.chanceOfMutationChange();
 
  if( op >  mutation_segment &&
      op <= cfg.chanceOfMutationPermute() + mutation_segment )  { 
    // prepend permutation
    
    Word w1 = randomPermutation() * w;
    
    return  w1;
   
  }
  return  w;
}



Word ACGA::crossover1( const Word& w1,const Word& w2 )
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


Word ACGA::crossover2( const Word& w1,const Word& w2 )
{
  int len1 = w1.length();
  int len2 = w2.length();
  int minLen, maxLen;
  Word smallW, bigW;
  if ( len1 > len2 ){
    minLen = len2;
    smallW = w2;
    bigW = w1;
    maxLen = len1;
  } else {
    minLen = len1;
    smallW = w1;  
    bigW = w2;
    maxLen = len2;   
  }

  Word a,b,c;
  if( minLen > 0 ) {
    int num1 = 1+int(UniformRandom().rand() * (minLen-1));
    int num2 = 1+int(UniformRandom().rand() * (minLen-1));
    for (int i = min(num1,num2); i< max(num1,num2);i++ )
      c *=smallW[i];
  }
  if( maxLen > 0 ) {
    int num1 = 1+int(UniformRandom().rand() * (maxLen-1));
    int num2 = 1+int(UniformRandom().rand() * (maxLen-1));
    a = bigW.initialSegment(min(num1,num2));
    b = bigW.initialSegment(min(num1,num2));


  }
  
  Word res = a * c * b;
  return res;
}  


int ACGA::randomGen( ) 
{
  // numberOfRelators*2 - two additional 
  // transformations for each relator:
  // 1 - make random permutation
  // 2 - conjugate by a random word.
  int gen = int(r.rand() * (numberOfTrans-1 + numberOfRelators*2)) + 1;
  //int gen = int(r.rand() * (numberOfTrans-1 )) + 1;
  return gen;
  //return (r.rand() <= 0.5) ? gen : -gen;
}

Word ACGA::randomPermutation( ) 
{
  
  int rel = int(r.rand() * (numberOfRelators - 1) );
  Word theRelator = relatorsVector[rel];
  int numOfPer =  int(r.rand() * (theRelator.length() - 1) );
  Word retW;

  for ( int j=0; j < numOfPer;j++) {
    Generator g = theRelator[0];
    Word w = Word(inv(g) * Generator(numberOfGens+rel+1) * g);
    int i = transformations.indexOf( w );
    if ( i < 0 ) 
      error("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
    Generator permGen( i+1 );
    retW *= permGen;
  }
  return retW;
  
}

int  ACGA::genToInsertAfter( Generator g )
{
  while ( true ){
    int gen = randomGen( );
    if ( !reduces.bound( abs(g) - 1 ) )
      return gen;
    if ( reduces.valueOf(abs(g) - 1) != gen - 1 )
      return gen;
  }
}

int  ACGA::genToInsertBefore( Generator g )
{
  while ( true ){
    int gen = randomGen( );
    if ( !reduces.bound( gen - 1 ) )
      return gen;
    if ( reduces.valueOf(gen - 1) != abs(g) - 1 )
      return gen;
  }
}

int  ACGA::genToInsert( Generator g1, Generator g2 )
{
  while ( true ){
    int gen = genToInsertAfter( g1 );

    if ( !reduces.bound( gen - 1 ) )
      return gen;

    if ( reduces.valueOf(gen - 1) != abs(g2) - 1 )
      return gen;
  }  
}



Word ACGA::randomWord( )
{
  Word w;
  do { 
    int vLen =  int(r.rand() * 20 ) + 1;
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

Word ACGA::randomWordInGroup( )
{
  Word w;
  do { 
    int vLen =  int(r.rand() * 5 ) + 1;
    //int vLen = maxWordLen/2;
    if ( vLen > 0 ) {
      VectorOf<Generator> v(vLen);
      v[0] = int(r.rand() * (numberOfGens-1 )) + 1;
      v[0] =  (r.rand() <= 0.5) ? v[0] : inv(v[0]);
      
      for( int i = 1; i < vLen; ++i ){
	v[i] = int(r.rand() * (numberOfGens-1 )) + 1;
	v[i] =  (r.rand() <= 0.5) ? v[i] : inv(v[i]);
      }
      w = Word(v);
    }
  } while( w.length() == 0 );
  
  return  w.freelyReduce();
}

void   ACGA::applyWhitehead( ostream& out){
  /*
  ACConfig c( 50, 1000, 1, 
	      85, 95, 1, 
	      0, 1,
	      75, 5,15,5,0,0
	      );
  VectorOf<Word> bestRelators(numberOfRelators);
  for ( int i = 0; i < numberOfRelators; i++ )
    bestRelators[i] = currentBest[i+numberOfGens];

  FreeGroup fg(theGroup.namesOfGenerators());
  out << "\nApply Whitehead automorphisms ...\n";
  GAIsPartOfBasis minimize(fg,c,bestRelators);
  
  //  File f;
  ofstream of("result");

  minimize.isPartOfBasis(of,of);
  
  of.close();

  VectorOf<Word> tmpWords = minimize.getBestWords();
  
  out << "Results:\n";
  for (int i=0; i < relatorsVector.length();i++){
    theGroup.printWord(out,relatorsVector[i]);
    out << " -> ";
    theGroup.printWord(out,tmpWords[i]);
    out << endl;
  }
  //  out << "Details in the file : " << of.getFileName() << endl;
  out << endl;

  relatorsVector = tmpWords;
  */
}


void ACGA::print( Word& u, const ListOf<Word>& conj, ostream& out ) 
{
  Word v = u;
  int vLen = v.length();
  ListIterator<ListOf<Word> > I(conj);
  FPGroup t( trNames ); 
  Word transformed;
  int indexOfTransformed = 0;
  bool updateGens = false;
  Chars randomTransfName;
  int best_fit = MAXINT;;
  int best_tr_index = 0;
  int counter = 1;
  int fit = cfg.useSumFitness() ? 0 : MAXINT;

  // make an image of transformations ( ti -> w(r,x) )
  transferImage = VectorOf<Word>( numberOfRelators + numberOfGens );
  // image for Whitehesad transformations
  VectorOf<Word> gensTransformed( numberOfGens );
  for (int k=0;k<numberOfGens;k++)
    gensTransformed[k] = Generator( k+1 );

  // add generators
  for ( int i = 0; i < numberOfGens; i++ )
    transferImage[i] = Generator( i+1 );
  // add relators
  for ( int i = 0; i < numberOfRelators; i++ )
    transferImage[i+numberOfGens] = relatorsVector[i];
  
  // apply transformations
  for ( int i = 0; i < vLen; i++ ){
    int trInd = abs(v[i])-1;
    updateGens = false; 
    indexOfTransformed = 0;
    Word tr;

    if ( trInd > numberOfTrans - 1) {
      // make addidional random transformations
      // transformation index [0,numberOfRelators*2)
      int trans_index = trInd - numberOfTrans;

      // compute index of the relator to be changed
      indexOfTransformed = int((trans_index)/numberOfRelators);

      // transformation type [0,1]
      // 0 - random permutation
      // 1 - conjugate by a random word
      int transf_type = trans_index - indexOfTransformed*2;
      indexOfTransformed += numberOfGens;
      if ( I.done() )
	error(" Doesn't have proper conjugator initialized");
      
      Word conj = I.value();
      transformed = 
	(conj * 
	 transferImage[ indexOfTransformed ] * 
	 conj.inverse()).freelyReduce();
      
      I.next();

      if ( transf_type ){
	randomTransfName = "Conjugate by a word";
      } else {
	randomTransfName = "Make a permutation";
      }
      
    } else {
      if (power(v[i]) > 0 )
	tr = transformations[trInd];
      else
	tr = transformations[trInd].inverse();
      
      // get what we will transfer generator or a relator
      if ( trInd < numberOfRelTransf ){
	// transfer relator
	int relInd = ( trInd ) / 
	  ( (numberOfRelTransf) / numberOfRelators );
	
	transformed = tr.replaceGenerators( transferImage );
	indexOfTransformed = relInd + numberOfGens;
      } else {
	// transfer generator
	int genInd = ( trInd-numberOfRelTransf ) / 
	  ( (numberOfGenTransf) / numberOfGens );
	
	updateGens = true; 
	transformed = tr.replaceGenerators( gensTransformed );
	//      transformed = tr;
	indexOfTransformed = genInd;     
      }
    } // end else
    if ( indexOfTransformed >= numberOfGens){
      out << counter << ": ";
      if (trInd > numberOfTrans - 1){
	out << randomTransfName << ".";
      } else {
	out << "Apply ";
	out << trNames[indexOfTransformed];
	out << " -> ";
	t.printWord( out, tr );
	out << " transformation.";
      }
      out << endl<< "   ";
      out << trNames[indexOfTransformed] << " = ";
      theGroup.printWord(out, transferImage[ indexOfTransformed ]);
      out << " -> ";
      theGroup.printWord(out, transformed.freelyReduce() );
      out << endl << endl;
      
      counter++;
    }
    if (updateGens){
      gensTransformed[ indexOfTransformed ] = transformed.freelyReduce();
    }  
    else
      transferImage[ indexOfTransformed ] = transformed.freelyReduce();
    
    
  }
  
  VectorOf<Word> newRel(numberOfRelators);
  
  out << counter << ": Apply ";
  for (int i=0; i<numberOfGens;i++ ){
    out << trNames[i] << " -> ";
    theGroup.printWord(out,gensTransformed[i]);
    out << ", ";
  }
  out << endl<< "   ";
  
  for ( int i = 0;i<numberOfRelators;i++){
    // substitute images of the generators into relators
    if (useWhitehead)
      transferImage[i+numberOfGens] = 
	(transferImage[i+numberOfGens].replaceGenerators( gensTransformed )).
	freelyReduce();
    
    newRel[i] = transferImage[i+numberOfGens];
    out << "r" << i << " -> ";
    theGroup.printWord(out,newRel[i]);
    out << endl<< "   ";
  }

  out << endl;
  
  // apply Nielsen transformations
  FreeGroup F( numberOfGens );
  out << "Apply  Nielsen transformations." << endl; 
  VectorOf<Word> nielsenRel =  F.nielsenBasis(newRel);
  
  for ( int i = 0;i<numberOfRelators;i++){
    out << "r" << i << " = ";
    theGroup.printWord(out,newRel[i]);
    out << " -> ";
    theGroup.printWord(out,nielsenRel[i]);
    out << endl << endl;
  }
}


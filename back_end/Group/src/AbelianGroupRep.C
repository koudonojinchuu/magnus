// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the AbelianGroupRep class.
//
// Principal Author: Dmitry Bormotov, Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//
// Special Notes:
//


//#define debug_AbelianGroup

#include "SmithNormalForm1.h"
#include "GaussTransformation.h"
#include "MatrixComputations.h"
#include "PrimeNumbers.h"
#include "File.h"
#include "AbelianSGPresentation.h"
#include "conversions.h"
#include "AbelianEquations.h"
#include "WordEnumerator.h"

// ---------------------------- AbelianGroupRep ---------------------------- //

const int PRIMES=0, POWERS=1, NEWTOOLD = 2, INV = 3, OLDTONEW = 4;
// Here is enumerating for columns of primeBasicMatrix, it was done
// for easy acces.
// @am I should change this some time.

AbelianGroupRep::AbelianGroupRep( const FPGroup& G, bool makeFile )
  : theGroup( G ),
    bMakeFile( makeFile ),
    bHaveCyclicDecomposition( false ),
    primeBasisFound( false )
{
/*
  if ( G.getRelators().cardinality() == 0 ) {
    rankOfFreePart = G.numberOfGenerators();
    bHaveCyclicDecomposition = true;
  }
*/
}


void AbelianGroupRep::fillTransformationVectors( )
  // * The transformation matrices are suppose to exist when
  //   one calls that function.
{
  int numOfGens = theGroup.numberOfGenerators();
  int size = min(width, height);

  VectorOf<Integer> diag(width);
  for( int i = 0; i < size; ++i )
    diag[i] = matrix[i][i];
  for( int i = size; i < width; ++i )
    diag[i] = 0;
  
  VectorOf<Integer> zero(numOfNewGens);
  for( int i = 0; i < numOfNewGens; ++i )
    zero[i] = 0;
  
  theOldToNewGens = VectorOf<AbelianWord>( numOfGens );
  for( int i = 0; i < numOfGens; ++i )
    theOldToNewGens[i] = zero;
  
  int num = 0;
  
  for( int i = 0; i < width; ++i )
    if( sign(diag[i]) == 0 ) {
      for( int j = 0; j < width; ++j )
	theOldToNewGens[j][num] = oldToNewGensMatrix[j][i];
      ++num;
    }

  for( int i = 0; i < width; ++i )
    if( diag[i] > 1 ) {
      for( int j = 0; j < width; ++j )
	theOldToNewGens[j][num] = oldToNewGensMatrix[j][i];
      ++num;
    }

  for( int i = 0; i < width; ++i )
    minimizeWordInNewGens(theOldToNewGens[i]);

  zero = VectorOf<Integer>(width);
  for( int i = 0; i < width; ++i )
    zero[i] = 0;

  theNewToOldGens = VectorOf<AbelianWord>( numOfNewGens );
  for( int i = 0; i < numOfNewGens; ++i )
    theNewToOldGens[i] = zero;
  num = 0;

  for( int i = 0; i < width; ++i )
    
    if( sign(diag[i]) == 0 ) {
      
      for( int j = 0; j < width; ++j )
	theNewToOldGens[num][j] = newToOldGensMatrix[i][j];
      ++num;
    }
  
  for( int i = 0; i < width; ++i )

    if( diag[i] > 1 ) {

      for( int j = 0; j < width; ++j )
	theNewToOldGens[num][j] = newToOldGensMatrix[i][j];
 
     ++num;
    }
}


void AbelianGroupRep::computeCyclicDecomposition( )
{
  if( bHaveCyclicDecomposition ) return;

  height = theGroup.getRelators().cardinality();
  width = theGroup.numberOfGenerators();
  makeTransformationMatrices();
  makeMainMatrix();
  int i, j;
   
    // The main algorithm.
  
  i = j = 0;
  int k, l, done;
  Integer quotient;

  while ( i < height && j < width ) {
    
    // Check if j^{th} generator is free of relations in abelianization.
    k = 0;
    if ( sign(matrix[i][j]) == 0 )
      for ( k = i + 1; k < height; k++ )
	if ( sign(matrix[k][j]) != 0 ) {
	  Integer *temp = matrix[i];
	  matrix[i] = matrix[k];
	  matrix[k] = temp;
	  break;
	}
    
    if ( k == height ) {
      for ( k = j + 1; k < width; k++ )
	if ( sign(matrix[i][k]) != 0 ) {
	  swapColumns(j, k);
	  break;
	}
      if ( k == width ) {
	++i;
	++j;
	continue;
      }
    }
    done = 0;
    while ( !done ) {
      for ( k = i + 1; k < height; k++ )
	while ( sign(matrix[k][j]) != 0 )
	  if ( abs( matrix[k][j] ) < abs( matrix[i][j] ) ) {
	    Integer *temp = matrix[i];
	    matrix[i] = matrix[k];
	    matrix[k] = temp;
	  }
	  else {
	    quotient = matrix[k][j] / matrix[i][j];
	    for ( l = j; l < width; l++ ) 
	      matrix[k][l] = (Integer)matrix[k][l] - quotient*matrix[i][l];
	  }

      done = 1;
      for ( k = j + 1; k < width; k++ )
	while ( sign(matrix[i][k]) != 0 )
	  if ( abs( matrix[i][k] ) < abs( matrix[i][j] ) ) {
	    done = 0;  // Possibly we spoiled j^{th} column.
	    quotient = ( matrix[i][j] - sign(matrix[i][j]) ) / matrix[i][k];
	    addColumn(j, k, -quotient);
	  }
	  else {
	    quotient = matrix[i][k] / matrix[i][j];
	    addColumn(k, j, -quotient);
	  }
    }
    ++i;
    ++j;
  }


    // The computation is finished, now it's time to prepare results.

  int size = min ( width, height);
  rankOfFreePart = width - j;
  
  for (i = 0; i < size; i++)
    matrix[i][i] = abs(matrix[i][i]);
  
  int pos = 0;
  for ( i = pos; i < size; i++ )
    if ( sign(matrix[i][i]) == 0) {
      swapInvariants(pos, i);
      pos++;
    }
  
  rankOfFreePart += pos;
  for ( i = pos; i < size; i++ )
    if ( matrix[i][i] == 1) {
      swapInvariants(pos, i);
	pos++;
    }
  
  // m_i | m_{i+1}
  for ( i = pos; i < size - 1; i++ )
    for ( j = i + 1; j < size; j++ )
      canoniseInvariants(i, j);

  
    // Calculate invariants of the cyclic decomposition.

  int invNumber = 0;
  for( i = 0; i < size; ++i )
    if( matrix[i][i] > 1 )
      ++invNumber;
  

  numOfNewGens = rankOfFreePart + invNumber;
  theInvariants = VectorOf<Integer>(invNumber);
  
  invNumber = 0;
  for( i = 0; i < size; ++i )
    if( matrix[i][i] > 1 )
      theInvariants[invNumber++] = matrix[i][i];
  
  fillTransformationVectors();
  
#ifdef debug_AbelianGroup
  printMatrix("Main: ", matrix, height, width);
  printMatrix("NewToOLd: ", newToOldGensMatrix, width, width);
  printMatrix("OldToNew: ", oldToNewGensMatrix, width, width);
#endif

    // Delete the matrices, prepare the file with new generators and exit
  
  destroyMatrices();
  
  bHaveCyclicDecomposition = true;

  if( bMakeFile )
    makeFile();

}


void AbelianGroupRep::addColumn( int i, int j, Integer k )
{
  for ( int l = 0; l < height; l++ )
    matrix[l][i] += k * matrix[l][j];
  
  for ( int l = 0; l < width; l++ ) {
    newToOldGensMatrix[j][l] -= k * newToOldGensMatrix[i][l];
    oldToNewGensMatrix[l][i] += k * oldToNewGensMatrix[l][j];
  }
}


void AbelianGroupRep::swapGenColumns( int i, int j )
{
  for ( int l = 0; l < width; l++ ) {
    Integer temp = oldToNewGensMatrix[l][i];
    oldToNewGensMatrix[l][i] = oldToNewGensMatrix[l][j];
    oldToNewGensMatrix[l][j] = temp;
  }

  Integer *tmp = newToOldGensMatrix[i];
  newToOldGensMatrix[i] = newToOldGensMatrix[j];
  newToOldGensMatrix[j] = tmp;
}


void AbelianGroupRep::swapColumns( int i, int j )
{
  for ( int l = 0; l < height; l++ ) {
    Integer temp = matrix[l][i];
    matrix[l][i] = matrix[l][j];
    matrix[l][j] = temp;
  }
  swapGenColumns(i, j);
}


void AbelianGroupRep::swapInvariants( int i, int j )
{
  Integer temp = matrix[i][i];
  matrix[i][i] = matrix[j][j];
  matrix[j][j] = temp;
  swapGenColumns(i, j);
}


void AbelianGroupRep::canoniseInvariants( int i, int j )
{
  Integer quotient;
  addColumn(i, j, 1);
  while ( matrix[i][i] != matrix[j][i]) {
    if ( matrix[i][i] > matrix[j][i]) {
      quotient = (matrix[i][i] - 1) / matrix[j][i];
      matrix[i][i] -= quotient * matrix[j][i];
      matrix[i][j] -= quotient * matrix[j][j];
    }
    else {
      quotient = (matrix[j][i] - 1) / matrix[i][i];
      matrix[j][i] -= quotient * matrix[i][i];
      matrix[j][j] -= quotient * matrix[i][j];
    }
  }
  matrix[j][i] -= matrix[i][i];
  matrix[j][j] -= matrix[i][j];

  quotient = matrix[i][j] / matrix[i][i];
  addColumn(j, i, -quotient);
  matrix[j][j] = abs(matrix[j][j]);
}


void AbelianGroupRep::makeFile( )
{
  File file;
   file.setColor(titleColor);
   file << "New generators expressed in terms of the given generators:" << endl;  
   file.setColor(mainColor);
 
  for( int i = 0; i < rankOfFreePart; ++i ) {
    file << "f" << i+1 << " = ";
    theGroup.printWord
      ( file, newToOldGens
	( AbelianWord( numOfNewGens, Word(Generator(i+1)) ) ).getWord() );
    file << endl;
  }
  
  for( int i = rankOfFreePart; i < numOfNewGens; ++i ) {
    file << "t" << i - rankOfFreePart + 1 << " = ";
    theGroup.printWord
      ( file, newToOldGens
	( AbelianWord( numOfNewGens, Word(Generator(i+1)) ) ).getWord() );
    file << endl;
    file << "The order of t" << i - rankOfFreePart + 1 << " is " 
	 << theInvariants[i-rankOfFreePart] << endl; 
  }
  file << endl;
  file.setColor(titleColor);
  file << "Given generators expressed in terms of the generators for the canonical decomposition:" << endl;  
  file.setColor(mainColor);

  VectorOf<AbelianWord> oldtonew = oldToNewGens();
  int numOfOldGens = theGroup.numberOfGenerators();
  for (int i=0;i<numOfOldGens;i++){
    theGroup.printWord( file,Word(Generator(i+1) ));
    file << " = ";
    printWordInNewGens(file,oldtonew[i]);   
    file << endl;
  }

  file << end;
  theFileName = file.getFileName();
}


Chars AbelianGroupRep::getFileName( ) const
{
#if SAFETY > 0
  if( !bMakeFile )
    error("Chars AbelianGroupRep::getFileName( ) : "
	  "The file has not been created.");
#endif
  
  return theFileName;
}


int AbelianGroupRep::rankOfFreeAbelianFactor( ) const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "int AbelianGroupRep::rankOfFreeAbelianFactor( ) const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif
  
  return rankOfFreePart;
}


VectorOf<Integer> AbelianGroupRep::invariants( ) const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "VectorOf<Integer> AbelianGroupRep::invariants( ) const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif

  return theInvariants;
}


VectorOf<AbelianWord> AbelianGroupRep::oldToNewGens() const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "VectorOf<AbelianWord> AbelianGroupRep::oldToNewGens() const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif

  return theOldToNewGens;
}
    

VectorOf<AbelianWord> AbelianGroupRep::newToOldGens() const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "VectorOf<AbelianWord> AbelianGroupRep::newToOldGens() const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif

  return theNewToOldGens;
}
    

Integer AbelianGroupRep::order() const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "Integer AbelianGroupRep::order() const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif

  if( rankOfFreePart != 0 )
    return 0;
  else
    return orderOfTheTorsionSubgroup();
}


bool AbelianGroupRep::isTrivial() const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "bool AbelianGroupRep::isTrivial() const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif

  return (numOfNewGens == 0);
}


bool AbelianGroupRep::isFree() const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "bool AbelianGroupRep::isFree() const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif

  return (theInvariants.length() == 0);
}


bool AbelianGroupRep::isomorphicTo( const AbelianGroupRep& G) const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "bool AbelianGroupRep::isIsomorphicTo(const AbelianGroup&) const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif
  
  return ( rankOfFreePart == G.rankOfFreePart
	   && theInvariants == G.theInvariants );
}

AbelianGroupRep AbelianGroupRep::computeIntegralHomology( int n ) const 
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "AbelianGroup AbelianGroupRep::integralHomology( int ) const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif

  VectorOf< VectorOf< VectorOf<int> > > hSeries( n + 1 );
  
  if( n == 0 )
    {
      VectorOf<Chars> gens(1);
      gens[0] = Chars('x');
      return AbelianGroupRep( FPGroup( gens ) );
    }
  
  if( n == 1 )
    return AbelianGroupRep( theGroup );

  AbelianGroupRep A = getCanonicalSmithPresentation();
  FPGroup G = A.getFPGroup();
  
  int i;
  
  for( i = 0 ; i < n + 1 ; i++ )
    {
      VectorOf< VectorOf<int> > v(2);
      v[0] = VectorOf<int>( 1 );
      
      if( i != 0 )
	v[0][0] = 0;
      else
	v[0][0] = 1;
      
      hSeries[i] = v;
    }

  for( i = 0 ; i < G.numberOfGenerators() ; i++ )
    {
      Generator g( i + 1 );
      int rank = A.orderOfElt( Word( g ) ).as_long();
      
      if( !rank )
	{
	  for( int j = n ; j > 1 ; j-- )
	    {
	      hSeries[j][0][0] += hSeries[j-1][0][0];
	      if( j == 2 )
		hSeries[j][0][0] += hSeries[j-1][0][0];
	      
	      for( int i1 = 0 ; i1 < hSeries[j-1][1].length() ; i1++ )
		{
		  hSeries[j][1].append( hSeries[j-1][1][i1] );
		  if( j == 2 )
		    hSeries[j][1].append( hSeries[j-1][1][i1] );
		}
	    }
	  
	  hSeries[1][0][0] ++;
	}
      else
	{
	  VectorOf< VectorOf< VectorOf<int> > > tmp( n + 1 );
	  
	  for( int i1 = 0 ; i1 < n + 1 ; i1++ )
	    {
	      VectorOf< VectorOf<int> > v(2);
	      v[0] = VectorOf<int>( 1 );
	      v[0][0] = 0;
	      
	      tmp[i1] = v;
	    }
	  
	  tmp[0] = hSeries[0];
	  tmp[1] = hSeries[1];
	  tmp[1][1].append( rank );
	  
	  for( int j = 2 ; j < n + 1 ; j++ )
	    {
	      int q = 0;
	      VectorOf<int> t;
	      
	      for( int k = 0 ; k < j + 1 ; k++ )
		{
		  if( k == 0 )
		    {
		      q += hSeries[j-k][0][0];
		      
		      for( int i1 = 0 ; i1 < hSeries[j-k][1].length() ; i1++ )
			t.append( hSeries[j-k][1][i1] );
		    }
		  
		  if( k % 2 )
		    {
		      for( int i1 = 0 ; i1 < hSeries[j-k][0][0] ; i1++ )
			t.append( rank );

		      for( int i1 = 0 ; i1 < hSeries[j-k][1].length() ; i1++ )
			if( abs( gcd( rank , hSeries[j-k][1][i1] ) ) != 1 )
			  t.append( abs( gcd( rank , hSeries[j-k][1][i1] ) ).as_long() );
		    }
		}
	      
	      if( !(j % 2) )
		{
		  for( int i1 = 0 ; i1 < hSeries[j / 2][0][0] ; i1++ )
		    t.append( rank );
		  
		  for( int i1 = 0 ; i1 < hSeries[j / 2][1].length() ; i1++ )
		    if( abs( gcd( rank , hSeries[j / 2][1][i1] ) ) != 1 )
		      t.append( abs( gcd( rank , hSeries[j / 2][1][i1] ) ).as_long() );
		}
	      
	      tmp[j][0][0] = q;
	      tmp[j][1] = t;
	    }
	  
	  hSeries = tmp;
	}
    }
  
  int r = hSeries[n][0][0];
  VectorOf<int> torsion = hSeries[n][1];
  
  int numberOfGens = r + torsion.length();
  FPGroup G1( numberOfGens );
  
  SetOf<Word> rels;
  for( i = 0 ; i < torsion.length() ; i++ )
    {
      Generator g( r + i + 1 );
      rels.adjoinElement( G1.raiseToPower( Word( g ) , torsion[i] ) );
    }
  
  G1.addRelators( rels );
  
  return AbelianGroupRep( G1 );
}

Integer AbelianGroupRep::orderOfTheTorsionSubgroup( ) const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "Integer AbelianGroupRep::orderOfTheTorsionSubgroup( ) const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif

  int invLen = theInvariants.length();
  Integer order = 1;
  
  for( int i = 0; i < invLen; ++i )
    order *= theInvariants[i];

  return order;
}


bool AbelianGroupRep::isTrivial( const Word& w ) const 
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "bool AbelianGroupRep::isTrivial( const AbelianWord& w ) const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif
  
  return oldToNewGens( oldInAbelianForm(w) ).isTrivial();
}


Integer AbelianGroupRep::orderOfElt( const Word& w ) const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "Integer AbelianGroupRep::orderOfElt( const AbelianWord& ) const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif

  VectorOf<Integer> v = oldToNewGens( oldInAbelianForm(w) ).getPowers();
  int invLen = theInvariants.length();

  for( int i = 0; i < rankOfFreePart; ++i )
    if ( sign(v[i]) != 0 ) return 0; // Infinity
  
  for( int i = 0; i < invLen; ++i ) {
    int num = i + rankOfFreePart;
    
    if( sign(v[num]) != 0 )
      v[num] = theInvariants[i] / gcd( theInvariants[i], v[num] );
  }
  
  Integer LCM = 1;
  for( int i = rankOfFreePart; i < numOfNewGens; ++i )
    if( sign(v[i]) != 0 )
      LCM = lcm(LCM,v[i]);
  
  return LCM;
}


bool AbelianGroupRep::areEqual( const Word& u, const Word& v ) const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "bool AbelianGroupRep::areEqual( const Word& u,"
	   " const Word& v ) const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif
  
  return ( oldToNewGens( oldInAbelianForm(u) ) 
	   == oldToNewGens( oldInAbelianForm(v) ) );
}


AbelianWord AbelianGroupRep::newToOldGens( const AbelianWord& w ) const
{
#if SAFETY > 0

  if ( !bHaveCyclicDecomposition )
    error( "AbelianWord AbelianGroupRep::newToOldGens( const AbelianWord& )"
	   " const : "
	   "tried to obtain results before the computation had been finished"
	   );
  
  if ( w.numberOfGenerators() != numOfNewGens )
    error( "AbelianWord AbelianGroupRep::newToOldGens( const AbelianWord& )"
	   " const : "
	   "w has the wrong number of generators"
	   );
  
#endif
  
  int numOfGens = theGroup.numberOfGenerators();
  AbelianWord result(numOfGens);
  int wNumOfGens = w.numberOfGenerators();
  
  for( int i = 0; i < wNumOfGens; ++i ) {
    Integer power = w[i];
    for( int j = 0; j < numOfGens; ++j )
      result[j] += power * theNewToOldGens[i][j];
  }

  return result;
}


AbelianWord AbelianGroupRep::oldToNewGens( const AbelianWord& w ) const
{
#if SAFETY > 0

  if ( !bHaveCyclicDecomposition )
    error( "AbelianWord AbelianGroupRep::oldToNewGens( const AbelianWord& w )"
	   " const : "
	   "tried to obtain results before the computation had been finished"
	   );

  if ( w.numberOfGenerators() != theGroup.numberOfGenerators() )
    error( "AbelianWord AbelianGroupRep::oldToNewGens( const AbelianWord& w )"
	   " const : "
	   "w has the wrong number of generators"
	   );

#endif
  
  int numOfGens = theGroup.numberOfGenerators();
  AbelianWord result(numOfNewGens);
  int wNumOfGens = w.numberOfGenerators();
  
  for( int i = 0; i < wNumOfGens; ++i ) {
    Integer power = w[i];
    for( int j = 0; j < numOfNewGens; ++j )
      result[j] += power * theOldToNewGens[i][j];
  }

  minimizeWordInNewGens(result);
  return result;
}


void AbelianGroupRep::makeTransformationMatrices( ) 
  // * height and width are suppose to be initialiazed already.
{
  //@njz
  //  newToOldGensMatrix = new (Integer*)[width];
  //  oldToNewGensMatrix = new (Integer*)[width];
  newToOldGensMatrix = new Integer*[width];
  oldToNewGensMatrix = new Integer*[width];
  //
  for ( int i = 0; i < width; i++ ) {
    newToOldGensMatrix[i] = new Integer[width];
    oldToNewGensMatrix[i] = new Integer[width];
    for ( int j = 0; j < width; j++ ) {
      newToOldGensMatrix[i][j] = 0;
      oldToNewGensMatrix[i][j] = 0;
    }
    newToOldGensMatrix[i][i] = 1;
    oldToNewGensMatrix[i][i] = 1;
  }
}


void AbelianGroupRep::makeMainMatrix( )
  // * height and width are suppose to be initialiazed already.
{
  // Put the group relators into matrix
  
  SetIterator<Word> I( theGroup.getRelators() );
  int i = 0;
  //
  //  matrix = new (Integer*)[height];
  matrix = new Integer*[height];
  //
    // matrix[i][j] = exponent of j^{th} generator in i^{th} relation.

  while ( !I.done() ) {

    VectorOf<Integer> v = AbelianWord( width, Word( I.value() ) ).getPowers();
    matrix[i] = new Integer[width];
    
    for ( int j = 0; j < width; ++j )
      matrix[i][j] = v[j];
    
    I.next();
    ++i;
  }

}


void AbelianGroupRep::destroyMatrices( ) 
{
  if( newToOldGensMatrix && oldToNewGensMatrix && matrix ) {
    
    for ( int i = 0; i < width; i++ ) {
      delete [] newToOldGensMatrix[i];
      delete [] oldToNewGensMatrix[i];
    }

    for ( int i = 0; i < height; i++ )
      delete [] matrix[i];

    delete [] newToOldGensMatrix;
    delete [] oldToNewGensMatrix;
    delete [] matrix;
  }
}


void AbelianGroupRep::printOn( ostream& ostr ) const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "void AbelianGroupRep::printOn( ostream& ostr ) const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif

  int invLen = theInvariants.length();
  bool bFirst = true;

  if( rankOfFreePart > 0 ) {
    bFirst = false;
    ostr << " Z";
    if( rankOfFreePart > 1 )
      ostr << "^" << rankOfFreePart;
  }

  for( int i = 0; i < invLen; ++i ) {
    if( !bFirst )
      ostr << " x";
    ostr << " Z_" << theInvariants[i];
    bFirst = false;
  }

  if( bFirst )
    ostr << " 1";
}


void AbelianGroupRep::printWordInNewGens( ostream& ostr, 
					  const AbelianWord& w ) const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "void AbelianGroupRep::printWordInNewGens( ostream& ostr, "
	   "const AbelianWord& w ) const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif

  VectorOf<Integer> form = w.getPowers();
  int len = form.length();
  bool bFirst = true;
   for (int i=0;i<len;i++){
     if (form[i]!=0){
            if (!bFirst )
                     ostr <<" ";    
            else
                     bFirst = false;
         if (i<rankOfFreePart)
                ostr << "f"<<i+1;
            else
                ostr << "t"<<i+1-rankOfFreePart;

            if (form[i]!=1)
                ostr << "^" << form[i];
      }
   }
   if (bFirst) ostr << "1";
   /*  
  for( int i = 0; i < rankOfFreePart; ++i ) {

    Integer coef = form[i];

    if ( coef > 0 ) {
      if ( ! bFirst )
	ostr << " +";
      bFirst = false;
      if( coef > 1 )
	ostr << " " << coef;
      ostr << " f" << i + 1;
    }
    else
      if ( coef < 0 ) {
	ostr << " -";
	bFirst = false;
	if( coef < -1 )
	  ostr << " " << - coef;
	ostr << " f" << i + 1;
      }
    
  }

  for( int i = rankOfFreePart; i < len; ++i ) {

    Integer coef = form[i];

    if ( coef > 0 ) {
      if ( ! bFirst )
	ostr << " +";
      bFirst = false;
      if( coef > 1 )
	ostr << " " << coef;
      ostr << " t" << i + 1 - rankOfFreePart;
    }
    else
      if ( coef < 0 ) {
	ostr << " -";
	bFirst = false;
	if( coef < -1 )
	  ostr << " " << - coef;
	ostr << " t" << i + 1 - rankOfFreePart;
      }
    
  }
  
  if ( bFirst )
    ostr << " 0";*/
}


void AbelianGroupRep::write( ostream& ostr ) const
{
  ostr < bMakeFile < theFileName < bHaveCyclicDecomposition < numOfNewGens
    < rankOfFreePart < theInvariants < theNewToOldGens < theOldToNewGens ;
  ostr < theGroup;
  ostr < primeBasisFound;
  ostr < primeBasicMatrix; 
  ostr < theFileNameOfPD; 
}


void AbelianGroupRep::read( istream& istr )
{
  istr > bMakeFile > theFileName > bHaveCyclicDecomposition  > numOfNewGens
    > rankOfFreePart > theInvariants > theNewToOldGens > theOldToNewGens  ;
  istr > *((FPGroup* )(&theGroup));
  istr > primeBasisFound;
  istr > primeBasicMatrix; 
  istr > theFileNameOfPD; 
}


SetOf<Word> AbelianGroupRep::getAllRelators( ) const
{
  SetOf<Word> S(theGroup.getRelators());
  int numOfGens = theGroup.numberOfGenerators();
  VectorOf<Generator> v(4);
  for ( int i = 1; i < numOfGens; i++ )
    for ( int j = i+1; j <= numOfGens; j++ ) {
      v[0] = i;  v[1] = j; v[2] = -i; v[3] = -j;
      S |= Word(v);
    }
  return S;
}


AbelianGroupRep AbelianGroupRep::getCanonicalSmithPresentation( ) const 
{
 #if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error( "AbelianGroupRep AbelianGroupRep::getCanonicalSmithPresentation( ) "
	   "const : "
	   "tried to obtain results before the computation had been finished"
	   );
#endif
  
  // Make a presentation of the cyclic decomposition.

  // Prepare generators.

  int invLen = theInvariants.length();
  VectorOf<Chars> genNames(numOfNewGens);
  
  for( int i = 0; i < rankOfFreePart; ++i ) {
    char s[10];
    sprintf(s, "%d", i+1);
    genNames[i] = Chars("f") + s;
  }

  int j = 0;
  for( int i = rankOfFreePart; i < numOfNewGens; ++i ) {
    char s[10];
    sprintf(s, "%d", ++j);
    genNames[i] = Chars("t") + s;
  }

  // Prepare relators.

  SetOf<Word> relators;

  for( int i = 0; i < invLen; ++i ) {
    Word w;
    for( j = 0; j < theInvariants[i]; ++j )
      w *= Word( Generator( rankOfFreePart + i + 1 ) );
    relators |= w;
  }

  // The presentation is ready.

  FPGroup G(genNames, relators);
  AbelianGroupRep rep( G );

  rep.bHaveCyclicDecomposition = true;
  rep.numOfNewGens = numOfNewGens;
  rep.rankOfFreePart = rankOfFreePart;
  rep.theInvariants = theInvariants;

  rep.theNewToOldGens = VectorOf<AbelianWord>(numOfNewGens);
  rep.theOldToNewGens = VectorOf<AbelianWord>(numOfNewGens);
  
  for( int i = 0; i < numOfNewGens; ++i ) {
    AbelianWord w = AbelianWord( numOfNewGens, Word( Generator(i+1) ) );
    rep.theNewToOldGens[i] = w;   
    rep.theOldToNewGens[i] = w;
  }
  
  return rep;
}

//  ------------------------ makeSubgroupPresentation. ---------------------//

AbelianSGPresentationRep AbelianGroupRep::makeSubgroupPresentation(const VectorOf<Word>& vG) const {
  int i,j,m,n,m1,n1;
  m = theGroup.getRelators().cardinality();
  n = theGroup.numberOfGenerators();
  m1 = vG.length();
  DArray<Integer> A(m+m1,n);
  SetIterator<Word> I(theGroup.getRelators());
  for(i=0; i<m; i++,I.next()) for(j=0; j<n; j++)
    A[i][j] = I.value().exponentSum(Generator(j+1));
  for(i=0; i<m1; i++) for(j=0; j<n; j++) 
    A[i+m][j] = vG[i].exponentSum(Generator(j+1));
  SmithNormalForm1 smith(A);
  VectorOf<Integer> inv = smith.invariants();
  n1 = inv.length();
  VectorOf<Chars> genNames(n1);
  for(i=0; i<n1; i++) {
    char s[10];
    sprintf(s, "s%u", i+1);
    genNames[i] = s;
  }
  FPGroup thePresentation(genNames);
  SetOf<Word> nRelators;
  DArray<Integer> P = smith.LeftFactor();
  P.reSize(m,n1);
  for(i=0; i<m; i++)
    nRelators.adjoinElement(AbelianWord(P.getRow(i)).getWord());
  thePresentation.setRelators(nRelators);
  P = smith.RightFactor();
  P.reSize(n1,n);
  for(i=0; i<n1; i++) for(j=0;j<n;j++) P[i][j] *= inv[i];
  return AbelianSGPresentationRep(AbelianGroup((AbelianGroupRep*)this), vG, 
    thePresentation, P, smith.RightFactorInv(), inv, true);
}

// ------------------------  Primary Decompositions ------------------------//

void AbelianGroupRep::findPrimaryBasis()
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error("the abelianization was not "
	  "done:AbelianGroupRep::findPrimaryBasis() ");
#endif
  
  Integer invariant, power;
  PrimeNumbers pr;
  Integer x, y, gcdC;
  int prIndex=0;
  // Follow all invariants
  for (int i=0;i<theInvariants.length();i++){
    invariant = theInvariants[i];  
   
    pr.setCurrent(2);
    power = 1;
    // if current invariant a prime number
    while (!pr.isPrime(invariant)){

      // if we have prime number which divides the invariant
      if (invariant%pr.current()==0){
	// add a row in the matrix
	if (!prIndex)
	  primeBasicMatrix.reSize(1,5);
	 else
	   primeBasicMatrix.rowInsAfter(prIndex-1,1);
	
	// initialize the data
	primeBasicMatrix[prIndex][POWERS] = 1;
	primeBasicMatrix[prIndex][PRIMES] = 1;
	primeBasicMatrix[prIndex][NEWTOOLD] = 1;

	// compute powers of the prime number 
	while (invariant%pr.current()==0){
	  invariant = invariant / pr.current();
	  primeBasicMatrix[prIndex][POWERS]*=pr.current();
	  primeBasicMatrix[prIndex][PRIMES]=pr.current();
	}
	// if we have an invariant cyclic, say < t > = < p > + < q >,
	// where p and q are relativly prime, than p = Order( < q > )*t
       	// so we have "new to old" image for p. 
	primeBasicMatrix[prIndex][NEWTOOLD] = invariant*power;
	primeBasicMatrix[prIndex][INV] = i;
	power *=primeBasicMatrix[prIndex][POWERS];
	
	prIndex++;
	
      }
      
      pr.nextNumber();
    }
    // if invariant is a prime or order of a prime number
    if (invariant>1){
      if (!prIndex)
	primeBasicMatrix.reSize(1,5);
      else
	primeBasicMatrix.rowInsAfter(prIndex-1,1);
      
      primeBasicMatrix[prIndex][POWERS] = invariant;
      primeBasicMatrix[prIndex][PRIMES] = invariant;
      primeBasicMatrix[prIndex][NEWTOOLD] = power;
      primeBasicMatrix[prIndex][INV] = i;
      
      prIndex++;
      
    }
  }   
  
  // compute OLDTONEW generators
  // first put the invariants in order
  sortVector(primeBasicMatrix,INV,0,primeBasicMatrix.height());
  // for each invariant compute it's image
  int nextInvariant = 0;
  for (int i = 0;i<theInvariants.length();i++)
    nextInvariant = invariantToNewGens(i,theInvariants[i],nextInvariant) + 1;
  
  // We need to sort the decomposition in primes, and their powers
  sortPrimeDecom(primeBasicMatrix); 
  primeBasisFound = true;
  if (bMakeFile)
    makeFileOfPDGens();
  
}


int AbelianGroupRep::invariantToNewGens( int invPos, Integer orderOfCyclic,
					  int stPos, Integer power)
{
#if SAFETY > 0
    if ( invPos < 0 || invPos >= theInvariants.length() )
      error("Wrong invariant position in"
	    " AbelianGroupRep::invariantToNewGens( int )"); 
    
    if ( stPos < 0 || stPos >= primeBasicMatrix.height() )
      error("Wrong start position in"
	    " AbelianGroupRep::invariantToNewGens( int )");      
#endif        
    
    // If < p > is the last for this invariant or in the whole basis
    // then leave.
    if ( (stPos + 1 ) == primeBasicMatrix.height() ){
      primeBasicMatrix[stPos][OLDTONEW]
	= power%primeBasicMatrix[stPos][POWERS];
      return stPos;
    }
    if ( invPos != primeBasicMatrix[stPos + 1][INV] ){
      primeBasicMatrix[stPos][OLDTONEW] 
	= power%primeBasicMatrix[stPos][POWERS];
      return stPos;
    }    
    
    // fint the order of the end product of cyclics,
    // say we have < t > of order is equal to orderOfCyclic and
    // < t > = < p > + < q >, so we compute the order of < q > here
    // and p is a current prime from primeBasicMatrix[stPos]
    Integer Oq = orderOfCyclic/primeBasicMatrix[stPos][POWERS];

    // We know that if p and q are relativly prime, than 
    // there exist such x and y that xp + yq = 1
    // Also p = Order( < q > )*t and q =  Order( < p > )*t =>
    // t = x(Order( < q > )*t) + y(Order( < p > )*t) so to find
    // t we need to find x and y and than t = xp + yq.
    Integer x, y;

    IntProblems().findCoeff(Oq,primeBasicMatrix[stPos][POWERS],
			    x, y, orderOfCyclic);
    
    
    primeBasicMatrix[stPos][OLDTONEW]
      = (power*x)%primeBasicMatrix[stPos][POWERS];

    // < q > itself can be a product of primes, so repeat the
    // procedure for < q >
    
    return invariantToNewGens( invPos, Oq, stPos + 1, power*y );

}

void AbelianGroupRep::printPrimaryDec( ostream& ostr) const
  {
#if SAFETY > 0
    if ( !primeBasisFound )
      error("the prime decompozition was not done before"
	    " AbelianGroupRep::printPrimaryDec(ostream)");
#endif        
     if (rankOfFreePart==0 && primeBasicMatrix.height()==0){
       ostr << "1";
       return;
     }
     if (rankOfFreePart){
       ostr << "Z";
       if (rankOfFreePart>1)
	 ostr << "^"<<rankOfFreePart;
       if (primeBasicMatrix.height()>0)
	 ostr <<" x ";
     }
     for (int i=0;i<primeBasicMatrix.height();i++){
       ostr << "Z_"<<primeBasicMatrix [i][POWERS];
       if ( i < primeBasicMatrix.height()-1) ostr << " x ";
     }      
  }

AbelianWord AbelianGroupRep::findEltPrimeForm(const Word& w) const
{
#if SAFETY > 0
  if ( !primeBasisFound )
    error("the primery basis was not "
	  "found:AbelianGroupRep::findEltPrimeForm(const Word&) ");
#endif   
  VectorOf<Integer> eltBasis(primeBasicMatrix.height()+rankOfFreePart);
  VectorOf<Integer> eltInNewGen(theInvariants.length()+rankOfFreePart);
  eltInNewGen = oldToNewGens(oldInAbelianForm(w)).getPowers();
  for (int i=0;i<eltBasis.length();i++){
    if (i<rankOfFreePart)
      eltBasis[i] = eltInNewGen[i];
    else {
      eltBasis[i] = 
	eltInNewGen[primeBasicMatrix[i-rankOfFreePart][INV].as_long() + 
		   rankOfFreePart]*
	primeBasicMatrix[i-rankOfFreePart][OLDTONEW] % 
	primeBasicMatrix[i-rankOfFreePart][POWERS];
    }
  }
  return AbelianWord(eltBasis);
}

AbelianWord AbelianGroupRep::primeFormInOldGens(const AbelianWord& w) const
{
  VectorOf<Integer> tGen(rankOfFreePart+theInvariants.length() ); 
  VectorOf<Integer> powers =  w.getPowers();
  for (int i=0;i<rankOfFreePart;i++)
    tGen[i] = powers[i];
  for (int i = 0;i<primeBasicMatrix.height();i++){
    if (powers[i+rankOfFreePart]!=0){
      if (tGen[primeBasicMatrix[i][INV].as_long()+rankOfFreePart] == 0)
	tGen[primeBasicMatrix[i][INV].as_long()+rankOfFreePart] = 1;
      tGen[primeBasicMatrix[i][INV].as_long()+rankOfFreePart] *= 
	powers[i+rankOfFreePart]*primeBasicMatrix[i][NEWTOOLD];
    }
  }
  AbelianWord result = newToOldGens(AbelianWord(tGen) );
  return result;
}

void AbelianGroupRep::printInPrimaryForm(ostream& ostr, const AbelianWord& aw) const
{
#if SAFETY > 0
  if ( aw.numberOfGenerators() != primeBasicMatrix.height() + rankOfFreePart )
    error("AbelianGroupRep::printInPrimaryForm(ostream& , const AbelianWord& )"
	  " : parameter is not a word in the primary  form ");
#endif    
  
  VectorOf<Integer> vc = aw.getPowers();
   bool firstGen = true;
   for (int i=0;i<vc.length();i++){
     if (vc[i]!=0){
       if (!firstGen )
	 ostr <<" ";  
       else
	 firstGen = false;
       if (i<rankOfFreePart)
	 ostr << "f"<<i+1;
       else
	 ostr << "p"<<i+1-rankOfFreePart;
       
       if (vc[i]!=1)
	 ostr << "^" << vc[i];
     }
   }
   if (firstGen) ostr << "1";
}

void AbelianGroupRep::sortPrimeDecom(DArray<Integer>& m){
  if (m.height()>1){
    sortVector(m,PRIMES,0,m.height()-1);
    Integer tmp = m[0][PRIMES];
    int start = 0;
    for (int i=1;i<m.height();i++){
      if (m[i][PRIMES]!=tmp){
	sortVector(m,POWERS,start,i-1);
	tmp = m[i][PRIMES];
	start = i;
      }
    }
  }
}
void AbelianGroupRep::sortVector(DArray<Integer>& vc,int colSort,int start, int finish)
{
#if SAFETY > 0
  if ( start > finish || start <0 || finish > vc.height())
    error("wrong parameters in "
	  "AbelianGroupRep::sortVector(DArray<Integer>& vc,"
	  "int colSort,int start, int finish)");
#endif
  int i,j;
  VectorOf<Integer> v;
  for (j=start+1;j<finish;j++){
    v = vc[j];
    i=j-1;
    while (i>=start && vc[i][colSort] > v[colSort]){
      vc[i+1]=vc[i];
      i--;
    }
    vc[i+1] = v;
  }
}

AbelianWord AbelianGroupRep::pBlockOfElt( const AbelianWord& w,Integer p )const
{
#if SAFETY > 0
  if ( !primeBasisFound )
    error("The primary decomposition was not done before the call of"
	  "AbelianWord AbelianGroupRep::pBlockOfElt( const AbelianWord& w)");
#endif
  int torsLength = primeBasicMatrix.height();
  VectorOf<Integer> retWord(torsLength + rankOfFreePart);
  int numberOfPElts = 0;
  
  // Copy only powers of the generators of the p-block
  for (int i = 0;i<torsLength;i++){
    if (primeBasicMatrix[i][PRIMES] == p){
      retWord[rankOfFreePart + i] = w[rankOfFreePart + i];
    }
  }
  return AbelianWord( retWord );
}

Integer AbelianGroupRep::pHeightOfElt(const Word& w, const Integer& p) const
{
#if SAFETY > 0
  if ( !primeBasisFound )
    error("The primary decomposition was not done before the call of"
	  " AbelianGroupRep::pHeightOfElt(Word,Integer)");
#endif
  Integer pHeight;
  VectorOf<Integer> elemInPrime = findEltPrimeForm(w).getPowers();
  if(rankOfFreePart==0) pHeight = -1;
  else {
    Integer d = elemInPrime[0];
    for (int i=1; i<rankOfFreePart; i++) d = gcd(d,elemInPrime[i]);
    pHeight = IntProblems().powerOf(d,p);
    if (pHeight<=0) return 0;
  }
  for(int i=0; i<primeBasicMatrix.height(); i++) if(p==primeBasicMatrix[i][PRIMES] && elemInPrime[i+rankOfFreePart]!=0) {
    int k;      
    for(k=0; elemInPrime[i+rankOfFreePart]%p==0; k++) elemInPrime[i+rankOfFreePart] /= p;
    if(pHeight<0 || k<pHeight) pHeight = k; 
  }
  return pHeight;
}

void AbelianGroupRep::makeFileOfPDGens( )
{
  File file;
  file.setColor(titleColor);
  file << "New generators expressed in terms of the given generators:" 
       << endl;  
  file.setColor(mainColor);
  for( int i = 0; i < rankOfFreePart; ++i ) {
    file << "f" << i+1 << " = ";
    theGroup.printWord
      ( file, newToOldGens
	( AbelianWord( numOfNewGens, Word(Generator(i+1)) ) ).getWord() );
    file << endl;
  }
  VectorOf<AbelianWord> tsInOld = newToOldGens();
  VectorOf<Integer> tGenInOld;
  for( int i = 0; i < primeBasicMatrix.height(); ++i ) {
    file << "p" << i + 1 << " = ";
    tGenInOld = tsInOld[primeBasicMatrix[i][INV].as_long()+rankOfFreePart].getPowers(); 
    for (int j=0;j<tGenInOld.length();j++)
      tGenInOld[j] = tGenInOld[j]*primeBasicMatrix[i][NEWTOOLD];
    theGroup.printWord( file, AbelianWord (tGenInOld).getWord() );
    tGenInOld = VectorOf<Integer>();
    file << endl;
    file << "The order of p"<<i+1<<" is "<<
      primeBasicMatrix[i][PRIMES]<<"^"<<
      IntProblems().powerOf(primeBasicMatrix[i][POWERS],
			    primeBasicMatrix[i][PRIMES])<<endl;
  }
  file  << endl;
  file.setColor(titleColor);
  file << "Given generators expressed in terms of "
       << "the generators for the primary decomposition:" << endl;  
  file.setColor(mainColor);
  
  VectorOf<Integer> oldGens(theGroup.numberOfGenerators( ) );
  for (int i=0;i<theGroup.numberOfGenerators( );i++){
    oldGens[i] = 1;
    theGroup.printWord( file,AbelianWord(oldGens).getWord() );
    file << " = ";
    printInPrimaryForm(file,findEltPrimeForm( AbelianWord(oldGens).getWord()));
    oldGens[i] = 0;
    file << endl;
  }
  
  file << end;
  
  theFileNameOfPD = file.getFileName();
}

Chars AbelianGroupRep::getFileNameOfPDGens( ) const
{
#if SAFETY > 0
  if( !bMakeFile )
    error("Chars AbelianGroupRep::getFileNameOfPDGens( ) : "
	  "The file has not been created.");
#endif
  
  return theFileNameOfPD;
}

// ----------------------- Methods for subgroups -------------------------//
VectorOf<Word> AbelianGroupRep::findSubgroupIsolator(const VectorOf<Word>& vG) const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error("the abelianization was not done before "
	  "AbelianGroupRep::findSubgroupIsolator(const VectorOf<Word>& vG) ");
#endif
  SetOf<Word> subgroupRel = theGroup.getRelators();
  for (int i=0;i<vG.length();i++)
    subgroupRel.adjoinElement(vG[i]);
  
  AbelianGroupRep subgroupRep(FPGroup(theGroup.numberOfGenerators(),
				      subgroupRel));
  
  subgroupRep.computeCyclicDecomposition();
  VectorOf<Word> retV(vG.length()+subgroupRep.theInvariants.length()); 
  // And generators of invariant part of subgroupRep
  
  VectorOf<AbelianWord> nTO = subgroupRep.newToOldGens();
  for (int i=0;i<vG.length();i++){
    retV[i] = vG[i];
  }
  for (int i=vG.length();i<retV.length();i++){
    retV[i] = nTO[i-vG.length()+subgroupRep.rankOfFreePart].getWord( );
  }  
  return retV;
}


VectorOf<Word> AbelianGroupRep::findVirtFreeComplementOfSG(const VectorOf<Word>& vG) const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error("VectorOf<Word> AbelianGroupRep::findVirtFreeComplementOfSG( ... )"
	  "the abelianization was not done");
#endif
  
  SetOf<Word> subgroupRel = theGroup.getRelators();
  for (int i=0;i<vG.length();i++)
    subgroupRel.adjoinElement(vG[i]);
  
  AbelianGroupRep subgroupRep(FPGroup(theGroup.numberOfGenerators(),
				      subgroupRel));
  subgroupRep.computeCyclicDecomposition();
  VectorOf<Word> retV(subgroupRep.rankOfFreePart);  
  VectorOf<AbelianWord>  nTO = subgroupRep.newToOldGens();
  for (int i=0;i<subgroupRep.rankOfFreePart;i++)
    retV[i] = nTO[i].getWord( ) ;
  return retV;
}


bool AbelianGroupRep::isPureCyclSubgroup(const Word& w) const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error("the abelianization was not done before "
	  "AbelianGroupRep::isPureCyclSubgroup(Word)");
#endif
  
  // if trivial word than subgroup is pure
  
  if ( isTrivial( w ) )
    return true;
  
  // get cyclic decomposition of a word
  VectorOf<Integer> inNewGen(rankOfFreePart + theInvariants.length());
  inNewGen = oldToNewGens(oldInAbelianForm(w)).getPowers(); 
  
  // if the word is of infinite order,
  // GCD of the free part must be equal to 1 for a
  // subgroup to be pure
  if (orderOfElt(w) == 0){
    if (rankOfFreePart>1){
      Integer d=gcd(inNewGen[0],inNewGen[1]);
      if (d>1) return false;
      for (int j=2;j<rankOfFreePart;j++)
	if ((d=gcd(d,inNewGen[j]))>1) return false;
    } else {
      if (abs(inNewGen[0])>1)
	return false;
    }
    return true;
    
  }
  
#if SAFETY > 0
  if ( !primeBasisFound )
    error("the prime decompozition was not done before "
	  "AbelianGroupRep::isPureCyclSubgroup(Word)");
#endif
  
  // The element without the free part
  
  // get Primary form of an element
  VectorOf<Integer> elemInPrime = findEltPrimeForm(w).getPowers();
  bool areMultiples = true;
  bool blockExist = false;
  Integer savP;
  int i=0;

  // if all powers of the element in a p-block are multiples
  // than the subgroup is not pure.
  while (i<primeBasicMatrix.height()){
    // start a p-block
    savP=primeBasicMatrix[i][PRIMES];
    while (savP==primeBasicMatrix[i][PRIMES] ){
      // check does this block exist ( means there are powers != 0 )
      if ( elemInPrime[i + rankOfFreePart] > 0 ){
	blockExist = true;
	// if p devides the all powers in p-block, than they are 
	// multiples  
	if ((elemInPrime[i + rankOfFreePart]%savP)!=0)
	  areMultiples = false;
      }
      i++;
      if (i==primeBasicMatrix.height())
	break;
    }
    // if the block exist and all powers in it are multiples, 
    // than the subgroup is not pure
    if (areMultiples && blockExist)
      return false;
    areMultiples = true;
    blockExist = false;
  }
  return true;  
}

VectorOf<Word> AbelianGroupRep::joinSubgroups(const VectorOf<Word>& vG1,const VectorOf<Word>& vG2) const
{
  VectorOf<Word> jG(vG1.length()+vG2.length());
  for (int i=0;i<vG1.length();i++)
    jG[i]=vG1[i];
  for (int i=0;i<vG2.length();i++)
    jG[i+vG1.length()]=vG2[i];
  return jG;
}

VectorOf<Word> 
AbelianGroupRep::findSubgIntersection( const VectorOf<Word>& s1 , 
				       const VectorOf<Word>& s2 , 
				       File& file ) const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error("the abelianization was not done before "
	  "AbelianGroupRep::findSubgIntersection( const VectorOf<Word>& , const VectorOf<Word>& , File& )");
#endif
  
  // computations will be done in the preimage of given abelian group
  
  AbelianGroup A( FPGroup(theGroup.namesOfGenerators()) );
  
  int i;
  VectorOf<AbelianWord> v1( s1.length() );
  
  for( i = 0 ; i < s1.length() ; i++ )
    {
      AbelianWord aw = A.oldInAbelianForm( s1[i] );
      v1[i] = aw;
    }
  
  VectorOf<AbelianWord> v2( s2.length() );
  
  for( i = 0 ; i < s2.length() ; i++ )
    {
      AbelianWord aw = A.oldInAbelianForm( s2[i] );
      v2[i] = aw;
    }
  
  SetOf<Word> rels = theGroup.getRelators();
    
  SetIterator<Word> I( rels );
  
  while( !I.done() )
    {
      AbelianWord aw = A.oldInAbelianForm( I.value() );
      
      if( v1.indexOf( aw ) == -1 )
	v1.append( aw );
      
      if( v2.indexOf( aw ) == -1 )
	v2.append( aw );
      
      I.next();
    }
  
  // constructing the system of equations
  
  int r = v1.length() + v2.length() + 1;
  
  VectorOf<Chars> basis( r );
  
  for( i = 0 ; i < r - 1 ; i++ )
    basis[i] = Chars( "x" ) + Chars( i+1 );
  
  basis[ r-1 ] = Chars( "a" );
  
  FPGroup F( basis );
  
  VectorOf<Word> system( theGroup.numberOfGenerators() );
  
  for( i = 0 ; i < theGroup.numberOfGenerators() ; i++ )
    {
      Word w;
      for( int j = 0 ; j < v1.length() + v2.length() ; j++ )
	if( j < v1.length() )
	  {
	    Generator g(j+1); 
	    w *= F.raiseToPower( Word(g) , v1[j][i].as_long() );
	  }
	else
	  {
	    Generator g(j+1); 
	    w *= F.raiseToPower( Word(g) , -v2[j - v1.length()][i].as_long() );
	  }
    
      system[i] = w;
    }
  
  AbelianGroup Z( F );
  
  AbelianEquationsSolver AES( Z , system , v1.length() + v2.length() );
  
  AES.findSolutions( file , false );
  
  VectorOf< VectorOf< VectorOf<int> > > params = AES.getParametrizedPart();
  
  SetOf<Word> s;
  SetOf<Word> retValue;
  
  int rank = 0;
  bool flag = true;
  for( int j = 0 ; j < AES.getNumberOfParams() ; j++ )    
    {
      VectorOf<int> sol1( v1.length() );
      VectorOf<int> sol2( v2.length() );
      
      for( i = 0 ; i < v1.length() ; i++ )
	sol1[i] = 0;
      for( i = 0 ; i < v2.length() ; i++ )
	sol2[i] = 0;
      
      for( i = 0 ; i < v1.length() ; i++ )
	for( int k = 0 ; k < params[i].length() ; k++ )
	  if( params[i][k][0] == j + 1 )
	    sol1[i] = params[i][k][1];
      
      for( i = 0 ; i < v2.length() ; i++ )
	for( int k = 0 ; k < params[ i + v1.length() ].length() ; k++ )
	  if( params[i + v1.length()][k][0] == j + 1 )  
	    sol2[i] = params[i + v1.length()][k][1];
    
      AbelianWord w( theGroup.numberOfGenerators() , Word() );
      for( i = 0 ; i < sol1.length() ; i++ )
	for( int k = 0 ; k < abs( sol1[i] ) ; k++ )
	  if( sol1[i] > 0 )
	    w = w * v1[i];
	  else
	    w = w * v1[i].inverse();
	  
      Word res = w.getWord();
  
      AbelianGroup T( FPGroup( theGroup.namesOfGenerators() , s ) );
      T.computeCyclicDecomposition();
      
      if( !T.isTrivial( res ) )
	{
	  s.adjoinElement( res );
	  
	  if( isTrivial( res ) )
	    continue;
	  
	  rank++;
	  retValue.adjoinElement( res );
	  
	  if( flag )
	    {
	      file << "The generators of intersected subgroups: " << endl << endl;
	      int q;
	      for( q = 0 ; q < s1.length() ; q++ )
		{
		  file << "x" << q + 1 << " = ";
		  theGroup.printWord( file , s1[q] );
		  file << endl;
		}
	      
	      file << endl;
	      
	      for( q = 0 ; q < s2.length() ; q++ )
		{
		  file << "y" << q + 1 << " = ";
		  theGroup.printWord( file , s2[q] );
		  file << endl;
		}
	      
	      file << endl << "The generators for intersection expressed in terms of intersected subgroups: " << endl << endl;
	      
	      flag = false;
	    }
	  
	  file << "z" << rank << " = ";
	  
	  theGroup.printWord( file , res );
	  
	  bool flag1 = true;
	  for( int q = 0 ; q < s1.length() ; q++ )
	    if( sol1[q] )
	      {
		if( flag1 )
		  {
		    file << " = ";
		    flag1 = false;
		  }
		
		file << "x" << q + 1;
		
		if( sol1[q] == 1 )
		  file << " ";
		else
		  file << "^" << sol1[q] << " ";
	      }
	  
	  if( flag1 )
	    file << "1";
	  
	  bool flag2 = true;
	  for( int q = 0 ; q < s2.length() ; q++ )
	    if( sol2[q] )
	      {
		if( flag2 )
		  if( flag1 )
		    {
		      file << " = ";
		      flag2 = false;
		    }
		  else
		    {
		      file << "= ";
		      flag2 = false;
		    }
		
		file << "y" << q + 1;
		
		if( sol2[q] == 1 )
		  file << " ";
		else
		  file << "^" << sol2[q] << " ";
	      }
	  
	  if( flag2 )
	    file << " = 1";
	  
	  file << endl;
	}
    }
  
  if( flag )
    file << "Intersection is trivial." << endl;
  
  return makeVectorOf( retValue );
}

VectorOf<Word> 
AbelianGroupRep::fixedPointsOfAuto( const VectorOf<Word>& v ) const
{
  // computations will be done in the preimage of given abelian group
  
  AbelianGroup A( FPGroup(theGroup.namesOfGenerators()) );
  
  int i,j;
  VectorOf<AbelianWord> v1;
  
  for( i = 0 ; i < v.length() ; i++ )
    v1.append( A.oldInAbelianForm( v[i] ) );
  
  VectorOf<Chars> genNames( v.length() + 1 );
  for( i = 0 ; i < v.length() ; i++ )
    genNames[i] = Chars("x") + Chars( i + 1 );
  
  genNames[ v.length() ] = Chars("a");
  
  FPGroup G( genNames );
    
  VectorOf<Word> system( theGroup.numberOfGenerators() );
  
  // we assume that the map defined by v is automorphism i.e. 
  // v.length() == theGroup.numberOfGenerators()
  
  for( i = 0 ; i < theGroup.numberOfGenerators() ; i++ )
    {
      Word w;
      for( j = 0 ; j < v.length() ; j++ )
	{
	  Generator g(j+1); 
	  Integer power = ( i == j ) ? v1[j][i] - 1 : v1[j][i];
	  
	  w *= G.raiseToPower( Word( g ) , power.as_long() );
	}
    
      system[i] = w;
    }

  AbelianGroup Z( G );
  
  AbelianEquationsSolver AES( Z , system , v.length() );
  
  File file;
  AES.findSolutions(file);
  
  if( AES.solIsAllGroup() )
    {
      VectorOf<Word> sol( v.length() );
      
      for( i = 0 ; i < v.length() ; i++ )
	sol[i] = Generator( i + 1 );

      return sol;
    }

  if( !AES.haveSolutions() )
    {
      return VectorOf<Word>();
    }
  
  VectorOf<Word> basic = AES.getBasicSolutions();
  VectorOf< VectorOf< VectorOf<int> > > params = AES.getParametrizedPart();
  
  SetOf<Word> s = theGroup.getRelators();
  SetOf<Word> fix;
  
  int rank = 0;
  bool flag = true;
  for( i = 0 ; i < AES.getNumberOfParams() ; i++ )
    {
      VectorOf<Integer> sol( theGroup.numberOfGenerators() );
      
      for( j = 0 ; j < v.length() ; j++ )
	{
	  sol[j] = basic[j].freelyReduce().length();
	  for( int k = 0 ; k < params[j].length() ; k++ )
	    if( params[j][k][0] == i + 1 )
	      {
		sol[j] += params[j][k][1];
		break;
	      }
	}
      
      AbelianWord aw( sol );
      Word w = aw.getWord();
      
      AbelianGroup T( FPGroup( theGroup.namesOfGenerators() , s ) );
      T.computeCyclicDecomposition();
      
      if( !T.isTrivial( w ) )
	{
	  s.adjoinElement( w );
	  fix.adjoinElement( w );
	}
    }
  
  return makeVectorOf( fix );
}

Integer AbelianGroupRep::powerOfEltInSubgroup(const Word& w,
					      const VectorOf<Word>& sGroup) const
{
  SetOf<Word> newGroupRel = theGroup.getRelators();
  for (int i=0;i<sGroup.length();i++)
    newGroupRel.adjoinElement(sGroup[i]);
  AbelianGroupRep newGroupRep(FPGroup(theGroup.numberOfGenerators(),
				      newGroupRel));
  newGroupRep.computeCyclicDecomposition();
  return newGroupRep.orderOfElt( w );
}

bool AbelianGroupRep::isEltProperPower(const Word& w) const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error("the abelianization was not done before"
	  " AbelianGroupRep::isEltProperPower(const Word& w)");
#endif
  
  // If an element of finite order than always a proper power
  Integer gOrder = orderOfElt(w);
  if (gOrder != 0)
    return true;
  
  // Get canonical decomposition of an element
  VectorOf<Integer> gInCyclic = 
    oldToNewGens(oldInAbelianForm(w) ).getPowers();
  
  // Extract the frea part of an element
  VectorOf<Integer> gFreePart(rankOfFreePart);
  for (int i=0;i<rankOfFreePart;i++)
    gFreePart[i] =  gInCyclic[i];
  
  // Get GCD of powers of the generators of a free part
  Integer d = IntProblems().gcdOfVector(gFreePart);
  
#if SAFETY > 0
  if ( d<1 )
    error("Parameters for gcd are wrong in "
	  "AbelianGroupRep::isEltProperPower(const Word& w)");
#endif
  // If the powers of free generators in the canonical decomposition
  // are prime to each other, than not a proper power
  if (d==1) return false;
  
  
  
  bool haveTorsion = false;
  for (int i = rankOfFreePart;i<gInCyclic.length();i++)
    if ( gInCyclic[i] > 0 )
      haveTorsion = true;
  
  if ( haveTorsion ) {
    // Get d  as a decomposition of primes and their powers
    DArray<Integer> pDecOfg = PrimeNumbers().getPrimeDecom(d);
    for (int i=0;i<pDecOfg.height();i++)
      if (pHeightOfElt( w,pDecOfg[i][0]) > 0 )   
	return true;
    return false;
  } else {
    // d > 1 and no torsion part -> d is a maximal exponent
    // and w is a proper power
    return true;
  }
  
}

int AbelianGroupRep::isPowerOfSecond(const Word& word1, 
				     const Word& word2) const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error("the abelianization was not done before"
	  " AbelianGroupRep::isEltProperPower(const Word& w)");
#endif
  
  FPGroup G( theGroup );
  SetOf<Word> S;
  S |= word2;
  G.addRelators(S);
  
  AbelianGroup AG(G);
  AG.computeCyclicDecomposition();
  int retPower = 0;
  int powerIndex = 1;
  Word w1 = word1;
  Word w2 = word2;
  if (!AG.isTrivial( w1 ))
    return retPower;
  Integer orderOfW2 = orderOfElt(w2);
  while (!retPower){
    if (areEqual(w1,w2))
      retPower = powerIndex;
    w2 = w2*word2;
#if SAFETY > 0
    if( powerIndex > orderOfW2 &&  orderOfW2 != 0 )
      error( "void IsAbelianWordPowerOfSecondArcer::computePower( ) : "
	     "Power became bigger than the order of elt.");
#endif
    powerIndex++;
  }
  return retPower;
}

void AbelianGroupRep::abelianMaximalRoot(const Word& w, Word& maxRoot, Integer& maxExp) const
{
#if SAFETY > 0
  if ( !bHaveCyclicDecomposition )
    error("the abelianization was not done before " 
	  "AbelianGroupRep::abelianMaximalRoot(Word,Word&,Integer&)");
#endif
  Integer gOrder = orderOfElt(w);
  
  // If element of finite order, than no maximal root
  if (gOrder != 0){
    maxExp = 0;
    maxRoot = w;
    return;
  }
  
  VectorOf<Integer> gInCyclic = 
    oldToNewGens(oldInAbelianForm(w) ).getPowers();
  
  // Get free part of an element
  VectorOf<Integer> gFreePart(rankOfFreePart);
  for (int i=0;i<rankOfFreePart;i++)
    gFreePart[i] =  gInCyclic[i];
  
  // GCD of powers of the generators of the free pert
  Integer d = IntProblems().gcdOfVector(gFreePart);
#if SAFETY > 0
  if ( d<1 )
    error("something is wrong with gcd in "
	  "AbelianGroupRep::abelianMaximalRoot(Word,Word&,Integer&)");
#endif
  
  // Not a proper power, maximal root is equal to the element itself
  if (d==1){
    maxExp = 1;
    maxRoot = w;
    return;
  }
  
  bool haveTorsion = false;
  for (int i = rankOfFreePart;i<gInCyclic.length();i++)
    if ( gInCyclic[i] > 0 )
      haveTorsion = true;
  
  if ( haveTorsion ) {
    
    // Decompose d to the product of primes and thier powers
    DArray<Integer> pDecOfd = PrimeNumbers().getPrimeDecom(d);
    
    maxExp = 1;
    VectorOf<Integer> pHeights(pDecOfd.height());
    VectorOf<Integer> alfas(pDecOfd.height());
    Integer exp0 = 1;
    
    for (int i=0;i<pHeights.length();i++){
      pHeights[i] = pHeightOfElt( w,pDecOfd[i][0]);
      if (pHeights[i] < 0) 
	alfas[i] = pDecOfd[i][1];
      else
	if (pHeights[i] < pDecOfd[i][1])
	  alfas[i] = pHeights[i];
	else
	  alfas[i] = pDecOfd[i][1];
      maxExp*=pow(pDecOfd[i][0],alfas[i]);
      exp0*=pow(pDecOfd[i][0],pDecOfd[i][1]-alfas[i]);
    }
    
    VectorOf<Integer> retVec(rankOfFreePart + primeBasicMatrix.height() );
    
    for (int i=0;i<rankOfFreePart;i++)
      retVec[i]=(gInCyclic[i]*exp0)/d;
    
    Integer savBlockNum;
    int blockIndex = 0;
    int index=rankOfFreePart;
    
    while (index<retVec.length()){
      
      if (pDecOfd[blockIndex][0] == 
	  primeBasicMatrix[index-rankOfFreePart][PRIMES] ){
	retVec[index]=
	  ( gInCyclic[index] *
	    pow( pDecOfd[blockIndex][0],
		 pDecOfd[blockIndex][1] - alfas[blockIndex])  ) / 
	  pow( pDecOfd[blockIndex][0],pDecOfd[blockIndex][1]);
	index++;   
      }
      else{
	
	if (blockIndex < pDecOfd.height())
	  blockIndex++;
	else
	  index++;
	retVec[index] = 0;
      }
    }
    maxRoot = primeFormInOldGens( AbelianWord(retVec) ).getWord();
  } else {
    // If free part only
    // exponent is equal to the GCD of a free part == d
    maxExp = d;
    
    // Max root
    VectorOf<Integer> vMaxRoot( rankOfFreePart );
    for (int i = 0; i<rankOfFreePart; i++ ){
      vMaxRoot[i] = gFreePart[i]/d;
    }
    maxRoot = newToOldGens(AbelianWord(vMaxRoot) ).getWord();    
  }
  return;
}

void AbelianGroupRep::minimizeWordInNewGens( AbelianWord& w ) const
{
  int wLen = w.numberOfGenerators();
  for( int i = rankOfFreePart; i < wLen; ++i ) {
    w[i] = w[i] % theInvariants[i-rankOfFreePart];
    if( sign(w[i]) < 0 )
      w[i] += theInvariants[i-rankOfFreePart];
  }
}

VectorOf<Word> AbelianGroupRep::inverseAuto(const VectorOf<Word>& V) const
{
#if SAFETY > 0
  if ( V.length() != theGroup.numberOfGenerators() ) 
    error("Wrong argument in AbelianGroupRep::orderOfAuto");
#endif
  int sizeOfMatrix;
  sizeOfMatrix = theGroup.numberOfGenerators();
  
  Matrix<Integer> autoMatrix(sizeOfMatrix);
  
  for (int i=0;i<sizeOfMatrix;i++)
    autoMatrix[i] = oldInAbelianForm(V[i]).getPowers();
  
  MatrixComputations<Integer> computeMatrix(autoMatrix);
  computeMatrix.invertMatrix();
#if SAFETY > 0
  if (computeMatrix.isInvertibleMatrix() == no)
    error("AbelianGroupRep::inverceAuto(...) : "
	  "Vector does not define an automorphism");
#endif
  
  Matrix<Integer> savInverse(computeMatrix.getInverseMatrix());
  VectorOf<Word> returnAuto(sizeOfMatrix);
  for (int i=0;i<sizeOfMatrix;i++){
    returnAuto[i] = AbelianWord(savInverse[i]).getWord();
  }
  return returnAuto;
}

int AbelianGroupRep::orderOfAuto(const VectorOf<Word>& V) const
{
  bool haveRelators = !(theGroup.getRelators().cardinality()==0);
#if SAFETY > 0
  if ( V.length() != theGroup.numberOfGenerators() ) 
    error("Wrong argument in AbelianGroupRep::orderOfAuto");
  if ( !bHaveCyclicDecomposition && haveRelators )
    error("the abelianization was not done before "
	  "AbelianGroupRep::orderOfAuto");
#endif
  Map map(theGroup,theGroup,V);
  int sizeOfMatrix;
  if (!haveRelators)
    sizeOfMatrix = theGroup.numberOfGenerators();
  else
    sizeOfMatrix = rankOfFreePart;
  Matrix<Integer> autoMatrix(sizeOfMatrix);
  
  for (int i=0;i<sizeOfMatrix;i++){
    if (haveRelators){
      Word imageOfNewGen = map.imageOf(theNewToOldGens[i].getWord());
      AbelianWord tmpWord = oldToNewGens(oldInAbelianForm(imageOfNewGen));
      
      if (tmpWord.numberOfGenerators() > rankOfFreePart)
	for (int j=0;j<rankOfFreePart;j++)
	  autoMatrix[i][j] = tmpWord[j];
      else
	autoMatrix[i] = tmpWord.getPowers();
    }
    else
      autoMatrix[i] = oldInAbelianForm(V[i]).getPowers();
  }
  int P = matrixMult(autoMatrix,false);
  if (!haveRelators || P < 0)
    return P;
  else
    if (theInvariants.length()==0)
      return P;
  
  sizeOfMatrix = theInvariants.length();
  autoMatrix.reSize(sizeOfMatrix,sizeOfMatrix);
  for (int i=0;i<sizeOfMatrix;i++){
    Word imageOfNewGen = 
      map.imageOf(theNewToOldGens[i+rankOfFreePart].getWord());
    AbelianWord tmpWord = oldToNewGens(oldInAbelianForm(imageOfNewGen));
    for (int j=0;j<sizeOfMatrix;j++)
      autoMatrix[i][j] = tmpWord[j+rankOfFreePart];
  }
  int torsionP = matrixMult(autoMatrix,true);
  if (torsionP == P) return P;
  
  if (gcd(torsionP,P) !=1 || P == 1 || torsionP == 1)
    return max(torsionP,P);
  else
    return -1;
  //    error("Can't solve problem in AbelianGroupRep::orderOfAuto");
}

Bool AbelianGroupRep::matrixMult(const Matrix<Integer>& m,
				 bool haveTorsion) const
{
  Matrix<Integer> tmpMatrix(m);
  for (int k=1;k<1000;k++){
    //    cout << tmpMatrix << endl;
    if (MatrixComputations<Integer>(tmpMatrix).isIdentity()){
      return k;
    }
    tmpMatrix = tmpMatrix*m;
    if (haveTorsion)
      for (int j=0;j<tmpMatrix.height();j++)
	for( int i = 0; i < theInvariants.length(); ++i ) {
	  tmpMatrix[j][i] = tmpMatrix[j][i] % theInvariants[i];
	  if( sign(tmpMatrix[j][i]) < 0 )
	    tmpMatrix[j][i].negate();
	}
  }
  return -1;
  //  error("Can't solve problem in AbelianGroupRep::matrixMult");
}

Bool AbelianGroupRep::isEpimorphism(const VectorOf<Word>& V) const
{
#if SAFETY > 0
  if ( V.length() != theGroup.numberOfGenerators() ) 
    error("Wrong argument in AbelianGroupRep::isEpimorphism");
#endif
  SetOf<Word> S;
  int vLen = V.length();
  for( int i = 0; i < vLen; ++i )
    S |= V[i];
  FPGroup G(theGroup);
  G.addRelators(S);
  AbelianGroup abg(G);
  abg.computeCyclicDecomposition();
  if (!abg.isTrivial()) return FALSE;
  return TRUE;
}

#ifdef debug_AbelianGroup

void AbelianGroupRep::printMatrix( char *name, Integer **A, 
				   int height, int width ) 
{
  cout << name << endl;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++)
      // cout << " " << A[i][j];
      A[i][j].printon(cout,10,7);
      
    cout << endl;
  }
  cout << endl;
}

#endif

// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the AbelianEquationSolver class.
//
// Principal Authors: Denis Serbin
//
// Status: In progress
//
// Revision History:
//
// Special Notes:
//
//


#include "AbelianEquations.h"
#include <iostream.h>


AbelianEquationsSolver::AbelianEquationsSolver( const AbelianGroup& a , 
						const VectorOf<Word>& v ,
						int numOfVar )
  : rawA( a ),
    A( FPGroup() ),
    rawSystem( v ),
    system( v.length() ),
    b( v.length() ),
    x( numOfVar ),
    torsion( numOfVar ),
    params( numOfVar ),
    numberOfVariables( numOfVar ),
    sysRank( 0 ),
    haveSol( -1 )
{
  FPGroup G( a.getFPGroup() );
  VectorOf<Chars> q( G.numberOfGenerators() - numberOfVariables );
  
  for( int i = numberOfVariables ; i < G.numberOfGenerators() ; i++ )
    q[ i - numberOfVariables ] = (G.namesOfGenerators())[i];
  
  if( G.getRelators().cardinality() )
    {
      SetOf<Word> s = G.getRelators();
      SetIterator<Word> I(s);
      SetOf<Word> news;
      
      while( !I.done() )
	{
	  Word w = I.value();
	  for( int j = 0 ; j < w.length() ; j++ )
	    {
	      int p = Generator( w[j] ).hash();
	      if( p > 0 )
		w[j] = Generator( p - numberOfVariables );
	      else
		w[j] = Generator( p + numberOfVariables );
	    }
	  
	  news.adjoinElement( w );
	  I.next();
	}
      
      FPGroup G1( q , news );
      
      A = AbelianGroup( G1 );
    }
  else
    A = AbelianGroup( FPGroup(q) );
  
}

//@njz:default value removed. defined in .h file
//void AbelianEquationsSolver::findSolutions( File& file , bool out = true )
void AbelianEquationsSolver::findSolutions( File& file , bool out)
//
{
  haveSol = 0;
  
  if( !A.haveCyclicDecomposition() )
    A.computeCyclicDecomposition();
  
  if( !A.havePrimaryDecomposition() )
    A.findPrimaryBasis();
  
  if( !rawA.haveCyclicDecomposition() )
    rawA.computeCyclicDecomposition();
  
  if( !rawA.havePrimaryDecomposition() )
    rawA.findPrimaryBasis();
  
  makeSystem();

  if( out )
   {
     if( system.length() > 1 )
       file << "The system of equations: " << endl << endl;
     else
       file << "The equation: " << endl << endl;
     
     printRawSystem( file );

     file << endl << "can be transformed to the one: " << endl << endl;

     printSystem( file );
  
     if( system.length() > 1 )
       file << endl << "Finding solutions of this system ..." << endl << endl;
     else
       file << endl << "Finding solutions of this equation ..." << endl << endl;
   }

  //@njz
  //  int **matrix = new (int*)[ system.length() ];
  int **matrix = new int*[ system.length() ];
  //
  
  int i,j,k,i1,j1;
  
  for( i = 0 ; i < system.length() ; i++ )
   {
     matrix[i] = new int[ numberOfVariables ];
     for( j = 0 ; j < numberOfVariables ; j++ )
       {
	 Integer u = (AbelianWord( numberOfVariables , system[i] ))[j];
	 matrix[i][j] = u.as_long();
       }
   }

  VectorOf< VectorOf<int> > transform;
  VectorOf<int> trans(3);

  // diagonalization of the matrix
  
  sysRank = 0;
  
  for( i = 0 ; ( i < numberOfVariables && i < system.length() ) ; i++ )
    {
      bool flag = false;
      
      for( j = i ; j < system.length() ; j++ )
	{
	  for( k = i ; k < numberOfVariables ; k++ )
	    if( matrix[j][k] )
	      {
		flag = true;
		break;
	      }
	  
	  if( flag ) break;
	}
      
      if( k == numberOfVariables && j == system.length() )
	{    
	  if( sysRank )
	    break;
	  
	  int q;
	  for( q = 0 ; q < b.length() ; q++ )
	    if( !A.isTrivial( Word( b[q] ) ) )
	      {
		haveSol = -1;
		
		if( out )
		  if( system.length() > 1 )
		    file << "while computing the canonical form of the system it was found that this system has no solutions." << endl;
		  else
		    file << "while computing the canonical form of the equation it was found that this equation has no solutions." << endl;
		
		return;
	      }
	  
	  haveSol = 0;
	  
	  if( out )
	    if( system.length() > 1 )
	      file << "while computing the canonical form of the system it was found that this system has all group as a set of solutions." << endl;
	    else
	      file << "while computing the canonical form of the equation it was found that this equation has all group as a set of solutions." << endl;
	  
	  return;
	  
	}
      
      int *tmp = matrix[i];
      matrix[i] = matrix[j];
      matrix[j] = tmp;
      
      Word r = b[i];
      b[i] = b[j];
      b[j] = r;
      
      for( j = 0 ; j < system.length() ; j++ )
	{
	  int t = matrix[j][i];
	  matrix[j][i] = matrix[j][k];
	  matrix[j][k] = t;
	}
      
      if( i != k )
      {
	trans[0] = i;
	trans[1] = k;
	trans[2] = 0;
	
	transform.append( trans );
      }
      
      while( true )
	{
	  bool check;
	  bool flag;
	  int z;
	  bool done = false;
	  int count = i + 1;
	  
	  while( !done )
	    {
	      for( j = count ; j < system.length() ; j++ )
		if( matrix[j][i] && abs(matrix[i][i]) != abs(matrix[j][i]) )
		  break;
	      
	      if( j == system.length() )
		break;
	      
	      count = j + 1;
	      
	      flag = false;
	      
	      while( !flag )
		{
		  if( abs(matrix[i][i]) > abs(matrix[j][i]) )
		    {
		      z = matrix[i][i] / matrix[j][i];
		      for( k = i ; k < numberOfVariables ; k++ )
			matrix[i][k] = matrix[i][k] - matrix[j][k] * z;
		      b[i] = b[i] * A.getFPGroup().raiseToPower(b[j],-z);
		    }
		  else
		    {
		      z = matrix[j][i] / matrix[i][i];
		      for( k = i ; k < numberOfVariables ; k++ )
			matrix[j][k] = matrix[j][k] - matrix[i][k] * z;
		      b[j] = b[j] * A.getFPGroup().raiseToPower(b[i],-z);
		    }
		  
		  if( !matrix[i][i] || !matrix[j][i] ) 
		    {
		      if( matrix[i][i] == 0 )
			{  
			  tmp = matrix[i];
			  matrix[i] = matrix[j];
			  matrix[j] = tmp;
			  
			  r = b[i];
			  b[i] = b[j];
			  b[j] = r;
			}
		      flag = true;
		    }
		}
	      
	      if( count == system.length() ) 
		done = true;
	    }
	  
	  check = true;
	  for( j = i + 1 ; j < numberOfVariables ; j++ )
	    if( matrix[i][j] % matrix[i][i] )
	      {
		check = false;
		break;
	      }
	  
	  if( check )
	    break;

	  done = false;
	  count = i + 1;
	  while( !done )
	    {
	      for( j = count ; j < numberOfVariables ; j++ )
		if( matrix[i][j] && abs(matrix[i][i]) != abs(matrix[i][j]) )
		  break;
	      
	      if( j == numberOfVariables )
		break;
	      
	      count = j + 1;
	      
	      flag = false;
	      	      
	      while( !flag )
		{
		  if( abs(matrix[i][i]) > abs(matrix[i][j]) )
		    {
		      z = matrix[i][i] / matrix[i][j];
		      for( k = i ; k < system.length() ; k++ )
			matrix[k][i] = matrix[k][i] - matrix[k][j] * z;
		     
		      trans[0] = j;
		      trans[1] = i;
		      trans[2] = -z;
		      
		      transform.append( trans );
      		    }
		  else
		    {
		      z = matrix[i][j] / matrix[i][i];
		      for( k = i ; k < system.length() ; k++ )
			matrix[k][j] = matrix[k][j] - matrix[k][i] * z;
		      
		      trans[0] = i;
		      trans[1] = j;
		      trans[2] = -z;
		      
		      transform.append( trans );
		    }
		  
		  if( !matrix[i][i] || !matrix[i][j] ) 
		    {
		      if( matrix[i][i] == 0 )
			{  
			  for( k = i ; k < system.length() ; k++ )
			    {
			      int a = matrix[k][i];
			      matrix[k][i] = matrix[k][j];
			      matrix[k][j] = a;
			    }
			  
			  if( i != j )
			    {
			      trans[0] = i;
			      trans[1] = j;
			      trans[2] = 0;
			  
			      transform.append( trans );
			    }
			}
		      flag = true;
		    }
		}
	      
	      if( count == numberOfVariables ) 
		done = true;
	    }
	  
	  check = true;
	  for( j = i + 1 ; j < system.length() ; j++ )
	    if( matrix[j][i] % matrix[i][i] )
	      {
		check = false;
		break;
	      }
	  
	  if( check )
	    break;
	}
      
      for( j = i + 1 ; j < system.length() ; j++ )
	if( matrix[j][i] )
	  {
	    int z = matrix[j][i] / matrix[i][i];
	    for( k = i ; k < numberOfVariables ; k++ )
	      matrix[j][k] = matrix[j][k] - matrix[i][k] * z;
	    
	    b[j] = b[j] * A.getFPGroup().raiseToPower(b[i],-z);
	  }
      
      for( j = i + 1 ; j < numberOfVariables ; j++ )
	if( matrix[i][j] )
	  {
	    int z = matrix[i][j] / matrix[i][i];
	    for( k = i ; k < system.length() ; k++ )
	      matrix[k][j] = matrix[k][j] - matrix[k][i] * z;
	    
	    trans[0] = i;
	    trans[1] = j;
	    trans[2] = -z;
	    
	    transform.append( trans );
	  }
      
      sysRank++;
    }
  
  for( i = sysRank ; i < system.length() ; i++ )
    {
      if( !A.isTrivial( Word( b[i] ) ) )
	{
	  haveSol = -1;
	  
	  if( out )
	    if( system.length() > 1 )
	      file << "while computing the canonical form of the system it was found that this system has no solutions." << endl;
	    else
	      file << "while computing the canonical form of the equation it was found that this equation has no solutions." << endl;
	  
	  return;
	}
    }

  // finding solutions and output in file

  if( out )
    {
      file << "The canonical form: ";
      file << endl << endl;
    }
  
  for( int p = 0 ; p < sysRank ; p++ )
    {
      if( out )
	{      
	  file << "x" << p + 1;
	  
	  if( matrix[p][p] != 1 )
	    file << "^" << matrix[p][p];
	  
	  file << " = ";
	}
      
      AbelianWord w = A.oldInAbelianForm( b[p] );
      w = A.oldToNewGens( w );
      w = A.newToOldGens( w );    
      b[p] = w.getWord().freelyReduce();
      
      if( out )
	{
	  A.getFPGroup().printWord( file , b[p] );
	  file << endl;
	}
    }
  
  VectorOf<int> xNums( numberOfVariables );
  
  for( i = 0 ; i < sysRank ; i++ )
    if( root( b[i] , matrix[i][i] ) )
      {   
	x[i] = b[i];
	xNums[i] = i;
	
	if( !A.isFree() )
	  {
	    VectorOf<int> tmp(2);
	    tmp[0] = ( matrix[i][i] > 0 ) ? matrix[i][i] : -matrix[i][i];
	    tmp[1] = 1;
	    torsion[i].append( tmp );
	  }
      }
    else
      {
	haveSol = -1;
	
	if( out )
	{
	  if( system.length() > 1 )
	    file << endl << "The system is unsolvable because there are no solutions for: " << endl << endl;
	  else
	    file << endl << "The equation is unsolvable because there are no solutions for: " << endl << endl;
	      
	  file << "x" << i + 1;
	
	  if( matrix[i][i] != 1 )
	    file << "^" << matrix[i][i];
	
	  file << " = ";
	  A.getFPGroup().printWord( file , b[i].freelyReduce() );
	}
	
	return;
      }
  
  for( j = sysRank ; j < numberOfVariables ; j++ )
    {
      x[j] = Word();
      xNums[j] = j;

      VectorOf<int> tmp(2);
      tmp[0] = j - sysRank + 1;
      tmp[1] = 1;
      params[j].append( tmp );
    }
  
  for( i = transform.length() - 1 ; i >= 0 ; i-- )
    {
      trans = transform[i];
      
      if( !trans[2] )
	{
	  int ind1 = xNums.indexOf( trans[0] );
	  int ind2 = xNums.indexOf( trans[1] );
	  
	  xNums[ind1] = trans[1];
	  xNums[ind2] = trans[0];
	}
      else
	{
	  int ind1 = xNums.indexOf( trans[0] );
	  int ind2 = xNums.indexOf( trans[1] );
	  
	  x[ind1] *= A.getFPGroup().raiseToPower( x[ind2] , trans[2] ); 
	  
	  int len = torsion[ind1].length();
	  for( j = 0 ; j < torsion[ind2].length() ; j++ )
	    {
	      bool f = false;
	      
	      for( k = 0 ; k < len ; k++ )
		if( torsion[ind1][k][0] == torsion[ind2][j][0] )
		  {
		    f = true;
		    break;
		  }
		  
	      if( f )
		torsion[ind1][k][1] += torsion[ind2][j][1] * trans[2];
	      else
		{
		  VectorOf<int> tmp = torsion[ind2][j];
		  tmp[1] *= trans[2];
		  torsion[ind1].append( tmp );
		}
	    }
	     
	  len = params[ind1].length();
	  for( j = 0 ; j < params[ind2].length() ; j++ )
	    {
	      bool f = false;
	      
	      for( k = 0 ; k < len ; k++ )
		if( params[ind1][k][0] == params[ind2][j][0] )
		  {
		    f = true;
		    break;
		  }
		  
	      if( f )
		params[ind1][k][1] += params[ind2][j][1] * trans[2];
	      else
		{
		  VectorOf<int> tmp = params[ind2][j];
		  tmp[1] *= trans[2];
		  params[ind1].append( tmp );
		}
	    }
	}
    }

  for( i = 0 ; i < x.length() ; i++ )
    {
      AbelianWord w = A.oldInAbelianForm( x[i] );
      w = A.oldToNewGens( w );
      w = A.newToOldGens( w );
      x[i] = w.getWord();
    }
  
  // output in file
  if( out )
    {
      file << endl << "The set of solutions can be presented as follows: " << endl << endl;
      FPGroup G = rawA.getFPGroup();
      FPGroup G1 = A.getFPGroup();
      
      for( i = 0 ; i < numberOfVariables ; i++ )
	{
	  int n = xNums.indexOf( i );
	  bool flag = false;
	  
	  G.printWord( file , Generator( i + 1 ) );
	  file << " -> ";
	  
	  if( x[n].length() )
	    {
	      G1.printWord( file , x[n].freelyReduce() );
	      flag = true;
	    }
	  
	  for( j = 0 ; j < torsion[n].length() ; j++ )
	    {
	      VectorOf<int> tmp = torsion[n][j];
	      int z = tmp[1] % tmp[0];
	      
	      if( z )
		if( tmp[1] > 0 )
		  {
		    if( flag )
		      file << " + ";
		    
		    if( tmp[1] != 1 )
		      file << tmp[1] << " p( " << tmp[0] << " )";
		    else
		      file << "p( " << tmp[0] << " )";
		    
		    flag = true;
		  }
		else
		  {
		    file << " - ";
		    
		    if( tmp[1] != -1 )
		      file << -tmp[1] << " p( " << tmp[0] << " )";
		    else
		      file << "p( " << tmp[0] << " )";
		    
		    flag = true;
		  }
	    }
	  
	  for( j = 0 ; j < params[n].length() ; j++ )
	    {
	      VectorOf<int> tmp = params[n][j];
	      
	      if( tmp[1] )
		if( tmp[1] > 0 )
		  {
		    if( flag )
		      file << " + ";
		    
		    if( tmp[1] != 1 )
		      file << tmp[1] << " t" << tmp[0];
		    else
		      file << "t" << tmp[0];
		    
		    flag = true;
		  }
		else
		  {
		    file << " - ";
		    
		    if( tmp[1] != -1 )
		      file << -tmp[1] << " t" << tmp[0];
		    else
		      file << "t" << tmp[0];
		    
		    flag = true;
		  }
	    }
	  
	  if( !flag )
	    file << "1 ";
	      
	  if( i != numberOfVariables )
	    file << ",";
	  
	  file << endl;
	}
  
      file << endl << "where  p( n ) is any element of n-heigth and t_i - any element of the group." << endl;
    }
  
  VectorOf< VectorOf< VectorOf<int> > > t( numberOfVariables );
  VectorOf< VectorOf< VectorOf<int> > > p( numberOfVariables );
  VectorOf<Word> x1( numberOfVariables );
  
  for( i = 0 ; i < numberOfVariables ; i++ )
    {
      int n = xNums.indexOf( i );
      x1[i] = x[n];
      t[i] = torsion[n];
      p[i] = params[n];
    }
  
  torsion = t;
  params = p;
  x = x1;
  
  for( i = 0 ; i < system.length() ; i++ )
    delete [] matrix[i];
  delete [] matrix;
}

bool AbelianEquationsSolver::solIsAllGroup() const
{
  return haveSol == 0;
}

bool AbelianEquationsSolver::haveSolutions() const
{
  return ( haveSol >= 0 );
}

// can be invoced only after the cyclic decomposition has been computed
bool AbelianEquationsSolver::root( Word& w , int n ) const
{
  AbelianGroup A1 = A.getCanonicalSmithPresentation();
  FPGroup G = A1.getFPGroup(); 
  
  AbelianWord nw = A.oldInAbelianForm( w );
  nw = A.oldToNewGens( nw );
  Word ans;
  
  VectorOf<Integer> powers = nw.getPowers();
  
  for( int i = 0 ; i < powers.length() ; i++ )
    {
      Generator g( i + 1 );
      
      int ord = A1.orderOfElt( g ).as_long();
      int pow = powers[i].as_long();
      
      if( pow )
	if( !ord )
	  {
	    if( pow % n )
	      return no;
	    else
	      for( int j = 0 ; j < abs(pow / n) ; j++ )
		ans *= ( (pow / n) > 0 ) ? g : inv(g);
	  }
	else
	  if( pow % n )
	    {
	      ord = ( ord > 0 ) ? ord : -ord;
	      
	      int x;
	      for( x = -ord ; x < ord ; x++ )
		if( ! ( (n * x - pow) % ord ) )
		  break;
	      
	      for( int j = 0 ; j < abs( x ) ; j++ )
		ans *= ( x > 0 ) ? g : inv(g);
	    }
	  else
	    for( int j = 0 ; j < abs(pow / n) ; j++ )
	      ans *= ( (pow / n) > 0 ) ? g : inv(g);
    }
  
  nw = AbelianWord( G.numberOfGenerators() , ans );
  nw = A.newToOldGens( nw );
  w = nw.getWord();
  
  return yes;
}
 
void AbelianEquationsSolver::makeSystem()
{
  for( int i = 0 ; i < rawSystem.length() ; i++ )
    {
      for( int j = 0 ; j < rawSystem[i].length() ; j++ )
	{
	  Generator g = rawSystem[i][j];
	  
	  if( abs( g.hash() ) > numberOfVariables )
	    {
	      Generator newg;
	      if( g.hash() > 0 )
		newg = Generator( g.hash() - numberOfVariables );
	      else
		newg = Generator( g.hash() + numberOfVariables );
	      
	      b[i] *= inv(newg);
	    }
	  else
	    system[i] *= g;
	}
      
      system[i] = system[i].freelyReduce();
      b[i] = b[i].freelyReduce();
    }
}
  
void AbelianEquationsSolver::printRawSystem( File& file ) const
{
  for( int i = 0 ; i < rawSystem.length() ; i++ )
    {
      rawA.getFPGroup().printWord( file , rawSystem[i] );
      file << " = 1" << endl;
    }
}

void AbelianEquationsSolver::printSystem( File& file ) const
{
  for( int i = 0 ; i < system.length() ; i++ )
    {
      AbelianWord a( numberOfVariables , system[i] );
      
      rawA.getFPGroup().printWord( file , a.getWord() );
    
      file << " = ";
      
      A.getFPGroup().printWord( file , b[i].freelyReduce() );
      
      file << endl;
    }
}



// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the FPGroupRep class.
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//
// Special Notes:
//
//

#include "FPGroupRep.h"
#include "FreeGroup.h"
#include "PresentationParser.h"
#include "AbelianGroup.h"
#include "conversions.h"
#include "GCD.h"

//@rn Since many of these are no longer in use, they just return dontknow:

int FPGroupRep::order() const
{
  return -1;
}


Trichotomy FPGroupRep::isFinite( ) const
{
  return DONTKNOW;
}


Trichotomy FPGroupRep::isInfinite( ) const
{
  return DONTKNOW;
}


Trichotomy FPGroupRep::isTrivial( ) const
{
  return DONTKNOW;
}


Trichotomy FPGroupRep::isAbelian( ) const
{
  return DONTKNOW;
}


Trichotomy FPGroupRep::isFree( ) const
{
  return DONTKNOW;
}

bool FPGroupRep::compare( const GroupRep* G ) const
{
  if( ! FGGroupRep::compare(G) ) 
    return false;

  const FPGroupRep *G1 = (const FPGroupRep*) G;

  if( relators.cardinality() != G1->relators.cardinality() )
    return false;

  for(SetIterator<Word> I = relators; !I.done(); I.next() ) {
    if( !G1->relators.contains( I.value() ) )
      return false;
  }

  return true;
}

void FPGroupRep::printRelators(ostream& ostr) const
{
  int doComma = 0;
  SetIterator<Word> I(relators);
  while ( !I.done() ) {
	 if ( doComma ) ostr << ',';
	 printWord( ostr, I.value() );
	 doComma = 1;
	 I.next();
  }
}


void FPGroupRep::printOn(ostream& ostr) const
{
  ostr << "< ";
  printGenerators( ostr );
  if ( relators.cardinality() > 0 ) ostr << " ; ";
  printRelators( ostr );
  ostr << " >";
}


GroupRep* FPGroupRep::readFrom(istream& istr, Chars& errMesg) const
{
  PresentationParser P(istr);
  return P.parseFPGroup( errMesg );
}



// Methods dealing with group elements:

 
Word FPGroupRep::shortenByRelators(const Word& w) const
 
 
// This method takes a word argument w and shortens it by relators of
// the group. Before this operation, the set of relators is symmetrized.
// If word argument w contains a piece of some relator of group, and the
// rest of relator is smaller than this piece, procedure shortens word w.
 
{
    SetOf<Word> SymRelators(relators);
 
    // Symmetrize the relators set
    symmetrise(SymRelators);
 
//@dp  We symmetrize relators every time the method is called.
//     It's rather expensive; maybe we'd better store the symmetrized
//     set in the representation.
 
    Word u = w.freelyReduce();
 
    Bool wasShortened = TRUE; // whether u was shortened on current iteration
    SetIterator<Word> I(SymRelators);
 
    do
    {   // do one iteration with all relators:
        // check whether the word u may be shortened by a relator.
 
        wasShortened = FALSE;
        I.reset();
        while (!I.done())
        {
            Word result = u.shortenByRelator(I.value()).freelyReduce();
            if( result < u )
            {
                u = result;
                wasShortened = TRUE;
                break;
            }
            I.next();
        }
    } while( wasShortened );
    return u;
}

void computation(int **matrix,int *b,int *x,VectorOf<Word>& rels,int n)
{
  FreeGroup F(n);
  
  int i,j,k;
  
  for( i = 0 ; i < n && i < rels.length() ; i++ )
    {
      bool flag = false;
      
      for( j = i ; j < n ; j++ )
	{
	  for( k = i ; k < rels.length() ; k++ )
	    if( matrix[j][k] )
	      {
		flag = true;
		break;
	      }
	  
	  if( flag ) break;
	}
      
      if( j == n && k == rels.length() )
	break;
      
      int *tmp = matrix[i];
      matrix[i] = matrix[j];
      matrix[j] = tmp;

      int q = b[i];
      b[i] = b[j];
      b[j] = q;
      
      for( j = 0 ; j < n ; j++ )
	{
	  int t = matrix[j][i];
	  matrix[j][i] = matrix[j][k];
	  matrix[j][k] = t;
	}
      
      Word a = rels[i];
      rels[i] = rels[k];
      rels[k] = a;
      
      int gcd = matrix[i][i];
      for( j = i + 1 ; j < rels.length() ; j++ )
	gcd = GCD(gcd,matrix[i][j]);
      
      for( j = i ; j < rels.length() ; j++ )
	matrix[i][j] = matrix[i][j] / gcd;

      b[i] = b[i] / gcd;
	
      if( abs(matrix[i][i]) != 1 )
	{
	  bool done = false;
	  int count = i + 1;
	  while( !done )
	    {
	      for( j = count ; j < rels.length() ; j++ )
		if( matrix[i][j] && abs(matrix[i][i]) != abs(matrix[i][j]) )
		  break;
	      
	      count = j + 1;
	      
	      gcd = GCD(matrix[i][i],matrix[i][j]);
	      flag = false;
	      int z;
	      
	      while( !flag )
		{
		  if( abs(matrix[i][i]) > abs(matrix[i][j]) )
		    {
		      z = matrix[i][i] / matrix[i][j];
		      for( k = 0 ; k < n ; k++ )
			matrix[k][i] = matrix[k][i] - matrix[k][j] * z;
		      rels[i] = rels[i] * F.raiseToPower(rels[j],-z);
		    }
		  else
		    {
		      z = matrix[i][j] / matrix[i][i];
		      for( k = 0 ; k < n ; k++ )
			matrix[k][j] = matrix[k][j] - matrix[k][i] * z;
		      rels[j] = rels[j] * F.raiseToPower(rels[i],-z);
		    }
		  
		  if( !matrix[i][i] || !matrix[i][j] ) 
		    {
		      if( matrix[i][i] == 0 )
			{  
			  for( k = 0 ; k < n ; k++ )
			    {
			      int t = matrix[k][i];
			      matrix[k][i] = matrix[k][j];
			      matrix[k][j] = t;
			    }
			  a = rels[i];
			  rels[i] = rels[j];
			  rels[j] = a;
			}
		      flag = true;
		    }
		}
	      
	      if( abs(gcd) == 1) 
		done = true;
	    }
	}
      
      if( matrix[i][i] < 0 )
	{
	  for( j = i ; j < rels.length() ; j++ )
	    matrix[i][j] = - matrix[i][j];
	  
	  b[i] = - b[i];
	}
      
      for( j = i + 1 ; j < n ; j++ )
	{
	  int d = matrix[j][i];
	  for( k = i ; k < rels.length() ; k++ )
	    matrix[j][k] = matrix[j][k] - d * matrix[i][k];
	  
	  b[j] = b[j] - d * b[i];
	}
    }
  
  i--;
    
  for( j = rels.length() - 1 ; j > i ; j-- ) 
    x[j] = 0;
  
  for( j = i ; j >= 0 ; j-- )
    {
      int sum = 0;
      for( k = i ; k > j ; k-- )
	sum += matrix[j][k] * x[k];
      
      x[j] = b[j] - sum;
    }
  
  return;
}

Chars FPGroupRep::productOfCommutators(const Word& w,File& file)
{
  VectorOf<Chars> groupGens(theNamesOfGenerators);
  FreeGroup F(groupGens);
  
  if( w.allExponentSumsZero() )
    {
      file << "Exponent sums for all letters in  ";
      F.printWord(file,w);
      file << "  are equal to zero. " << endl;

      return F.productOfCommutators(w,file);
    }
  
  FPGroup G( theNamesOfGenerators , relators );
  AbelianGroup A( G );
  
  A.computeCyclicDecomposition();
  if( !A.isTrivial(w) )
    return Chars("not in derived");

  SetIterator<Word> I(relators);
  VectorOf<Word> rels;
  Word u;

  while( !I.done() )
    {
      u = I.value();
      if( rels.indexOf(u) == -1 && !u.allExponentSumsZero() )
	rels.append(u);
      I.next();
    }
    
  int n = theNumberOfGenerators;
  //@njz
  //  int **matrix = new (int*)[n];
  int **matrix = new int*[n];
  //
  int *b = new int[n];
  int *x = new int[rels.length()];
  int i;
  
  for( i = 0 ; i < n ; i++ )
    {
      matrix[i] = new int[rels.length()];
      b[i] = w.exponentSum(i+1);

      for( int j = 0 ; j < rels.length() ; j++ )
	matrix[i][j] = rels[j].exponentSum(i+1); 
    }

  file << "In abelianization  ";
  F.printWord(file,w);
  file << "  can be presented as a product of relators: " << endl;
    
  computation(matrix,b,x,rels,n);
  
  Word nw = w;
  for( i = 0 ; i < rels.length() ; i++ )
    {
      if( x[i] )
	{
	  if( x[i] != 1 )
	    file << "( ";
	  
	  F.printWord(file,rels[i]);
	  
	  if( x[i] != 1 )
	    file << " )^" << x[i] << " ";
	}
      nw *= raiseToPower(rels[i],-x[i]);
    }
  
  file << endl << endl << "The new word " << endl;
  F.printWord(file,nw.freelyReduce());
  file << endl << "is equal to" << endl;
  F.printWord(file,w);
  file << endl << "but exponent sums for all letters in it are equal to zero, so it can be rewritten as a product of commutators in the free group" << endl;

  delete [] b;
  delete [] x; 
  for( i = 0 ; i < n ; i++ )
    delete [] matrix[i];
  delete [] matrix;

  Chars r = F.productOfCommutators(nw,file);
  file << endl;
   
  return r;
}

Chars FPGroupRep::productOfSquares(const Word& w,File& file)
{
  VectorOf<Chars> groupGens(theNamesOfGenerators);
  FreeGroup F(groupGens);
  
  bool flag1 = true;
  for( int i = 0 ; i < w.length() ; i++ )
    {
      Generator g = w[i];
      if( w.numberOfOccurrences(g) % 2 )
	{
	  flag1 = false;
	  break;
	}
    }
  
  if( flag1 )
    {
      file << "Exponent sums for all letters in  ";
      F.printWord(file,w);
      file << "  are even." << endl;
      
      Chars r = F.productOfSquares(w,file);
      file << endl;

      return r; 
    }
  
  SetOf<Word> nrels(relators);
  
  for( int p = 0 ; p < theNumberOfGenerators ; p++ )
    nrels.adjoinElement( Generator(p+1) * Generator(p+1) );
    
  FPGroup G( theNamesOfGenerators , nrels );
  AbelianGroup A( G );
  
  A.computeCyclicDecomposition();
  if( !A.isTrivial(w) )
    return Chars("not in square");

  SetIterator<Word> I(relators);
  VectorOf<Word> rels;
  Word u;

  while( !I.done() )
    {
      u = I.value();
      if( rels.indexOf(u) == -1 )
	rels.append(u);
      I.next();
    }
  
  int n = theNumberOfGenerators;
  //@njz
  //  int **matrix = new (int*)[n];
  int **matrix = new int*[n];
  //
  int *b = new int[n];
  int *x = new int[rels.length()];
  int i;
  
  for( i = 0 ; i < n ; i++ )
    {
      matrix[i] = new int[rels.length()];
      b[i] = w.exponentSum(i+1) % 2;

      for( int j = 0 ; j < rels.length() ; j++ )
	matrix[i][j] = rels[j].exponentSum(i+1) % 2; 
    }

  file << "In abelianization  ";
  F.printWord(file,w);
  file << "  can be presented as a product of relators: " << endl;
  
  computation(matrix,b,x,rels,n);
  
  Word nw = w;
  for( i = 0 ; i < rels.length() ; i++ )
   {
     if( x[i] )
       {
	 if( x[i] != 1 )
	   file << "( ";
	 
	 F.printWord(file,rels[i]);
	 
	 if( x[i] != 1 )
	   file << " )^" << x[i] << " ";
       }
     nw *= raiseToPower(rels[i],x[i]);
   }

  file << endl << endl << "The new word " << endl;
  F.printWord(file,nw.freelyReduce());
  file << endl << "is equal to" << endl;
  F.printWord(file,w);
  file << endl << "but exponent sums for all letters in it are even, so it can be rewritten as a product of squares in the free group" << endl;
  
  delete [] b;
  delete [] x;
  for( i = 0 ; i < n ; i++ )
    delete [] matrix[i];
  delete [] matrix;
  
  Chars r1 = F.productOfSquares(nw,file);
  file << endl;
  
  return r1;
}


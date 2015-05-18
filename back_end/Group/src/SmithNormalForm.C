// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the SmithNormalForm class
//
// Principal Authors: Sergei Lyutikov, Roger Needham
//
// Status: provisional
//
// Revision History:
//


#include "SmithNormalForm.h"



SmithNormalForm::SmithNormalForm(Integer** theMatrix, int rows, int cols) :
  matrix(theMatrix),
  resultTemp(1)
{
  height = rows;
  width = cols;
  i = 0;
  j = 0;
  done = false;
}



bool SmithNormalForm::continueComputation( )
{
  int k, l;
  Integer quotient;
  
  while ( i < height && j < width ) {
	 
	 // Check if j^{th} generator is free of relations in abelianization.
	 k = 0;
	 if ( matrix[i][j] == 0 )
		for ( k = i + 1; k < height; k++ )
		  if ( matrix[k][j] != 0 ) {
			 Integer* temp = matrix[i];
			 matrix[i] = matrix[k];
			 matrix[k] = temp;
			 break;
		  }
	 if ( k == height ) {
		resultTemp[0] = (Integer)resultTemp[0] + 1;
		++j;
		continue;
	 }
	 
	 done = false;
	 while ( !done ) {
		
		for ( k = i + 1; k < height; k++ )
		  while ( matrix[k][j] != 0 )
			 if ( abs( matrix[k][j] ) < abs( matrix[i][j] ) ) {
				Integer* temp = matrix[i];
				matrix[i] = matrix[k];
				matrix[k] = temp;
			 }
			 else {
				quotient = matrix[k][j] / matrix[i][j];
				for ( l = j; l < width; l++ )
				  matrix[k][l] = (Integer)matrix[k][l] - quotient*matrix[i][l];
			 }
		
		done = true;
		for ( k = j + 1; k < width; k++ )
		  while ( matrix[i][k] != 0 )
			 if ( abs( matrix[i][k] ) < abs( matrix[i][j] ) ) {
				done = false;  // Possibly we spoiled j^{th} column.
				quotient = ( matrix[i][j] - sign(matrix[i][j]) ) / matrix[i][k];
				for ( l = i; l < height; l++ )
				  matrix[l][j] -= quotient*matrix[l][k];
			 }
			 else {
				quotient = matrix[i][k] / matrix[i][j];
				for ( l = i; l < height; l++ )
				  matrix[l][k] -= quotient*matrix[l][j];
			 }
	 }
	 
	 resultTemp.append( abs( matrix[i][j] ) );
	 i++;
	 j++;
  }
  resultTemp[0] = (Integer)resultTemp[0] + width - j;

  /*@rn*/
  
  cerr << "\n\nReduced matrix:\n";
  for( int idx_i = 0; idx_i < height; ++idx_i ) {
	 cerr << endl;
	 for( int idx_j = 0; idx_j < width; ++idx_j )
		cerr << matrix[idx_i][idx_j] << ' ';
	 cerr << endl << endl << flush;
  }
  /**/

  
  for ( i = 0; i < height; i++ ) delete [] matrix[i];
  delete [] matrix;
  
  // m_i | m_{i+1}
  for ( i = 1; i < resultTemp.length() - 1; i++ )
	 for ( j = i + 1; j < resultTemp.length(); j++ ) {
		Integer temp = GCD( resultTemp[i], resultTemp[j] );
		resultTemp[j] = (Integer)resultTemp[j] * (Integer)resultTemp[i];
		resultTemp[j] = (Integer)resultTemp[j] / temp;
		resultTemp[i] = temp;
	 }
  
  rankOfFreePart = (int)(((Integer)resultTemp[0]).as_long());
  for ( i = 1; i < resultTemp.length(); i++ )
	 // m_i > 1
	 if ( resultTemp[i] != 1 ) theInvariants.append( resultTemp[i] );

  return ( done = true );
  //@rn Bogus! Do this properly.
}


int SmithNormalForm::getTorsionFreeRank( ) const
{
#if SAFETY > 0
  if ( !done )
	 error("called SmithNormalForm::getTorsionFreeRank() when !done");
#endif

  return rankOfFreePart;
}


VectorOf<Integer> SmithNormalForm::getTorsionInvariants( ) const
{
#if SAFETY > 0
  if ( !done )
	 error("called SmithNormalForm::getTorsionInvariants() when !done");
#endif

  return theInvariants;
}


Integer SmithNormalForm::GCD( Integer a, Integer b ) const
{
  Integer quotient;
  if ( a == 0 ) return b;
  if ( b == 0 ) return a;
  while ( b != 0 )
	 if ( abs(b) < abs(a) ) {
		quotient = ( a - sign(a) ) / b;
		a -= quotient*b;
	 }
	 else {
		quotient = b / a;
		b -= quotient*a;
	 }
  return a;
}

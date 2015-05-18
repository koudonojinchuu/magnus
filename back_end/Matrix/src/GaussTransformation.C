// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the GaussTransformation class
//
// Principal Authors: Dmitry Bormotov, Alexey Myasnikov
//
// Status: Useable
//
// Revision History:
//
//


#include "GaussTransformation.h"


// ------------------------- GaussTransformation ----------------------------//
template <class T>
void GaussTransformation<T>::runRow(int rowNum){
   #if SAFETY > 0
    if (currentRow+rowNum > matrix.height() )
      error("void GaussTransformation::runRow( ) : to many rows to run.");
   #endif
   untilRow = currentRow+rowNum-1;
   continueComputation( RUNROW);
  }

template <class T>
void GaussTransformation<T>::runUntilDiagHasZero(){
        continueComputation( UNTILNOT_0);
  }

template <class T>
void GaussTransformation<T>::startComputation( ) 
  {
  #if SAFETY > 0   
    if ( bStart )
      error("void GaussTransformation::startComputation( ) : "
	    "the computation has been already started.");
   if ( matrix.width()==0 || matrix.height()==0 )
      error("void GaussTransformation::startComputation( ) : "
	    "the matrix has zero bounds.");
  #endif
     if (buildTransformations){
          if (transMatrix) delete transMatrix;
          transMatrix = new Matrix<T>(matrix.height());
          for (int i = 0; i<matrix.height();i++)
            for (int j = 0; j<matrix.height();j++)
            if (i==j)
                 (*transMatrix)[i][j] = 1;
            else
                 (*transMatrix)[i][j] = 0;
       } 
     if (buildInverseTransformations){
          if (invTransMatrix) delete invTransMatrix;
          invTransMatrix = new Matrix<T>(matrix.height());
          for (int i = 0; i<matrix.height();i++)
            for (int j = 0; j<matrix.height();j++)
            if (i==j)
                 (*invTransMatrix)[i][j] = 1;
            else
                 (*invTransMatrix)[i][j] = 0;
       } 
    bStart = true;
    bDone = false;
    currentRow = 0;
    currentCol = 0;
    transformed = false;
    isSingularMatrix = dontknow;
    isInvertibleMatrix = dontknow;
  }  


template <class T>
void GaussTransformation<T>::addRow( int firstRow, int secondRow, T koef )
{
  for( int l = currentCol; l < matrix.width(); l++ ){
    matrix[firstRow][l] += koef * matrix[secondRow][l];
  }
  transformed = true;
  if (transMatrix){
     for (int i=0;i<(*transMatrix).height();i++)
       (*transMatrix)[firstRow][i] += koef * (*transMatrix)[secondRow][i];
  }
 if (invTransMatrix){
     for (int i=0;i<(*invTransMatrix).height();i++)
       (*invTransMatrix)[i][secondRow] -= koef * (*invTransMatrix)[i][firstRow];
  }
}

template <> void GaussTransformation<Integer>::makeZero( int& row1, int& row2 )
{
  while( true ) {
    if( abs(matrix[row1][currentCol]) >= abs(matrix[row2][currentCol]) ) {
      addRow(row1, row2, -matrix[row1][currentCol] / matrix[row2][currentCol]);
      if( sign(matrix[row1][currentCol]) == 0 ) {
	row1 = row2;
	break;
      }
    }
    else {
      addRow(row2, row1, -matrix[row2][currentCol] / matrix[row1][currentCol]);
      if( sign(matrix[row2][currentCol]) == 0 )
	break;
    }
  }
}

template <> void GaussTransformation<Rational>::makeZero( int& row1, int& row2 )
{
  addRow(row2, row1, -matrix[row2][currentCol] / matrix[row1][currentCol]);
}

template <> void GaussTransformation<double>::makeZero( int& row1, int& row2 )
{
  addRow(row2, row1, -matrix[row2][currentCol] / matrix[row1][currentCol]);
}

template <class T>
void GaussTransformation<T>::continueComputation(int whatDeal )
{
#if SAFETY > 0
  if ( !bStart )
    error("void GaussTransformation<T>::continueComputation( ) : "
	  "tried to continue computation before it's started");
  if ( bDone )
    error("void GaussTransformation<T>::continueComputation( ) : "
	  "tried to continue computation after it's done");
#endif

 while (true){
  if( currentRow < matrix.height()-1 && currentCol < matrix.width() ) {
    int row1 = -1;
    
    for( int i = currentRow; i < matrix.height(); i++ ) 
      if( sign( matrix[i][currentCol] ) != 0 ) {
	row1 = i;
	break;
      }
    
    if( row1 == -1 ) {
      isSingularMatrix = yes;
      isInvertibleMatrix = no;
      currentCol ++;   
      if (whatDeal ==  UNTILNOT_0 ||  whatDeal == UNTIL_1) return;
      if( whatDeal == RUNROW && currentRow > untilRow ) return;
      // Returns, if we need to know are diagonal elements 0 or not

    }
    else{
    
    int row2 = row1;

    while( true ) {
      bool bSuccess = false;
      for( int i = row2 + 1; i < matrix.height(); i++ ) {
	if( sign( matrix[i][currentCol] ) != 0 ) {
  	 row2 = i;
	 bSuccess = true;
	 break;
        }
      }
      if( !bSuccess ) {
	if( row1 != currentRow ) {
	  addRow(currentRow, row1, 1);
	  addRow(row1, currentRow, -1);
	}
	currentCol++;
	currentRow++;
	  if( abs( matrix[currentRow-1][currentCol-1] ) != T(1) ){
              isInvertibleMatrix = no;
              if( whatDeal == UNTIL_1 ) return;
          }
	  if( sign( matrix[currentRow-1][currentCol-1] ) == 0 ){
              isSingularMatrix = yes;
              if( whatDeal == UNTILNOT_0 ) return;
          }
	  if( whatDeal == RUNROW && currentRow > untilRow ) return;
          // return, if we made needed number of rows
          break;
      }
      else 
	makeZero(row1, row2);
    }
    }
  }
  else {
    int savcurrentCol = currentCol;
    int savcurrentRow = currentRow;
    if( currentCol == matrix.width() ){
           --savcurrentCol;
    }else
        currentRow++;
        currentCol++;
    if( sign(matrix[savcurrentRow][savcurrentCol]) == 0 )
      finishComputation(yes, no);
    else
	if( abs( matrix[savcurrentRow][savcurrentCol] ) != T(1) )
	  finishComputation(no, no);
	else
	  finishComputation(no, yes);
    return;
  }
 }
}

// This was made because, the checkin is matrix invertible or not makes sence
// only for Integers
template <class T>
void GaussTransformation<T>::runWhileDiagHasSingles(){}

template <> void GaussTransformation<Integer>::runWhileDiagHasSingles()
{
        continueComputation( UNTIL_1);
}

template <class T>
Trichotomy GaussTransformation<T>::isInvertible() const { return dontknow;}

template <> Trichotomy GaussTransformation<Integer>::isInvertible() const 
{
    return isInvertibleMatrix;
}

template class GaussTransformation<Integer>;
template class GaussTransformation<Rational>;
template class GaussTransformation<double>;

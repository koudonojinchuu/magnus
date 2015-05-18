// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementations of classes Matrix, MatrixParser. 
//
// Principal Author: Dmitry Bormotov, Alexey Myasnikov
//
// Status: In development
//
// Usage:
//
// Discussion:
//
// Revision History:
//
// Next Implementation Steps:
//


#include "Matrix.h"
#include "Int2.h"


//------------------------------------------------------------------------//
//-------------------------------- MatrixRep -----------------------------//
//------------------------------------------------------------------------//


template <class R>
MatrixRep<R>& MatrixRep<R>::operator += ( const MatrixRep& M )
{
#if SAFETY > 0
  if( this->width != M.width || this->height != M.height )
    error("the different size of matrices in Matrix::operator += "
	  "(const Matrix&)");
#endif
  for( int i = 0; i < this->height; i++ ){
    for( int j = 0; j < this->width; j++ ) 
      this->theArray[this->rowBegin+i][j] = this->theArray[this->rowBegin+i][j] + M.theArray[M.rowBegin+i][j];
  }
  return *this;
}


template <class R>
MatrixRep<R>& MatrixRep<R>::operator -= ( const MatrixRep& M )
{
#if SAFETY > 0
  if( this->width != M.width || this->height != M.height)
    error("the different size of matrices in Matrix::operator -= "
	  "(const Matrix&)");
#endif
  
  for( int i = 0; i < this->height; i++ )
    for( int j = 0; j < this->width; j++ )
      this->theArray[this->rowBegin+i][j] = this->theArray[this->rowBegin+i][j] - M.theArray[M.rowBegin+i][j];

  return *this;
}


template <class R>
MatrixRep<R>& MatrixRep<R>::operator *= ( const MatrixRep& M )
{
#if SAFETY > 0
  if( this->width != M.height || this->width!=M.width ) 
    error("the different size of matrices in Matrix::operator *= "
	  "(const Matrix&)");
#endif
  MatrixRow<R>* tmpArray = this->theArray;
  int tmpRowBegin = this->rowBegin;
  int tmpColBegin = this->colBegin;
  makeArray(this->height,this->width);
  for( int i = 0; i < this->height; i++ )
    for( int j = 0; j < M.width; j++ ) {
      R amount = 0;
      for( int k = 0; k < this->width; k++ )
	amount += tmpArray[tmpRowBegin+i][k] * M.theArray[M.rowBegin+k][j];
      this->theArray[i][j] = amount;
    }

  //@dp another wrong code:
  this->deleteArray();
  // we should use: deleteArray( tmpArray );
  
  return *this;
}


template <class R>
MatrixRep<R> MatrixRep<R>::operator + (const MatrixRep& M) const
{
#if SAFETY > 0
  if( this->width != M.width || this->height != M.height)
    error("the different size of matrices in Matrix::operator + "
	  "(const Matrix&)");
#endif

  MatrixRep<R> result(M);
  
  for( int i = 0; i < this->height; i++ )
    for( int j = 0; j < this->width; j++ )
      result.theArray[i][j] = result.theArray[i][j] + this->theArray[this->rowBegin+i][j];

  return result;
}


template <class R>
MatrixRep<R> MatrixRep<R>::operator - () const
{
  MatrixRep result(this->height,this->width);
  for( int i = 0; i < this->height; i++ )
    for( int j = 0; j < this->width; j++ )
      result.theArray[i][j] = - this->theArray[this->rowBegin+i][j];
  
  return result;
}


template <class R>
MatrixRep<R> MatrixRep<R>::operator - (const MatrixRep& M) const
{
#if SAFETY > 0
  if( this->width != M.width || this->height != M.height)
    error("the different size of matrices in Matrix::operator - "
	  "(const Matrix&)");
#endif

  MatrixRep result(M);
  
  for( int i = 0; i < this->height; i++ )
    for( int j = 0; j < this->width; j++ ) 
      result.theArray[i][j] = this->theArray[this->rowBegin+i][j] - result.theArray[i][j];

  return result;
}


template <class R>
MatrixRep<R> MatrixRep<R>::operator * (const MatrixRep& M) const
{
#if SAFETY > 0
  if( this->width != M.height ) 
    error("the different size of matrices in Matrix::operator * "
	  "(const Matrix&)");
#endif

  MatrixRep result(this->height,M.width);
  
  for( int i = 0; i < this->height; i++ )
    for( int j = 0; j < M.width; j++ ) {
      R amount = 0;
      for( int k = 0; k < this->width; k++ )
	amount += this->theArray[this->rowBegin+i][k] * M.theArray[M.colBegin+k][j];
      result.theArray[i][j] = amount;
    }
  
  return result;
}


/*
template <class R>
void MatrixRep<R>::printOn(ostream& ostr) const
{
  ostr << '[' << endl;
  for( int i = 0; i < size; i++ ) {
    ostr << "[ ";
    for( int j = 0; j < size; j++ ) {
      ostr << theMatrix[i][j];
      if( j != size - 1 )
	cout << ", ";
    }
    ostr << " ]";
    if( i < size - 1 )
      ostr << ',';
    ostr << endl;
  }
  ostr << ']';
}


*/

template class MatrixRep<Integer>;
template class MatrixRep<Rational>;
template class MatrixRep<double>;

#include "../../Polynomial/include/Polynomial.h"
template class MatrixRep< Polynomial<Integer> >;
template class MatrixRep< Polynomial<Int2> >;


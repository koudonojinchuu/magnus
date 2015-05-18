// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class Matrix.
//
// Principal Author: Dmitry Bormotov, Alexey Myasnikov
//
// Status: Useable
//
// Usage:
//
// Revision History:
//
//


#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "DArray.h"
#include "DerivedObjectOf.h"

//------------------------------------------------------------------------//
//------------------------------- MatrixRep ------------------------------//
//------------------------------------------------------------------------//


template <class R> class MatrixRep : public DArrayRep<R> {

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  MatrixRep( int height, int width) : DArrayRep<R>(height,width) {}
  // Makes MatrixRep with height == h and width == w

  MatrixRep( int n ) : DArrayRep<R>(n) {}
  // Makes MatrixRep with height=width=n

  MatrixRep* clone( ) { return new MatrixRep(*this); }
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Operators:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  MatrixRep& operator += ( const MatrixRep& );

  MatrixRep& operator -= ( const MatrixRep& );

  MatrixRep& operator *= ( const MatrixRep& );  

  MatrixRep operator + ( const MatrixRep& ) const; 

  MatrixRep operator - ( const MatrixRep& ) const; 

  MatrixRep operator * ( const MatrixRep& ) const; 

  MatrixRep operator - ( ) const;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

  //friend int main( );

#endif

};

//------------------------------------------------------------------------//
//------------------------------- Matrix    ------------------------------//
//------------------------------------------------------------------------//


template <class R> class Matrix : public DerivedObjectOf<DArray<R> ,MatrixRep<R> >{

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  Matrix(int n = 0 ) : DerivedObjectOf<DArray<R>,MatrixRep<R> >(new MatrixRep<R>(n)) { }
  // Makes Matrix with height=width=n


  Matrix(int h, int w) : DerivedObjectOf<DArray<R>,MatrixRep<R> >(new MatrixRep<R>(h, w)){ }
  // Makes Matrix with height == h and width == w

  // copy constructor supplied by compiler

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Operators:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Matrix& operator += ( const Matrix& M ) {
     *this->change() += *(M.look());
     return *this;
  }

  Matrix& operator -= ( const Matrix& M ) {
     *this->change() -= *(M.look());
     return *this;
  }

  Matrix& operator *= ( const Matrix& M ) {
      *this->change() *= *(M.look());
      return *this;
  }
  Matrix operator + ( const Matrix& M) const {
           return Matrix(*this->look() + *(M.look())); } 

  Matrix operator - ( const Matrix& M ) const {
           return Matrix(*this->look() - *(M.look())); } 

  Matrix operator * ( const Matrix& M ) const {
           return Matrix(*this->look() * *(M.look())); } 

  Matrix operator - ( ) const {return Matrix(- *this->look());}


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O:                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Inherited from DArray  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Inherited from DArray  

  protected :
	Matrix( MatrixRep<R> newrep ) : 
            DerivedObjectOf<DArray<R>,MatrixRep<R> >(new MatrixRep<R>(newrep)) {}
	Matrix( MatrixRep<R>* newrep ) : 
            DerivedObjectOf<DArray<R>,MatrixRep<R> >(newrep) {}

};

#endif





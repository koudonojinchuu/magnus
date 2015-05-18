// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class SquareMatrix.
//
// Principal Author: Alexey Myasnikov
//
// Status: In development
//
// Usage:
//
// Revision History:
//
//


#ifndef _MATRIX_COMPUTATIONS_H_
#define _MATRIX_COMPUTATIONS_H_

#include "GaussTransformation.h"


//------------------------------------------------------------------------//
//------------------------- MatrixComputations ---------------------------//
//------------------------------------------------------------------------//


template <class R> class MatrixComputations {

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  MatrixComputations( const Matrix<R>& matrix) : 
    isInvertible(dontknow), 
    inverseMatrix(NULL)
  {
    if (matrix.height() != matrix.width())
      error ("Matrix is not quadratic");
    theMatrix = matrix;
    detKnown = false;
  }

  ~MatrixComputations()
  {
     if (inverseMatrix)
        delete inverseMatrix;
  }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
    const Matrix<R>& matrix() const { return theMatrix;}

  int size() const { return theMatrix.height();}
  // Returns the size of SquareMatrix

  bool isIdentity() const;
  // True if matrix is identical

  R getDeterminant();
  // Returns determinant if exists, if not then computes it.

  bool detKnow() const { return detKnown;}
  // true if determinant was computed

  Trichotomy isInvertibleMatrix() const { return isInvertible;}
  // Yes if matrix invertible
  
  void invertMatrix();
  // Makes inverse matrix if it exists
 
  const Matrix<R>& getInverseMatrix() const {
    #if SAFETY > 0
      if ( isInvertible != yes )
      error("Trichotomy SquareMatrix::getInverseMatrix( ) : "
	    "tried to get result before the computation or this matrix is not Invertible.");
    #endif
    return *inverseMatrix;
  }

  R det() const {  return determinant;}
  // This function nedet only for constant acces to determinant
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  IPC tools:                                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  friend ostream& operator < ( ostream& ostr, const MatrixComputations& DA )
  {
    DA.write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, MatrixComputations& DA)
  {
    DA.read(istr);
    return istr;
  }
private:
  void write( ostream& ostr ) const{
     ostr < theMatrix;
     ostr < detKnown;
     ostr < isInvertible;
     ostr < determinant;
     if (isInvertible == yes) ostr < *inverseMatrix;
  };
 
  void read( istream& istr ){
     istr > theMatrix;
     istr > detKnown;
     istr > isInvertible;
     istr > determinant;
     delete inverseMatrix;
     if (isInvertible == yes){
      Matrix<R> tmpMatrix;
      istr > tmpMatrix;
      inverseMatrix  = new Matrix<R>(tmpMatrix);
     }
  }

  void abolishCoefficients(Matrix<R>& matrix );

  bool detKnown;
  // True if determinant was computed

  Trichotomy isInvertible;
  // Yes if inverse matrix was built, no if matrix couldn't be inverted, 
  // otherwize -  dontknow

  R determinant;
  // determinant

  Matrix<R>* inverseMatrix;
  // Holds inverse matrix, if it was built

  Matrix<R> theMatrix;
  // Holds the matrix.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

  //friend int main(...);

#endif

};


#endif

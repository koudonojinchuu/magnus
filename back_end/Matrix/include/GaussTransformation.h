// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the GaussTransformation class.
//
// Principal Authors: Dmitry Bormotov, Aleksey Myasnikov
//
// Status: Useable
//
// Revision History:
//


#ifndef _GAUSS_TRANSFORMATION_H_
#define _GAUSS_TRANSFORMATION_H_

#include "Integer.h"
#include "Rational.h"
#include "Matrix.h"

// --------------------------------------------------------------------------//
// ------------------------- GaussTransformation ----------------------------//
// --------------------------------------------------------------------------//



template <class T> class GaussTransformation {


public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  // If buildTransMatrix or buildInvTransMatrix = true the transformation matrix would be built
  GaussTransformation(const Matrix<T>& M, bool buildTransMatrix = false, bool buildInvTransMatrix = false) : 
    matrix( M ), 
    bDone( false ), bStart( false ),transformed(false), 
    isSingularMatrix( dontknow ), isInvertibleMatrix( dontknow ),
    transMatrix(NULL),invTransMatrix(NULL),buildTransformations(buildTransMatrix),
    buildInverseTransformations(buildInvTransMatrix) 
  { }

  // Copy constructor and operator = are disabled.
  GaussTransformation(const GaussTransformation&);
  GaussTransformation operator = ( const GaussTransformation& );

  ~GaussTransformation() {
    if (transMatrix)  delete transMatrix;
  }
   
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Activation members:                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // This was left, because sometimes Gauss runing with breaks or restarting,
  // so it's needed to reinitialize the algorithm
  void startComputation( );
         
  // Runs until the end of transformation. It's long time algorithm
  void run(){
        continueComputation( RUNALL);
  }

  // Run for 'rowNum' rows, then suspends.
  void runRow(int rowNum);

  // Runs until a zero element appears on diagonal.
  void runUntilDiagHasZero();

  // Runs while diagonal elements are 1 or -1.
  // It's a very stupid name for such procedure, but i couldn't find something better.
  void runWhileDiagHasSingles();

  // returns the indexs of current diagonal element.
  int getCurrentRow() const { return currentRow-1;}
  int getCurrentCol() const { return currentCol-1;}
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Status Queries:                                                     //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool canChange( ) const { return !bStart; }
  // You can change matrix only if GausTransformation is not in process.
  // Return true if you can change matrix.

  bool done( ) const { return bDone; }
  // True if computation was finished
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  Trichotomy isSingular( ) const 
  {
    return isSingularMatrix;
  }

  Trichotomy isInvertible( ) const; 
  
  // Returns transformated matrix
  const Matrix<T>& getMatrix() const
    {
      return matrix;
    }
  // Returns transformation matrix
  const Matrix<T>& getTrMatrix()const
    {
   #if SAFETY > 0
    if (!transMatrix )
      error("void GaussTransformation::getTrMatrix( ) : object was not built.");
   #endif
      return *transMatrix;
    }
  // Returns inverse transformation matrix
  const Matrix<T>& getInvTrMatrix()const
    {
   #if SAFETY > 0
    if (!invTransMatrix )
      error("void GaussTransformation::getInvTrMatrix( ) : object was not built.");
   #endif
      return *invTransMatrix;
    }
  
  // Returns reference on transformated matrix
  Matrix<T>& refMatrix()
    {
   #if SAFETY > 0
    if (!canChange() )
      error("void GaussTransformation::refMatrix( ) : transformations are in process.");
   #endif
      return matrix;
    }
    bool isTransformed() const{
      return transformed;
    }

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool bDone; 
  bool bStart;
  bool transformed;
  Trichotomy isSingularMatrix;
  Trichotomy isInvertibleMatrix;

  Matrix<T> matrix;  // Main  matrix
  Matrix<T>* transMatrix; // Transformation matrix
  Matrix<T>* invTransMatrix; // Inverse transformation matrix
  int untilRow;  // Number of rows to run gaus transformation for
  int currentRow; // Current row of transformation
  int currentCol; // Current column of transformation
  bool buildTransformations; // True if transformation matrix is building
  bool buildInverseTransformations; // True if inverse transformation matrix is building

  // Constants for types of running a computations.
  static const int  UNTILNOT_0 = 0;
  static const int  UNTIL_1 = 1;
  static const int  RUNROW = 2;
  static const int  RUNALL = 3;
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Inner methods:                                                      //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  void continueComputation( int whatDeal);

  void finishComputation( Trichotomy isSingular, Trichotomy isInvertible )
  {
    if (isSingularMatrix != yes)
        isSingularMatrix = isSingular;
    if (isInvertibleMatrix != no)
        isInvertibleMatrix = isInvertible;
    if (matrix.height()!=matrix.width())
        isSingularMatrix = no;
    bDone = true;
    bStart = false;
  }

  void addRow( int firstRow, int secondRow, T koef);
  // Adds elements of secondRow * koef to elements of firstRow

  void makeZero( int& row1, int& row2 );
  // Makes a zero in column

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

  //friend int main( );

#endif

};
#endif




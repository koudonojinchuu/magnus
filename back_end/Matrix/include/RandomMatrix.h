// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definitions of class RandomMatrix.
//           
//
// Principal Author: Dmitry Bormotov
//
// Status: In development
//
//
// Usage:
//
//
// Revision History:
//
//


#ifndef _RANDOM_MATRIX_H_
#define _RANDOM_MATRIX_H_

#include "Matrix.h"


//------------------------------ RandomMatrix -------------------------------//


template <class R> class RandomMatrix 
{

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  RandomMatrix( int n = 0 ) 
  {
  #if SAFETY > 0
    if( n < 0 )
      error("RandomMatrix::RandomMatrix( int ) : "
	    "incorrect matrix size.");
  #endif  

    size = n;
  }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int getSize() const { return size; }  
    
  Matrix<R> getRandomMatrix( );  
 

private:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int size;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int coefficient( );

  Matrix<R> getAtomicMatrix( );

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




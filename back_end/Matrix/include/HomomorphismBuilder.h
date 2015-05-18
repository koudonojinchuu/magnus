// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the HomomorphismBuilder class.
//
// Principal Authors: Dmitry Bormotov
//
// Status: in development
//
// Revision History:
//


#ifndef _HOMOMORPHISM_BUILDER_H_
#define _HOMOMORPHISM_BUILDER_H_


#include "FPGroup.h"
#include "Matrix.h"


// ------------------------- HomomorphismBuilder ----------------------------//


template <class R> class HomomorphismBuilder 
{

 public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  HomomorphismBuilder(FPGroup& G, int sizeOfMatrix = 2) : group( G ), 
    matrixSize( sizeOfMatrix ), bDone( false ), bStart( false ),
    homomorphism( G.numberOfGenerators() ), 
    invertedMatrices( G.numberOfGenerators() )
  {
  #if SAFETY > 0
    if( sizeOfMatrix < 2 )
      error("HomomorphismBuilder::HomomorphismBuilder( FPGroup&, int ) :"
	    "size of matrix must be 2 or more.");
    if( G.numberOfGenerators() == 0 ) 
      error("HomomorphismBuilder::HomomorphismBuilder( FPGroup&, int ) :"
	    "The group must be non trivial.");
  #endif  
  }
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Activation members:                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void startComputation( ) 
  {
  #if SAFETY > 0   
    if ( bStart )
      error("void HomomorphismBuilder::startComputation( ) : "
	    "the computation has been already started.");
  #endif
  
    bStart = true;
    bDone = false;

  #ifdef DEBUG
    iterCount = 0;
  #endif
  }  


  void continueComputation( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Status Queries:                                                     //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool done( ) const { return bDone; }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  VectorOf< Matrix<R> > getHomomorphism( ) const 
  {
  #if SAFETY > 0
    if ( !bDone )
      error("VectorOf< Matrix<R> > HomomorphismBuilder::getHomomorpism() const"
	    " : tried to get result before the computation is finished.");
  #endif

    return homomorphism;
  }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool bDone;
  bool bStart;
  
  VectorOf< Matrix<R> > homomorphism;
  VectorOf< Matrix<R> > invertedMatrices;
  int matrixSize;
  FPGroup& group;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private methods:                                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
    
  void finishComputation( ) 
  {
    bDone = true;
    bStart = false;
    
  #ifdef DEBUG
    cout << iterCount << endl;
  #endif
  }
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

  int iterCount;

  friend int main( );

#endif

};



#endif




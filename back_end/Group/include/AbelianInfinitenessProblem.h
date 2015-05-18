// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the AbelianInfinitenessProblem class.
//
// Principal Authors: Dmitry Bormotov
//
// Status: 
//
// Revision History:
//
// Special Remarks:
//


#ifndef _ABELIANINFINITENESSPROBLEM_H_
#define _ABELIANINFINITENESSPROBLEM_H_


#include "FPGroup.h"
#include "GaussTransformation.h"


// ---------------------  AbelianInfinitenessProblem ------------------------//


class AbelianInfinitenessProblem 
{

public:

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AbelianInfinitenessProblem( const FPGroup& G) : theGroup( G ),
    bStart( false ), bDone( false ) { }

  ~AbelianInfinitenessProblem( ) 
  {
    if( !bDone ) // clear a memory
      finishComputation();
  }
      

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Activation members:                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void startComputation( );
  // Start the computation. 
  // You shouldn't call this more than one time.
             
  bool continueComputation( ) 
  {
  #if SAFETY > 0
    if ( !bStart )
      error("void AbelianInfinitenessProblem::continueComputation( ) : "
	    "tried to continue computation before it's started");
  #endif
 
    if( bDone )
      return bDone;

    GT->run();

    if( GT->done() ) {
      itIsInfinite = ( GT->isSingular() == YES );
      finishComputation();
    }

    return bDone;
  }
  // Advance the computation. 
  // You have to call startComputation() before.
  // This function returns true if the computation is done.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Status Queries:                                                     //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool done( ) const { return bDone; }
  // true only when the computation is finished.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // You can call all these functions iff the computation is finished
  // ( when the done() functions returns true ).

  bool isInfinite( ) 
  {
  #if SAFETY > 0
    if ( !bDone )
      error("bool AbelianInfinitenessProblem::isInfinite( ) : "
	    "tried to get result before the computation is finished.");
  #endif

    return itIsInfinite;
  }

  
private:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool bDone;
  bool bStart;
  bool itIsInfinite;
  Matrix<Rational> *matrix;
  int width;
  int height;
  const FPGroup theGroup;
  GaussTransformation<Rational> *GT;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private methods:                                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AbelianInfinitenessProblem( const AbelianInfinitenessProblem& );
  // It is hidden, not implemented.

  AbelianInfinitenessProblem& operator = ( const AbelianInfinitenessProblem& );
  // It is hidden, not implemented.

  void finishComputation( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Debugging stuff:                                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
  //friend int main( );
#endif

};

#endif

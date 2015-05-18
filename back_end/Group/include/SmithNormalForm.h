// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the SmithNormalForm utility class.
//           This encapsulates the computation which puts
//           an Integer matrix into diagonal form.
//
// Principal Authors: Sergei Lyutikov, Roger Needham
//
// Status: provisional
//
// Special Remarks:
//
// * The libg++ Integer class uses the basic wrapped pointer scheme,
//   "However, constructors and assignments operate by copying entire
//    representations, not just pointers."
//   This does not impose an unacceptable cost here, since the Integers
//   will not typically be enormous, and objects in this class will not
//   be instantiated very frequently.
//   If these assumptions change, we can replace type Integer with
//   type Integer* in the computations.
//
// Revision History:
//
// * 6/95 Roger adapted this from class Abelianization.
//

#ifndef _SMITH_NORMAL_FORM_H_
#define _SMITH_NORMAL_FORM_H_


#include "ExtendedIPC.h"

#include "Vector.h"


class SmithNormalForm {
public:

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Constructors:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////

  SmithNormalForm(Integer** theMatrix, int rows, int cols);
  // This reduces theMatrix in place, and deletes it when done.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Activation members:                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool continueComputation( );
  // Return true iff the computation is complete.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int getTorsionFreeRank( ) const;
  // It is an error to call this unless continueComputation() has returned
  // true.

  VectorOf<Integer> getTorsionInvariants( ) const;
  // It is an error to call this unless continueComputation() has returned
  // true.


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Integer** matrix;
  // The matrix we are reducing in place

  int height, width;

  // The answer:

  VectorOf<Integer> theInvariants;
  int               rankOfFreePart;

  // Control variables for continueComputation():
  
  int i, j;

  VectorOf<Integer> resultTemp;

  bool done;
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private methods:                                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Integer abs( const Integer& a ) const { return ( a > 0 ) ? a : -a; }

  Integer sign( const Integer& a ) const {
	 if ( a == 0 ) return 0;
	 return ( a > 0 ) ? 1 : -1;
  }

  Integer GCD( Integer a, Integer b ) const;

};

#endif

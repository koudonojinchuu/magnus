// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of utility class Homology, which wraps a
//           BlackBox connected to the Homology C Library.
//
// Principal Author: Roger Needham
//
// Status: Under development
//
// Usage:
//
//   The Homology C Library is more efficient if it computes the entire
// desired range of groups in one program run, instead of being invoked
// separately for each dimension, but the latter is permitted.
// 
//   Pass a KBMachine for a finite, confluent and terminating rewrite
// system, along with the desired range of dimensions, to the
// constructor.
//   To begin computing the first homology group, call workOnNextGroup().
// It returns true iff the computation of the current group is complete,
// and may be extracted with getTorsionFreeRank() and
// getTorsionInvariants(). It is an error to call either of these if the
// last call to workOnNextGroup() returned false, or has never been
// called.
//   After workOnNextGroup() returns true, the next call will start work
// on the next group. The previous group may not be extracted at this
// point.
//   It is an error to call workOnNextGroup() more times than is
// consistent with the indicated range of dimensions desired.
//   To prepare for computing `all' homology, i.e. until the resources
// are exhausted, pass a large number such as MAXINT for the upper
// dimension. Note that the computation will finish only for fairly small
// dimensions.
//
//
// Revision History:
//


#ifndef _HOMOLOGY_UNIT_H_
#define _HOMOLOGY_UNIT_H_


#include "BlackBox.h"
#include "KBMachine.h"
#include "SmithNormalForm.h"


class Homology {

public:

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Constructors:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////

  Homology(const KBMachine M, int start_dim, int end_dim);
  // M specifies the group of which we want the homology, and
  // 1 < start_dim <= end_dim are the dimensions we want.

  ~Homology( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Activation members:                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool workOnNextGroup( );
  // See usage comments

  //////////////////////////////////////////////////////////////////////
  //                                                                  //
  // Accessors:                                                       //
  //                                                                  //
  //////////////////////////////////////////////////////////////////////

  int getTorsionFreeRank( ) const;
  // See usage comments

  VectorOf<Integer> getTorsionInvariants( ) const;
  // See usage comments

  // Could also allow queries on the status of the current computation.


private:

  //////////////////////////////////////////////////////////////////////
  //                                                                  //
  // Data Members:                                                    //
  //                                                                  //
  //////////////////////////////////////////////////////////////////////

  char tempFileName[100];
  // The executable chom wants its input from a file; this is the name.
  
  BlackBox* chom;
  // Wraps the chom executable.

  enum State { READING, REDUCING, GOT_ONE };
  State myState;

  int dimensionToDo;
  // Calls to workOnNextGroup() work on this dimension.

  int lastDimension;
  // A check against runaway calls to workOnNextGroup().

  SmithNormalForm* SNF;
  // The current smith normal form computation, or NULL if none.

  bool booted;
  // true iff we have read and reduced the first, `bootstrap' matrix.

  int previousTorsionFreeRank;
  int columns;

  //////////////////////////////////////////////////////////////////////
  //                                                                  //
  // Private Members:                                                 //
  //                                                                  //
  //////////////////////////////////////////////////////////////////////

  Integer** readMatrix(int& rows, int& cols) const;
  // Read the next matrix from *chom's output.
};

#endif

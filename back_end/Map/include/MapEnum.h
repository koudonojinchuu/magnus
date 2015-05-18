// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class MapEnum, for enumerating Maps,
//           and helper class IntTuples
//
// Principal Author: Roger Needham
//
// Status: In development
//
// Further Implementation Steps:
//
// * Need to deal with 1-generator domains.
//
// Revision History:
//


#ifndef _MAP_ENUM_H_
#define _MAP_ENUM_H_


#include "Map.h"


//------------------------------------------------------------------------//
//----------------------------- IntTuples --------------------------------//
//---------------------------- helper class ------------------------------//


class IntTuples
{
  // Objects in this class enumerate all tuples of non-negative integers.
  // Pass the length of tuple desired, and the starting radius in the
  // taxicab metric.

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  IntTuples(const int tupleLength, const int startRadius = 1);
  // tupleLength >= 1, startRadius >= 1

  ~IntTuples( ) { delete [] tuple; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const int* nextTuple( );
  // You know how long the tuple is.
  // This retains custody of the int*.


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int r;
  // Current radius, i.e., for n == 2:
  //
  // . . .           . . .           * . .
  // . . . r == 0,   * . . r == 1,   . * . r == 2, etc.
  // * . .           . * .           . . *
  //
  // Notice that the entries in a tuple add up to r.

  int* tuple;
  // The tuple we return.

  int* end;
  // Points to last entry of tuple

  int* sp;
  // The `stack' pointer.
};



//------------------------------------------------------------------------//
//------------------------------ MapEnum ---------------------------------//
//------------------------------------------------------------------------//

class MapEnum
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  MapEnum(const FGGroup& domain, const FGGroup& range, int radius = 1);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Map nextMap(int jump = 1);


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private Members:                                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Word kthWord(int k, int n);
  // Returns the kth freely reduced word on n semigroup generators.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int domainRank;
  int rangeRank;

  const FGGroup& theDomain;
  const FGGroup& theRange;

  IntTuples theTuples;
};

#endif

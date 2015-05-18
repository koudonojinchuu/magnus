// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of classes UniformRandom and NormalRandom,
//           for generating random numbers.
//           The algorithms come from ``Numerical Recipies in C''.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:


#include "RandomNumbers.h"


void UniformRandom::reseed(int seed)
{
  if ( seed < 0 ) seed = -seed;
  ix1 = (IC1 + seed) % M1;
  ix1 = (IA1 * ix1 + IC1) % M1;
  ix2 = ix1 % M2;
  ix1 = (IA1 * ix1 + IC1) % M1;
  ix3 = ix1 % M3;
  for( int j = 1; j <= 97; ++j ) {
    ix1 = (IA1 * ix1 + IC1) % M1;
    ix2 = (IA2 * ix2 + IC2) % M2;
    //@njz:
    //    r[j] = (ix1 + ix2 * RM2) * RM1;
    r[j] = (ix1 + ix2 / (float)M2) / (float)M1;
  }
}


float UniformRandom::rand( ) {
  ix1 = (IA1 * ix1 + IC1) % M1;
  ix2 = (IA2 * ix2 + IC2) % M2;
  ix3 = (IA3 * ix3 + IC3) % M3;
  int j = 1 + ((97 * ix3) / M3);
  float temp = r[j];
  //@njz:
  //  r[j] = (ix1 + ix2 * RM2) * RM1;
  r[j] = (ix1 + ix2 / (float)M2) / (float)M1;
  //
  return temp;
}


int UniformRandom::rand(int lower, int upper)
{
  int result;  // Let's play it damn safe:
  do {
    result = int(lower + (upper - lower) * rand() + 0.5);
  } while ( (result < lower) || (result > upper) );
  return result;
}



float NormalRandom::rand( ) {
  float fac, r, v1, v2;
  
  if ( iset == 0 ) {
    do {
      v1 = 2.0 * uniformDeviates.rand( ) - 1.0;
      v2 = 2.0 * uniformDeviates.rand( ) - 1.0;
      r = v1 * v1 + v2 * v2;
    } while ( r >= 1.0 );
    fac = sqrt(-2.0 * log(r) / r);
    gset = v1 * fac;
    iset = 1;
    return v2 * fac;
  } else {
    iset = 0;
    return gset;
  }
}


int NormalRandom::rand(int mean, int stddev)
{
  return int(mean + stddev * rand() + 0.5);
}

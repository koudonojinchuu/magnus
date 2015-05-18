// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definitions of classes UniformRandom and NormalRandom,
//           for generating random numbers.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
//
// * 07/96 Aleksey M. implemented IPC tools.
//
// * 09/97 Dmitry B. added function long TimeSeed( ).
//         From now on UniformRandom and NormalRandom use it as a
//         default seed value. 
//

#ifndef _RANDOMNUMBERS_H_
#define _RANDOMNUMBERS_H_

#include <iostream.h>
#include "IPC.h"
#include "Timer.h"

//@njz: removed to get rid of complain different exceptions thrown in standard libraries
//extern "C" double sqrt(double);
//extern "C" double log(double);
#include <cmath>
//


inline long TimeSeed() {
  timeval tp;
  gettimeofday(&tp,0);
  return tp.tv_sec;
}


class UniformRandom {

// An object in this class generates uniformly random numbers of type
// int and float.

public:

  UniformRandom(int seed = TimeSeed()) { reseed(seed); }

  void reseed(int seed);
  // In particular, calling this with the same seed as passed to the
  // constructor starts the random sequence over.

  float rand( );
  // Returns uniformly random floats in [0.0, 1.0].

  int rand(int lower, int upper);
  // Returns uniformly random ints in [lower, upper]


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const UniformRandom& unRand )
  {
    ostr < unRand.ix1 < unRand.ix2 < unRand.ix3;
    for (int i = 0; i<98; i++) { ostr << unRand.r[i] << ' ';}// There is no > and < 
    return ostr;                                             // operators for float in IPC.h
  }
  
  friend istream& operator > ( istream& istr, UniformRandom& unRand)
  {
    istr > unRand.ix1 > unRand.ix1 > unRand.ix2 > unRand.ix3;
    for (int i = 0; i<98; i++) { istr >> unRand.r[i];}
     return istr;
  }

private:

  static const int   M1  = 259200;
  static const int   IA1 = 7141;
  static const int   IC1 = 54773;

  //@njz:unable to solve reference with gcc-4.0, so use M1 directly
  //  static const float RM1 = 1.0/M1;
  //

  static const int   M2  = 134456;
  static const int   IA2 = 8121;
  static const int   IC2 = 28411;

  //@njz: unable to solve reference with gcc-4.0, so use M2 directly
  //  static const float RM2 = 1.0/M2;
  //

  static const int   M3  = 243000;
  static const int   IA3 = 4561;
  static const int   IC3 = 51349;

  long ix1, ix2, ix3;
  float r[98];
};



class NormalRandom {

// An object in this class generates normally distributed random numbers
// of type int and float.

public:

  NormalRandom(int seed = TimeSeed()) : uniformDeviates(seed), iset(0) { }

  void reseed(int seed) { uniformDeviates.reseed(seed); }
  // In particular, calling this with the same seed as passed to the
  // constructor starts the random sequence over.

  float rand( );
  // Returns normally distributed random floats with mean 0 and variance 1.

  int rand(int mean, int stddev);
  // Returns normally distributed random ints with mean mean and standard
  // deviation stddev. For example, rand(100, 20) returns ints, roughly 68%
  // of which fall within 20 of 100, and 95% within 40 of 100.
  // Beware that rand(100, 20) could return a negative int, though it is
  // unlikely. You may want something like
  //   do { i = G.rand(100, 20); } while ( i < 1 );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const NormalRandom& noRand )
  {
    ostr < noRand.uniformDeviates < noRand.iset;
    ostr << noRand.gset << ' ';
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, NormalRandom& noRand)
  {
    istr > noRand.uniformDeviates > noRand.iset;
    istr >> noRand.gset;
    return istr;
  }

private:

  UniformRandom uniformDeviates;
  int iset;
  float gset;
};

#endif

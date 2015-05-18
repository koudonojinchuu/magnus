// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class Permutation
//
// Principal Author: Dmitry Bormotov
//
// Status: Under development
//
// Usage:
//
// Revision History:
//


#ifndef _Permutation_h_
#define _Permutation_h_

#include "IStreamPoll.h"
#include "Vector.h"


// ------------------------------ Permutation ------------------------------ //


class Permutation
{
  // There's no need to make rep: the only data member is VectorOf<int>,
  // which is reped, i.e., a pointer.

public:

  //@njz: removed
  //  struct GAPPermutation;
  //  friend struct GAPPermutation;
  //

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Constructors:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////

  Permutation::Permutation( VectorOf<int> v = VectorOf<int>() );
  // Initialize the permutation given its bottom row v; v must define
  // a permutation; there's no check for correctness; if you need to
  // check use operator >>.

  // CC, destructor and op = supplied by compiler.


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Accessors:                                               //
  //                                                          //
  //////////////////////////////////////////////////////////////

  Permutation inverse( ) const;
  // computes the inverse of the permutation


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O:                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator << ( ostream&, const Permutation& );

  friend IStreamPoll operator >> ( istream&, Permutation& );

  // the following is used only by inGAP()

  //@njz: moved from private to public
  struct GAPPermutation {
    GAPPermutation( Permutation& p ) : perm(p) { }
    Permutation& perm;
  };
  //

  GAPPermutation inGAP( ) { return GAPPermutation(*this); }
  // Used to print permutations in GAP format; manipulator like.
  // Usage: 
  //         Permutation p;
  //         cout << p.inGAP() << endl;
  //         cout << p << p.inGAP() << p.inGAP() << p << ...;
 
private:
  

  friend ostream& operator << ( ostream& o, const GAPPermutation& gp );
  
  friend IStreamPoll operator >> ( istream& i, const GAPPermutation& gp );
  

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Private functions:                                       //
  //                                                          //
  //////////////////////////////////////////////////////////////

  void printInGAP( ostream& ) const;
  // print permutation in GAP format; internal use only; use inGAP() instead

  void readInGAP( istream&, Chars& );
  // read permutation in GAP format; internal use only; use inGAP() instead


  //////////////////////////////////////////////////////////////
  //                                                          //
  // Data members:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////

  VectorOf<int> perm; // the bottom row of the permutation      
};

#endif




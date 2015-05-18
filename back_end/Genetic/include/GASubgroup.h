// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of GASubgroup class
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _GASubgroup_H_
#define _GASubgroup_H_

#include "SubgroupGraph.h"
#include "RandomNumbers.h"


//------------------------------ GASubgroup ---------------------------------//


class GASubgroup
{

public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  GASubgroup( );
  
  GASubgroup( const SetOf<Word>& generators ) : gens( generators ) { }
 
  // copy constructor supplied by compiler.
  
  // destructor supplied by compiler.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SetOf<Word> generators( ) const { return gens; }

  int fitness( const GASubgroup& S ) const;

  GASubgroup mutate( ) const;

  GASubgroup crossover( const GASubgroup& S ) const;

  GASubgroup randomSubgroup( ) const;
      

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // OI:                                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  
  friend ostream& operator << ( ostream& ostr, const GASubgroup& S )
  {
    ostr << S.gens << endl;
    return ostr;
  }

  /*
  friend istream& operator >> ( istream& istr, GHNConfig& C )
  {
    C.readFrom(istr);
    return istr;
  }
  */

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Word randomWord( ) const;
  
  int randomGen( ) const;

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  SetOf<Word> gens; // subgroup generators
  static int maxCard;
  static int maxWordLen;
  static int numOfGens;
  static UniformRandom r;
};

#endif

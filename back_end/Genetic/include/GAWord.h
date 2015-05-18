// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of GAWord class
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _GAWord_H_
#define _GAWord_H_

#include "RandomNumbers.h"
#include "Map.h"
#include "FreeGroup.h"


// ------------------------------ GAWord ---------------------------------- //


class GAWord
{

public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  GAWord( ) : numOfGens( 0 ) { }
  
  GAWord( int NumOfGens, const Word& w ) : numOfGens(NumOfGens), theWord( w )
  { }
 
  // copy constructor supplied by compiler.
  
  // destructor supplied by compiler.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Word getWord( ) const { return theWord; }

  //int fitness( const GAWord& w ) const; 

  GAWord mutate( ) const;

  GAWord crossover( const GAWord& w ) const;

  GAWord randomWord( ) const;
      

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IO:                                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  
  friend ostream& operator << ( ostream& ostr, const GAWord& w )
  {
    ostr << w << endl;
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

  int randomGen( ) const;

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  int numOfGens;
  Word theWord;
  static int maxWordLen;
  static UniformRandom r;
};

#endif

// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of the ShortenByRelators class
//
//
// Principal Author: Dmitry Bormotov, Dmitry Pechkin
//
// Status: in progress
//
// Usage:
//

#ifndef _SHORTEN_BY_RELATORS_H_
#define _SHORTEN_BY_RELATORS_H_

#include "SymmetricRelators.h"


//------------------------- class ShortenByRelators --------------------------//


class ShortenByRelators {

public:

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Constructors:                                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  // No default constructor
  // Copy constructor provided by compiler (does logical deep copy).
  
  ShortenByRelators ( const SymmetricRelators& symmetricRelators );
  // To construct a ShortenByRelators class of given symmetric relators.

  ShortenByRelators ( const SetOf<Word>& ); 
  // Special hack to forbid construct ShortenByRelatos of 
  // non symmetrised set of relators.

  ~ShortenByRelators( );
    
  ///////////////////////////////////////////////////////
  //                                                   //
  // Accessors:                                        //
  //                                                   //
  ///////////////////////////////////////////////////////

  Word getShortenWord( const Word& w ) const;  
  // Return shorten form of word w.
  
private:

  ///////////////////////////////////////////////////////
  //                                                   //
  //  Data members:                                    //
  //                                                   //
  ///////////////////////////////////////////////////////

  Word* sortRelators;    // relators in sorted order, used for optimization 
  int sortRelatorsLen;   // the number of sorted relators

};


#endif



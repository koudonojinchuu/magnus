// Copyright (C) 1999 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class PowerSeriesWP
//
// Principal Author: Dmitry Bormotov
//
// Status: in use
//
// Revision History:
//


#ifndef _PowerSeriesWP_H_
#define _PowerSeriesWP_H_

#include "FreeGroup.h"
#include "Word.h"


// ---------------------------- PowerSeriesWP ------------------------------ //


// Alternative way to solve the word problem for a free nilpotent group

class PowerSeriesWP {

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  PowerSeriesWP( const FreeGroup& group, int nClass );

  // copy constructor, destructor and operators =, == supplied by compiler.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool isTrivial(const Word& w);


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data types:                                                         //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  struct State { 
    
    State( int Coef = 1, int WPos = 0, int VPos = 0, int Power = 1) { 
      coef = Coef; wPos = WPos; vPos = VPos, power = Power; }
    
    int coef; int wPos; int vPos; int power; 
  };

  struct Stack{ 
    
    Stack( int c ) : stackLen(0) { states = new State[c]; }
    
    ~Stack( ) { delete [] states; }
    
    void put( State s ) { states[stackLen++] = s; }
    
    void get( State& s) 
      { 
	if( stackLen > 0 )
	  s = states[--stackLen];
      }
    
    int length( ) { return stackLen; }
    
    State& state() { return states[stackLen - 1]; }
    
    private:
    
    State* states;
    int stackLen;
  };
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FreeGroup F;
  int c;
};

#endif

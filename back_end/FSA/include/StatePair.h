// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: The deterministic finite state automaton class
//
// Principal Author: Paul Wakefield
//
// Status: in progress
//
// Revision History:
//
#ifndef _StatePair_H_
#define _StatePair_H_

/*
For the time being this file cannot be dependent on other FSA stuff,
since this file is #included by ..../general/src/Set.C
#include "DFSAData.h"
typedef DFSAData::State State ;
*/
typedef int State ;

class StatePair {

public:
//constructor
StatePair(): first(0),second(0){};

//copy constructor
StatePair( const StatePair& s )
{ first = s.getFirstElt() ; second =  s.getSecondElt() ; };

//destructor
~StatePair() {};


//member functions
State getFirstElt() const { return first ;}
State getSecondElt() const { return second ;}

void setFirstElt(State f) { first=f;}
void setSecondElt(State s) { second=s;}

int hash() const {

int k = 1; int l = 1;
return (1 << k)*l;

}

//operators
int operator== ( const StatePair& s ) const
{
  if( first == s.first && second == s.second ) return 1 ;
  else return 0 ;
}

StatePair&  operator= ( const StatePair& s )
{
  first = s.first ;
  second = s.second ;
  return *this ;
}

private:
//data
State first;
State second;
 
};

#endif

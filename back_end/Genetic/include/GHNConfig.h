// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of GHNConfig class
//
// Chars is a char* which knows its length, wrapped up as a user class.
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _GHNConfig_H_
#define _GHNConfig_H_

#include "global.h"


//------------------------------ GHNConfig ----------------------------------//


class GHNConfig
{

public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  //GHNConfig( );
  
  GHNConfig( int pop = 50, int gen = -1, int cross = 100, 
	     int mut = 100, int elit = 1, bool fit = true );
  
  // copy constructor supplied by compiler.
  
  // destructor supplied by compiler.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int populationSize( ) const { return population; }

  int numOfGenerations( ) const { return generations; }

  double chanceOfCrossover( ) const { return double(crossover)/100; }
  // returns value betweeen 0 and 1
  
  double chanceOfMutation( ) const { return double(mutation)/100; }
  // returns value betweeen 0 and 1
  
  bool haveElitistSelection( ) const { return elitistSelection == 1; }

  bool haveStrongElitistSelection( ) const { return elitistSelection == 2; }

  bool haveFitnessScaling( ) const { return bFitnessScaling; }
  
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // OI:                                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  // assignment operator supplied by compiler

  friend ostream& operator << ( ostream& ostr, const GHNConfig& C )
  {
    C.printOn(ostr);
    return ostr;
  }
    
  friend istream& operator >> ( istream& istr, GHNConfig& C )
  {
    C.readFrom(istr);
    return istr;
  }


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void readFrom( istream& istr );

  void printOn( ostream& ostr ) const;

  void setVariable( const char*, int );
  // used by readFrom() to set variables

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  int population;
  int generations;
  int crossover;
  int mutation;
  int elitistSelection;
  bool bFitnessScaling;
};

#endif

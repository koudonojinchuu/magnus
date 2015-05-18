// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of ACConfig class
//
// Principal Author: Alexei Miasnikov
//
// Status: in progress
//
// Revision History:
//


#ifndef _ACConfig_H_
#define _ACConfig_H_

#include "global.h"

//#ifndef BSD
//#include <values.h>
//#endif
//

//------------------------------ ACConfig ----------------------------------//


class ACConfig
{

public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  //GHNConfig( );
  
  ACConfig( int pop = 50, int gen = MAXINT, int fit_scale = 1, 
	    int cross_prob = 70, int mut_prob = 85, int elit = true, 
	    int penalty = 0, int cross_type = 1,
	    int mut_app_prob = 30, int mut_insert_prob = 20,
	    int mut_delete_prob = 20,
	    int mut_change_prob = 10,
	    int mut_permute_prob = 20,
	    int use_sum_fit = 1,
	    int num_min = 1000,
	    int us_w = 1
	    );
  
  // copy constructor supplied by compiler.
  
  // destructor supplied by compiler.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors/Modifiers:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  int populationSize() const { return population; }
  
  int numOfGenerations() const { return generations;}
  
  bool haveFitnessScaling() const { return fitness_scale; }

  bool useSumFitness() const { return use_sum_fitness; }

  double chanceOfCrossover( ) const { return double(crossover_prob)/100; }
  
  double chanceOfMutation( ) const { return double(mutation_prob)/100; }
  
  int numOfElitistSelection( ) const { return elittest; }
  
  int penaltyRate() const { return penalty_rate; }
  
  int crossoverType() const { return crossover_type; }
  
  double chanceOfMutationAppend( ) const { 
    return double(mutation_app_prob)/100; 
  }
  
  double chanceOfMutationInsert( ) const { 
    return double(mutation_insert_prob)/100; 
  }
  
  double chanceOfMutationDelete( ) const { 
    return double(mutation_delete_prob)/100; 
  }
  
  double chanceOfMutationChange( ) const { 
    return double(mutation_change_prob)/100; 
  }

  double chanceOfMutationPermute() const{
    return double(mutation_permute_prob)/100; 
  }

  int numMinmizeAfterOf() const {
    return num_min_after;
  }
  
  bool useWhitehead() const {
    return use_whitehead;
  }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // OI:                                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  // assignment operator supplied by compiler

  friend ostream& operator << ( ostream& ostr, const ACConfig& C )
  {
    C.printOn(ostr);
    return ostr;
  }
    
  friend istream& operator >> ( istream& istr, ACConfig& C )
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
  int fitness_scale;
  int crossover_prob; 
  int mutation_prob;
  int elittest; 
  int penalty_rate;
  int crossover_type;
  int mutation_app_prob;
  int mutation_insert_prob;
  int mutation_delete_prob;
  int mutation_change_prob;
  int mutation_permute_prob;
  int use_sum_fitness;
  int num_min_after;
  int use_whitehead;
};

#endif

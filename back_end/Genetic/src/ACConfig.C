// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
// 
// Contents: Implementation of ACConfig class
//
// Principal Author: Alexei Miasnikov
//
// Status: in progress
//
// Revision History:
//
//


#include "Chars.h"
#include "ACConfig.h"

ACConfig::ACConfig( int pop, int gen, int fit_scale, 
		    int cross_prob, int mut_prob, int elit, 
		    int penalty, int cross_type,
		    int mut_app_prob, int mut_insert_prob,
		    int mut_delete_prob,
		    int mut_change_prob,
		    int mut_permute_prob,
		    int use_sum_fit,
		    int num_min,
		    int us_w
		    )
  : population( pop ),
    generations( gen ),
    fitness_scale( fit_scale ),
    crossover_prob( cross_prob ),
    mutation_prob( mut_prob ),
    elittest( elit ),
    penalty_rate( penalty ),
    crossover_type( cross_type ),
    mutation_app_prob( mut_app_prob ),
    mutation_insert_prob( mut_insert_prob ),
    mutation_delete_prob( mut_delete_prob ),
    mutation_change_prob( mut_change_prob ),
    mutation_permute_prob( mut_permute_prob ),
    use_sum_fitness(use_sum_fit),
    num_min_after( num_min ),
    use_whitehead( us_w )
{  
  float mut_sum = 
    mutation_app_prob +
    mutation_insert_prob +
    mutation_delete_prob +
    mutation_change_prob +
    mutation_permute_prob;
  
  if ( mut_sum != 100 )
    error("Sum of chances of different kinds of mutations must be 100"
	  " please check your configuration file.");
}
  

void ACConfig::readFrom( istream& istr )
{
  while( !istr.eof() ) {
    
    char s[500],c;
    istr.get(s,499,'\n');
    istr.get(c);
    if( strlen(s) == 0 || s[0] == '#' ) continue;

    //@njz
    //    istrstream in(s);
    std::istrstream in(s);
    //
    char var[100];
    int value;
    in.get(var,99,':');
    in.get(c);
    in >> value;

    setVariable(var,value);
  }
}


void ACConfig::setVariable( const char* varName, int value )
{
  // get rid of all spaces
  char s[100];
  int vnLen = strlen(varName), j = 0;
  for( int i = 0; i < vnLen; ++i )
    if( varName[i] != ' ' )
      s[j++] = varName[i];
  s[j] = 0;

  Chars name(s);
  if( name == Chars("Population") )
    population = value;
  else if( name == Chars("Generations") )
    generations = value;
  else if ( name == Chars("UseFitnessScale") )
    fitness_scale = value;
  else if ( name == Chars("CrossoverChance") )
    crossover_prob = value;
  else if ( name == Chars("MutationChance") )
    mutation_prob = value;
  else if ( name == Chars("UseElittestSelection") )
    elittest = value;
  else if ( name == Chars("PenaltyRate") )
    penalty_rate = value;
  else if ( name == Chars("CrossoverType") )
    crossover_type = value;
  else if ( name == Chars("MutationAppendChance") )
    mutation_app_prob = value;
  else if ( name == Chars("MutationInsertChance") )
    mutation_insert_prob = value;
  else if ( name == Chars("MutationDeleteChance") )
    mutation_delete_prob = value;
  else if ( name == Chars("MutationChangeChance") )
    mutation_change_prob = value;
  else if ( name == Chars("MutationPermuteChance") )
    mutation_permute_prob = value;
  else if ( name == Chars("UseSumFitnessFunction") )
    use_sum_fitness = value;
  else if ( name == Chars("MinimizeAfter") )
    num_min_after = value;
  else if ( name == Chars("UseWhitehead") )
    use_whitehead = value;

  else
    error(Chars("void GHNConfig::setVariable( const char*, int ) : ") +
	  "unknown variable" + varName + ".");
}


void ACConfig::printOn( ostream& ostr ) const
{
  ostr << "Population : " << population << endl
       << "Generations : " << generations << endl
       << "UseFitnessScale : " << fitness_scale << endl
       << "CrossoverChance : " << crossover_prob << endl
       << "MutationChance : " << mutation_prob << endl
       << "UseElittestSelection : " << elittest << endl
       << "PenaltyRate : " << penalty_rate << endl
       << "CrossoverType : " << crossover_type << endl
       << "MutationAppendChance : " << mutation_app_prob << endl
       << "MutationInsertChance : " << mutation_insert_prob << endl
       << "MutationDeleteChance : " << mutation_delete_prob << endl
       << "MutationChangeChance : " << mutation_change_prob << endl
       << "MutationPermuteChance : " << mutation_permute_prob << endl
       << "UseSumFitnessFunction : " << use_sum_fitness << endl
       << "MinimizeAfter : "   <<   num_min_after << endl
       <<  "UseWhitehead : " <<   use_whitehead;
}




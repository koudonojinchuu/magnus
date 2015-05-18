// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
// 
// Contents: Implementation of HNConfig class
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
//


#include "Chars.h"
#include "GHNConfig.h"


//------------------------------ HNConfig -----------------------------------//

/*
GHNConfig::GHNConfig( )
  : population( 0 ),
    generations( 0 ),
    crossover( 0 ),
    mutation( 0 ),
    elitistSelection( 0 ),
    bFitnessScaling( false )
{ }
*/

GHNConfig::GHNConfig( int pop, int gen, int cross, 
		      int mut, int elit, bool fit ) 
  : population( pop ),
    generations( gen ),
    crossover( cross ),
    mutation( mut ),
    elitistSelection( elit ),
    bFitnessScaling( fit )
{ }
  

void GHNConfig::readFrom( istream& istr )
{
  while( !istr.eof() ) {

    char s[500],c;
    istr.getline(s,499,'\n');
    if( strlen(s) == 0 || s[0] == '#' ) continue;

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


void GHNConfig::setVariable( const char* varName, int value )
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
  else if( name == Chars("Crossover") )
    crossover = value;
  else if( name == Chars("Mutation") )
    mutation = value;
  else if( name == Chars("ElitistSelection") )
    elitistSelection = value;
  else if( name == Chars("FitnessScaling") )
    bFitnessScaling = value ? true : false;
  else
    error(Chars("void GHNConfig::setVariable( const char*, int ) : ") +
	  "unknown variable" + varName + ".");
}


void GHNConfig::printOn( ostream& ostr ) const
{
  ostr << "# Population size" << endl
       << "Population : " << population << endl << endl
       << "# Number of generations" << endl
       << "Generations : ";
  if( generations < 0 )
    ostr << "unlimited";
  else
    ostr << generations;
  ostr << endl << endl
       << "# Chance of crossover" << endl
       << "Crossover : " << crossover << endl << endl
       << "# Chance of mutation " << endl
       << "Mutation : " << mutation << endl << endl
       << "# Type of elitist selection" << endl
       << "ElitistSelection : " << elitistSelection << endl << endl
       << "# Have fitness scaling" << endl
       << "FitnessScaling : " << (bFitnessScaling ? 1 : 0) << endl << endl;
}




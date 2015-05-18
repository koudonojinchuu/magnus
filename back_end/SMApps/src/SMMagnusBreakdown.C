// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class SMMagnusBreakdown
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//

#include "SMMagnusBreakdown.h"
#include "MagnusBreakdown.h"
#include "OutMessages.h"
#include "OneRelatorProblems.h"

static void printListOfGenerators( ostream& ostr, 
				   const ListOf<Generator>& lg, 
				   const FPGroup& G )
{
  ostr << "{ ";
  bool comma = false;
  for ( ListIterator< ListOf<Generator> > I = lg; !I.done(); I.next() ) {
    if( comma ) ostr << ", ";
    G.printWord( ostr, I.value() );
    comma = true;
  }
  ostr << " }";
}

SMMagnusBreakdown::SMMagnusBreakdown( const SMFPGroup& smo )
  : theGroup( smo )
{
  FPGroup FP = theGroup.getFPGroup();

  OneRelatorGroup G( FP.namesOfGenerators(), theGroup.gic.getOneRelator() );
  MagnusBreakdown M(G); 

  InvokingMessage message;

  message <<
    " defineMagnusBreakdown "
    " title {Define a generator(s)} "
    " descr { "
      "Specify a stable letter or two generators (separated by comma) with"
      " non-zero exponent sum or use the default button."
    "} "
    " zero ";
  printListOfGenerators(message, M.getGeneratorsWithZeroExpSum(), FP );
  message << " nonzero ";
  printListOfGenerators(message, M.getGeneratorsWithNonzeroExpSum(), FP );
  message << " default ";
  printListOfGenerators(message, M.getDefaultBreakdownGenerators(), FP );
  message << " message { " << oid().unwrap()  << ' ' << ACCEPT << " } ";
  message.send();
}

void SMMagnusBreakdown::readMessage( istream& istr )
{
  TagMessage tag;

  istr >> (int&)tag;

  if ( tag == ACCEPT ) {

    FPGroup FP = theGroup.getFPGroup();

    Chars err;
    VectorOf<Word> generators = FP.readVectorOfWords( istr, err );

    if ( err.length() > 0 ) {
      // ParseErrorMessage( name, err ).send();    
      Message message("Warning");
      message << "You entered incorrect generators.";
      message.send();
    }
    else {

      int numOfGens = generators.length();    
      if( numOfGens > 2 ) {
	Message message("Warning");
	message << "2 generators should be specified at most.";
	message.send();
      }
      else if( numOfGens == 0 ) {

	// register new object. 

	ORFindHNNPresentation *smo = new ORFindHNNPresentation( theGroup );

	SMObject *group  = (SMObject *)&theGroup;
	group->addDependent( *smo );
      }
      else {

	for( int i = 0; i < numOfGens; ++i ) {
	  if( generators[i].length() != 1 ) {
	    Message message("Warning");
	    message << "You should specify only generators, not words.";
	    message.send();
	    return;
	  }
	}

	Generator stableGen, accompGen;
	
	stableGen = abs(ord(generators[0][0]));
	if( numOfGens == 2 ) accompGen = abs(ord(generators[1][0]));
	
	OneRelatorGroup G( FP.namesOfGenerators(), theGroup.gic.getOneRelator() );
	MagnusBreakdown M(G); 
	SetOf<Generator> gensWithZeroExpSum = makeSetOf( M.getGeneratorsWithZeroExpSum() );
	SetOf<Generator> gensWithNonzeroExpSum = makeSetOf( M.getGeneratorsWithNonzeroExpSum() );
	
	if( numOfGens == 1 && !gensWithZeroExpSum.contains(stableGen)
	    && FP.numberOfGenerators() > 1 ) {
	  Message message("Warning");
	  message << "You should specify two generators with non-zero exponent sum.";
	  message.send();
	}
	else if( ( M.getGeneratorsWithZeroExpSum().contains( stableGen ) 
		   || M.getGeneratorsWithZeroExpSum().contains( accompGen ) )
		 && numOfGens == 2 ) {
	  Message message("Warning");
	  message << "You should specify only 1 generator with zero exponent sum.";
	  message.send();
	}
	else if( numOfGens == 2 && stableGen == accompGen ) {
	  Message message("Warning");
	  message << "You should specify different generators.";
	  message.send();
	}
	else {
	  // register new object. 
	  
	  ORFindHNNPresentation *smo;
	  
	  if( numOfGens == 1 ) 
	    smo = new ORFindHNNPresentation( theGroup, stableGen );
	  else
	    smo = new ORFindHNNPresentation( theGroup, stableGen, accompGen );
	  
	  // theGroup.addDependent( *smo );
	  SMObject *group  = (SMObject *)&theGroup;
	  group->addDependent( *smo );
	}
      }
    }
    
    // delete myself.
    TheObjects::remove( *this );
  }
}

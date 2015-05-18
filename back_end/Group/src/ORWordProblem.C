// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of class ORWordProblem
//
// Principal Author: Roger Needham
//
// Status: Under development
//
// Revision History:
//


#include "ORWordProblem.h"
#include "MagnusHome.h"


ORWordProblem::ORWordProblem(const Word& relator) :
  theRelator(relator),
  orwp(NULL),
  status(true)
{ }



void ORWordProblem::findAnswer(const Word& testWord)
{
  if ( ! status ) return; // Can safely ignore this faux pas

  if ( ! orwp ) {
	 // Make the BlackBox

         //@njz
         // ostrstream o;
         std::ostrstream o;
	 //

	 o << MagnusHome::magnusHome()
		<< "/back_end/black_boxes/orwp/orwp "
		<< ends;
	 orwp = new BlackBox( o.str() );

	 if ( ! orwp ) {
		status = false;
		return;
	 }

	 o.freeze(0);
	 orwp->start();  //@rn Check status
  }

  // Pipe out the relator and test word.

  printWord(theRelator, orwp->toCommand());
  printWord(testWord, orwp->toCommand());
  orwp->toCommand() << flush;

  // Now wait for result, and block, by gum, until we get it.
  // Should get either "Yes!" or "No!"

  Chars answer;
  orwp->fromCommand() >> answer;

  if ( answer == Chars("Yes!") )
	 result = true;
  else if ( answer == Chars("No!") )
	 result = false;
  else
	 status = false;  // Give up forever on unexpected result.
}


bool ORWordProblem::isTrivial( )
{
  if ( ! status )
	 error("Called ORWordProblem::isTrivial( ) with bad status");

  return result;
}


void ORWordProblem::printWord(const Word& w, ostream& ostr)
{
  int len = w.length();
  ostr << len << endl;
  for( int i = 0; i < len; ++i ) {
	 int g = ord(w[i]);
	 ostr << char( abs(g) + 'a' - 1 );
	 if ( g < 0 ) ostr << -1;
	 else ostr << 1;
  }
  ostr << endl;
}

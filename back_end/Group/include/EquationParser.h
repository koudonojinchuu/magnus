// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of class EquationParser
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//

#ifndef _EQUATIONPARSER_H_
#define _EQUATIONPARSER_H_


#include "PresentationParser.h"


// The grammar for an equation (see the Word grammar):
//
// <equation> ::= '(' <generator list>('|' | ';' | ':')<word>('='<word>)? ')'
//

// It seems expedient to require the end user to enter equations in a way
// similar to presentations, so that the existing parsers can be used.


//---------------------------------------------------------------------------//
//-------------------------- EquationParser ---------------------------------//
//---------------------------------------------------------------------------//

class EquationParser : protected PresentationParser
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  EquationParser(istream &istr) : PresentationParser(istr) { }
  // Initialize the parser with the istream from which to read.

  // Destructor supplied by compiler.

  // The following two methods should be redefined

  virtual Word parseRelator( const VectorOf<Chars>&, Chars& );
  virtual void getToken( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Parsing Methods:                                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Word parseEquation( const VectorOf<Chars>& names,
							 VectorOf<Chars>& new_names,
							 Chars& errMesg
							 );
  // Reads an equation and returns it as a word. The new vector of print
  // names (generators first, then variables) is returned in new_names.
  // As usual, if there is a parse error, the description is returned in
  // errMesg.

};

#endif

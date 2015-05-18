// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of the EqSystemParser class.
//
// Principal Authors: Denis Serbin
//
// Status: In progress
//
// Revision History:
//
// Special Notes:
//
//

#include "EquationParser.h"
#include "PresentationParser.h"
#include "conversions.h"

#ifndef _EqSystemParser_h_
#define _EqSystemParser_h_


//---------------------------------------------------------------------------//
//-------------------------- EqSystemParser ---------------------------------//
//---------------------------------------------------------------------------//

class EqSystemParser : protected PresentationParser
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  EqSystemParser(istream &istr) : PresentationParser(istr) { }
  // Initialize the parser with the istream from which to read.

  // Destructor supplied by compiler.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Parsing Methods:                                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  VectorOf<Word> parseEqSystem( const VectorOf<Chars>& names,
				VectorOf<Chars>& new_names,
				Chars& errMesg
				);
  // Reads a system equations and returns it as a vector of words. 
  // The new vector of print names (generators first, then variables) is 
  // returned in new_names.
  // As usual, if there is a parse error, the description is returned in
  // errMesg.

  Word parseEquation( const VectorOf<Chars>& names,
		      VectorOf<Chars>& new_names,
		      Chars& errMesg
		      );
  
  virtual void getToken( );
  virtual Word parseRelator( const VectorOf<Chars>&, Chars& );
  
};

#endif

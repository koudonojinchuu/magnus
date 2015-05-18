// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of class AmalgamatedProductParser
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//

#ifndef _AMALG_PRODUCT_PARSER_H_
#define _AMALG_PRODUCT_PARSER_H_


#include "PresentationParser.h"
#include "APofFreeGroups.h"
#include "APwithOneRelator.h"

// The grammar for an Amalgamated Product of Free Groups(see the Word grammar):
//
// <AmalgamatedProductOfFreeGroups> ::= '<' <free group> '*' <free group>
//         ('|' | ';' | ':') (<word> '=' <word>)* '>'
//
// <APwithOneRelator> ::= '<' <free group> '*' <free group>
//         ('|' | ';' | ':') (<word> '=' <word>)? '>'


//---------------------------------------------------------------------------//
//-------------------- AmalgamatedProductParser -----------------------------//
//---------------------------------------------------------------------------//

class AmalgamatedProductParser : protected PresentationParser
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AmalgamatedProductParser(istream &istr) : PresentationParser(istr) { }
  // Initialize the parser with the istream from which to read.

  // Destructor supplied by compiler.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Parsing Methods:                                                    //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  VectorOf<Word> parseAPRelator(const VectorOf<Chars>& names, Chars& errMesg);

  VectorOf<Word> parseAPRelatorList(const VectorOf<Chars>& names,
				    Chars& errMesg, int axRelators);
  
  void parseFactorsAndRelations(FreeGroup& f1, FreeGroup& f2,
				VectorOf<Word>& gens1, VectorOf<Word>& gens1,
				int maxRelators, Chars& errMesg);
  
  AmalgProductOfFreeGroupsRep* parseAPofFreeGroups( Chars& errMesg );
  
  APwithOneRelatorRep* parseAPwithOneRelator( Chars& errMesg );

};

#endif

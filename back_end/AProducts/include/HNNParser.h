// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of HNNExtensionParser class.
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Usage:
//
//
// Special Notes:
//
// Revision History:
//

#ifndef _HNN_EXTENSION_PARSER_H_
#define _HNN_EXTENSION_PARSER_H_

#include "PresentationParser.h"
#include "HNNExtOfFreeGroup.h"

// The grammar for an HNN-extension of Free Group (see the Word grammar):
//
// <HNNExtOfFreeGroup> ::= '<' generator ',' <free group>
//         ('|' | ';' | ':') (<word> '=' <word>)* '>'
//

//---------------------------------------------------------------------------//
//------------------------ HNNExtensionParser -------------------------------//
//---------------------------------------------------------------------------//

class HNNExtensionParser : public PresentationParser {

public:  
  HNNExtensionParser( istream& istr ) : PresentationParser(istr) { }
  // Initialize the parser with the istream from which to read.
  
  // Destructor supplied by compiler.
  
  HNNExtOfFreeGroupRep* parseHNNExtensionOfFreeGroup( Chars& );
  // Read a group presentation and return it as a new group rep object.
  
protected:
  
  void parseHNNRelator( const VectorOf<Chars>& names, Word& left, Word& right, 
			Chars& errMesg);
  
  VectorOf<Word> parseHNNRelatorsList( const VectorOf<Chars>& names,
				       Chars& errMesg, int maxRelators );

  void parseBasisFreeGroupAndRelations( FreeGroup& F, Chars& stableGenName, 
					VectorOf<Word>& A, VectorOf<Word>& B, 
					int maxRelators, Chars& errMesg );



};

#endif



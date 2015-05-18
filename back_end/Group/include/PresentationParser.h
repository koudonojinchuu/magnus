// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of parser for presentations.
//
// Principal Author: Roger Needham
//
// Status: useable
//
// Revision History:
//

#ifndef _PRESENTATION_PARSER_H_
#define _PRESENTATION_PARSER_H_

#include "Set.h"
#include "WordParser.h"
#include "FreeGroup.h"
#include "FPGroup.h"


// The grammar for Word and FPGroup:
//
// Literals are delimited by ''.
// The symbols ., *, +, -, |, ?, [, ], (, and ) are part of regular expressions
// unless quoted.
//
// <whitespace>     ::= ' ' | '\t' | '\n' | '#'.*'\n'
// <index>          ::= 0 | [1-9]+[0-9]*
// <integer>        ::= <index> | -<index>
// <generator>      ::= [a-zA-Z]+<index>?
// <atom>           ::= '1' | <generator> | '('<word>')' |
//                      '['<word>','<word>(','<word>)*']'
// <term>           ::= <atom>('^'<atom>)* | <term>'^'<integer>
// <word>           ::= <term>('*'<term>)* | <term>(<whitespace><term>)*
// <relator>        ::= <word>('='<word>)?
// <generator list> ::= (<generator>(','<generator>)*)?
// <word list>      ::= <word>(','<word list>)?
// <relator list>   ::= <word>('='<word>)* |
//                      (<word>('='<word>)*(','<word>('='<word>)*)?)?
// <FPGroup>        ::= '<'<generator list>('|' | ';' | ':')<relator list>'>' |
//                      '<'<generator list>'>'
//
// - whitespace may occur anywhere except within lexemes.
// 
// Semantic conventions:
//
// - x^y is y^-1 x y.
// - ^ is left-associative, so a^t^2 is (a^t)^2.
// - [x,y] is x^-1 y^-1 x y.
// - [x1, x2, ..., xn] is left-normed: [[x1,x2], x3, ..., xn].
// - Given the print name of a generator, the print name of its inverse
//   has each character changed to the opposite case.
//   Thus aB2^-1 is the same as Ab2.


class PresentationParser : public WordParser {

public:

  enum BraceType { SET, PAREN };

public:

  PresentationParser(istream &istr) : WordParser(istr) { }
  // Initialize the parser with the istream from which to read.

  // Destructor supplied by compiler.

  VectorOf<Chars> parseGeneratorList( Chars& );
  // Read a generator list.

  SetOf<Word> parseWordSet( const VectorOf<Chars>&, Chars& );
  // Read a list of words in the given generator names.
  // The list is terminated when a word is followed by any non-whitespace
  // char other than ','.

  VectorOf<Word> parseWordList( const VectorOf<Chars>&, Chars& );
  // Read a list of words in the given generator names.
  // The list is terminated when a word is followed by any non-whitespace
  // char other than ','.

  SetOf<Word> parseSetOfWords( const VectorOf<Chars>&, Chars& );
  // Read a list of words in the given generator names delimited by '{', '}'.

  VectorOf<Word> parseVectorOfWords( const VectorOf<Chars>&, Chars&, 
				     BraceType brace = SET );
  // Read a list of words in the given generator names delimited by '{', '}'
  // or '(', ')'.

  virtual Word parseRelator( const VectorOf<Chars>&, Chars& );
  // Read a relator in the given generator names.

  SetOf<Word> parseRelatorList( const VectorOf<Chars>&, Chars& );
  // Read a list of words and relations in the given generator names.
  // The list is terminated when anything (non-whitespace) other than
  // a relator or ',' is encountered.

  FreeGroupRep* parseFreeGroup( Chars& );

  FPGroupRep* parseFPGroup( Chars& );
  // Read a group presentation and return it as a new group rep object.
private:
  bool getGeneratorRange( const Chars& , VectorOf<Chars>&, Chars& );
  bool getRangeOf( const Chars& ,Chars&, int& );
};

#endif

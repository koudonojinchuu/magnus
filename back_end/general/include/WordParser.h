// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of parser for Words, and private helper classes.
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//
// * 4/20/95 rn altered
//     void parseError(const char*)
//   to store the error message in the form: <n> <mesg>
//   where n is the index in the input of the problem, and mesg
//   is the description.
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//
// * 05/98 Alexei M. added void isIvertibleName( char* ) and changed 
//   bool invertName( char* ) functions
//

#ifndef _WORD_PARSER_H_
#define _WORD_PARSER_H_

#include "Word.h"
#include "Chars.h"
#include "Vector.h"


// The grammar for Word:
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



#define NAME_SIZE      100
#define INPUT_BUF_SIZE 1024
#define MAX_WORD_LENGTH 2000000000

typedef enum
{
   LANGLE, RANGLE, LPAREN, RPAREN, LSQUARE, RSQUARE, STAR, CARET, COMMA, BAR,
   COLON, SEMICOLON, EQUALS, GENERATOR, INT, EOS, BAD, INIT, LSET, RSET,
   ARROW, DOT
}  TokenType;


class ParseNode;  // defined below

class WordParser {

public:

  WordParser(istream &str) : istr(str) {
	 tokenBufIndex = 0;
	 curToken = INIT;
  }

  // Destructor supplied by compiler.

  void popToken( ) { getToken(); }  // To read over unwanted tokens.

  // If there is no parse error, the following return the parsed object,
  // and do not change the Chars argument.
  // Otherwise an error message is put in the Chars argument, and a
  // default object is returned.
  //
  // None read more from the stream than needed, and all leave the
  // internal state ready for subsequent calls.

  Word parseWord( const VectorOf<Chars>&, Chars& );
  // Read a word in the given generator names, and freely reduce it.

  Word parseWordVerbatim( const VectorOf<Chars>&, Chars& );
  // Read a word in the given generator names, without freely reducing it.


protected:

  int          tokenInt;                 // Value of INT token, or ord
                                         //  of GENERATOR token
  int          tokenBufIndex;            // Index into tokenBuf
  char         tokenName[NAME_SIZE+1];   // Print name of GENERATOR token
  char         tokenBuf[INPUT_BUF_SIZE]; // Input buffer
  TokenType    curToken;                 // The current token

  VectorOf<Chars>  genNames;  // Put these where getToken can find them.

  Chars          parseErrorMessage;

  istream&       istr;       // The caller owns istr.


  // Utility functions

  char peekCh( );
  char getCh( );
  virtual void getToken( );
  void parseError(const char*);
  Bool atStartOfWord( );
  void invertName(char*);

  bool isInvertibleName( char* ); 
  // @am according to the new standard
  // names of gerators consisting of several letters 
  // can't be inverted by switching the letters cases.

  ParseNode *parseExpression( );
  ParseNode *parseTerm( );
  ParseNode *parseAtom( );

#ifdef DEBUG
  //friend int main( );
#endif

};



// Helper classes for recursive descent parsing of Words:


typedef enum {
  ILLEGAL_NODE, COMMUTATOR_NODE, INT_NODE, CONCAT_NODE,
  IDENT_NODE, POWER_OR_CONJUGATE_NODE
} ParseNodeType;


typedef enum {
  PT_OK = 0, PT_TOO_LONG
} ParseTypeState;

typedef int ParseData;

//typedef int ParseType;  // eval methods return ptr to this

class ParseType {
public:
  ParseType( ) : len(0), data(0), status(PT_OK) { }
  ParseType( int gen );

  Word makeWord( ) const;
  ParseType invertWord( ) const;
  ParseType makeCopies( unsigned int numOfCopies ) const;
  static ParseType join( const ParseType& x, const ParseType& y );

  void destroy( ) { delete data; data = 0; status = PT_OK; }

  ParseTypeState state( ) const { return status; }
  Chars errorMessage( ) const;

private:

  ParseType( ParseTypeState s, ParseData *ptr, int plen ) 
    : len(plen), data(ptr), status(s) { }

  int len;
  ParseData *data;
  ParseTypeState status;
};


class  ParseNode        // Base for Parsing Nodes
{
public:
  ParseNode( ) { }
  virtual ~ParseNode( ) { }
  virtual ParseType eval( ) = 0; // { return 0; }
  virtual ParseNodeType whatAmI( ) = 0;

  // Utility functions
  //  int getLength( ParseType* );
  //  ParseType *invertWord( ParseType* );
  //  ParseType *makeCopies( int, ParseType* );
  //  ParseType *join( ParseType*, ParseType* );
};


class Int : public ParseNode
{	
public:
  Int( int n ) { number = n; }
  ParseType eval( ) { return ParseType(0); }
  ParseNodeType  whatAmI( ) { return INT_NODE; }

  int intEval( ) { return number; }

private:
  int number;
};


class Ident : public ParseNode
{
public:
  Ident( ParseType val ) {
	 gen = val;
  }
  ~Ident( ) { }

  ParseNodeType  whatAmI( ) { return IDENT_NODE; }

  ParseType eval( ) {
    return ParseType( gen );
  }

private:
  ParseType  gen;
};


class BinOp : public ParseNode
{  
public:

  BinOp( ParseNode *left, ParseNode *right ) : arg1(left), arg2(right) { }

  ~BinOp( ) {
    delete arg1;
    delete arg2;
    result1.destroy();
    result2.destroy(); 
  }

protected:
  ParseNode *arg1, *arg2;
  ParseType result1, result2;
};


class Concat : public BinOp
{
public:

  Concat( ParseNode *left, ParseNode *right ) : BinOp( left, right ) { }
  ParseType eval( );
  ParseNodeType whatAmI( ) { return CONCAT_NODE; }
};


class  PowerOrConjugate : public BinOp
{
public:

  PowerOrConjugate( ParseNode *left, ParseNode *right) : BinOp( left, right) {}
  ParseType eval( );
  ParseNodeType whatAmI( ) { return POWER_OR_CONJUGATE_NODE; }
};


class  Commutator : public BinOp
{
public:

  Commutator( ParseNode *left, ParseNode *right ) : BinOp( left, right ) { }
  ParseType eval( );
  ParseNodeType whatAmI( ) { return COMMUTATOR_NODE; }
};

#endif

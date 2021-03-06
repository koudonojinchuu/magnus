// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class EquationParser
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#include "EquationParser.h"


//---------------------------------------------------------------------------//
//-------------------------- EquationParser ---------------------------------//
//---------------------------------------------------------------------------//

Word EquationParser::parseEquation( const VectorOf<Chars>& names,
				    VectorOf<Chars>& new_names,
				    Chars& errMesg
				    )
// <equation> ::= '(' <generator list>('|' | ';' | ':')<word>('='<word>)? ')'
// <equation> ::= <word>('='<word>)?
{
  Word defaultResult;


  // Catenate the generator and variable names.
  genNames = names;

  Word equation;
 	 
  if ( curToken == INIT ) getToken();
  equation = parseRelator(genNames, errMesg );
  new_names = genNames;

  if ( errMesg.length() > 0 ) {
    return defaultResult;
  }
	 

  // QEqnSolution implementation requires variables to go first;
  // in new_names, they go last. So we have to remake new_names
  // and E.

  int numOfGen = new_names.length();
  int numOfConst = names.length();
  int numOfVar = numOfGen - numOfConst;


  // The equation must have at least one variable
  if(numOfVar == 0) {
    parseError("Equation must have at least one variable");
    errMesg = parseErrorMessage;
    return defaultResult;
  }

  curToken = INIT;  // Set up for subsequent call.

  VectorOf<Chars> newestNames(numOfGen);    // :)
  for(int i=0; i < numOfVar; i++) {
    newestNames[i] = new_names[ i+ numOfConst];
  }
  for(int i=0; i < numOfConst; i++) {
    newestNames[ numOfVar + i ] = new_names[i];
  }
    
  for(int i=0; i < equation.length(); i++) {
    int v = ord(equation[i]);
    int a = abs(v);
    int p = (v>0) ? 1 : -1;
    if(a <= numOfConst ) 
      equation[i] = Generator(p * (a + numOfVar));
    else
      equation[i] = Generator(p * (a - numOfConst));
  }

  


  new_names = newestNames;

  return equation;
}

void  EquationParser::getToken( )

// Put next token in curToken.
// If the token is a generator, and genNames.length() > 0, also put the
// generator's index in tokenInt.
// For convenience, this knows about all tokens for a presentation, rather
// than having a separate getToken method for PresentationParser.

{
  char  ch;
  int i = 0;

  // Flush whitespace, comments, and look for end of stream.
  do {
    //@rn	 if ( istr.eof() ) { curToken = EOS; return; }
    if ( peekCh() == '#' )
      while ( peekCh() != '\n' ) {
	//@rn		  if ( istr.eof() ) { curToken = EOS; return; }
	istr.get();
      }
    if ( ( ch = peekCh() ) == ' ' || ch == '\t' || ch == '\n' ) {
      ch = ' ';
      getCh();
    }
  } while ( ch == ' ' );

  // Maybe a single char token?
  switch (ch) {
  case '<' : curToken = LANGLE;    break;
  case '>' : curToken = RANGLE;    break;
  case '(' : curToken = LPAREN;    break;
  case ')' : curToken = RPAREN;    break;
  case '[' : curToken = LSQUARE;   break;
  case ']' : curToken = RSQUARE;   break;
  case '*' : curToken = STAR;      break;
  case '^' : curToken = CARET;     break;
  case ',' : curToken = COMMA;     break;
  case '|' : curToken = BAR;       break;
  case ':' : curToken = COLON;     break;
  case ';' : curToken = SEMICOLON; break;
  case '=' : curToken = EQUALS;    break;
  case '{' : curToken = LSET;      break;
  case '}' : curToken = RSET;      break;
  default    : curToken = BAD;       break;
  }
  if ( curToken != BAD ) { getCh(); /*cout << curToken << endl;*/ return; }

  // Could be "->". Distinguish from negative int carefully:

  if ( ch == '-' ) {
    tokenName[i++] = getCh();
    ch = peekCh();
    if ( ch == '>' ) {
      tokenName[i++] = getCh();
      curToken = ARROW;
      return;
    }
  }

  // Must be an integer or generator.
  // To be bulletproof, we don't want to use >>.

  //@rn  if ( ch == '-' || ( ch >= '0' && ch <= '9' ) ) {
  if ( ch >= '0' && ch <= '9' ) {
    tokenName[i++] = getCh();
    ch = peekCh();
    while ( ch >= '0' && ch <= '9' ) {
      if ( i == NAME_SIZE ) error("buffer overflow in getToken");
      tokenName[i++] = getCh();
      ch = peekCh();
    }
    if ( i == 1 && tokenName[0] == '-' ) { curToken = BAD; /*cout << "BAD" << endl;*/ return; }
    tokenName[i] = '\0';
    tokenInt = atoi(tokenName);
    curToken = INT;
    /*cout << -tokenInt << endl;*/
    return;
  }

  while ( ( ch >= 'a' && ch <= 'z' ) || ( ch >= 'A' && ch <= 'Z' ) ) {
    if ( i == NAME_SIZE ) error("buffer overflow in getToken");
    tokenName[i++] = getCh();
    ch = peekCh();
  }
  if ( i == 0 ) { curToken = BAD; /*cout << "BAD" << endl;*/ return; }
  while ( ch >= '0' && ch <= '9' ) {
    if ( i == NAME_SIZE ) error("buffer overflow in getToken");
    tokenName[i++] = getCh();
    ch = peekCh();
  }
  curToken = GENERATOR;
  tokenName[i] = '\0';
  tokenInt = genNames.indexOf(Chars(tokenName)) + 1;
  if ( tokenInt == 0 ) {
    invertName(tokenName);
    tokenInt = - 1 - genNames.indexOf(Chars(tokenName));
    invertName(tokenName);
    if ( tokenInt == 0 ) {

      // @am if starts with a capital letter then it is inverse
      if ( isInvertibleName( tokenName ) && 
	   tokenName[0] >= 'A' && tokenName[0] <= 'Z' ){
	char genName[100];
	strcpy( genName, tokenName );
	invertName( genName );
	genNames.append(Chars( genName ));
	tokenInt = -1 - genNames.indexOf(Chars( genName ));
      }else{
	genNames.append(Chars(tokenName));
	tokenInt = genNames.indexOf(Chars(tokenName)) + 1;
      }
    }
  }
}

Word
EquationParser::parseRelator( const VectorOf<Chars>& names, Chars& errMesg )
{
  genNames = names;

  if ( curToken == INIT ) getToken();
  

  if ( !atStartOfWord() ) {
    parseError("Expected a word here");
    errMesg = parseErrorMessage;
    return Word();
  } else {
    Word u = parseWord( genNames, errMesg );
    if ( errMesg.length() > 0 ) {
      return Word();
    }
    if ( curToken == EQUALS ) {
      getToken();
      Word v = parseWord( genNames, errMesg );

      if ( errMesg.length() > 0 ) {
	return Word();
      }
      return ((Word)(u * v.inverse())).cyclicallyReduce();
    } 
    else {
      return u.cyclicallyReduce();
    }
      
  }
}





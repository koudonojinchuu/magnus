// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of parser for Words, and private helper classes.
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//
// * 11/1/95 Roger made input words freely reduced.
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//

#include "WordParser.h"
#include "Vector.h"


Word WordParser::parseWordVerbatim( const VectorOf<Chars>& names, 
				    Chars& errMesg)
{
  genNames = names;

//  cout << "curToken = " << curToken << endl;

  if ( curToken == INIT ) getToken();

  ParseNode *wexp = parseExpression( );
  if ( wexp == NULL ) {
	 errMesg =  parseErrorMessage;
	 return Word( );
  }

  ParseType wp = wexp->eval( );
  if ( wp.state() != PT_OK ) {
    //errMesg = Chars("{{0} {Sorry, internal bug in parser eval method}}");
    errMesg = Chars("{{0} {") + wp.errorMessage() + Chars("}}");
    return Word( );
  }

  Word result = wp.makeWord();
  delete wexp;
  wp.destroy();

  return result;
}


Word WordParser::parseWord( const VectorOf<Chars>& names, Chars& errMesg)
{
  return parseWordVerbatim(names, errMesg).freelyReduce();
}



char WordParser::peekCh( )
{
  return istr.peek();
}


char WordParser::getCh( )
{
  if ( istr.peek() == '\n' || tokenBufIndex == INPUT_BUF_SIZE - 2 )
	 tokenBufIndex = 0;
  return tokenBuf[tokenBufIndex++] = istr.get();
}


void  WordParser::getToken( )

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
	 case '.' : curToken = DOT;      break;
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
	 if ( i == 1 && tokenName[0] == '-' ) { 
	   curToken = BAD; /*cout << "BAD" << endl;*/ 
	   return; 
	 }
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
  
  while ( ch >= '0' && ch <= '9' || ch == '_' || ch == '-' ) {
	 if ( i == NAME_SIZE ) error("buffer overflow in getToken");

	 //@dp hack for Magnus breakdown.
	 if( ch == '-' ) {
	   // look forward for a next char to be a numeric.
	   istr.get();
	   char nextch = istr.peek();
	   istr.unget();
	   if( nextch < '0' || nextch > '9' )
	     break;
	 }

	 tokenName[i++] = getCh();
	 ch = peekCh();
  }
  curToken = GENERATOR;
  tokenName[i] = '\0';
  tokenInt = genNames.indexOf(Chars(tokenName)) + 1;
  if ( tokenInt == 0 ) {
	 invertName(tokenName);
	 tokenInt = - 1 - genNames.indexOf(Chars(tokenName));
	 if ( tokenInt == 0 ) parseError("Unknown generator");
	 invertName(tokenName);
  }
  // Error is detected by caller: can't have
  // curToken == GENERATOR && tokenInt == 0

  /*cout << tokenName << endl;*/
}


void WordParser::parseError(const char *message)
{
  parseErrorMessage = dec(tokenBufIndex);
  parseErrorMessage += " {";
  parseErrorMessage += message;
  parseErrorMessage += ".}";

/*
  int i = 0;
  char  tmpBuf[128];

  while ( i < 128 && !EOF && istr.peek() != '\n' )
	 tmpBuf[i++] = istr.get();

  int len = 2*tokenBufIndex + strlen(message) + i + 4;
  char* tmpMesg = new char[len];
  int j = 0;
  for( int k = 0; k < tokenBufIndex; k++ )
	 tmpMesg[j++] = tokenBuf[k];
  tmpMesg[j++] = '\n';
  for( k = 0; k < tokenBufIndex; k++ )
	 tmpMesg[j++] = ' ';
  for( k = 0; k < i; k++ )
	 tmpMesg[j++] = tmpBuf[k];
  tmpMesg[j++] = '\n';
  for( k = 0; k < strlen(message); k++ )
	 tmpMesg[j++] = message[k];
  tmpMesg[j++] = '\n';
  tmpMesg[j++] = '\0';
  parseErrorMessage = Chars(tmpMesg);
*/
}


Bool WordParser::atStartOfWord( )
{
  return ( curToken == GENERATOR || ( curToken == INT && tokenInt == 1 ) ||
			  curToken == LSQUARE   || curToken == LPAREN );
}


void WordParser::invertName(char *s)
{
  // @am as invertName is used only to invert names
  // it is not dangerous to return the name of generator itself. 
  if ( !isInvertibleName( s ) )
    return;

  int i = 0;
  while ( s[i] ) {
	 if ( s[i] >= 'a' && s[i] <= 'z' )
		s[i] += 'A' - 'a';
	 else if ( s[i] >= 'A' && s[i] <= 'Z' )
		s[i] += 'a' - 'A';
	 ++i;
  }
}

bool WordParser::isInvertibleName( char *s )
{
  int i = 0;
  while ( s[i] ) {
    if ( i > 0 && !( s[i] >= '0' && s[i] <= '9' ) )
      return false;
    i++;
  }  
  return true;
    
}


ParseNode *WordParser::parseExpression( )
{
  ParseNode *root = parseTerm( );
  if ( root == NULL ) return NULL;
  
  ParseNode *tmp;
  while ( 1 )
	 switch ( curToken ) {
		
	 case STAR :
		getToken( );
		tmp = parseTerm( );
		if ( tmp == NULL ) {
		  delete root;
		  return NULL;
		}
		root = new Concat( root, tmp );
		break;
		
	 default :
		if ( atStartOfWord() ) {
		  tmp = parseTerm( );
		  if ( tmp == NULL ) {
			 delete root;
			 return NULL;
		  }
		  root = new Concat( root, tmp );
		  break;
		} else return root;
	 }
}


ParseNode *WordParser::parseTerm( )
{
  ParseNode *root = parseAtom( );
  if ( root == NULL ) return NULL;

  if ( root->whatAmI( ) == INT_NODE ) {
	 parseError("Sorry, <integer>^ (even <integer>^<integer> !) "
		    "is illegal");
	 return NULL;
  }

  ParseNode *tmp;  
  while ( 1 )
	 switch ( curToken ) {

	 case CARET :
		getToken( );
		tmp = parseAtom();
		if ( tmp == NULL ) {
		  delete root;
		  return NULL;
		}
		root = new PowerOrConjugate( root, tmp );
		break;
		
	 default :
		return root;
	 }
}


ParseNode *WordParser::parseAtom( )
{
  ParseNode *root;
  
  switch ( curToken ) {
	 
  case GENERATOR : {
	 if ( tokenInt == 0 ) return NULL;
	 root = new Ident( tokenInt );
	 getToken( );
	 return root;
  }
	 
  case LSQUARE : {
	 getToken( );
	 ParseNode *firstTerm = parseExpression( );
	 if ( firstTerm == NULL ) return NULL;
	 if ( curToken != COMMA ) {
		parseError("Expected a ',' here");
		delete firstTerm;
		return NULL;
	 }
	 getToken( );
	 
	 ParseNode *secondTerm = parseExpression( );
	 if ( secondTerm == NULL ) {
		delete firstTerm;
		return NULL;
	 }
    
	 root = new Commutator( firstTerm, secondTerm );

	 while ( curToken == COMMA ) {
		getToken( );
		secondTerm = parseExpression( );
		if ( secondTerm == NULL ) {
		  delete root;
		  return NULL;
		}
		root = new Commutator( root, secondTerm );
	 }

	 if ( curToken != RSQUARE ) {
		parseError("Expected a ']' here");
		delete root;
		return NULL;
	 }
	 getToken( );

	 return  root;
  }
	 
  case LPAREN : {
	 getToken( );
	 root = parseExpression( );
	 if ( curToken != RPAREN ) {
		parseError("Expected a ')' here");
		delete root;
		return NULL;
	 }
	 getToken( );
	 return root;
  }

  case INT :
	 if ( tokenInt == 1 ) {
	   // It is harmless to interpret w^1 as 1^-1*w*1.
	   root = new Ident( 0 );   
	   getToken( );
	   return root;
	 }
	 root = new Int( tokenInt );
	 getToken( );
	 return root;
	 
  default :
	 parseError("Expected a generator, an integer, '(', or '['  here");
	 return NULL;
  }
}


// -------------------------- ParseNode methods ------------------------------


ParseType Commutator::eval( )
{ 
  result1 = arg1->eval();
  result2 = arg2->eval();

  ParseType arg1Reverse = result1.invertWord( );
  ParseType arg2Reverse = result2.invertWord( );

  ParseType temp1 = ParseType::join( result1, result2 );
  ParseType temp2 = ParseType::join( arg1Reverse, arg2Reverse );

  ParseType finalResult = ParseType::join( temp2, temp1 );

  temp1.destroy();
  temp2.destroy();
  arg1Reverse.destroy();;
  arg2Reverse.destroy();;

  return finalResult;
}


ParseType PowerOrConjugate::eval( )
{
  result1 = arg1->eval();
  if ( arg2->whatAmI( ) == INT_NODE ) {
    int power = ((Int *)arg2)->intEval( );
    
    if ( power < 0 ) {
      ParseType rev = result1.invertWord(  );
      ParseType result = rev.makeCopies( abs( power ) );
      rev.destroy();
      return result;
    } else
      return  result1.makeCopies( power );
  } else {
    result2 = arg2->eval();
    ParseType rev = result2.invertWord();      
    ParseType temp = ParseType::join( rev, result1 );
    ParseType result = ParseType::join( temp, result2 );
    temp.destroy();
    rev.destroy();
    return result;
  }
}


ParseType Concat::eval( )
{
  result1 = arg1->eval();
  result2 = arg2->eval();
  return ParseType::join( result1, result2 );
}

////////////////////////////////////////////////////////////////////////
//                                                                    //
//   ParseType methods                                                //
//                                                                    //
////////////////////////////////////////////////////////////////////////

ParseType::ParseType( int gen )
  : status( PT_OK )
{
  if( gen == 0 ) {
    len = 0;
    data = 0;
  }
  else {
    len = 1;
    data = new ParseData [len+1];
    data[0] = gen;
    data[1] = 0;
  }
}


ParseType ParseType::invertWord( ) const
{
  if( status ) return ParseType( status, 0, 0 );

  ParseData *result = new ParseData[len + 1];
  
  for ( int i = 0, j = len - 1; i < len; i++, j-- )
    result[i] = -data[j];
  result[len] = 0;
  
  return  ParseType( PT_OK, result, len );
}


ParseType ParseType::makeCopies( unsigned int numOfCopies ) const
{
  if( status ) return ParseType( status, 0, 0 );

  if( len * numOfCopies > MAX_WORD_LENGTH ) 
    return ParseType( PT_TOO_LONG, 0, 0 );

  int newLen = numOfCopies*len;
  ParseData *result = new ParseData[ newLen + 1];
  
  int k = 0;
  for( int i = 0; i < numOfCopies; ++i )
    for( int j = 0; j < len; ++j )
      result[k++] = data[j];
  result[k] = 0;
  
  return  ParseType( PT_OK, result, newLen );
}


ParseType ParseType::join( const ParseType& x, const ParseType &y )
{
  if( x.status )
    return ParseType( x.status, 0, 0 );
  if( y.status )
    return ParseType( y.status, 0, 0 );

  int len1 = x.len;
  int len2 = y.len;

  if( x.len + y.len > MAX_WORD_LENGTH ) 
    return ParseType( PT_TOO_LONG, 0, 0 );
  
  ParseData *result = new ParseData[ x.len + y.len + 1];
  
  int k = 0;
  for( int i = 0; i < x.len; ++i )
    result[k++] = x.data[i];
  for( int i = 0; i < y.len; ++i )
    result[k++] = y.data[i];
  
  result[k] = 0;
  
  return  ParseType( PT_OK, result, x.len + y.len );
}


Chars ParseType::errorMessage( ) const 
{
  switch( status ) {
  case PT_OK:
    return "No error with a ParseType object, but there is an error "
      "in caller.";
  case PT_TOO_LONG: 
    //@dp Here should be used MAX_WORD_LENGTH
    int dummy = MAX_WORD_LENGTH;
    return "Word exceeded max length of 32000.";
  }

  return "Internal error with a ParseType object.";
}

Word ParseType::makeWord( ) const 
{
  //@rn terrible kludge!
  VectorOf<Generator> kludge(len);
  for( int i = len-1; i >= 0; --i ) 
    kludge[i] = data[i];

  return Word( kludge );
}

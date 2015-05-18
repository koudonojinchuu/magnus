// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of parser for presentations.
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#include "PresentationParser.h"
#include "conversions.h"


VectorOf<Word>
PresentationParser::parseWordList( const VectorOf<Chars>& names, Chars& errMesg )
{
  genNames = names;

  if ( curToken == INIT ) getToken();

  ListOf<Word> result;

  if ( curToken == INIT ) getToken();

  while ( atStartOfWord() ) {
    Word w = parseWord( names, errMesg );
    if ( errMesg.length() > 0 ) {
      return makeVectorOf(result);
    }
    result.append( w );

    if ( curToken == COMMA ) {
      getToken();
      if ( !atStartOfWord() ) {
	parseError("Expected a word here");
	errMesg = parseErrorMessage;
	return makeVectorOf(result);
      }
    }
  }

  return makeVectorOf(result);
}


SetOf<Word>
PresentationParser::parseWordSet( const VectorOf<Chars>& names, Chars& errMesg )
{
  genNames = names;

  if ( curToken == INIT ) getToken();

  SetOf<Word> result;

  if ( curToken == INIT ) getToken();

  while ( atStartOfWord() ) {
	 Word w = parseWord( names, errMesg );
	 if ( errMesg.length() > 0 ) {
		return result;
	 }
	 result.adjoinElement( w );

	 if ( curToken == COMMA ) {
		getToken();
      if ( !atStartOfWord() ) {
		  parseError("Expected a word here");
		  errMesg = parseErrorMessage;
		  return result;
		}
	 }
  }

  return result;
}


SetOf<Word>
PresentationParser::parseSetOfWords( const VectorOf<Chars>& names,
				     Chars& errMesg)
{
  genNames = names;

  if ( curToken == INIT ) getToken();

  if ( curToken != LSET ) {
	 parseError("Expected a '\\{' here");
	 errMesg = parseErrorMessage;
	 SetOf<Word> empty;
	 return empty;
  }

  getToken();
  SetOf<Word> result = parseWordSet( names, errMesg );

  if ( curToken != RSET ) {
	 parseError("Expected a '\\}' here");
	 errMesg = parseErrorMessage;
	 SetOf<Word> empty;
	 return empty;
  }
  curToken = INIT;  // Set up for subsequent call.
  return result;
}


VectorOf<Word>
PresentationParser::parseVectorOfWords( const VectorOf<Chars>& names,
					Chars& errMesg, BraceType brace)
{
  TokenType theBraces[2][2] = { {LSET, RSET}, {LPAREN, RPAREN} };
  genNames = names;
  

  if ( curToken == INIT ) getToken();

  if ( curToken != theBraces[brace][0] ) {
	 parseError("Expected a '\\{' here");
	 errMesg = parseErrorMessage;
	 VectorOf<Word> empty;
	 return empty;
  }

  getToken();
  VectorOf<Word> result = parseWordList( names, errMesg );

  if ( curToken != theBraces[brace][1] ) {
	 parseError("Expected a '\\}' here");
	 errMesg = parseErrorMessage;
	 VectorOf<Word> empty;
	 return empty;
  }
  curToken = INIT;  // Set up for subsequent call.
  return result;
}



Word
PresentationParser::parseRelator( const VectorOf<Chars>& names, Chars& errMesg )
{
  genNames = names;

  if ( curToken == INIT ) getToken();

  if ( !atStartOfWord() ) {
	 parseError("Expected a word here");
	 errMesg = parseErrorMessage;
	 return Word();
  } else {
	 Word u = parseWord( names, errMesg );
	 if ( errMesg.length() > 0 ) {
		return Word();
	 }
	 if ( curToken == EQUALS ) {
		getToken();
		Word v = parseWord( names, errMesg );
		if ( errMesg.length() > 0 ) {
		  return Word();
		}
		return ((Word)(u * v.inverse())).cyclicallyReduce();
	 } else return u.cyclicallyReduce();
  }
}


SetOf<Word>
PresentationParser::parseRelatorList(const VectorOf<Chars>& names,Chars& errMesg)
{
  genNames = names;

  SetOf<Word> result;

  if ( curToken == INIT ) getToken();

  while ( atStartOfWord() ) {
	 Word u = parseWord( names, errMesg );
	 if ( errMesg.length() > 0 ) {
		return result;
	 }

	 if ( curToken != EQUALS )
		result.adjoinElement( u.cyclicallyReduce() );
	 else {
		while ( curToken == EQUALS ) {
		  getToken();
		  Word v = parseWord( names, errMesg );
		  if ( errMesg.length() > 0 ) {
			 return result;
		  }
		  result.adjoinElement( ((Word)(u * v.inverse())).cyclicallyReduce() );
		}
	 }

	 if ( curToken == COMMA ) {
		getToken();
      if ( !atStartOfWord() ) {
		  parseError("Expected a word here");
		  errMesg = parseErrorMessage;
		  return result;
		}
	 }
  }

  return result;
}

bool PresentationParser::getRangeOf( const Chars& name ,Chars& c, int& n){
  char last = name[name.length()-1];
  // If last char is not a number 
  if ( !(last >= '0' && last <= '9') ) return false;
  bool endOfNum = false;
  Chars charPart;
  Chars numPart;
  for (int i = name.length()-1;i>=0;i--){
    char ch = name[i];
    if(!endOfNum && (ch >= '0' && ch <= '9'))
      numPart = Chars(ch)+numPart;
    else {
      endOfNum = true;
      charPart = Chars(ch)+charPart;
    }
  }
  // If name consists of numbers only
  if (!endOfNum) return false;
  c = charPart;
  n = atoi(numPart);
  return true;
}

bool PresentationParser::getGeneratorRange( const Chars& name,
					    VectorOf<Chars>& result,
					    Chars& errMesg )
{
  if ( curToken != DOT ) 
    error("bool PresentationParser::getGeneratorRange() "
	  "can't read the range");
  // Make sure, that we have 3 dots in a row
  for (int i = 0;i<2;i++){
    getToken();
    if ( curToken != DOT ) {
      parseError("Expected '.' here");
      errMesg = parseErrorMessage;
      return false;
    }   
  }
  getToken();
  
   // Supose to be ','
  if ( curToken != COMMA ) {
    parseError("Expected ',' here");
    errMesg = parseErrorMessage;
    return false;
  }    

  getToken();
  
  // Supose to be a generator
  if ( curToken != GENERATOR ) {
    parseError("Expected a generator here");
    errMesg = parseErrorMessage;
    return false;
  }    
  Chars name1;
  int beginRange;
  
  // Generators must be in the form: <name><index>, where
  // index is the first and the last generator index in a range.
  // 
  Chars rangeErrMsg = "When defining a set of generators using a range of "
    "subscripts, both the smallest subscript and the largest subscript"
    " must be given and the smallest subscript mast be less then the "
    "largest one";

  if ( !getRangeOf(name,name1,beginRange)) {
    parseError(rangeErrMsg);
    errMesg = parseErrorMessage;
    return false;    
  }
  Chars name2;
  int endRange;  
  if ( !getRangeOf(tokenName,name2,endRange)) {
    parseError(rangeErrMsg);
    errMesg = parseErrorMessage;
    return false;    
  }

  // The last index must be greater than the first one
  if ( endRange <= beginRange) {
    parseError(rangeErrMsg);
    errMesg = parseErrorMessage;
    return false;    
  }
 
  // Names gave to be equal
  if ( name2 != name1) {
    parseError(rangeErrMsg);
    errMesg = parseErrorMessage;
    return false;    
  }

  // Generate generators of type: name1<index>, where
  // beginRange < index < endRange
  int numberOfGens = endRange - beginRange - 1;
  if (numberOfGens > 0){
    VectorOf<Chars> gens(numberOfGens);
    
    for(int i = 0;i<numberOfGens;i++){
      gens[i] = name1+Chars(beginRange + 1 + i);
      // Check for duplication and presence of inverses.
      if ( result.indexOf(gens[i]) >= 0 ) {
	parseError("Duplicate generator");
	errMesg = parseErrorMessage;
	return false;
      } else {
	char str[100];
	strcpy(str,gens[i]);
	invertName(str);
	if ( result.indexOf(Chars(str)) >= 0 ) {
	  parseError("Duplicate generator: formal inverse");
	  errMesg = parseErrorMessage;
	  return false;
	}
      }
    }
    result = concatenate(result, gens);
  }
  return true;
}

VectorOf<Chars> PresentationParser::parseGeneratorList( Chars& errMesg )
{
  VectorOf<Chars> result;

  if ( curToken == INIT ) getToken();

  while ( curToken == GENERATOR ) {

	 Chars name(tokenName);

    // Check for duplication and presence of inverses.
	 if ( result.indexOf(name) >= 0 ) {
		parseError("Duplicate generator");
		errMesg = parseErrorMessage;
		return result;
	 } else {
		invertName(tokenName);
		if ( result.indexOf(Chars(tokenName)) >= 0 ) {
		  parseError("Duplicate generator: formal inverse");
		  errMesg = parseErrorMessage;
		  return result;
		}
	 }

	 result.append(name); // Ok, it's inefficient, but time is $$

	 getToken();

	 if ( curToken == COMMA ) {
	   getToken();
	   if ( curToken == DOT ) {
	     if (!getGeneratorRange(name,result,errMesg))
	       return result;
	   } else {
	     if ( curToken != GENERATOR ) {
	       parseError("Expected a generator here");
	       errMesg = parseErrorMessage;
	       return result;
	     }
	   }
	 }
  }

  return result;
}


FreeGroupRep* PresentationParser::parseFreeGroup( Chars& errMesg )
{
  if ( curToken == INIT ) getToken();

  if ( curToken != LANGLE ) {
	 parseError("Expected '<' here");
	 errMesg = parseErrorMessage;
	 return NULL;
  }

  getToken(); // Pop the '<'

  VectorOf<Chars> names = parseGeneratorList( errMesg );
  if ( errMesg.length() > 0 ) return NULL;

  genNames = names;
  // curToken should be one of '|' | ':' | ';' | '>',
  // so we've set genNames just in time.

  if ( curToken == BAR || curToken == COLON || curToken == SEMICOLON ) {
	 getToken(); // Pop the '|' | ':' | ';'
  }
	 
  if ( curToken != RANGLE ) {
	 parseError("Expected '>' here");
	 errMesg = parseErrorMessage;
	 return NULL;
  }

  curToken = INIT;  // Set up for subsequent call.

  return new FreeGroupRep( names );
}



FPGroupRep* 
PresentationParser::parseFPGroup( Chars& errMesg )
{
  if ( curToken == INIT ) getToken();

  if ( curToken != LANGLE ) {
	 parseError("Expected '<' here");
	 errMesg = parseErrorMessage;
	 return NULL;
  }

  getToken(); // Pop the '<'

  VectorOf<Chars> names = parseGeneratorList( errMesg );
  if ( errMesg.length() > 0 ) {
	 return NULL;
  }

  genNames = names;
  // curToken should be one of '|' | ':' | ';' | '>',
  // so we've set genNames just in time.

  SetOf<Word> relators;
  if ( curToken == BAR || curToken == COLON || curToken == SEMICOLON ) {
	 getToken(); // Pop the '|' | ':' | ';'
	 
	 relators = parseRelatorList( names, errMesg );
	 if ( errMesg.length() > 0 ) {
		return NULL;
	 }
  }
	 
  if ( curToken != RANGLE ) {
	 parseError("Expected '>' here");
	 errMesg = parseErrorMessage;
	 return NULL;
  }

  curToken = INIT;  // Set up for subsequent call.

  return new FPGroupRep( names, relators );
}

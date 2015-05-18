// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class HNNExtensionParser.
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
//   Valid input: < t, <> | >, < t, <a,b> | >, < t, <a,b> | a^t = b >
//
// Revision History:
//

    

#include "HNNParser.h"


void HNNExtensionParser::parseHNNRelator( const VectorOf<Chars>& names,
  Word& left, Word& right, Chars& errMesg)
{
  left = right = Word();

  if ( curToken == INIT ) getToken();
	
  if ( !atStartOfWord() ) {
    parseError("Expected a word here");
    errMesg = parseErrorMessage;
    return;
  } 
  else {
    left = parseWord( names, errMesg );
    if ( errMesg.length() > 0 ) {
      left = Word();
      return;
    }

    if ( curToken != EQUALS ) {
      parseError("Expected '=' here");
      errMesg = parseErrorMessage;
      return;
    }
    
    getToken(); // pop '='
    
    right = parseWord( names, errMesg );
    if ( errMesg.length() > 0 ) {
      left = right = Word();
      return;
    }
  }
}


VectorOf<Word>
HNNExtensionParser::parseHNNRelatorsList( const VectorOf<Chars>& names,
  Chars& errMesg, int maxRelators )
{
  genNames = names;
  
  VectorOf<Word> defaultResult;
  VectorOf<Word> result(0,true);
  
  if ( curToken == INIT ) getToken();
  
  while ( atStartOfWord() ) {
    Word left = parseWord( names, errMesg );
    if ( errMesg.length() > 0 ) {
      return defaultResult;
    }
    
    if ( curToken != EQUALS ) {
      parseError("Expected '=' here");
      errMesg = parseErrorMessage;
      return defaultResult;
    }
    getToken(); // pop '='
    
    Word right = parseWord( names, errMesg );
    if ( errMesg.length() > 0 ) {
      return defaultResult;
    }
    
    result.append(left);
    result.append(right);
    
    if( maxRelators > 0 ) {
      --maxRelators;
      if( maxRelators == 0 ) return result;
    }
    
    if ( curToken == COMMA ) {
      getToken();
      if ( !atStartOfWord() ) {
	parseError("Expected a word here");
	errMesg = parseErrorMessage;
	return defaultResult;
      }
    }
  }
  
  return result;
}


void HNNExtensionParser::parseBasisFreeGroupAndRelations( FreeGroup& F,
  Chars& stableGenName, VectorOf<Word>& A, VectorOf<Word>& B, 
  int maxRelators, Chars& errMesg )
{
  if ( curToken == INIT ) getToken();

  if ( curToken != LANGLE ) {
    parseError("Expected '<' here");
    errMesg = parseErrorMessage;
  }
  
  getToken(); // pop '<'

  bool stableLetterDefined = false;

  if( curToken == GENERATOR ) {

    stableGenName = tokenName;
    getToken();  // pop a generator (stable letter)
    
    if( curToken != COMMA ) {
      parseError("Expected a comma here");
      errMesg = parseErrorMessage;
      return;
    }

    getToken(); // pop ','
    stableLetterDefined = true;
  }


  if( curToken != LANGLE ) {
    parseError("Expected a '<' here");
    errMesg = parseErrorMessage;
    return;
  }

  // parse basis free group.
    
  FreeGroupRep *tmp_free_group = parseFreeGroup( errMesg );
  if( errMesg.length() > 0 ) return;
  F = FreeGroup( tmp_free_group->theNamesOfGenerators );
  delete tmp_free_group;

  if ( curToken == INIT ) getToken();

  if( !stableLetterDefined ) {
    
    if( curToken != COMMA ) {
      parseError("Expected a comma here");
      errMesg = parseErrorMessage;
      return;
    }

    getToken(); // pop ','

    if( curToken != GENERATOR ) { 
      parseError("Expected a generator here");
      errMesg = parseErrorMessage;
      return;
    }

    stableGenName = tokenName;
    getToken(); // pop a generator (stable letter)
  }

  // parse a list of relators.

  if ( curToken == INIT ) getToken();

  if ( curToken != BAR && curToken != COLON && curToken != SEMICOLON ) {
    parseError("Expected '|' or ';' or ':' here");
    errMesg = parseErrorMessage;
    return;
  }
  
  VectorOf<Chars> names = F.namesOfGenerators();
  names.append( stableGenName );

  genNames = names;

  getToken(); // pop a separator ('|' ';' ':').

  VectorOf<Word> relators = parseHNNRelatorsList( names, errMesg, maxRelators );
  if( errMesg.length() > 0 )
    return;

  int freeGens = F.numberOfGenerators();
  Generator stableLetter( freeGens+1 );
  Generator invStableLetter = inv(stableLetter);
  int numberOfRelators = relators.length() / 2;
  A = VectorOf<Word>( numberOfRelators );
  B = VectorOf<Word>( numberOfRelators );

  for( int i = 0, j = 0; j < numberOfRelators; ++j, i+=2 ) {

    Word left = relators[i];
    Word right = relators[i+1];
    Word relator = left * right.inverse();
    int relLen = relator.length();

    int pos1;
    for( pos1 = 0; pos1 < relLen && invStableLetter != relator[0]; ++pos1 )
      relator.cyclicallyPermute(1);
    
    int pos2;
    for( pos2 = 1; pos2 < relLen &&stableLetter != relator[pos2]; ++pos2 ) {
      /* empty body*/ 
    }

    if( pos1 == relLen || pos2 == relLen ) {
      // a stable letter and the inverse one not found.
      parseError("Relators must contain the stable letter and inverse of one.");
      errMesg = parseErrorMessage;
      return;
    }

    if( pos2 == 1 || pos2 == relLen-1 ) {
      parseError("Generators of subgroups cannot be the empty word.");
      errMesg = parseErrorMessage;
      return;
    }

    A[j] = relator.subword( 1, pos2 );
    B[j] = relator.subword( pos2 + 1, relLen ).inverse();
    
    if( abs(ord(B[j].maxOccurringGenerator())) > freeGens ) {
      parseError("The stable letter should occur only twice in relator:"
		 "once with degree 1 and once with degree -1.");
      errMesg = parseErrorMessage;
      return;
    }
  }

  if ( curToken != RANGLE ) {
	 parseError("Expected '>' here");
	 errMesg = parseErrorMessage;
	 return;
  }

  curToken = INIT;  // Set up for subsequent call.
}

HNNExtOfFreeGroupRep* 
HNNExtensionParser::parseHNNExtensionOfFreeGroup( Chars& errMesg )
{

  FreeGroup F;
  Chars stableGenName;
  VectorOf<Word> A,B;

  parseBasisFreeGroupAndRelations( F, stableGenName, A, B, 0, errMesg );

  if( errMesg.length() > 0 ) 
    return NULL;

  SGofFreeGroup Sa( F, A );
  SGofFreeGroup Sb( F, B );    
  return new HNNExtOfFreeGroupRep( F, stableGenName, Sa, Sb );
}

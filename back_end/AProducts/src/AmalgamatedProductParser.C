// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class AmalgamatedProductParser
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//


#include "AmalgamatedProductParser.h"


//---------------------------------------------------------------------------//
//-------------------- AmalgamatedProductParser -----------------------------//
//---------------------------------------------------------------------------//


VectorOf<Word>
AmalgamatedProductParser::parseAPRelator( const VectorOf<Chars>& names, 
  Chars& errMesg )
{
  VectorOf<Word> defaultResult;
  
  if ( curToken == INIT ) getToken();
	
  if ( !atStartOfWord() ) {
    parseError("Expected a word here");
    errMesg = parseErrorMessage;
    return defaultResult;
  } else {
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
    defaultResult.append(left);
    defaultResult.append(right);
  }
  return defaultResult;
}


VectorOf<Word>
AmalgamatedProductParser::parseAPRelatorList( const VectorOf<Chars>& names,
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
      if( maxRelators == 0 ) return result; //???
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


void AmalgamatedProductParser::parseFactorsAndRelations(FreeGroup& f1,
  FreeGroup& f2, VectorOf<Word>& gens1, VectorOf<Word>& gens2,
  int maxRelators, Chars& errMesg)
{
  if ( curToken == INIT ) getToken();

  if ( curToken != LANGLE ) {
    parseError("Expected '<' here");
    errMesg = parseErrorMessage;
  }
  
  getToken(); // pop '<'
  

  FreeGroupRep *free_group = parseFreeGroup(errMesg);
  if( errMesg.length() > 0 ) return;  
  f1 = FreeGroup( free_group->theNamesOfGenerators );
  delete free_group;
  
#ifdef DEBUG_APGroup
  cerr << "f1 = " << f1 << endl;
#endif
  
  if ( curToken == INIT ) getToken();
  
  if( curToken != STAR ) {
    parseError("Expected '*' here");
    errMesg = parseErrorMessage;
    return;
  }
  
  getToken(); // pop '*'
  
  free_group = parseFreeGroup(errMesg);
  if( errMesg.length() > 0 ) return;
  f2 = FreeGroup( free_group->theNamesOfGenerators );
  delete free_group;
  
#ifdef DEBUG_APGroup
  cerr << "f2 = " << f2 << endl;
#endif
  
  if ( curToken == INIT ) getToken();

  if ( curToken != BAR && curToken != COLON && curToken != SEMICOLON ) {
    parseError("Expected '|' or ';' or ':' here");
    errMesg = parseErrorMessage;
    return;
  }
  
  VectorOf<Chars> names = concatenate( f1.namesOfGenerators(), f2.namesOfGenerators() );
  genNames = names;
  getToken(); // pop a separator ('|' ';' ':').
  
  VectorOf<Word> relators;
  relators = parseAPRelatorList( names, errMesg, maxRelators );
  if( errMesg.length() > 0 ) {
    return;
  }
  
#ifdef DEBUG_APGroup
  if( relators.length() % 2 != 0 )
    error("%relators% has odd length.");
  
  FreeGroup tmp_group(names);
  tmp_group.printVectorOfWords(cerr, relators);
  cerr << endl;
#endif
  
  VectorOf<Word> emptyRelators;
  AmalgProductOfFreeGroups ap(f1, f2, emptyRelators, emptyRelators);
  int relCount = relators.length() / 2;
  VectorOf<Word> leftRelators(relCount);
  VectorOf<Word> rightRelators(relCount);
  for(int i = 0, j = 0; j < relCount; j++, i+=2) {
    Word left = relators[i];
    Word right = relators[i+1];
    if( ap.factorOfFormalWord(left)  == LEFT_FACTOR && 
       ap.factorOfFormalWord(right) == RIGHT_FACTOR ) {
      leftRelators[j] = left;
      rightRelators[j] = ap.globalToLocal(right);
    }
    else if( ap.factorOfFormalWord(left)  == RIGHT_FACTOR && 
	    ap.factorOfFormalWord(right) == LEFT_FACTOR ) {
      leftRelators[j] = right;
      rightRelators[j] = ap.globalToLocal(left);
    }
    else {
      parseError("Parts of relators must belong to different factors");
      errMesg = parseErrorMessage;
      return;
    }
  }
  
  if( curToken != RANGLE ) {
    parseError("Expected '>' here");
    errMesg = parseErrorMessage;
    return;
  }
  
  curToken = INIT;  // Set up for subsequent call.
  
#ifdef DEBUG_APGroup
  cerr << "APGroup (data):" << endl;
  cerr << "free group 1 is " << f1 << endl;
  cerr << "free group 2 is " << f2 << endl;
  cerr << "relators are " << endl;
  int doComma = 0;
  for(int k = 0; k<leftRelators.length(); k++) {
    if( doComma ) cerr << ", ";
    f1.printWord(cerr, leftRelators[k]);
    cerr << " = ";
    f2.printWord(cerr, rightRelators[k]);
    doComma = 1;
  }
  cerr << endl;
#endif
  
  gens1 = leftRelators;
  gens2 = rightRelators;
}



AmalgProductOfFreeGroupsRep*
AmalgamatedProductParser::parseAPofFreeGroups( Chars& errMesg )
//
// <AmalgamatedProductOfFreeGroups ::= '<' <free group> '*' <free group> 
//         ('|' | ';' | ':') (<word> '=' <word>)* '>'
{
  FreeGroup f1, f2;
  VectorOf<Word> gens1, gens2;
  parseFactorsAndRelations(f1, f2, gens1, gens2, 0, errMesg);
  if( errMesg.length() > 0 ) 
    return NULL;
  else
    return new AmalgProductOfFreeGroupsRep(f1, f2, gens1, gens2);
}

APwithOneRelatorRep*
AmalgamatedProductParser::parseAPwithOneRelator( Chars& errMesg )
//
// <APwithOneRelator ::= '<' <free group> '*' <free group> 
//         ('|' | ';' | ':') (<word> '=' <word>)? '>'
{
  FreeGroup f1, f2;
  VectorOf<Word> gens1, gens2;
  parseFactorsAndRelations(f1, f2, gens1, gens2, 1, errMesg); 
  if( errMesg.length() > 0 ) 
    return NULL;
  else
    return new APwithOneRelatorRep(f1, f2, gens1, gens2);
}




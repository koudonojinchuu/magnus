// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of parser for Maps.
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//
// * 02/27 Dmitry B. added couple of new checks in parseMap().
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//

#include "MapParser.h"


MapRep* MapParser::parseMap( const FGGroup& domain, const FGGroup& range,
			     Chars& errMesg )
{
  if( domain.numberOfGenerators() == 0 ) {
    parseError("Cannot define a map because the domain group is trivial");
    errMesg = parseErrorMessage;
    return NULL;
  }

  if( range.numberOfGenerators() == 0 ) {
    parseError("Cannot define a map because the range group is trivial");
    errMesg = parseErrorMessage;
    return NULL;
  }

  int len = domain.namesOfGenerators().length();
  VectorOf<Word> images(len);
  VectorOf<bool> seen(len);
  for( int i = 0; i < len; ++i ) {
	 seen[i] = false;
  }

  if ( curToken == INIT ) getToken();

  if ( curToken != LSET ) {
	 parseError("Expected a '\\{' here");
	 errMesg = parseErrorMessage;
	 return NULL;
  }

  do {

	 genNames = domain.namesOfGenerators();
	 getToken();

	 if ( curToken != RSET ) {
		
		if ( curToken != GENERATOR ) {
		  parseError("Expected a generator here");
		  errMesg = parseErrorMessage;
		  return NULL;
		}

		// The parser catches the use of formal inverses when it tries to
		// read past the generator. Case switching is not caught, so we
		// have to.

		int index = tokenInt - 1;

		if ( index < 0 ) {
		  parseError("Sorry, you cannot give the image of an `inverse' generator");
		  errMesg = parseErrorMessage;
		  return NULL;
		}

		if ( seen[index] == true ) {
		  parseError("You gave the image of a generator twice");
		  errMesg = parseErrorMessage;
		  return NULL;
		} else {
		  seen[index] = true;
		}
		
		getToken();
		if ( curToken != ARROW ) {
		  parseError("Expected \"->\" here");
		  errMesg = parseErrorMessage;
		  return NULL;
		}
		
		genNames = range.namesOfGenerators();
		getToken();
		Word w = parseWord( range.namesOfGenerators(), errMesg );
		if ( errMesg.length() > 0 ) {
		  return NULL;
		}
		images[index] = w;
		
		if ( (curToken != COMMA) && (curToken != RSET) ) {
		  parseError("Expected a ',' or '\\}' here");
		  //@rn This winds up as '\}' in the FE. See PresentationParser.C too.
		  errMesg = parseErrorMessage;
		  return NULL;
		}
	 }

  } while ( curToken != RSET );

  for( int i = 0; i < len; ++i )
    if( !seen[i]) {
      parseError("You have to define images of all generators");
      errMesg = parseErrorMessage;
      return NULL;
    }
  

  return new MapRep(domain, range, images);
}

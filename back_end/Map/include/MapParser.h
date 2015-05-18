// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of parser for Maps.
//
// Principal Author: Roger Needham
//
// Status: useable
//
// Revision History:
//

#ifndef _MAP_PARSER_H_
#define _MAP_PARSER_H_


#include "WordParser.h"
#include "Map.h"


// A Map defined on generators { x, y, z } has the form
// {
//   x -> w(x,y,z),
//   y -> w(x,y,z),
//   z -> w(x,y,z)
// }
// where the images of x, y, z need not be in order, whitespace is optional,
// and w(x,y,z) are words in the generators. Missing images default to the
// identity.


class MapParser : public WordParser {

public:

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Constructors:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////

  MapParser(istream &istr) : WordParser(istr) { }
  // Initialize the parser with the istream from which to read.

  // Destructor supplied by compiler.

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Raison d'etre:                                           //
  //                                                          //
  //////////////////////////////////////////////////////////////

  MapRep* parseMap(
    const FGGroup& domain,
    const FGGroup& range,
    Chars& errMesg
  );

};

#endif

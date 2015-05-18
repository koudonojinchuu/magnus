// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the FreeGroup class.
//
// Principal Author: Stephane Collart
//
// Status: in progress
//
// Revision History:
//

#include "FreeGroup.h"


// friend functions to FreeGroup :

#ifdef DEBUG
  void debugPrint(ostream& s, const FreeGroup& g ) {
 
    s << "FreeGroup:: this : " << &g;
    s << "; rep : " << g.look();
    s << "; nxrefs : " << g.look()->nxrefs();
  }
#endif


//------------------------ FreeGroupRep ----------------------------//
//------------------------- Static members -------------------------//
 
const Type FreeGroupRep::theFreeGroupType = Type( Type::unique() );
 
 
//-------------------------- FreeGroup -----------------------------//
//------------------------- Static members -------------------------//


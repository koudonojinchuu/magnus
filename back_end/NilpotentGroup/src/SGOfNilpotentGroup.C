
// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of SGOfNilpotentGroupRep class.
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Revision History:
//


#include "SGOfNilpotentGroup.h"

#include "SGOfFNGRep.h"
#include "SGOfFPNGRep.h"
#include "FreeNilpotentGroupRep.h"
#include "FPNilpotentGroupRep.h"

#include "PolyWordIterator.h"
#include "FreeGroup.h"
#include "conversions.h"

#include <stdio.h>


//=======================================================================
//=============== class SGOfNilpotentGroup ==============================
//=======================================================================


// constructs the subgroup of nilpotent group ng
SGOfNilpotentGroup::SGOfNilpotentGroup(const NilpotentGroup& ng,
				       const VectorOf<Word>& gens)
  : ObjectOf<SGOfNilpotentGroupRep> (makeRep(ng, gens))
{}


//Produces a representation depending on group's type
SGOfNilpotentGroupRep* SGOfNilpotentGroup::makeRep(const NilpotentGroup& ng,
						   const VectorOf<Word>& gens) {

  if(ng.actualType() == FreeNilpotentGroupRep::type())
    return new SGOfFreeNilpotentGroupRep(ng, gens);

  else if(ng.actualType() == FPNilpotentGroupRep::type())
    return new SGOfFPNilpotentGroupRep(ng, gens);

  else {
    error("SGOfNilpotentGroup: bad parent group type");
    return NULL;
  }
}
  

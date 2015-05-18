// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the AbelianGroupRep class.
//
// Principal Author: Alexey Myasnikov
//
// Status: in progress
//
// Revision History:
//

#include "AbelianSGPresentation.h"

AbelianSGPresentation AbelianGroup::makeSubgroupPresentation(const VectorOf<Word>& vG) const{
  return AbelianSGPresentation(new AbelianSGPresentationRep
			       (look()->makeSubgroupPresentation(vG)));
}

 

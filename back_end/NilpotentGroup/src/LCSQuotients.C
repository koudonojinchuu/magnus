// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes 
//
// Principal Author: Eugene Paderin
//
// Status: Draft
//
// Description:
//
//
//
// Special Notes:
//
//
// Revision History:
//

#include "PolyWord.h"
#include "SGOfNilpotentGroup.h"
#include "AbelianGroup.h"
#include "LCSQuotients.h"

//========================================================================
//==================== LCSQuotient =======================================
//========================================================================

ostream& operator < ( ostream& s, const LCSQuotient& q ) {
  return s < q.abelianization < q.generators < q.infNumber
    < q.numerationShift < q.basis;
}
// IPC output

istream& operator > ( istream& s, LCSQuotient& q ) {
  return s > q.abelianization > q.generators > q.infNumber
    > q.numerationShift > q.basis;
}
// IPC input


//========================================================================
//======================= BasisWord ======================================
//========================================================================

ostream& operator < ( ostream& s, const BasisWord& w ) {

  s < w.theWord < w.theWeight < w.theOrder < w.theLocalNumber 
    < w.theGlobalNumber < w.theName;
}
// IPC output

istream& operator > ( istream& s, BasisWord& w ) {

  s > w.theWord > w.theWeight > w.theOrder > w.theLocalNumber 
    > w.theGlobalNumber > w.theName;
}

// IPC input



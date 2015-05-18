// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class SMPermutation
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#include "SMPermutation.h"


// --------------------------- SMPermutation ------------------------------- //

SMPermutation::SMPermutation(Permutation perm, const Chars heritage)
  : AlgebraicObject( heritage ),
    thePermutation( perm )
{ }


void SMPermutation::printProperties(ostream& ostr) const
{
  ostr << DataPair( Oid( *this ), Object( *this ) );
}


void SMPermutation::viewStructure(ostream& ostr) const
{
  ostr << "viewBuilder objectView viewParameters {viewID "
       << oid()
       << " heritage {"
       << heritage()
       << "}}";
}


void SMPermutation::printDefinition(ostream& ostr) const
{
  ostr << thePermutation;
}


// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class SMVectorOfWords
//
// Principal Author: Eugene Paderin
//
// Status: in progress
//
// Revision History:
//


#include "SMVectorOfWords.h"


//---------------------------- SMVectorOfWords ---------------------------------//


SMVectorOfWords::SMVectorOfWords( SMFPGroup& G, const VectorOf<Word>& V,
			    const Chars heritage)
  : AlgebraicObject( heritage ),
    theGroup( G ),
    theWords( V )
{ }


void SMVectorOfWords::printProperties(ostream& ostr) const
{
  ostr << DataPair( Parent( *this ), Object( theGroup ) );
}


void SMVectorOfWords::viewStructure(ostream& ostr) const
{
  ostr
    << "viewBuilder objectView viewParameters {viewID "
    << oid()
    << " heritage {"
    << heritage()
    << "}}";
}


void SMVectorOfWords::printDefinition(ostream& ostr) const
{
  theGroup.getFPGroup().printVectorOfWords( ostr, theWords );
}


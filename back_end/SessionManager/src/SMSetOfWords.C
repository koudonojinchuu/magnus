// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class SMSetOfWords
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#include "SMSetOfWords.h"


//---------------------------- SMSetOfWords ---------------------------------//


SMSetOfWords::SMSetOfWords( SMFPGroup& G, const SetOf<Word>& V,
			    const Chars heritage)
  : AlgebraicObject( heritage ),
    theGroup( G ),
    theWords( V )
{ }


void SMSetOfWords::printProperties(ostream& ostr) const
{
  ostr << DataPair( Parent( *this ), Object( theGroup ) );
}


void SMSetOfWords::viewStructure(ostream& ostr) const
{
  ostr
    << "viewBuilder objectView viewParameters {viewID "
    << oid()
    << " heritage {"
    << heritage()
    << "}}";
}


void SMSetOfWords::printDefinition(ostream& ostr) const
{
  theGroup.getFPGroup().printSetOfWords( ostr, theWords );
}





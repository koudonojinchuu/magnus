// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the EltRep class.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
//

#include "EltRep.h"


void EltRep::printOn(ostream& o) const { }

void EltRep::debugPrint(ostream& ostr) const {
#ifdef DEBUG
	ostr << "rep:" << this << ";" << "#xrefs:" << nxrefs();
#endif
}

//--------------------------- EltRep -------------------------------//
//------------------------ static members ---------------------------//

//
// static data members initialised in Elt.C
//

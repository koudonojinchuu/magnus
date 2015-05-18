// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of the SEIC, SMEqSystem classes.
//
// Principal Authors: Denis Serbin
//
// Status: In progress
//
// Revision History:
//
// Special Notes:
//
//


#include "SMEqSystem.h"


//---------------------------------------------------------------------------//
//-------------------------------- SEIC -------------------------------------//
//---------------------------------------------------------------------------//


SEIC::SEIC(OID eqs_oid)
  : eqSystemOID( eqs_oid ),
    solvedInAbelian( false ),
    haveSolInAbelian( false ),
    solved( false ),
    haveSol( false )
{ }

SEIC::~SEIC( )
{ }


//---------------------------------------------------------------------------//
//----------------------------- SMEqSystem ----------------------------------//
//---------------------------------------------------------------------------//

SMEqSystem::SMEqSystem(SMFPGroup& G, FreeGroup F, const VectorOf<Word> v, 
		       int nvar, const Chars heritage)
  : AlgebraicObject( heritage ),
    theGroup( G ),
    theFreeGroup( F ),
    theSystem( v ),
    numOfVar( nvar ),
    seic( oid() )
{ }

void SMEqSystem::printProperties(ostream& ostr) const
{
  ostr << DataPair( Parent( oid() ), Object( theGroup ) );
}


void SMEqSystem::viewStructure(ostream& ostr) const
{
  ostr
    << "viewBuilder objectView viewParameters {viewID "
    << oid()
    << " heritage {"
    << heritage()
    << "}}";
}


void SMEqSystem::printDefinition(ostream& ostr) const
{
  theGroup.getFPGroup().printVectorOfWords( ostr, theSystem );
}

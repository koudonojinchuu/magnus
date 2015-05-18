// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class SMObject, and helper IconID
//
// Principal Author: Roger Needham
//
// Status: useable
//
// Revision History:
//


#include <string.h>
#include "SMObject.h"
#include "TheObjects.h"


//---------------------------------------------------------------------------//
//---------------------------- SMObject -------------------------------------//
//---------------------------------------------------------------------------//


SMObject::SMObject(bool is_cm)
  : theOid( TheObjects::reserveOid() ), isCM( is_cm )
{
  TheObjects::enroll( this );
}



//---------------------------------------------------------------------------//
//------------------------------ IconID -------------------------------------//
//---------------------------------------------------------------------------//

bool IconID::operator == ( const IconID& iid ) const
{
  return strcmp( theName, iid.theName ) == 0;
}


bool IconID::operator != ( const IconID& iid ) const
{
  return strcmp( theName, iid.theName ) != 0;
}


//---------------------------------------------------------------------------//
//------------------------------ IconID -------------------------------------//
//----------------------- static initializations ----------------------------//

const IconID IconID::group = IconID("group");
const IconID IconID::subgroup = IconID("subgroup");
const IconID IconID::SetOfWords = IconID("SetOfWords");
const IconID IconID::VectorOfWords = IconID("VectorOfWords");
const IconID IconID::elt = IconID("elt");
const IconID IconID::equation = IconID("equation");
const IconID IconID::systemOfEquations = IconID("systemOfEquations");
const IconID IconID::map = IconID("map");
const IconID IconID::homomorphism = IconID("homomorphism");
const IconID IconID::permutation = IconID("permutation");
const IconID IconID::enumerator_problem = IconID("enumerator_problem");
const IconID IconID::enumerator_object = IconID("enumerator_object");
const IconID IconID::list_object = IconID("list_object");
const IconID IconID::problem = IconID("problem");
const IconID IconID::none = IconID("none");
const IconID IconID::do_not_display = IconID("do_not_display");

// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declarations of FE interface classes which define
//           viewStructures
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#include "viewStructure.h"


//---------------------------------------------------------------------------//
//--------------------------- ObjectView ------------------------------------//
//---------------------------------------------------------------------------//


ObjectView::ObjectView(ostream& ostr, OID oid, Chars heritage)
{
  ostr
	 << "viewBuilder objectView viewParameters {viewID "
	 << oid
	 << " heritage {"
	 << heritage
	 << "}}";
}


//---------------------------------------------------------------------------//
//--------------------------- ProblemView -----------------------------------//
//---------------------------------------------------------------------------//


ProblemView::ProblemView(ostream& o, OID oid, const Text& title,
    const Text& banner, const char* helpID, const Chars abbreviation)
  : state( INIT ), ostr( o )
{
  ostr
	 << "viewBuilder problemView viewParameters {viewID "
	 << oid
	 << " viewAbbreviation {"
	 << abbreviation
	 << "} viewTitle {"
	 << title
	 << "} viewBanner {"
	 << banner
	 << "} totalARCs {ARCSlotID -1 value 0} helpID " << helpID
	 // The canonical ARCSlotID for totalARCs is -1.
	 << " subproblems {";
}


void ProblemView::startItemGroup( )
{
  if ( state == DONE ) error("Continued writing to closed view");
  if ( state == ITEM_GROUP ) ostr << "}} ";  // Close previous
  ostr << "{1 {";
  state = ITEM_GROUP;
}


void ProblemView::startItemGroup(const Expression& E)
{
  if ( state == DONE ) error("Continued writing to closed menu");
  if ( state == ITEM_GROUP ) ostr << "}} ";  // Close previous
  ostr << "{{" << E << "} {";
  state = ITEM_GROUP;
}


void ProblemView::add(const Text& name, ARCSlotID asi, int value, bool enabled)
{
  ostr
	 << "{ARCSlotID " << asi
	 << " name {" << name
	 << "} state ";
  if ( enabled )
	 ostr << '1';
  else
	 ostr << '0';
  ostr
	 << " value " << value
	 << " helpID foo "       //@rn No real helpID yet.
	 << "} ";
}


void ProblemView::done( )
{
  if ( state == DONE ) error("Continued writing to closed menu");
  ostr << "}}}}" << flush;
  state = DONE;
}

// ------------------------ EnumertatorProblemView  ------------------------//

void EnumertatorProblemView::done(ostream& ostr) const {
  ostr
    << "viewBuilder EnumertatorProblemView viewParameters {viewID "
    << theOID
    << " viewAbbreviation {"
    << theAbbreviation
    << "} viewTitle {"
    << theTitle
    << "} viewBanner {"
    << theBanner
    << "} totalARCs {ARCSlotID -1 value 0} helpID " << theHelpID
    // The canonical ARCSlotID for totalARCs is -1.
    << " subproblems ";   
  theSubproblems.submit( ostr );
  ostr << " parameters ";
  theParameters.submit( ostr );
  ostr << " } " << flush;
  
  
}




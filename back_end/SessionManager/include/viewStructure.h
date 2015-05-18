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

#ifndef _VIEWSTRUCTURE_H_
#define _VIEWSTRUCTURE_H_


#include "FEData.h"
#include "ARCSlotID.h"
#include "ViewContents.h"


//---------------------------------------------------------------------------//
//--------------------------- ObjectView ------------------------------------//
//---------------------------------------------------------------------------//

class ObjectView
{
public:

  ObjectView(ostream& ostr, OID oid, Chars heritage);
};


//---------------------------------------------------------------------------//
//--------------------------- ProblemView -----------------------------------//
//---------------------------------------------------------------------------//

class ProblemView : public FEData
{
public:

  ProblemView(ostream& o, OID oid, const Text& title, const Text& banner,
	      const char* helpID = "none", const Chars abbreviation = "none");

  void startItemGroup( );
  void startItemGroup(const Expression& E);

  void add(const Text& name, ARCSlotID asi, int value = 0, bool enabled = true);

  void done( );

private:

  enum StateType { INIT, ITEM_GROUP, DONE };

  StateType state;

  ostream& ostr;

};

// ------------------------- EnumertatorProblemView -------------------------//

class EnumertatorProblemView : public FEData
{
 public: 
  EnumertatorProblemView(OID oid,const Chars& title, const Chars& banner, 
			 const char* helpID = "none",
			 const Chars abbreviation = "none")
    : theOID( oid ),
    theTitle( title ),
    theBanner( banner ),
    theHelpID( helpID ),
    theAbbreviation( abbreviation ),
    theParameters("Parameters"),
    theSubproblems("Subproblems")
    { }
  
  void addProblem(const Subproblem& sp){
    theSubproblems.add(sp);
  }
  
  void addParameter(const ViewContent& c){
    theParameters.add(c);
  }
  
  void done(ostream& ostr) const;

  ParameterStructure extractParameters()const {
    return ParameterStructure(theOID, theParameters);
  }
  
 private:
  OID theOID;
  Chars theTitle;
  Chars theBanner;
  Chars theHelpID;
  Chars theAbbreviation;
  GroupWindow theParameters;
  GroupWindow theSubproblems;
};


#endif

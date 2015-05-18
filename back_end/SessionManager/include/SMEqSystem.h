// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of the SEIC, SMEqSystem classes.
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

#ifndef _SMEQSYSTEM_H_
#define _SMEQSYSTEM_H_

#include "AlgebraicObject.h"
#include "SMFPGroup.h"
#include "Word.h"


//---------------------------------------------------------------------------//
//-------------------------------- SEIC -------------------------------------//
//---------------------------------------------------------------------------//

class SEIC : protected FEData
{
public:
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  SEIC(OID eqs_oid);

  ~SEIC( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Reporting Information:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void putIsSolvedInAbelianization() {
    solvedInAbelian = yes;
  }
  
  void putHaveSolutionsInAbelianization( bool sol ) { 
    haveSolInAbelian = sol; 
  }

  void putIsSolved() {
    solved = yes;
  }
  
  void putHaveSolutions( bool sol ) { 
    haveSol = sol; 
  }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessing Information:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool isSolvedInAbelianization() const { 
    return solvedInAbelian; 
  }
  
  bool haveSolutionsInAbelianization( ) const { 
    return haveSolInAbelian; 
  }

  bool isSolved() const { 
    return solved; 
  }
  
  bool haveSolutions( ) const { 
    return haveSol; 
  }
  
 
private:
  
  SEIC(const SEIC&); // Disable copying

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  OID eqSystemOID;
  // Need this for composing messages about what has been found.

  bool solvedInAbelian;
  bool haveSolInAbelian;
  bool solved;
  bool haveSol;
};

//---------------------------------------------------------------------------//
//----------------------------- SMEqSystem ----------------------------------//
//---------------------------------------------------------------------------//

class SMEqSystem : public AlgebraicObject
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMEqSystem( SMFPGroup& G, FreeGroup F, const VectorOf<Word> v, int nvar, 
	      const Chars heritage);
  
  SMEqSystem( SMFPGroup& G ) 
    : AlgebraicObject(""), 
      theGroup(G), 
      seic( oid() ) 
    { }
  
  //@rn Bogus ctor; see `But no' comment in Menu.h.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  VectorOf<Word> getSystem( ) const { return theSystem; };

  SMFPGroup& getParent( ) const { return theGroup; };

  const FreeGroup& getFreeGroup( ) const { return theFreeGroup; };

  int numberOfVariables( ) const { return numOfVar; };

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const char* type( ) { return "SMEqSystem"; };

  const char* typeID( ) const { return type(); };        // overrides SMObject

  const IconID iconID( ) const { return IconID::systemOfEquations; };
  // overrides SMObject

  void viewStructure(ostream& ostr) const;              // overrides SMObject

  void printProperties(ostream& ostr) const;            // overrides SMObject

  void printDefinition(ostream& ostr) const;            // overrides SMObject


public:

  SEIC seic; 
  // equation information center.

protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Restricted Access:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void readMessage(istream&) { };                     // overrides SMObject


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& theGroup;
  // The group that this equation is over.

  FreeGroup theFreeGroup;
  // The free group generated by theGroup's generators, along with the
  // variables.

  VectorOf<Word> theSystem;
  // A Word in theFreeGroup, represents the equation.

  int numOfVar;
  // a number of variables

};

#endif

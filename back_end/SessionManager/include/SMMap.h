// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class SMMap
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//

#ifndef _SMMAP_H_
#define _SMMAP_H_

#include "AlgebraicObject.h"
#include "SMFPGroup.h"
#include "MIC.h"
#include "Map.h"
#include "AbelianProblems.h"



// ------------------------------- MCM ------------------------------------- //


class MCM : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  MCM( class SMMap& M );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMMap& getMap( ) const { return theMap; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void viewStructure(ostream& ostr) const { }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Subordinate Computation Managers:                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Subordinate<MCM,AbelianHomIsMonoComp> abelianHomIsMono;
  Subordinate<MCM,AbelianHomIsEpiComp> abelianHomIsEpi;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Control:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void takeControl( );        // overrides ComputationManager


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void start( ) { }           // overrides ComputationManager

  void terminate( ) { }       // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  class SMMap& theMap;

  bool didFastChecks;
  // The takeControl method does some sneaky checks of obvious properties
  // (this should be formalized, since they are not always that fast).
  // This records the first call, so that they are not done over and over.

};

//---------------------------------------------------------------------------//
//----------------------------- SMMap ---------------------------------------//
//---------------------------------------------------------------------------//

class SMMap : public AlgebraicObject
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMMap(SMFPGroup& G, Map m, const Chars heritage);

  SMMap(SMFPGroup& G, SMFPGroup& H, Map m, const Chars heritage);

  SMMap(SMFPGroup& G);
 
  SMMap(SMFPGroup& G, SMFPGroup& H);

  //@rn Bogus ctors; see `But no' comment in Menu.h

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Map getMap( ) const { return theMap; }

  SMFPGroup& getDomain( ) const { return theDomain; }

  SMFPGroup& getRange( ) const { return theRange; }

  MCM& mcm() const { return *theMCM; }

        InformationCenter* infoCenter()       { return &mic; }
  const InformationCenter* infoCenter() const { return &mic; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static const char* type( ) { return "SMMap"; }

  const char* typeID( ) const { return type(); }        // overrides SMObject

  const IconID iconID( ) const { return IconID::map; }  // overrides SMObject

  void viewStructure(ostream& ostr) const;              // overrides SMObject

  void printProperties(ostream& ostr) const;            // overrides SMObject

  void printDefinition(ostream& ostr) const;            // overrides SMObject


protected:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Restricted Access:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void readMessage(istream&) { }                        // overrides SMObject


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

public:

  MIC mic;

private:

  const Map theMap;

  SMFPGroup& theDomain;

  SMFPGroup& theRange;

  MCM* theMCM;  // TheObjects removes scm when this is removed.

};

#endif

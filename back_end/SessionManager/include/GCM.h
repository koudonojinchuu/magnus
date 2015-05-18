// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class GCM
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#ifndef _GCM_H_
#define _GCM_H_

#include "FPGroup.h"
#include "ComputationManager.h"
#include "Supervisor.h"
#include "GIC.h"

// Computation Managers supervised by a GCM:

#include "NormalClosure.h"
#include "AbelianInvariants.h"
#include "KBModule.h"
#include "AGModule.h"
#include "ToddCoxeter.h"
#include "HToddCoxeter.h"
#include "NilpotentQuotients.h"


//---------------------------------------------------------------------------//
//-------------------------------- GCM --------------------------------------//
//---------------------------------------------------------------------------//


class GCM : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  GCM( class SMFPGroup& G );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& getSMFPGroup( ) const { return theSMFPGroup; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void viewStructure(ostream& ostr) const;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Subordinate Computation Managers:                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Subordinate<GCM,NormalClosure> normalClosure;

  Subordinate<GCM,AbelianInvariants> abelianInvariants;

  Subordinate<GCM,AbelianPrimes> abelianPrimes;

  Subordinate<GCM,AbelianRank> abelianRank;

  Subordinate<GCM,KBSupervisor> kbSupervisor;

  Subordinate<GCM,AGSupervisor> agSupervisor;

  Subordinate<GCM,NilpotentQuotients> nilpotentQuotients;

  Subordinate<GCM,NGcomputeBasis> computeBasis;

  Subordinate<GCM,ToddCoxeter> theToddCoxeter;

  Subordinate<GCM,HToddCoxeter> ghToddCoxeter;

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

  void start( );              // overrides ComputationManager

  void terminate( );          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  class SMFPGroup& theSMFPGroup;

  bool didFastChecks;
  // The takeControl method does some sneaky checks of obvious properties
  // (this should be formalized, since they are not always that fast).
  // This records the first call, so that they are not done over and over.

};

#endif

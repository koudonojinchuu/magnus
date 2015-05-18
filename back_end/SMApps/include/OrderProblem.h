// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class OrderProblem
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#ifndef _ORDERPROBLEM_H_
#define _ORDERPROBLEM_H_


#include "Supervisor.h"
#include "CosetEnumerator.h"
#include "ToddCoxeter.h"
#include "HToddCoxeter.h"
#include "SMFPGroup.h"


//---------------------------------------------------------------------------//
//--------------------------- OrderProblem ----------------------------------//
//---------------------------------------------------------------------------//


class OrderProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  OrderProblem(SMFPGroup& G);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void viewStructure(ostream& ostr) const;  // overrides SMObject

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Control:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void takeControl( );         // overrides ComputationManager

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void start( ) { }              // overrides ComputationManager

  void terminate( ) { }          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& theGroup;
  
  // GCM components this uses; only for automatic ARC passing:
/*@db
  MirrorSubordinate abelianInvariants;

  MirrorSubordinate abelianRank;
*/
  MirrorSubordinate kbSupervisor;

  MirrorSubordinate theToddCoxeter;
  MirrorSubordinate agSupervisor;
  MirrorSubordinate computeBasis;
  MirrorSubordinate ghToddCoxeter;
  
};

#endif

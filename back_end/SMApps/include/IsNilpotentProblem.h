// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class IsNilpotentProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _IsNilpotentProblem_h_
#define _IsNilpotentProblem_h_


#include "Supervisor.h"
#include "SMFPGroup.h"
#include "CommutatorsChecker.h"
#include "NilpotentQuotients.h"



// -------------------------- IsNilpotentProblem --------------------------- //


class IsNilpotentProblem : public Supervisor
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  IsNilpotentProblem( class SMFPGroup& G, int nilpClass = 2 );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

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

  void start( ) { }           // overrides ComputationManager

  void terminate( ) { }       // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& theGroup;

  int theClass;

  CommutatorsChecker theChecker;

  CommutatorsCheckerARCer theArcer;


  // GCM components this uses; only for automatic ARC passing:

  MirrorSubordinate normalClosure;

// @am It seems that we can`t use Abelianization here
// MirrorSubordinate abelianInvariants;

  MirrorSubordinate kbSupervisor;

  MirrorSubordinate agSupervisor;

  MirrorSubordinate nilpotentQuotients;

  Subordinate<IsNilpotentProblem,NilpotentWPInQuotients> nilpotentWPInQuotients;

};

#endif

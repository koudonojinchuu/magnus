// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class AreEltsEqual
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _AreEltsEqual_h_
#define _AreEltsEqual_h_


#include "Supervisor.h"
#include "SetOfWordsChecker.h"
#include "NilpotentProblems.h"
#include "GeneticProblems.h"


// --------------------------- AreEltsEqual -------------------------------- //


class AreEltsEqual : public Supervisor
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  AreEltsEqual( const class SMWord&, const class SMWord& );

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

  void start( ) { }            // overrides ComputationManager

  void terminate( ) { }        // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const SMWord& word1;

  const SMWord& word2;

  SetOfWordsChecker theChecker;


  // GCM components this uses; only for automatic ARC passing:

  MirrorSubordinate normalClosure;

  MirrorSubordinate abelianInvariants;

  MirrorSubordinate kbSupervisor;

  MirrorSubordinate agSupervisor;

  MirrorSubordinate nilpotentQuotients;

  Subordinate<AreEltsEqual,NilpotentWPInQuotients> nilpotentWPInQuotients;

  MirrorSubordinate computeBasis;

  Subordinate<AreEltsEqual,NilpotentWP> nilpotentWP;

  Subordinate<AreEltsEqual, GeneticWPCM> genetic;
};

#endif

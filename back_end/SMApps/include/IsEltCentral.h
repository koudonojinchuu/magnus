// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class IsEltCentral
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _IsEltCentral_h_
#define _IsEltCentral_h_


#include "Supervisor.h"
#include "SetOfWordsChecker.h"
#include "NilpotentQuotients.h"
#include "GeneticProblems.h"


// --------------------------- IsEltCentral -------------------------------- //


class IsEltCentral : public Supervisor
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  IsEltCentral( const class SMWord& );

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

  const SMWord& theWord;

  SetOfWordsChecker theChecker;


  // GCM components this uses; only for automatic ARC passing:

  MirrorSubordinate normalClosure;

  MirrorSubordinate abelianInvariants;

  MirrorSubordinate kbSupervisor;

  MirrorSubordinate agSupervisor;

  MirrorSubordinate nilpotentQuotients;

  Subordinate<IsEltCentral,NilpotentWPInQuotients> nilpotentWPInQuotients;

  MirrorSubordinate computeBasis;

  Subordinate<IsEltCentral,NilpotentWP> nilpotentWP;

  Subordinate<IsEltCentral, GeneticWPCM> genetic;
};

#endif

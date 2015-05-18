// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of classes IsTrivialChecker, IsTrivialProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _IS_TRIVIAL_PROBLEM_H_
#define _IS_TRIVIAL_PROBLEM_H_


#include "Supervisor.h"
#include "FEData.h"
#include "Word.h"
#include "FPGroup.h"
#include "Chars.h"
#include "GroupFastChecks.h"
#include "NilpotentQuotients.h"
#include "GeneticProblems.h"


// ------------------------ IsTrivialChecker ------------------------------- //


// This separates out the GIC-sniffing smarts needed to check a word for
// triviality.

class IsTrivialChecker
{
public:

  IsTrivialChecker( class SMFPGroup& );

  Trichotomy isTrivial( );

  Chars getExplanation( ) { return explanation; }

  Chars getDetailsFileName( ) const;

  bool haveDetails( ) const;

private:

  class SMFPGroup& theGroup;

  FPGroup G;

  GroupFastChecks checker;
    
  class GIC& gic;

  class GCM& gcm;


  // Housekeeping members:

  bool triedAbelianization;

  bool triedPreliminaryCheckings;

  Chars explanation;

  DetailedReport trivialDetails; 

  // Private functions:

  Trichotomy preliminaryCheckings();
};


// ----------------------- IsTrivialProblem -------------------------------- //


class IsTrivialProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  IsTrivialProblem( class SMFPGroup& );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy answer( ) const { return theAnswer; }
  // When a WordProblem is used internally, it does not report the result
  // to the FE, but just sits and waits to be polled through this member.
  // yes => trivial, no => not trivial, dontknow => not done yet.

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

  void start( );              // overrides ComputationManager

  void terminate( );          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const SMFPGroup& theGroup;

  IsTrivialChecker theChecker;


  // Housekeeping members:

  Trichotomy theAnswer;

  Chars explanation;


  // GCM components this uses; only for automatic ARC passing:

  MirrorSubordinate normalClosure;

  MirrorSubordinate abelianInvariants;

  MirrorSubordinate kbSupervisor;

  MirrorSubordinate agSupervisor;

  MirrorSubordinate nilpotentQuotients;

  Subordinate<IsTrivialProblem,NilpotentWPInQuotients> nilpotentWPInQuotients;

  Subordinate<IsTrivialProblem, GeneticWPCM> genetic;
};


//
//  Class IsTrivialChecker -- inline functions
//

inline
Chars IsTrivialChecker::getDetailsFileName( ) const 
{ 
  return trivialDetails.getFileName(); 
}

inline
bool IsTrivialChecker::haveDetails( ) const
{
  trivialDetails.haveDetails( );
}

#endif

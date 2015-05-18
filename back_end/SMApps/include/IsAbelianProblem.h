// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of classes IsAbelianChecker, IsAbelianProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _IsAbelianProblem_h_
#define _IsAbelianProblem_h_


#include "Supervisor.h"
#include "SMFPGroup.h"
#include "NilpotentQuotients.h"
#include "GeneticProblems.h"


// ------------------------- IsAbelianChecker ------------------------------ //


class IsAbelianChecker
{
public:

  IsAbelianChecker( class SMFPGroup& );

  Trichotomy isAbelian( );

  Chars getExplanation( ) { return explanation; }

  Chars commutator( VectorOf<int> components );

  bool haveDetails ( ) const;

  Chars getDetailsFileName( ) const;

private:

  // Data members

  FPGroup G;

  class GIC& gic;

  class GCM& gcm;


  // Housekeeping members:

  bool triedAbelianization;

  bool triedOneRelator;

  Chars explanation;

  DetailedReport abelianDetails;
};


// --------------------------- IsAbelianProblem ---------------------------- //


class IsAbelianProblem : public Supervisor
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  IsAbelianProblem( class SMFPGroup& );


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

  void start( );              // overrides ComputationManager

  void terminate( );          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& theGroup;

  IsAbelianChecker theChecker;


  // GCM components this uses; only for automatic ARC passing:

  MirrorSubordinate normalClosure;

  MirrorSubordinate abelianInvariants;

  MirrorSubordinate kbSupervisor;

  MirrorSubordinate agSupervisor;

  MirrorSubordinate computeBasis;

  Subordinate<IsAbelianProblem,NilpotentWPInQuotients> nilpotentWPInQuotients;

  MirrorSubordinate nilpotentQuotients;

  Subordinate<IsAbelianProblem,NilpotentWP> nilpotentWP;

  Subordinate<IsAbelianProblem, GeneticWPCM> genetic;
};



//
//  Class IsAbelianChecker -- inline functions
//

inline
bool IsAbelianChecker::haveDetails ( ) const
{
  return abelianDetails.haveDetails();
}

inline
Chars IsAbelianChecker::getDetailsFileName( ) const
{ 
  return abelianDetails.getFileName(); 
}

#endif

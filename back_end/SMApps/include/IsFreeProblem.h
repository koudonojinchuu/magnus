// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class IsFreeProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _IS_FREE_PROBLEM_H_
#define _IS_FREE_PROBLEM_H_


#include "SMFPGroup.h"
#include "ORProblems.h"
#include "ARCer.h"


// ----------------------- ORIsFreeProblemARCer ---------------------------- //


class ORIsFreeProblemARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ORIsFreeProblemARCer( ComputationManager& boss ) : ARCer( boss ) { } 


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments(ORProblems* );

  bool getRetValue() { return retValue; }

  // Inherited from ARCer:
  // bool takeControl( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
 
  void writeResults( ostream& );

  void readResults( istream& );

  
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ORProblems* theORProblems;
  
  bool retValue;
};


// ------------------------- ORIsFreeProblem ------------------------------- //


class ORIsFreeProblem : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ORIsFreeProblem( class SMFPGroup& );

  ~ORIsFreeProblem( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void viewStructure( ostream& ) const;  // overrides SMObject

  
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

  void start( ) { };          // overrides ComputationManager

  void terminate( ) { };      // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const SMFPGroup& theGroup;

  ORProblems* theORProblems;

  ORIsFreeProblemARCer arcer;
};


// --------------------------- IsFreeChecker ------------------------------- //


class IsFreeChecker
{

public:

  IsFreeChecker( class SMFPGroup& );

  Trichotomy isFree( );

  Chars getExplanation( ) { return explanation; }


private:

  class SMFPGroup& theGroup;

  FPGroup G;

  class GIC& gic;

  class GCM& gcm;


  // Housekeeping members:

  bool triedAbelianization;

  Chars explanation;
};


// -------------------------- IsFreeProblem -------------------------------- //


class IsFreeProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  IsFreeProblem( class SMFPGroup& );


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

  IsFreeChecker theChecker;

  // GCM components this uses; only for automatic ARC passing:

  MirrorSubordinate abelianInvariants;
};

#endif

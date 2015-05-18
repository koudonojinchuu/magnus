// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of classes ExtendToHomChecker, ExtendToHomProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _EXTEND_TO_HOM_PROBLEM_H_
#define _EXTEND_TO_HOM_PROBLEM_H_


#include "Supervisor.h"
#include "FPGroup.h"
#include "SMMap.h"
#include "CommutatorIterator.h"
#include "NilpotentProblems.h"
#include "GeneticProblems.h"

class ExtendToHomChecker;

/*
// ------------------ CheckRelatorsInNilpotentARCer ------------------------ //


class CheckRelatorsInNilpotentARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  CheckRelatorsInNilpotentARCer( ComputationManager& boss ) 
    : ARCer( boss ) { } 


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( ExtendToHomChecker* );

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

  ExtendToHomChecker* theExtendToHomChecker;
  
  bool retValue;
};
*/

// ------------------------ ExtendToHomChecker ----------------------------- //


class ExtendToHomChecker
{
public:

  ExtendToHomChecker(class ExtendToHomProblem& boss, class SMMap& );

  Trichotomy doesExtendToHom( );

  Chars getExplanation( ) const { return explanation; } 

  Trichotomy preliminaryCheckings();

  bool checkRelatorsInNilpotent() const;

private:
  class ExtendToHomProblem& theBoss;
  class SMMap& theMap;
  const Map M;
  
  class SMFPGroup& theDomain;
  class SMFPGroup& theRange;

  FPGroup G1;
  FPGroup G2;
  SetOf<Word> relators;
  
  class GIC& gic1;
  class GIC& gic2;
  class GCM& gcm1;
  class GCM& gcm2;

  int theClass;
  bool triedAbelianization;
//  bool triedPreliminaryCheckings;

  Chars explanation;

  // Private functions:

  SetOf<Word> getAllRelators( class SMFPGroup& ) const;
};


// ----------------------- ExtendToHomProblem ------------------------------ //


class ExtendToHomProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ExtendToHomProblem( class SMMap& );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy answer( ) const { return theAnswer; }
  Trichotomy nilpWPAnswer( ) ;
  Trichotomy nilpWPInQuotAnswer( ) ;
  

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
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void sendResultMessage( Chars );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMMap& theMap;
  const SMFPGroup& theDomain;
  const SMFPGroup& theRange;

  int theClass;

  ExtendToHomChecker theChecker;

  // Housekeeping members:

  Trichotomy theAnswer;
  Chars explanation;


  // GCM components this uses; only for automatic ARC passing:

  MirrorSubordinate normalClosure;
  MirrorSubordinate abelianInvariants;
  MirrorSubordinate kbSupervisor;
  MirrorSubordinate agSupervisor;
  MirrorSubordinate nilpotentQuotients;
  Subordinate<ExtendToHomProblem,NilpotentWPInQuotients> nilpotentWPInQuotients;
  MirrorSubordinate computeBasis;
  Subordinate<ExtendToHomProblem,NilpotentWP> nilpotentWP;
  Subordinate<ExtendToHomProblem, GeneticWPCM> genetic;
};

#endif


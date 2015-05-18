// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declarations of classes 
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _QuadEquationSolver_h_
#define _QuadEquationSolver_h_


#include "Supervisor.h"
#include "SMEquation.h"
#include "File.h"
#include "LogWatcher.h"
#include "fastProblems.h"

class EquationSolverARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  EquationSolverARCer( ComputationManager& boss ) : ARCer( boss ) { } 


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( class SMEquation& eq, 
		     const Chars& aBasicFileName, const Chars& aStabFileName,
		     bool workOnBasicSolutions, bool workOnStabilizer);


  int numberOfBasicSolutions( ) const;

  int numberOfRegStabGenerators( ) const;

  // Inherited from ARCer:
  // bool takeControl( );

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
  // Overrides ARCer::runComputation().

  void writeResults( ostream& ostr );
  void readResults( istream& istr );

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // class QEqnSolutionsInFreeGroup *solver;
  FreeGroup F;
  Word word;
  int numberOfVariables;
  bool workOnBasicSolutions;
  bool workOnStabilizer;

  Chars basicFileName;
  Chars stabFileName;
  int numOfBasicSolutions;
  int numOfRegStabGenerators;
};


//--------------------------- EquationSolver ------------------------//


class EquationSolver : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  EquationSolver( class EquationProblem& equationProblem );

  EquationSolver( class QuickEquationProblem& equationProblem );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  //  SMEquation& equation( ) { return theEquation; }

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

  void terminate( );          // overrides ComputationManager

  
private:

  void updateBasicSolutions( );
  void updateRegStabGenerators( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMEquation& equation;

  Chars basicFileName;
  Chars stabFileName;

  LogFileWatcher watchBasicSolutions;
  LogFileWatcher watchRegStabGenerators;

  bool workOnBasicSolutions;
  bool workOnStabilizer;

  ListOf<Endomorphism> foundBasicSolutions;
  ListOf<Automorphism> foundRegStabGenerators;

  EquationSolverARCer arcer;
};


// ------------------------------ ------------------------------

class EquationRandomSolutionARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  EquationRandomSolutionARCer( ComputationManager& boss ) : ARCer( boss ), equation(0) { } 


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( class SMEquation& eq, const Chars& aRandomFileName,
		     const Chars& aBasicFileName, const Chars& aStabFileName );

  // Inherited from ARCer:
  // bool takeControl( );

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
  // Overrides ARCer::runComputation().

  void writeResults( ostream& ostr ) { ostr < ' '; }
  void readResults( istream& istr ) { }

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  class SMEquation *equation;
  Chars randomFileName; // output file.
  Chars basicFileName;  // input file from another process.
  Chars stabFileName;   // input file from another process.
};


//--------------------------- EquationRandomSolutions ------------------------//


class EquationRandomSolutions : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  EquationRandomSolutions( class EquationProblem& ep );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // SMEquation& equation( ) { return theEquation; }


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

  EquationRandomSolutionARCer arcer;
};






class EquationProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  EquationProblem( class SMEquation& w );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMEquation& getEquation() const { return equation; }

  Chars getBasicSolutionsFileName( ) const { return basicSolutionsFileName; }
  
  Chars getRegStabGeneratorsFileName( ) const { return regStabGeneratorsFileName; }

  Chars getRandomSolutionsFileName( ) const { return randomSolutionsFileName; }

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

  void start( ) { };          // overrides ComputationManager

  void terminate( ) { };      // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMEquation& equation;
 
  bool linksSent;
  Chars basicSolutionsFileName;
  Chars regStabGeneratorsFileName;
  Chars randomSolutionsFileName;
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Subordinate Computation Managers:                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Subordinate<EquationProblem, EquationSolver> basicSubordinate;
  Subordinate<EquationProblem, EquationRandomSolutions> randomSubordinate;

};



class QuickEquationProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  QuickEquationProblem( class SMEquation& w );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMEquation& getEquation() const { return equation; }

  Chars getBasicSolutionsFileName( ) const { return basicSolutionsFileName; }
  
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

  void start( ) { };          // overrides ComputationManager

  void terminate( ) { };      // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMEquation& equation;
 
  bool linkSent;
  Chars basicSolutionsFileName;
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Subordinate Computation Managers:                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Subordinate<QuickEquationProblem, EquationSolver> basicSubordinate;

};



///////////////////// fast problems /////////////////////////////

class QEquationInFreeBasicSolutions : public FastComputation
{
public:

  QEquationInFreeBasicSolutions(const class SMEquation& e) : equation( e ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMEquation& equation;
};

class QEquationInFreeRegStabGenerators : public FastComputation
{
public:

  QEquationInFreeRegStabGenerators(const class SMEquation& e) : equation( e ) { }

  void takeControl( );              // overrides ComputationManager

private:

  const class SMEquation& equation;
};



#endif

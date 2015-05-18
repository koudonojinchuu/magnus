// Contents: Declaration of classes GAConjugacyForORGroup, GAConjugacyForORGroupProblem, GAConjugacyForORGroupARCer
//
// Principal Author: Alexander Ushakov
//
// Status: in progress
//
// Revision History:
//

#ifndef _GA_CONJ_PROBLEM_FOR_ORGROUP_H_
#define _GA_CONJ_PROBLEM_FOR_ORGROUP_H_


#include "Supervisor.h"
#include "SMWord.h"
#include "FreeGroup.h"
#include "File.h"
#include "GACPforORGSolver.h"


class ConjugacyProblem;


//---------------------------------------------------------------------------//
//---------------------- GAConjugacyForORGroupARCer -------------------------//
//---------------------------------------------------------------------------//


class GAConjugacyForORGroupARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  GAConjugacyForORGroupARCer( class GAConjugacyForORGroup& );

  ~GAConjugacyForORGroupARCer( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const OneRelatorGroup& G, const Word& u, const Word& v );

  Trichotomy answer( ) const { return theAnswer; }

  Chars getFileName( ) const { return solver->getFileName( ); }


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

  OneRelatorGroup theGroup;
  Word firstWord, secondWord;

  GAConjProblemForORGroupSolver* solver;

  Trichotomy theAnswer;
};



//---------------------------------------------------------------------------//
//------------------------ GAConjugacyForORGroup ----------------------------//
//---------------------------------------------------------------------------//



class GAConjugacyForORGroup : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  GAConjugacyForORGroup( const ConjugacyProblem& CP);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy answer( ) const { return theAnswer; }
  // This not report the result to the FE, but just sits and waits to
  // be polled through this member. yes => conjugate, no => not conjugate,
  // dontknow => not done yet.

  Chars getFileName( ) const { return arcer.getFileName( ); }

  OneRelatorGroup getGroup( ) const;

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

  const class ConjugacyProblem& problem;
  GAConjugacyForORGroupARCer arcer;
  Trichotomy theAnswer;
};


#endif


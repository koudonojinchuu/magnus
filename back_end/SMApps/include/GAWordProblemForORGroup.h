// Contents: Declaration of classes GAWordForORGroup, GAWordForORGroupARCer
//
// Principal Author: Alexander Ushakov
//
// Status: in progress
//
// Revision History:
//

#ifndef _GA_WORD_PROBLEM_FOR_ORGROUP_H_
#define _GA_WORD_PROBLEM_FOR_ORGROUP_H_


#include "Supervisor.h"
#include "SMWord.h"
#include "FreeGroup.h"
#include "File.h"
#include "GACPforORGSolver.h"


//---------------------------------------------------------------------------//
//----------------------- GAWordForORGroupARCer -----------------------------//
//---------------------------------------------------------------------------//


class GAWordForORGroupARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  GAWordForORGroupARCer( class GAWordForORGroup& );

  ~GAWordForORGroupARCer( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const OneRelatorGroup& G, const Word& u );

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
  Word theWord;

  GAConjProblemForORGroupSolver* solver;

  Trichotomy theAnswer;
};



//---------------------------------------------------------------------------//
//------------------------- GAWordForORGroup --------------------------------//
//---------------------------------------------------------------------------//



class GAWordForORGroup : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  GAWordForORGroup(class WordProblem& CP);

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

  const WordProblem& problem;
  GAWordForORGroupARCer arcer;
  Trichotomy theAnswer;
};


#endif


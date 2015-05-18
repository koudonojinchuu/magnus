// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class ConjugacyProblem, helpers FPConjugacyWrapper,
//           MSCConjugacyWrapper
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History: 
//
// @db & @au: 00/10/07 Added GA for solving conjugacy problem for OR groups  
//

#ifndef _CONJUGACYPROBLEM_H_
#define _CONJUGACYPROBLEM_H_


#include "Word.h"
#include "FPGroup.h"
#include "Supervisor.h"
#include "GAConjProblemForORGroup.h"


//---------------------------------------------------------------------------//
//------------------------ FPConjugacyWrapper -------------------------------//
//---------------------------------------------------------------------------//

class FPConjugacyARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPConjugacyARCer( ComputationManager& boss );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( 
		    const class SMFPGroup& G, 
		    const class Word& u, 
		    const class Word& w 
		    );

  Trichotomy answer( ) const { return theAnswer; }

  Word conjugator( ) const { return theConjugator; } 
  // Returns a valid result when answer() returns yes.

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

  const SMFPGroup *theGroup;
  Word firstWord, secondWord;

  Trichotomy theAnswer;
  Word theConjugator;
};

class FPConjugacyWrapper : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPConjugacyWrapper(class ConjugacyProblem& CP);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy answer( ) const { return theAnswer; }
  // This not report the result to the FE, but just sits and waits to
  // be polled through this member. yes => conjugate, no => not conjugate,
  // dontknow => not done yet.

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
  FPConjugacyARCer arcer;
  Trichotomy theAnswer;
};



//---------------------------------------------------------------------------//
//------------------------ MSCConjugacyWrapper ------------------------------//
//---------------------------------------------------------------------------//


class MSCConjugacyARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  MSCConjugacyARCer( ComputationManager& );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const FPGroup& G, const Word& u, const Word& v );

  Trichotomy answer( ) const { return theAnswer; }

  Word conjugator( ) const { return theConjugator; } 
  // Returns a valid result when answer() returns yes.

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

  FPGroup theGroup;
  Word firstWord, secondWord;

  Trichotomy theAnswer;
  Word theConjugator;

};


class MSCConjugacyWrapper : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  MSCConjugacyWrapper(class ConjugacyProblem& CP);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy answer( ) const { return theAnswer; }
  // This not report the result to the FE, but just sits and waits to
  // be polled through this member. yes => conjugate, no => not conjugate,
  // dontknow => not done yet.

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
  MSCConjugacyARCer arcer;
  Trichotomy theAnswer;
};



//---------------------------------------------------------------------------//
//------------------------- ConjugacyProblem --------------------------------//
//---------------------------------------------------------------------------//


class ConjugacyProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ConjugacyProblem(const class SMWord& w1, const class SMWord& w2);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMWord& firstWord( ) const { return u; }
  const class SMWord& secondWord( ) const { return v; }

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

  void start( ) { }              // overrides ComputationManager

  void terminate( ) { }          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMWord& u;
  const class SMWord& v;

  bool scMethodFailed;

  bool geneticLinkHasBeenSent;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Subordinate Computation Managers:                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Subordinate<ConjugacyProblem,FPConjugacyWrapper> fpConjugacyWrapper;
  Subordinate<ConjugacyProblem,MSCConjugacyWrapper> mscConjugacyWrapper;
  Subordinate<ConjugacyProblem,GAConjugacyForORGroup> GAConjugacy;

};

#endif

// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declarations of classes 
//           FreeIsPartOfBasisProblem, FreeIsPartOfBasis
//           FreeGeneralIsPartOfBasisProblem, FreeGeneralIsPartOfBasis
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _FREE_IS_PART_OF_BASIS_PROBLEM_H_
#define _FREE_IS_PART_OF_BASIS_PROBLEM_H_


#include "Supervisor.h"
#include "GeneralWhitehead.h"
#include "GAIsPartOfBasis.h"
#include "SMSetOfWords.h"
#include "SMWord.h"

// --------------------------- GAIsPartOfBasisArcer ------------------------ //


class GAIsPartOfBasisArcer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  GAIsPartOfBasisArcer( ComputationManager& );

  ~GAIsPartOfBasisArcer( ) { }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( FreeGroup f,Word w );

  Chars getComputationFileName() const { return compFile.getFileName();}
  Chars getResultFileName() const { return resultFile.getFileName();}
  const VectorOf<Word>& getAutomorphism() const { return theAuto;}
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
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  Word theWord;
  FreeGroup theGroup;
  File compFile;
  File resultFile;
  VectorOf<Word> theAuto;
};



// ---------------------------- GAIsPartOfBasisCM ------------------------- //


class GAIsPartOfBasisCM : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  GAIsPartOfBasisCM( class FreeIsPartOfBasisProblem&  );

  //  ~GeneticWPCM( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
   
  Trichotomy getAnswer() const { return answer;}
  
  Chars getComputationFileName() const { return arcer.getComputationFileName();}
  
  Chars getResultFileName() const { return arcer.getResultFileName();}

  const VectorOf<Word>& getAutomorphism() const { return arcer.getAutomorphism();}

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

  GAIsPartOfBasisArcer arcer;
  Trichotomy answer;
  const class SMWord& theWord;
  bool linkHasBeenSent;
};

// ------------------------ FreeIsPartOfBasis ------------------------------ //


class FreeIsPartOfBasis : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FreeIsPartOfBasis(class FreeIsPartOfBasisProblem& problemObject );

  ~FreeIsPartOfBasis( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy answer( ) const { return theAnswer; }
  // This not report the result to the FE, but just sits and waits to
  // be polled through this member. yes => conjugate, no => not conjugate,
  // dontknow => not done yet.

  Map getAutomorphism( ) const;
  // Returns a valid result when answer() returns yes, otherwise
  // it is an error.

  Chars getFileName( ) { return theFileName; }
  // Returns name of the file with computation details


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

  void start( ) { };              // overrides ComputationManager

  void terminate( ) { };          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool linkHasBeenSent;
  
  const class SMWord& theWord;

  Trichotomy theAnswer;
  
  FreeGroup theGroup;
  
  Map theAutomorphism;

  GeneralWhitehead* GW;

  Chars theFileName; 
  // Keeps name of the file containing details of the computation.

};


// -------------------- FreeIsPartOfBasisProblem --------------------------- //


class FreeIsPartOfBasisProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FreeIsPartOfBasisProblem(const class SMWord& w);


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMWord& getWord( ) const { return theWord; }


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

  const SMWord& theWord;
 
  bool link1HasBeenSent;
  bool link2HasBeenSent;
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Subordinate Computation Managers:                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Subordinate<FreeIsPartOfBasisProblem, FreeIsPartOfBasis> freeIsPartOfBasis;
  Subordinate<FreeIsPartOfBasisProblem, GAIsPartOfBasisCM> gaIsPartOfBasisCM;

};


// --------------------- FreeGeneralIsPartOfBasis -------------------------- //


class FreeGeneralIsPartOfBasis : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FreeGeneralIsPartOfBasis( class FreeGeneralIsPartOfBasisProblem&
			    problemObject );

  ~FreeGeneralIsPartOfBasis( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy answer( ) const { return theAnswer; }
  // This not report the result to the FE, but just sits and waits to
  // be polled through this member. yes => conjugate, no => not conjugate,
  // dontknow => not done yet.

  Map getAutomorphism( ) const;
  // Returns a valid result when answer() returns yes, otherwise
  // it is an error.

  Chars getFileName( ) { return theFileName; }
  // Returns name of the file with computation details


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

  void start( ) { };           // overrides ComputationManager

  void terminate( ) { };       // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMSetOfWords& theSet;

  Trichotomy theAnswer;
  
  FreeGroup theGroup;
  
  Map theAutomorphism;

  GeneralWhitehead* GW;

  Chars theFileName; 
  // Keeps name of the file containing details of the computation.

};


// ------------------- FreeGeneralIsPartOfBasisProblem --------------------- //


class FreeGeneralIsPartOfBasisProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FreeGeneralIsPartOfBasisProblem( const class SMSetOfWords& );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMSetOfWords& getSetOfWords( ) const { return theSet; }


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

  const SMSetOfWords& theSet;
  
  bool linkHasBeenSent;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Subordinate Computation Managers:                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Subordinate<FreeGeneralIsPartOfBasisProblem, FreeGeneralIsPartOfBasis>
    freeIsPartOfBasis;
};

#endif

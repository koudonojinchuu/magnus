// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of classes ORWordProblemARCer, ORWordProblemCM,
//           WordProblem
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _WordProblem_h_
#define _WordProblem_h_


#include "Supervisor.h"
#include "SMWord.h"
#include "SetOfWordsChecker.h"
#include "NilpotentQuotients.h"
#include "ARCer.h"
#include "GeneticProblems.h"
#include "FNWP.h"
#include "GAWordProblemForORGroup.h"


// ------------------------ ORWordProblemARCer ----------------------------- //


class ORWordProblemARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ORWordProblemARCer( ComputationManager& );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const Word& relator, const Word& testWord );

  bool getRetValue() { return retValue; }

  ProductOfRelatorConjugates wordDecomposition() const { return wDeco; }

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

  void printWord(const Word& w, ostream& ostr);


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Word theRelator;
  Word theTestWord;

  Trichotomy answer;
  bool retValue;
  ProductOfRelatorConjugates wDeco;
};



// -------------------------- ORWordProblemCM ------------------------------ //


class ORWordProblemCM : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ORWordProblemCM( class WordProblem& PO );

  //  ~ORWordProblemCM( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy isTrivial( ) { return itIsTrivial; }

  ProductOfRelatorConjugates wordDecomposition() const { return wDeco; }

    
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

  Word theRelator;
  const SMWord& testWord;
  
  Trichotomy itIsTrivial;
  bool bStarted;
  ProductOfRelatorConjugates wDeco;

  ORWordProblemARCer arcer;
};


// ----------------------------- WordProblem ------------------------------- //


class WordProblem : public Supervisor
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  WordProblem( class SMWord& w);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy answer( ) const { return theAnswer; }
  // When a WordProblem is used internally, it does not report the result
  // to the FE, but just sits and waits to be polled through this member.
  // yes => trivial, no => not trivial, dontknow => not done yet.

  const SMWord& getTestWord( ) const { return theWord; }

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

  void start( ) { }           // overrides ComputationManager

  void terminate( ) { }       // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMWord& theWord;

  SetOfWordsChecker theChecker;


  // Housekeeping members:

  Trichotomy theAnswer;

  Chars explanation;

  bool triedAbelianization;
  

  // GCM components this uses; only for automatic ARC passing:

  MirrorSubordinate normalClosure;

  MirrorSubordinate abelianInvariants;

  MirrorSubordinate kbSupervisor;

  MirrorSubordinate agSupervisor;

  MirrorSubordinate computeBasis;

  MirrorSubordinate nilpotentQuotients;

  Subordinate<WordProblem,NilpotentWPInQuotients> nilpotentWPInQuotients;

  Subordinate<WordProblem, ORWordProblemCM> orwp;

  Subordinate<WordProblem, NilpotentWP> nilpotentWP;

  Subordinate<WordProblem, GeneticWPCM> genetic;

  bool linkGenORHasBeenSent;
  Subordinate<WordProblem, GAWordForORGroup> geneticForORG;

  Subordinate<WordProblem, FNWPCM> fnwp;
};

#endif



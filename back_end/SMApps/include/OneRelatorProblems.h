// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declarations of classes
//
// Author: 
//
// Status: in progress
//
// Revision History:
//

#ifndef __ONE_RELATOR_PROBLEMS__
#define __ONE_RELATOR_PROBLEMS__

#include "fastProblems.h"

class ORFindHNNPresentation : public FastComputation
{
public:

  ORFindHNNPresentation(const SMFPGroup& G) 
    : theGroup( G ), numOfGens(0), stableGen(1), accompGen(1) { }

  ORFindHNNPresentation(const SMFPGroup& G, const Generator& stable) 
    : theGroup( G ), numOfGens(1), stableGen(stable), accompGen(1) { }

  ORFindHNNPresentation(const SMFPGroup& G, const Generator& stable, 
			const Generator& accomp) 
    : theGroup( G ), numOfGens(2), stableGen(stable), accompGen(accomp) { }

  void takeControl( );              // overrides ComputationManager

private:

  const SMFPGroup& theGroup;
  int numOfGens;
  Generator stableGen;
  Generator accompGen;
};

class ORGroup_IsMagnusSubgroup : public FastComputation 
{
public:

  ORGroup_IsMagnusSubgroup( const SMSubgroup& S )
    : subgroup( S )
  { }

  void takeControl( );

private:

  const SMSubgroup& subgroup;
};


//================================================================



class ORExtendedWordProblemARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ORExtendedWordProblemARCer( ComputationManager& );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const Word& relator, const Word& testWord, 
		     const VectorOf<Word>& target );

  bool getRetValue() const { return retValue; }

  Word wordInBasis( ) const { return theWordInBasis; } 

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
  VectorOf<Word> theTarget;

  Trichotomy answer;
  Word theWordInBasis;
  bool retValue;
};



// -------------------------- ORWordProblemCM ------------------------------ //


class ORExtendedWordProblemCM : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ORExtendedWordProblemCM( class ORExtendedWordProblem& PO );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy isExpressed( ) const { return itIsExpressed; }

  Word wordInBasis( ) const { return theWordInBasis; }
    
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

  void terminate( );          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const Word theRelator;
  const Word theTestWord;
  const VectorOf<Word> theTarget;
  
  Trichotomy itIsExpressed;
  Word theWordInBasis;
  bool bStarted;
  
  ORExtendedWordProblemARCer arcer;
};


// --------------------------- ORExtendedWordProblem ---------------------- //


class ORExtendedWordProblem : public Supervisor
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ORExtendedWordProblem( class SMSubgroup& target, class SMWord& w );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy answer( ) const { return theAnswer; }
  // When a WordProblem is used internally, it does not report the result
  // to the FE, but just sits and waits to be polled through this member.
  // yes => trivial, no => not trivial, dontknow => not done yet.

  Word wordInBasis( ) const { return theWordInBasis; }

  const SMWord& getTestWord( ) const { return theWord; }

  const SMSubgroup& getTarget( ) const { return theTarget; }

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

  SMSubgroup& theTarget;

  // Housekeeping members:

  Trichotomy theAnswer;
  Word theWordInBasis;

  // GCM components this uses; only for automatic ARC passing:

  Subordinate<ORExtendedWordProblem, ORExtendedWordProblemCM> orwp;
};


// --------------------- ORIsMagnusSubgroup --------------------------------- //

class ORIsMagnusSubgroup : public FastComputation 
{
public:

  ORIsMagnusSubgroup( const SMSubgroup& subgroup )
    : theSubgroup( subgroup )
  { }

  void takeControl( );

private:

  const SMSubgroup& theSubgroup;
};


/*
// --------------------- ORConsequencesEnumerator ------------------------ //

class ORConsequencesEnumeratorARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ORConsequencesEnumeratorARCer( ComputationManager& );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const VectorOf<Chars>& namesOfGenerators, 
		     const Word& relator, const Chars& fileName );

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

  VectorOf<Chars> theNamesOfGenerators;
  Word theRelator;
  Chars theFileName;
};



// -------------------------- ORWordProblemCM ------------------------------ //


class ORConsequencesEnumeratorCM : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ORConsequencesEnumeratorCM( class ORConsequencesEnumerator& CE );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

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

  void terminate( );          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMFPGroup& theGroup;
  Chars theFileName;

  bool arcerStarted;
  ORConsequencesEnumeratorARCer arcer;
};


// --------------------------- ORConsequencesEnumerator ---------------------- //


class ORConsequencesEnumerator : public Supervisor
{

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ORConsequencesEnumerator( class SMFPGroup& group );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const SMFPGroup& getGroup( ) const { return theGroup; }

  Chars getFileName( ) const { return theFileName; }

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

  const SMFPGroup& theGroup;
  Chars theFileName;
  bool linked;

  // GCM components this uses; only for automatic ARC passing:

  Subordinate<ORConsequencesEnumerator, ORConsequencesEnumeratorCM> enumerator;
};
*/

#endif

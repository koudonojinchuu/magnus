// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declarations of classes NilpotentWPARCer, NilpotentWPInQuotients,
//            NilpotentWP, NGcomputeBasisARCer, NGcomputeBasis,
//            NilpotentQuotients, NGcomputeBasisProblem, NGPresentationARCer,
//            NGdecomposeWordARCer, NGdecomposeWord 

//
// Principal Author: Roger Needham, Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//


#ifndef _NILPOTENTQUOTIENTS_H_
#define _NILPOTENTQUOTIENTS_H_


//@rn  This is specialized for the moment to work only on the specific class of
//@rn  a group which was checked in as nilpotent. It can, in principle, work
//@rn  on any nilpotent quotient of any FP group.


#include "Supervisor.h"
#include "NilpotentGroup.h"
#include "ComputationManager.h"
#include "List.h"
#include "File.h"
#include "Presentation.h"

// ---------------------- NilpotentWPARCer --------------------------- //


class NilpotentWPARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NilpotentWPARCer( ComputationManager& boss ) 
    : ARCer( boss ),theGroup(0) { } 

  ~NilpotentWPARCer( ) { delete theGroup;} 


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments(const SetOf<Word>& setW, const NilpotentGroup& NG);
  void setArguments(const VectorOf<Word>& vecW, const NilpotentGroup& NG);
  void setArguments(const int& length, const NilpotentGroup& NG);
  void setArguments(const int& length, const NilpotentGroup& NG,
		    const VectorOf<Word>& gen);
 // Inherited from ARCer:
  // bool takeControl( );

  bool getResult() const;
  Word getWord() const;
  VectorOf<int> getCommutator() const;
  
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
 
  void writeResults( ostream& );

  void readResults( istream& );


private:

  void commutatorsCheck();
  // Enumerates all commutators of length = theCommutatorsLength,
  // and checks them for triviality.
  void wordsCheck();
  // Checks theWords for triviality.
  void setGroup(const NilpotentGroup& NG);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  VectorOf<Word> theWords;
  int theCommutatorsLength;

  VectorOf<Word> generators;
  // Vector of generators, to commutate.
  NilpotentGroup* theGroup;
  bool result;
  bool isCommutatorsChecks;
  Word retWord;
  VectorOf<int> commutator;
};

//---------------------------------------------------------------------------//
//---------------------------- NilpotentWPInQuotients------------------------//
//---------------------------------------------------------------------------//


class NilpotentWPInQuotients : public ComputationManager
{
public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NilpotentWPInQuotients(Supervisor& sup);
  // Constructor gets abstract supervisor, so you can call this
  // computation manager from any supervisor.

  void initialize(const VectorOf<Word>& vecW,const class SMFPGroup* group,
		  int startClass = 2);
  void initialize(const SetOf<Word>& w,const class SMFPGroup* group,
		  int startClass = 2);
  // Initializing for checking of words set.
  // You have to call initialize, before running start, because we
  // need to initialize words and group for running problem.
  
  void initialize(const int& length,const class SMFPGroup* group,
		  int startClass = 2);
  // Initializing for checking  commutators.
  // You have to call initialize, before running start, becouse we
  // need to initialize words and group for running problem.

  void initialize(const int& length,const class SMFPGroup* group,
		  const VectorOf<Word>& gen,int startClass = 2);
  // Initializing for checking  commutators of generators in 'gen'.
  // You have to call initialize, before running start, becouse we
  // need to initialize words and group for running problem.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy isTrivial() const { return is_trivial; }
  // Returns no if there are non trivial words in set.
  Trichotomy isTrivial( int& nClass ) const;
  // Returns no if there are non trivial words in set and nClass =
  // class of nilpotent quotient, in which word is not trivial.

  Word getWord() const;
  // Returns non trivial word if it was found, and empty if was not.
  VectorOf<int> getCommutator() const;
  // Returns vector of generators in commutator if isCommutatorsCheck==true
  // If commutator [a,b,c], returns {1,2,3}				      

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

  void terminate( ) { };          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SetOf<Word> theWords;
  // Set of words to be checked.
  VectorOf<Word> generators;
  // Vector of generators, to commutate.
  // If generators.length == 0, then commutates generators of the group
  int theCommutatorsLength;
  // The length of commutators to be checked.
  bool isCommutatorsCheck;
  const SMFPGroup* theGroup;

  int currentClass;
  // Class of quotient in which the word problem is solving.

  int upperBound;
  // Upper class for quotients. For nilpotent groups - nilpotency class-1,
  // for others - 10.

  NilpotentWPARCer arcer;
  Trichotomy is_trivial;
  bool isStarted;
  Word retWord;
  VectorOf<int> commutator;
};

//---------------------------------------------------------------------------//
//---------------------------- NilpotentWP ----------------------------------//
//---------------------------------------------------------------------------//


class NilpotentWP : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NilpotentWP( Supervisor& sup);
  // Constructor gets abstract supervisor, so you can call this
  // computation manager from any supervisor.

  void initialize(const VectorOf<Word>& vecW,const class  SMFPGroup* group);
  void initialize(const SetOf<Word>& w,const class  SMFPGroup* group);
  // You have to call initialize, before running start, because we
  // need to initialize words and group for running problem.

  void initialize(const int& length,const class  SMFPGroup* group);
  // Initializing for checking  commutators.
  // You have to call initialize, before running start, because we
  // need to initialize words and group for running problem.

  void initialize(const int& length,const class  SMFPGroup* group,
		  const VectorOf<Word>& gen);
  // Initializing for checking  commutators of generators in 'gen'..
  // You have to call initialize, before running start, because we
  // need to initialize words and group for running problem.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy isTrivial() const;
  // Returns yes if all words in set are trivial.

  Word getWord() const;
  // Returns non trivial word if it was found, and empty if was not.
  VectorOf<int> getCommutator() const;
  // Returns vector of generators in commutator if isCommutatorsCheck==true
  // If commutator [a,b,c], returns {1,2,3}				      

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

  void terminate( ) { };          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SetOf<Word> theWords;
  const SMFPGroup* theGroup;
  
  int theCommutatorsLength;
  // The length of commutators to be checked.
  bool isCommutatorsCheck;
  VectorOf<Word> generators;
  // Vector of generators, to commutate.
  // If generators.length == 0, then commutates generators of the group.
  int theClass;
  // Class of nilpotency

  NilpotentWPARCer arcer;

  bool isStarted;
  Trichotomy is_trivial;
  Word retWord;
  VectorOf<int> commutator;
};

// ---------------------- NGcomputeBasisARCer --------------------------- //


class NGcomputeBasisARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NGcomputeBasisARCer( ComputationManager& boss, bool put_in_file = false) 
    : ARCer( boss ), init( 0 ),putInFile(put_in_file) { } 

  ~NGcomputeBasisARCer( ) { if (init) delete init; } 


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments(const FPGroup& group,const int& nilpClass);
  // Inherited from ARCer:
  // bool takeControl( );

  const NilpotentGroup& getGroup() const;
  Chars getFileName() const {return structFile.getFileName();}

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

  NilpotentGroup* init;
  File structFile;
  bool putInFile;
};

//---------------------------------------------------------------------------//
//------------------------- NGcomputeBasis ----------------------------------//
//---------------------------------------------------------------------------//


class NGcomputeBasis : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NGcomputeBasis(class GCM& gcm, bool internal = true);

  ~NGcomputeBasis( );

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

  void viewStructure(ostream& ostr) const { }  // overrides SMObject

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
  GCM& theGCM;			   
				   
  NGcomputeBasisARCer arcer;
  int arcsNumber;
  bool haveCD;
};


//---------------------------------------------------------------------------//
//------------------------ NilpotentQuotients -------------------------------//
//---------------------------------------------------------------------------//


class NilpotentQuotients : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NilpotentQuotients(class GCM& gcm, bool internal = true);
  
  ~NilpotentQuotients( );

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

  void viewStructure(ostream& ostr) const { }  // overrides SMObject

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
  bool increaseCurrentClass();
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  GCM& theGCM;

  SMFPGroup& theGroup;

  NGcomputeBasisARCer arcer;

  int currentClass;
  // This is the class of currently computing quotient.

  int upperBound; 
  // The hard-wired upper bound on the classes we'll compute.
  // For Nilpotent groups it Nilpotency class - 1, for others 10.
  
};



//---------------------------------------------------------------------------//
//--------------------- NGcomputeBasisProblem -------------------------------//
//---------------------------------------------------------------------------//


class NGcomputeBasisProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NGcomputeBasisProblem(class SMFPGroup& G);

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

  void terminate( ) { };      // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& theGroup;

  MirrorSubordinate computeBasis;

};


// ----------------- NGPresentationARCer ---------------------- //

class NGPresentationARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NGPresentationARCer( ComputationManager& boss ) 
    : ARCer( boss ),theGroup( 0 ), thePresentation( 0 ){ } 
  
  ~NGPresentationARCer( ) {  }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments(const NilpotentGroup& presentation);
  const PresentationForNG& getPresentation() const;

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

  PresentationForNG* thePresentation;
  NilpotentGroup* theGroup;
};

// ------------------ NGdecomposeWordARCer ---------------------- //


class NGdecomposeWordARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NGdecomposeWordARCer( ComputationManager& boss ) 
    : ARCer( boss ),theNG(NULL) { } 
  
  ~NGdecomposeWordARCer() { delete theNG; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const NilpotentGroup&, const Word& );

  PolyWord decomposition() { return retWord; }

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

  NilpotentGroup* theNG;
  
  PolyWord retWord;

  Word theWord;
};

//---------------------------------------------------------------------------//
//------------------------ NGdecomposeWord    -------------------------------//
//---------------------------------------------------------------------------//


class NGdecomposeWord : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  NGdecomposeWord(class WCM& wcm);
  
  ~NGdecomposeWord( );

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

  void viewStructure(ostream& ostr) const { }  // overrides SMObject

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

  class WCM& theWCM;

  class SMWord& theWord;

  NGdecomposeWordARCer arcer;

  bool  started;;
 
  
};


#endif

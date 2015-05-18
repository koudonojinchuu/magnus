// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class ToddCoxeterARCer ToddCoxeter,
//           SGIndexToddCoxeter, SchreierTransversal, PermutationRepProblem,
//           WordRepresentativeProblem, WordRepresentativeProblem 
//
// Principal Author: Alexei Myasnikov
//
// Status: Usable
//
// Revision History:
//


#ifndef _TODD_COXETER_H_
#define _TODD_COXETER_H_


#include "Supervisor.h"
#include "CosetEnumerator.h"
#include "File.h"

// ------------------ ToddCoxeterARCer ---------------------- //


class ToddCoxeterARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ToddCoxeterARCer( ComputationManager& boss,const FPGroup& group ) 
    : ARCer( boss ), enumerator(group),permutationRepresentation(NULL),
    theGroup(group) { } 

  ToddCoxeterARCer( ComputationManager& boss,const FPGroup& group,
		    const VectorOf<Word>& subgroup) 
    : ARCer( boss ), permutationRepresentation(NULL),
    enumerator(group,subgroup), theGroup(group) { } 
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int getRetValue() const{ return retValue; }

  // Inherited from ARCer:
  // bool takeControl( );
  const PermutationRepresentation& getTransversal()const;

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

  CosetEnumerator enumerator;
  FPGroup theGroup;

  int retValue;
  PermutationRepresentation* permutationRepresentation;
};

// -------------------------- ToddCoxeter ---------------------------- //


class ToddCoxeter : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ToddCoxeter(class GCM& gcm);


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

  void start( ) { };          // overrides ComputationManager

  void terminate( ) { };      // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  class GCM& theGCM;
  class SMFPGroup& theGroup;
  ToddCoxeterARCer* arcer;
};

// -------------------------- SGIndexToddCoxeter ------------------------- //


class SGIndexToddCoxeter : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SGIndexToddCoxeter(class SCM& scm);


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

  void start( ) { };          // overrides ComputationManager

  void terminate( ) { };      // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  class SCM& theSCM;
  class SMSubgroup& theSubgroup;
  ToddCoxeterARCer* arcer;
};


//---------------------------------------------------------------------------//
//--------------------------- SchreierTransversal ---------------------------//
//---------------------------------------------------------------------------//


class SchreierTransversal : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SchreierTransversal(class SMFPGroup& );
  SchreierTransversal(class SMSubgroup& );

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

  
  bool haveSchreierTransvl()const;
  
  const VectorOf<Word>& getRepresentatives()const;
  bool isInfinite()const;
 
  
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

  class AlgebraicObject& theObject;
  class SMFPGroup& theGroup;
  
  MirrorSubordinate theToddCoxeter;
  
};

//---------------------------------------------------------------------------//
//--------------------------- PermutationRepProblem -------------------------//
//---------------------------------------------------------------------------//


class PermutationRepProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  PermutationRepProblem(class SMFPGroup& );
  PermutationRepProblem(class SMSubgroup& );

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

  
  bool haveSchreierTransvl()const;
  bool isInfinite()const;
  const class PermutationRepresentation& getPermutations()const;
 
  
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

  class AlgebraicObject& theObject;
  class SMFPGroup& theGroup;
  
  MirrorSubordinate theToddCoxeter;
  
};

//---------------------------------------------------------------------------//
//--------------------------- WordRepresentativeProblem ---------------------//
//---------------------------------------------------------------------------//



class WordRepresentativeARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  WordRepresentativeARCer( ComputationManager& boss ) 
    : ARCer( boss ), permutationRepresentation(NULL)
    { } 

  void setArguments(const PermutationRepresentation&,const Word&);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Word getRetValue() const{ return retValue; }

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

  Word retValue;
  Word theWord;
  PermutationRepresentation* permutationRepresentation;
};








class WordRepresentative : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  WordRepresentative( class WordRepresentativeProblem& boss );

  WordRepresentative( class SubgroupContainmentProblem& boss );

  WordRepresentative( class IsSGNormal& boss );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool haveSchreierTransvl( ) const;

  bool isInfinite( ) const;

  const class PermutationRepresentation& getPermutations( ) const;

  VectorOf<bool> foundWordsReps( ) const { return theWordsRepsFound; }

  VectorOf<Word> getWordsReps( ) const { return theWordsReps; }

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

  const class AlgebraicObject& theParent;
  const class SMFPGroup& theGroup;
  VectorOf<Word> theTestWords;

  VectorOf<Word> theWordsReps;
  VectorOf<bool> theWordsRepsFound;
  int checkedWord;

  WordRepresentativeARCer arcer;
  bool arcerStarted;
};


class WordRepresentativeProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  WordRepresentativeProblem(class SMWord& );
  WordRepresentativeProblem(class SMSubgroup&,class SMWord& );

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

  
  bool haveSchreierTransvl()const;
  bool isInfinite()const;
  //  const class PermutationRepresentation& getPermutations()const;

  const class AlgebraicObject& getParent( ) const { return theParent; }
  const class SMFPGroup& getGroup() const { return theGroup; }
  const class SMWord& getWord( ) const { return theWord; }

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

  class AlgebraicObject& theParent;
  class SMFPGroup& theGroup;
  class SMWord& theWord;
  MirrorSubordinate theToddCoxeter;
  Subordinate<WordRepresentativeProblem,WordRepresentative> theWordRepCM;
};

#endif










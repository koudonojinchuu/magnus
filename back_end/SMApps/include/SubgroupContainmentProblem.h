// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes 
// 
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//
//

#ifndef _SUBGROUP_CONTAINMENT_PROBLEMS_H_
#define _SUBGROUP_CONTAINMENT_PROBLEMS_H_

#include "ComputationManager.h"
#include "Supervisor.h"
#include "SGNilpotentQuotients.h"
#include "ToddCoxeter.h"
#include "AP-fixups.h"

/*
class ExtendedNormalClosureARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ExtendedNormalClosureARCer( ComputationManager& boss );

  ~ExtendedNormalClosureARCer( );

    
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool answer( ) { return contains; }

  void setArguments( const FPGroup& G, const VectorOf<Word>& H, 
		     const SetOf<Word>& testWords );

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

  SetOf<Word> reduceTestingSet( class DecomposeInSubgroupOfFPGroup& D, 
				SetOf<Word>& testingSet );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FPGroup theGroup;
  VectorOf<Word> theSubgroup;
  SetOf<Word> theTestWords;
  bool contains;
};

*/

class SubgroupContainment : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SubgroupContainment( class SubgroupContainmentProblem& );

  SubgroupContainment( class IsSGNormal& );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy contains() const;

  bool haveDetails( ) const;

  Chars getDecompositionFileName( ) const;

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

  SetOf<Word> reduceTestingSet( const class DecomposeInSubgroupOfFPGroup& D, 
				SetOf<Word>& testingSet );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const SMSubgroup& theSubgroup;
  VectorOf<Word> theTestWords;
  SetOf<Word> wordsToCheck;
  Trichotomy theAnswer;

  DetailedReport wordsDecomposition;
};


class SubgroupContainmentProblem : public Supervisor 
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SubgroupContainmentProblem( class SMSubgroup& subgroup, class SMWord& w );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const SMSubgroup& getSubgroup() const { return theSubgroup; }

  const SMWord& getTestWord( ) const { return theTestWord; }

  //  Trichotomy answer( ) const { return theAnswer; }
  // When a WordProblem is used internally, it does not report the result
  // to the FE, but just sits and waits to be polled through this member.
  // yes => trivial, no => not trivial, dontknow => not done yet.

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
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const SMSubgroup& theSubgroup;
  const SMWord& theTestWord;

  // Housekeeping members:
  Trichotomy isWordInSubgroup;
  Chars explanation;

  // GCM components this uses; only for automatic ARC passing:

  Subordinate<SubgroupContainmentProblem,SubgroupContainment> subgroupContain;
  MirrorSubordinate normalClosure;

  MirrorSubordinate abelianInvariants;
  MirrorSubordinate sgNilpotentQuotients;
  Subordinate<SubgroupContainmentProblem, SCPinNilpotentQuotients> 
    scpInNilpotentQuotients;

  MirrorSubordinate toddCoxeter;
  Subordinate<SubgroupContainmentProblem, WordRepresentative> wordRepCM;
  
};




class IsSGNormal : public Supervisor 
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  IsSGNormal( class SMSubgroup& subgroup );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const SMSubgroup& getSubgroup() const { return theSubgroup; }

  SetOf<Word> getTestWords( ) const { return theTestWords; }

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

  SetOf<Word> makeTestWords( const class SMSubgroup& H ) const;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const SMSubgroup& theSubgroup;
  SetOf<Word> theTestWords;
  Trichotomy isNormal;
  Chars explanation;

  Subordinate<IsSGNormal,SubgroupContainment>  subgroupContain;
  MirrorSubordinate normalClosure;
  MirrorSubordinate abelianInvariants;
  MirrorSubordinate sgNilpotentQuotients;
  Subordinate<IsSGNormal, SCPinNilpotentQuotients>  scpInNilpotentQuotients;
  MirrorSubordinate toddCoxeter;
  Subordinate<IsSGNormal, WordRepresentative> wordRepCM;
};



inline
bool SubgroupContainment::haveDetails ( ) const 
{ 
  return wordsDecomposition.haveDetails(); 
}

inline
Chars SubgroupContainment::getDecompositionFileName( ) const 
{ 
  return wordsDecomposition.getFileName(); 
}

#endif

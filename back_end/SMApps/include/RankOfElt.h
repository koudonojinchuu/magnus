// Contents: Declaration of classes RankOfElement, RankOfElementProblem, 
//
// Principal Author: Alexander Ushakov
//
// Status: in progress
//
// Revision History:
//


#ifndef _RANK_OF_ELT_H_
#define _RANK_OF_ELT_H_


#include "FreeGroup.h"
#include "Supervisor.h"
#include "RankOfSubgroup.h"


// ------------------------  RankOfElement ------------------------------ //


class RankOfElement : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  RankOfElement(class RankOfElementProblem& problemObject );

  ~RankOfElement( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Trichotomy getAnswer() const { 
       return theARCer.getAnswer();
  }

  Chars getFileName( ) { return theARCer.getFileName(); }
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

  VectorOf<Word> constructVector( const Word& word );
    
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMWord& theWord;

  FreeGroup theGroup;

  RankOfSubgroupARCer theARCer;

};


// -------------------- RankOfElementProblem --------------------------- //


class RankOfElementProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  RankOfElementProblem(const class SMWord& w);
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMWord& getWord( ) const;


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
 
  bool linkHasBeenSent;
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Subordinate Computation Managers:                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Subordinate<RankOfElementProblem, RankOfElement> rankOfElement;
 
};


#endif

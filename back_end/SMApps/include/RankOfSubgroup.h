// Contents: Declaration of classes RankOfSubgroup, RankOfSubgroupProblem, RankOfSubgroupARCer
//
// Principal Author: Alexander Ushakov
//
// Status: in progress
//
// Revision History:
//

#ifndef _RANKOFSUBGROUP_H_
#define _RANKOFSUBGROUP_H_


#include "TurnerProperSubgroupEnumerator.h"

#include "Supervisor.h"
#include "SMWord.h"
#include "File.h"

//#include "SGofFreeGroup.h"
//#include "SMSetOfWords.h"
//#include "FreeGroup.h"



// ------------------------  RankOfSubgroupARCer ------------------------------ //



class RankOfSubgroupARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  RankOfSubgroupARCer( ComputationManager& boss, 
		  const class SMFPGroup& group, 
		  const SGofFreeGroup& sg );

  ~RankOfSubgroupARCer();

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void runComputation( );
 
  void writeResults( ostream& );

  void readResults( istream& );
  
  Chars getFileName( ) const { return PSE->getFileName(); }

  Trichotomy getAnswer() const { return answer;}

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


  bool isAFreeFactorIn( SGofFreeGroup& g ) const;

  const class SMFPGroup& theGroup;
  // given free group
  const SGofFreeGroup theSubgroup;
  // given subgroup

  ProperSubgroupEnumerator* PSE;
  // Subgroup enumerator 

  Trichotomy answer;

  void lookingup();
  // enumerate subgroup
  void printSG( ostream& file, const SGofFreeGroup& subgroup );
};


// ------------------------  RankOfSubgroup ------------------------------ //


class RankOfSubgroup : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  RankOfSubgroup(class RankOfSubgroupProblem& problemObject );

  ~RankOfSubgroup( );

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

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMSubgroup& theSubgroup;

  FreeGroup theGroup;

  RankOfSubgroupARCer theARCer;

};


// -------------------- RankOfSubgroupProblem --------------------------- //


class RankOfSubgroupProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  RankOfSubgroupProblem(const class SMSubgroup& sg);


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMSubgroup& getSubgroup( ) const;


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

  const SMSubgroup& theSubgroup;
 
  bool linkHasBeenSent;
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Subordinate Computation Managers:                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Subordinate<RankOfSubgroupProblem, RankOfSubgroup> rankOfSubgroup;
 
};



#endif


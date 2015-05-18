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


#ifndef _MAKE_RANDOM_PRESENTATION_H_
#define _MAKE_RANDOM_PRESENTATION_H_


#include "Supervisor.h"
#include "CosetEnumerator.h"
#include "File.h"

// ------------------ RandomPresentationARCer ---------------------- //


class RandomPresentationARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  RandomPresentationARCer( ComputationManager& boss ) 
    : ARCer( boss ), maxGens(10),  maxRels(10), averageRels(20) { } 


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors/Modifiers:                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments(int maxG,int maxR,int averR){
    maxGens = maxG;
    maxRels = maxR;
    averageRels = averR;
  }

  FPGroup getRetValue() const{ return theGroup; }

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
  int maxGens;
  int maxRels;
  int averageRels;
};

//--------------------------- MakeRandomPresentation ------------------------//



class MakeRandomPresentation : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  MakeRandomPresentation( /*SMFPGroup&*/ );

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

  void terminate( ) { }          // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  //class SMFPGroup& theGroup;
  
  RandomPresentationARCer arcer;
  
  bool arcerStarted;
  int nOfPresentations;
};

#endif










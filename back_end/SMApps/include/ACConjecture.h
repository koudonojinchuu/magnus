// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declarations of classes 
//           ACConjectureARCer, ACConjectureProblem
//
// Principal Author: Alexei Miasnikov
//
// Status: in progress
//
// Revision History:
//


#ifndef _AC_CONJECTURE_H_
#define _AC_CONJECTURE_H_


#include "Supervisor.h"
#include "ACGA.h"
#include "ACConfig.h"
#include "SMVectorOfWords.h"

// --------------------------- ACConjectureARCer ------------------------ //


class ACConjectureARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ACConjectureARCer( ComputationManager& );

  ~ACConjectureARCer( ) { }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( FPGroup );

  Chars getComputationFileName() const { return compFile.getFileName();}
  Chars getResultFileName() const { return resultFile.getFileName();}
  bool getResult() const { return result; }
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
  
  FPGroup theGroup;
  File compFile;
  File resultFile;
  bool result;
};



// ------------------- ACConjectureProblem --------------------- //


class ACConjectureProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ACConjectureProblem( const class SMVectorOfWords& );


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

  void start( );          // overrides ComputationManager

  void terminate( ) { };      // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const SMVectorOfWords& theTuple;
  
  bool linkHasBeenSent;


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Subordinate Computation Managers:                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ACConjectureARCer arcer;
};

#endif

// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declarations of classes TTArcer, TTCM, TTProblem.
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
//

#ifndef _TTProblem_H_
#define _TTProblem_H_

#include "Supervisor.h"
#include "SMFPGroup.h"
#include "TTP.h"


// ------------------------------ TTArcer ---------------------------------- //


class TTArcer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  TTArcer( ComputationManager& boss ) 
    : ARCer( boss ), ttp( 0 ), bHappy( true ) { } 

  ~TTArcer( ) { delete ttp; }
  
    
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const FPGroup& G );

  // Inherited from ARCer:
  // bool takeControl( );

  Chars getFileName( );

  bool isHappy( ) { return bHappy; }
  
  
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
  bool bHappy;
  TTP* ttp;
};


//--------------------------- KBSupervisor ----------------------------------//


class TTCM : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  TTCM(class TTProblem& PO);

  ~TTCM( );

   
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Chars getFileName( ) { return out.getFileName(); }

  bool isHappy() { return arcer.isHappy(); }

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

  void terminate( ) { }       // overrides ComputationManager

  
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const SMFPGroup& theGroup;

  long inPos;
  File out;
  FILE* in;
  bool bInstructions;

  TTArcer arcer;
};


// --------------------------- TTProblem ----------------------------------- //


class TTProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  TTProblem(class SMFPGroup& G);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const SMFPGroup& getGroup( ) const { return theGroup; }

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

  SMFPGroup& theGroup;
  bool linkHasBeenSent;
  Subordinate<TTProblem, TTCM> tt;
};

#endif

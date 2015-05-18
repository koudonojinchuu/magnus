// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of classes GeneticWPBase,GeneticWPArcer,GeneticWPCM
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _GeneticProblems_h_
#define _GeneticProblems_h_


#include "Supervisor.h"
#include "SMWord.h"
#include "ARCer.h"


// ---------------------------- GeneticWPBase ------------------------------ //


struct GeneticWPBase {

  enum DetailType { NO_DETAILS, WORD, SET_OF_WORDS, COMMUTATORS }; 

  // Customizes details output for various supervisors.
  // You can certainly include types like WORD_PROBLEM, IS_ABELIAN, etc.

};


// --------------------------- GeneticWPArcer ------------------------------ //


class GeneticWPArcer : public ARCer, public GeneticWPBase {

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  GeneticWPArcer( ComputationManager& );

  ~GeneticWPArcer( ) { delete file; }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const FPGroup&, const SetOf<Word>&, 
		     DetailType dt = NO_DETAILS );

  Trichotomy getRetValue() { return retValue; }

  Chars getFileName();

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
  SetOf<Word> theWords;
  Trichotomy retValue;
  DetailType dtype;
  File *file;
};



// ---------------------------- GeneticWPCM -------------------------------- //


class GeneticWPCM : public ComputationManager, public GeneticWPBase {

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  GeneticWPCM( class Supervisor& boss );

  //  ~GeneticWPCM( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void init( const FPGroup& group, const SetOf<Word> words, 
	     DetailType dt = NO_DETAILS ); 
  // you must call this function before any other accessor

  Trichotomy areTrivial( ) { return tAreTrivial; }

  Chars getFileName() { return arcer.getFileName(); }
   
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

  FPGroup theGroup;
  SetOf<Word> theWords;
  const Supervisor& theBoss;
  
  Trichotomy tAreTrivial;
  bool bStarted;
  bool bInited;
  DetailType dtype;
  
  GeneticWPArcer arcer;
};


#endif



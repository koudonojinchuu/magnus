// Copyright (C) 1999 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of classes FNWPArcer,FNWPCM
//
// Principal Author: Dmitry Bormotov
//
// Status: in use
//
// Revision History:
//


#ifndef _FNWP_h_
#define _FNWP_h_

#include "Supervisor.h"
#include "SMWord.h"
#include "ARCer.h"


// ----------------------------- FNWPArcer --------------------------------- //


class FNWPArcer : public ARCer {

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FNWPArcer( ComputationManager& );

  // no destructor


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const FreeGroup&, int nClass, const SetOf<Word>& ); 

  Trichotomy getRetValue() { return retValue; }

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

  FreeGroup theGroup;
  int c;
  SetOf<Word> theWords;
  Trichotomy retValue;
};


// ------------------------------ FNWPCM ----------------------------------- //


class FNWPCM : public ComputationManager {

public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  FNWPCM( class Supervisor& boss );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void init( const FreeGroup& group, int nClass, const SetOf<Word> words );
  // you must call this function before any other accessor

  Trichotomy areTrivial( ) { return tAreTrivial; }


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

  FreeGroup theGroup;
  int c;
  SetOf<Word> theWords;
  const Supervisor& theBoss;
  
  Trichotomy tAreTrivial;
  bool bStarted;
  bool bInited;
  
  FNWPArcer arcer;
};


#endif



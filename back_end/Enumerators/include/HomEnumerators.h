// Copyright (C) 2000 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of the HomEnumeratorARCer1, RandHomEnumeratorProblem
//           HomEnumeratorARCer2 TotalHomEnumeratorProble classes.
//
// Principal Authors: Denis Serbin
//
// Status: In progress
//
// Revision History:
//
// Special Notes:
//
//

#ifndef _HomEnumerators_h_
#define _HomEnumerators_h_

#include "Supervisor.h"
#include "SMEnumerator.h"
#include "WordEnumerator.h"
#include "File.h"
#include "Word.h"
#include "SMFPGroup.h"
#include "RandomNumbers.h"
#include "OutMessages.h"


// ------------------------- HomEnumeratorARCer1 --------------------------- //


class HomEnumeratorARCer1 : public EnumeratorARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  HomEnumeratorARCer1( ComputationManager& boss , SMListData& d , GIC& gic ) 
    : EnumeratorARCer( boss , d ), 
      rangeGIC( gic ),
      current( 0 ),
      number( 0 ),
      avgLength( 0 ),
      lengthPicker(2112),                        
      genPicker(1812)
    { }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  void setArguments( FPGroup g , FPGroup h ,  
		     int avgNumbers , int n );
  
  void setParams( int k , int n )
    {
      number = n;
      avgLength = k;
    } 
  
  // Inherited from ARCer:
  // bool takeControl( );
  
  Map getMap();
  
  bool extendToHom( Map& );
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void enumerate( );
  
  void writeResults( ostream& );

  void readResults( istream& );


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  int  current;
  int avgLength;
  int number;
  
  NormalRandom lengthPicker;
  UniformRandom genPicker;
  
  GIC& rangeGIC;
 
  FPGroup G;
  FPGroup H;
};

// ------------------------ RandHomEnumeratorProblem ----------------------- //

class RandHomEnumeratorProblem : public EnumeratorProblem< Map >
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  RandHomEnumeratorProblem( SMFPGroup& , SMFPGroup& );
  
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

  Chars getDataFileName() const { return theData.getDataFileName(); }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void start( );
  
  void terminate( ) {  }          // overrides ComputationManager
  
  void resume( ) {              // overrides ComputationManager
    arcer.submitSignal();
    
    int num = getParameters().getValue("P1");
    int avgNumGens = getParameters().getValue("P2");
    arcer.setParams( avgNumGens , num );
  }
   
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& G;
  SMFPGroup& H;
  HomEnumeratorARCer1 arcer;
  
  bool init;
  
  MirrorSubordinate kbSupervisor;
  MirrorSubordinate agSupervisor;
};

// ------------------------- HomEnumeratorARCer2 --------------------------- //


class HomEnumeratorARCer2 : public EnumeratorARCer
{
public:
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  HomEnumeratorARCer2( ComputationManager& boss , SMListData& d , GIC& gic ) 
    : EnumeratorARCer( boss , d ), 
      rangeGIC( gic ),
      current( 0 ),
      number( 0 ),
      we( FreeGroup() )
    { }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( FPGroup G , FPGroup H , int n );
  void setParam( int k )
    {
      number = k;
    }
  
  // Inherited from ARCer:
  // bool takeControl( );
  
  Map nextMap();
  
  bool extendToHom( Map& );
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void enumerate( );
 
  void writeResults( ostream& );

  void readResults( istream& );


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  WordEnumerator we;
  int number;
  int current;
  
  FPGroup G;
  FPGroup H;
  
  class GIC& rangeGIC;
};

// ------------------------ TotalHomEnumeratorProblem ---------------------- //

class TotalHomEnumeratorProblem : public EnumeratorProblem< Map >
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  TotalHomEnumeratorProblem( SMFPGroup& , SMFPGroup& );
  
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

  Chars getDataFileName() const { return theData.getDataFileName(); }
  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // State Transitions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void start( );
 
  void terminate( ) {  }          // overrides ComputationManager
  
  void resume( ) {              // overrides ComputationManager
    arcer.submitSignal();
    
    arcer.setParam( getParameters().getValue("P") );
  }
   
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& G;
  SMFPGroup& H;
  HomEnumeratorARCer2 arcer;
  
  bool init;

  MirrorSubordinate kbSupervisor;
  MirrorSubordinate agSupervisor;
};

#endif

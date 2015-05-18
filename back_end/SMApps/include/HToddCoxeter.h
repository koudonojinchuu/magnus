// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class HToddCoxeterARCer HToddCoxeter,
//           HSGIndexToddCoxeter
//
// Principal Author: Alexei Myasnikov
//
// Status: Usable
//
// Revision History:
//


#ifndef _H_TODD_COXETER_H_
#define _H_TODD_COXETER_H_


#include "Supervisor.h"
#include "HavasTC.h"
#include "File.h"

// ------------------ HToddCoxeterARCer ---------------------- //


class HToddCoxeterARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  HToddCoxeterARCer( ComputationManager& boss,const FPGroup& group ) : 
    ARCer( boss ),
    tc(group),
    theGroup(group),
    theIndex( 0 ),
    success( false ){ } 
  
  HToddCoxeterARCer( ComputationManager& boss,const FPGroup& group,
		     const VectorOf<Word>& subgroup) : 
    ARCer( boss ),
    tc(group,subgroup), 
    theGroup(group), 
    theIndex( 0 ),
    success( false ){ } 
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  int getIndex() const{ return theIndex; }

  bool isSuccessful() const {return success;}

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

  HavasTC tc;
  FPGroup theGroup;

  int theIndex;
  bool success;
};


// -------------------------- HToddCoxeter ---------------------------- //


class HToddCoxeter : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  HToddCoxeter(class GCM& boss);


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

  class GCM& theBoss;
  class SMFPGroup& theGroup;
  HToddCoxeterARCer* arcer;
};

// -------------------------- HSGIndexToddCoxeter ------------------------- //


class HSGIndexToddCoxeter : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  HSGIndexToddCoxeter(class SCM& scm);


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
  HToddCoxeterARCer arcer;
};
#endif










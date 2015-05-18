// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of class HomologyProblem
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#ifndef _HOMOLOGYPROBLEM_H_
#define _HOMOLOGYPROBLEM_H_

#include "Supervisor.h"
#include "Vector.h"

//---------------------------------------------------------------------------//
//------------------------- HomologyProblem ---------------------------------//
//---------------------------------------------------------------------------//

class HomologyARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  HomologyARCer( ComputationManager& );

  ~HomologyARCer( );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const class KBMachine& M, int startDimension, 
		     int endDimension );

  VectorOf<Integer> getTorsionInvariants( ) const { return torsionInvariants; }

  int getTorsionFreeRank() const { return torsionFreeRank; }

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
  
  class KBMachine *kbmachine;
  int startdim, enddim;
  VectorOf<Integer> torsionInvariants;
  int torsionFreeRank;
};


class HomologyProblem : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  HomologyProblem( const class HomologySupervisor& );
   
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool isSolved() const { return solved; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
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

  void start( ) { }              // overrides ComputationManager

  void terminate( ) { }          // overrides ComputationManager

  
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& theGroup;
  int theDimension;
  HomologyARCer homologyArcer;
  bool arcerStarted;
  bool solved;
};





class HomologySupervisor : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  HomologySupervisor(class SMFPGroup& G, int d = 2 );

/*
  HomologySupervisor(class SMFPGroup& G)
    : Supervisor(true), theGroup(G), 
      homology( *this ), kbSupervisor(*this,*((SubordinateBase*)0))
  { }
  //@rn Bogus ctor; see `But no' comment in Menu.h.
*/
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& group() const { return theGroup; }

  int dimension( ) const { return theDimension; }

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

  void start( ) { }           // overrides ComputationManager

  void terminate( ) { }       // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMFPGroup& theGroup;
  int theDimension;

  Subordinate<HomologySupervisor, HomologyProblem> homology;
  MirrorSubordinate kbSupervisor;
};

#endif


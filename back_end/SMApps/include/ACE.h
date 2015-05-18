// Copyright (C) 2001 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declarations of classes ACEArcer, ACECM, ACEProblem
//           ( interface to Gorge Havas's ACE package )
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//


#ifndef _ACE_H_
#define _ACE_H_


#include "Supervisor.h"
#include "SMSubgroup.h"


// --------------------------- ACEArcer ------------------------ //


class ACEArcer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ACEArcer( ComputationManager& );

  ~ACEArcer( ) { }


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( FPGroup g, VectorOf<Word> sg );

  void setTime( int t ) { iTime = t; }

  void setWorkspace( int w ) { iWS = w; }

  void setDifficulty( Chars d ) { rDiff = d; }

  void setExtra( Chars e ) { cExtra = e; }

  //BlackBox& getBB() { return ace; }

  Chars getFileName( ) { return in.getFileName(); }

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
  VectorOf<Word> theSubgroup;
  //BlackBox ace;
  File in;

  // ACE parameters
  int iTime, iWS;
  Chars rDiff, cExtra;
};



// ---------------------------- ACECM ------------------------- //


class ACECM : public ComputationManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ACECM( class ACEProblem& );


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
   
  bool isFinished() const { return bFinished; }

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Control:                                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void takeControl( );         // overrides ComputationManager

  ACEArcer& getArcer( ) { return arcer; }


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

  ACEArcer arcer;
  const SMSubgroup& theSubgroup;
  bool linkHasBeenSent;
  bool bFinished;
  //File file;
};


// --------------------ACEProblem --------------------------- //


class ACEProblem : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ACEProblem(const class SMSubgroup& sg);

  ACEProblem(class SMFPGroup& g);
  // No subgroup - for example, use this when running ACE to find the order 
  // of the whole group.


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const class SMFPGroup& getGroup( ) const { return theGroup; }

  const class SMSubgroup& getSubgroup( ) const { return theSubgroup; }


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

  void start( );             // overrides ComputationManager

  void terminate( ) { }      // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  const SMFPGroup& theGroup;
  const SMSubgroup& theSubgroup;
 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Subordinate Computation Managers:                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Subordinate<ACEProblem, ACECM> ace;

};

#endif

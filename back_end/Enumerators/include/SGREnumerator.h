// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of class RelatorEnumerator, an enumerator with
//           an integrated general word problem.
//           It allows the end user to guide the search for a general
//           solution of the word problem. If and when one is found,
//           it enumerates relators for a given subgroup, making some
//           attempt to filter out redundant ones.
//
// Principal Author: Roger Needham, Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//
// @am Implemented as SMEnumerator.

#ifndef _RELATORENUMERATOR_H_
#define _RELATORENUMERATOR_H_


#include "Supervisor.h"
#include "SMSubgroup.h"
#include "SMEnumerator.h"
#include "File.h"



//--------------------------------------------------------------------------//
//---------------------- SGRelatorEnumeratorARCer --------------------------//
//--------------------------------------------------------------------------//



class SGRelatorEnumeratorARCer : public EnumeratorARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SGRelatorEnumeratorARCer( ComputationManager& boss, SMListData& d ) 
    : EnumeratorARCer( boss, d ),
      firstStart(true),
      counter( 1 ),
      numberOfAll( 100 ){ } 
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const class SMFPGroup* group,
		     const class SMSubgroup* subgroup );

  // Inherited from ARCer:
  // bool takeControl( );
  void setNumberOfAll( int n ) { numberOfAll = n; }
  bool ORisTrivial(const Word&  theTestWord);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 
  void writeResults( ostream& );

  void readResults( istream& );

  Chars getFileName() const { return file.getFileName(); }
protected:
  
  void enumerate();
  
private:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  const class SMSubgroup* theSMSubgroup;
  const class SMFPGroup* theGroup;
  Word possibleRelator;
  bool firstStart;
  int counter;
  int numberOfAll;
  File file;
};


//--------------------------------------------------------------------------//
//----------------------- SGRelatorEnumerator ------------------------------//
//--------------------------------------------------------------------------//

class SGRelatorEnumerator : public  EnumeratorProblem< NoType >
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SGRelatorEnumerator( SMSubgroup&);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Front End Interfacing:                                              //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void viewStructure(ostream& ostr) const;  // overrides SMObject

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  class SMSubgroup& getSubgroup() const { return theSMSubgroup;}

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

  void start( ) {       // overrides ComputationManager
    int totalNum = getParameters().getValue("numberOfAll");
    arcer.setNumberOfAll( totalNum );
  }       
  
  void terminate( ) { }       // overrides ComputationManager

  void resume() {
    resumed = true;
    arcer.submitSignal();
    
    int totalNum = getParameters().getValue("numberOfAll");
    arcer.setNumberOfAll( totalNum );

  }

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMSubgroup& theSMSubgroup;
  SGRelatorEnumeratorARCer arcer;
 
  class GIC& theGIC;

  bool started;
  bool firstStart;  
  bool resumed;
  bool useORwordProblem;
  // GCM components this uses; only for automatic ARC passing:

  MirrorSubordinate kbSupervisor;

  MirrorSubordinate agSupervisor;

};

#endif

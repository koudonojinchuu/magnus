// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: 
//
// Principal Author: 
//
// Status: in progress
//
// Revision History:
//


#ifndef _OR_CONS_ENUMERATOR_H_
#define _OR_CONS_ENUMERATOR_H_


#include "OneRelatorGroup.h"
#include "Supervisor.h"
#include "SMSubgroup.h"
#include "SMEnumerator.h"
#include "File.h"


//--------------------------------------------------------------------------//
//---------------------- ORConsequencesEnumeratorARCer ---------------------//
//--------------------------------------------------------------------------//



class ORConsequencesEnumeratorARCer : public EnumeratorARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ORConsequencesEnumeratorARCer( ComputationManager& boss,SMListData& d,
				 const OneRelatorGroup& g ) 
    : EnumeratorARCer( boss, d ),
      theEnumerator( g ),   
      theGroup( g ),
      counter( 1 ),
      numberOfAll( 100 ){ } 
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////


  Chars getFileName() const {return file.getFileName();}
  // Inherited from ARCer:
  // bool takeControl( );
  void setNumberOfAll( int n ) { numberOfAll = n; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

 
  void writeResults( ostream& );

  void readResults( istream& );


protected:
  
  void enumerate();
  
private:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  EnumeratorOfConsequences theEnumerator;
  OneRelatorGroup theGroup;

  File file;
  int counter;
  int numberOfAll;
};


//--------------------------------------------------------------------------//
//----------------------- ORConsequencesEnumerator -------------------------//
//--------------------------------------------------------------------------//

class ORConsequencesEnumerator : public  EnumeratorProblem< Word >
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ORConsequencesEnumerator( SMFPGroup&);

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

  class SMFPGroup& getGroup() const { return theSMFPGroup;}

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

  SMFPGroup& theSMFPGroup;
  ORConsequencesEnumeratorARCer arcer;
  bool started;
};

#endif

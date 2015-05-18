// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class
//
// Principal Author: Alexei Myasnikov
//
// Status: in progress
//
// Revision History:
//

#ifndef _LIST_PROBLEMS_H_
#define _LIST_PROBLEMS_H_

#include "Supervisor.h"
#include "fastProblems.h"
#include "SMList.h"
#include "SMListIterator.h"



// ---------------------- SMListSupervisor ------------------------ //
// Parent class for supervisors which use general computation managers
// like SMListExtractTrivialWords. They need to pass SMList to computation 
// manager

class SMListSupervisor :  public Supervisor
{
 public:
  SMListSupervisor( SMList<Word>& l )
    : Supervisor( true ),
    theList( l ) { }

  SMList<Word>& getList() { return theList; }
  
 protected:
  
  SMList<Word>& theList;
  
};

// ---------------------- SMListJoinARCer ------------------------ //

template <class T>
class SMListJoinARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  SMListJoinARCer( ComputationManager& boss ) 
    : ARCer( boss ),
    l2Data( NULL ), l1Data( NULL ) { } 
  
  ~SMListJoinARCer( ) { 
   if (l1Data)
      delete l1Data;
    if (l2Data)
      delete l2Data;
  }
 
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void setArguments( const SMList<T>&, const SMList<T>& );

  const SMListData getJoinData() const { return joinData; }
  
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

  // FPGroup theGroup;
  SMListData* l1Data;
  SMListData* l2Data;
  SMListData joinData;
};



// --------------------- SMListJoin  ------------------------ //

template <class T> class SMListJoin  : public Supervisor
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

   SMListJoin( SMList<T>&, SMList<T>& );


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

  void start( ) { }


  void terminate( ) { }        // overrides ComputationManager


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  SMList<T>& list1;
  SMList<T>& list2;
  bool started;
  SMListJoinARCer<T> arcer;

};
// -------------------------- MakeSMListOf ------------------------- //

template <class T> class MakeSMListOf : public FastComputation
{
public:

  MakeSMListOf( EnumeratorProblem<T>& e):
    enumerator( e ) { }
  
  void takeControl( );              // overrides ComputationManager
  
protected:
  
  EnumeratorProblem<T>& enumerator;
    
};

// -------------------------- SMListCheckerARCer --------------- //
/*
class SMListCheckerARCer : public ARCer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  SMListCheckerARCer( ComputationManager& boss, char* sb, int l ) 
    : ARCer( boss ), theBuffer( sb ),bufferLength( l ) { } 
  

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Inherited from ARCer:
  // bool takeControl( );
  // virtual void runComputation( ) = 0;
  // virtual void writeResults( ostream& ) = 0;
  // virtual void readResults( istream& ) = 0;
 
  char getElementInBuffer( int ) const;
  
  void setElementInBuffer( int, char );

  int bLength() const { return bufferLength; }

 protected:


 private:
  char* theBuffer;
  int bufferLength;
};

*/

#endif

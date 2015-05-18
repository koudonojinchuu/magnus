// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class ARCer.
//
// Principal Author: Dmitry Bormotov.
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//

#ifndef _ARCER_H_
#define _ARCER_H_


#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fstream.h>
#include <iostream.h>
#include <sys/wait.h>
#include "MagnusHome.h"
#include "ComputationManager.h"


// ----------------------------- ARCer ------------------------------------- //

//@njz: added to avoid complain of GCC-4.0.1
class ComputationManager;
//

class ARCer
{

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ARCer( ComputationManager& boss, const int delay = 1 );

    
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  bool isInterrupted() { return bInterrupted; }
  
  int getPid( ) { return pid; }

  int delay( ) { return theDelay; }

  virtual bool takeControl( );
  
  virtual void terminate( );

       
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  virtual void runComputation( ) = 0;

  virtual void writeResults( ostream& ) = 0;

  virtual void readResults( istream& ) = 0;
 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  
  Chars startDir;

protected:
  
  int pid;
  int theDelay;
  bool bInterrupted;
  ComputationManager& theBoss;
};


// ------------------------- ExternalARCer --------------------------------- //


class ExternalARCer : public ARCer
{
  
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ExternalARCer( ComputationManager& boss, 
		 const char* cmdline, 
		 const int delay = 1 );

  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  // Inherited from ARCer:
  //
  // bool isInterrupted( );
  // int  getPid( );
  // void terminate( );

  virtual bool takeControl( );

       
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  virtual void writeArguments( ostream& ) = 0;

  virtual void readResults( istream& ) = 0;
 

private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void writeResults( ostream& ) { }

  void runComputation( );
  
 
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
    
  Chars theCmdLine;
  Chars argFN;
  Chars resultFN;
};


// @db The following class is an experimental version of new ARCer
// class and included here just because of the NormalClosure.

// ----------------------------- ARCer2 ------------------------------------ //


class ARCer2 : public ARCer
{
  
public:

  enum State { RUNNING, READING, FINISHED };

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ARCer2( ComputationManager& boss, const int delay = 1 );

  ~ARCer2( ) { delete in; }

    
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
 
  virtual bool takeControl( );

  void terminate( );
 
         
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC interaction members:                                            //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  virtual bool readResults2( istream& ) = 0;
 

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

private:
  
  State state;
  ifstream* in;
};


#endif


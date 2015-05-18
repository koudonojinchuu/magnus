// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class ARCer
//
// Principal Author: Dmitry Bormotov
//
// Status: in progress
//
// Revision History:
//
// * 7/96 Dmitry B. made porting to gcc 2.7.2.
//


#include "ARCer.h"

#define DELETE_IPC_FILES


// ----------------------------- ARCer ------------------------------------- //


ARCer::ARCer( ComputationManager& boss, const int delay )
  : bInterrupted( false ),
    theDelay( delay ),
    pid( -1 ),
    theBoss( boss ),
    startDir( MagnusHome::magnusHome()+"/back_end/SessionManager/test/bin" )
{
  boss.hireArcer(this);
}


void ARCer::terminate( )
{
  if( pid < 0 ) return;
  kill(-pid, SIGKILL);
  #ifdef BSD    
    while( waitpid(-pid, 0, WNOHANG) > 0 );
  #endif
  pid = -1;
  bInterrupted = false; 
}


bool ARCer::takeControl( )
{
  if( !bInterrupted ) {
    
    if ( ( pid = fork() ) == 0 ) {
      
      setpgid(0,0);
      runComputation();
      
      char sPid[100];
      sprintf(sPid, "%d", getpid());
		Chars fname(MagnusTmp::magnusTmp()+"/results."+sPid);
      ofstream out(fname, ios::out | ios::trunc );
      if( !out ) 
	error(Chars("Cannot open an output file ")+fname);
      
      writeResults(out);
      
      out.flush();
      _exit(0);
    }
    
    if( pid < 0 )
      error("fork failed.");
    
    kill(-pid, SIGSTOP);
  }
  
  kill(-pid, SIGCONT);
  sleep(theDelay);
  kill(-pid, SIGSTOP);
  bInterrupted = true;
  
  if( kill(pid, 0) != 0 ) {
    
    bInterrupted = false;
    
    //@db system porting

  #ifdef BSD    
    int status;
    wait(&status);
  #endif
    
    // get results of computation
    
    char sPid[100];
    sprintf(sPid, "/results.%d", pid);
    //@njz: removed ios::nocreate temporarily -TODO
    //    ifstream in(MagnusTmp::magnusTmp()+sPid, ios::in | ios::nocreate );
    ifstream in(MagnusTmp::magnusTmp()+sPid, ios::in );
    //

    if( !in ) {
      //error("Cannot open an input file.");
      
      theBoss.setCrashState(); 
      return false;
    }
    
    readResults(in);
     
    //@db Here we can delete the result file,
    //    but I want to keep it for a while.
    
    return true;
  }
  
  return false;
}


// ------------------------- ExternalARCer --------------------------------- //


ExternalARCer::ExternalARCer( ComputationManager& boss, const char* cmdline,
			      const int delay )
  : ARCer( boss, delay ),
    theCmdLine( cmdline )
{
  argFN = tempnam(MagnusTmp::magnusTmp(),"arg");
  resultFN = tempnam(MagnusTmp::magnusTmp(),"res");
}

void ExternalARCer::runComputation( )
{
  const char *cmdline = theCmdLine + " < " + argFN + " > " + resultFN; 
  execl("/bin/sh", "sh", "-c", cmdline, 0);

#if SAFETY > 0
  error("void ExternaARCer::runComputation( ): execl() failed.");
#endif    
}
  

bool ExternalARCer::takeControl( )
{
  if( !bInterrupted ) {

    ofstream out(argFN, ios::out | ios::trunc );

#if SAFETY > 0
    if( !out ) 
      error("bool ExternalARCer::takeControl( ): Cannot open an output file.");
#endif    

    writeArguments(out);
    
    if ( ( pid = fork() ) == 0 ) {
      setpgid(0,0);
      runComputation();
    }

#if SAFETY > 0
    if( pid < 0 )
      error("bool ExternaARCer::takeControl( ) : fork failed.");
#endif    
    
    kill(-pid, SIGSTOP);
  }
  
  kill(-pid, SIGCONT);
  sleep(theDelay);
  kill(-pid, SIGSTOP);
  bInterrupted = true;
  
  if( kill(pid, 0) != 0 ) {
    
    bInterrupted = false;
    
    //@db system porting

  #ifdef BSD    
    int status;
    wait(&status);
  #endif
    
    // get results of the computation


    //@njz: removed ios::nocreate temporarily -TODO
    //    ifstream in(resultFN, ios::in | ios::nocreate );
    ifstream in(resultFN, ios::in );
    //

    if( !in ) {
      // error("bool ExternalARCer::takeControl( ): "
      // "Cannot open an input file.");

      theBoss.setCrashState(); 
      return false;
    }

    readResults(in);

    // delete the IPC files

#ifdef DELETE_IPC_FILES
    unlink(argFN);
    unlink(resultFN);
#endif
    
    return true;
  }
  
  return false;
}


// @db The following class is an experimental version of new ARCer
// class and included here just because of the NormalClosure.

// ----------------------------- ARCer2 ------------------------------------ //


ARCer2::ARCer2( ComputationManager& boss, const int delay )
  : ARCer(boss, delay),
    state(FINISHED),
    in(0)
{ }


void ARCer2::terminate( )
{
  if( pid < 0 ) return;
  kill(-pid, SIGKILL);
  #ifdef BSD    
    while( waitpid(-pid, 0, WNOHANG) > 0 );
  #endif
  pid = -1;
  state = FINISHED; 
}


bool ARCer2::takeControl( )
{
  if( state == FINISHED ) {
    
    state = RUNNING;

    if ( ( pid = fork() ) == 0 ) {
      
      setpgid(0,0);
      runComputation();
      
      char sPid[100];
      sprintf(sPid, "%d", getpid());
      ofstream out(MagnusTmp::magnusTmp()+"/results."+sPid, 
		   ios::out | ios::trunc );
      if( !out ) 
	error("Cannot open an output file.");
      
      writeResults(out);
      
      out.flush();
      _exit(0);
    }
    
    if( pid < 0 )
      error("fork failed.");
    
    kill(-pid, SIGSTOP);
  }
  
  if( state == RUNNING ) {

    kill(-pid, SIGCONT);
    sleep(theDelay);
    kill(-pid, SIGSTOP);
    
    if( kill(pid, 0) != 0 ) {
      
      state = READING;
      
      //@db system porting
      
    #ifdef BSD    
      int status;
      wait(&status);
    #endif
    }
  }   
   
  if( state == READING ) {
    
    // get results of computation
    
    if( !in ) {
      
      char sPid[100];
      sprintf(sPid, "/results.%d", pid);

      //@njz: removed ios::nocreate temporarily -TODO
      //      in = new ifstream(MagnusTmp::magnusTmp()+sPid, ios::in | ios::nocreate );
      in = new ifstream(MagnusTmp::magnusTmp()+sPid, ios::in );
      //
      if( !(*in) )
	error("Cannot open an input file.");
    }
    
    if( readResults2(*in) ) {
      
      //@db Here we can delete the result file,
      //    but I want to keep it for a while.
      
      state = FINISHED;
      delete in;
      in = 0;
      return true;
    }
  }
  
  return false;
}

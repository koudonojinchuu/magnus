// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the BlackBox class.
//
// Principal Author: Roger Needham
//
// Status: experimental
//
// Revision History:


#include <signal.h>

#include "MagnusHome.h"
#include "BlackBox.h"
#include "List.h"
#include <errno.h>


ListOf< BlackBox* > BlackBox_known_BBs;
//@am Compiler complains about not
// defind operator > to input from stream need to work around to shut it up.
// The list of known black boxes.  This is so the signal handler can
// clean up all their pipes.


// The following is a signal handler which will clean up our pipes
// when we're killed, hung up on, or ^C'd.
// Unfortunately, it must be global.


//@njz:
//@db system porting
//#ifdef BSD
//extern "C"
//void BlackBox_signal_handler(int sig, int code, struct sigcontext* scp,
//			     char* addr)
//#else
//extern "C"
//void BlackBox_signal_handler(int sig)
//#endif
#if defined(BSD) &  GCC_VERSION >= 29600
extern "C"
void BlackBox_signal_handler(int sig, int code, struct sigcontext* scp,
			     char* addr)
#else
extern "C"
void BlackBox_signal_handler(int sig)
#endif
//

{
  switch (sig)
    {
    case SIGTERM:
    case SIGHUP:
    case SIGINT:
      {
		  ListIterator< ListOf< BlackBox* > > I( BlackBox_known_BBs );
		  while (! I.done() ) {
			 I.value()->~BlackBox();
			 I.next();
		  }
      }
      break;
    default:
      #ifdef DEBUG
        cerr << "BlackBox_signal_handler: Unexpected signal " << sig;
        cerr << " received" << endl;
      #endif
      break;
    }
  
  void* oldsig = (void *) signal(sig, SIG_DFL);
  if ((int)oldsig == -1) { perror("Signal"); exit(1); }
  
  int err = kill(getpid(), sig);
  if (err == -1) { perror("Kill"); exit(1); }
}




BlackBox::BlackBox(const Chars& startCommand, const Chars& restartCommand)
{
  initialize(startCommand, restartCommand);
}


BlackBox::BlackBox(const Chars& startCommand)
{
  initialize(startCommand, startCommand);
}


void BlackBox::initialize(const Chars& startCommand, const Chars& restartCommand)
{
  status = 0;         // Bad status until start or restart succeeds.
  started_p = FALSE;

  theStartCmd = startCommand;
  theRestartCmd = restartCommand;

  streamToCommand = NULL;
  streamFromCommand = NULL;

  //  *fyle_in = '\0';
  //  *fyle_out = '\0';

  // Set signal handlers

  BlackBox* tmp_ptr = this;
  BlackBox_known_BBs.append( tmp_ptr );
  void* ptr;

  ptr = (void *) signal(SIGTERM, BlackBox_signal_handler);
  #ifdef DEBUG
    if ((int)ptr == -1) perror("BlackBox::BlackBox: signal");
  #endif

  ptr = (void *) signal(SIGINT, BlackBox_signal_handler);
  #ifdef DEBUG
    if ((int)ptr == -1) perror("BlackBox::BlackBox: signal");
  #endif

  ptr = (void *) signal(SIGHUP,  BlackBox_signal_handler);
  #ifdef DEBUG
    if ((int)ptr == -1) perror("BlackBox::BlackBox: signal");
  #endif
}


Bool BlackBox::start(const char* greeting)
{
  if ( started_p ) return ( status == 1 );
  started_p = TRUE;

  // Get unique names for i/o pipes. Put in P_tmpdir defined in stdio.h.

  char* tmpStr = tempnam(MagnusTmp::magnusTmp(),"");
  strcpy(fyle_in,tmpStr);
  free(tmpStr);
  tmpStr = tempnam(MagnusTmp::magnusTmp(),"");
  strcpy(fyle_out,tmpStr);
  free(tmpStr);

  return doStart(theStartCmd, greeting);
}


Bool BlackBox::restart(const char* greeting)
{
  closeStreams();
  return doStart(theRestartCmd, greeting);
}


Bool BlackBox::doStart(const Chars& command, const char* greeting)
{
  if ( (strlen(fyle_in) == 0) || (strlen(fyle_out) == 0) ) return FALSE;

  int err;
  
  err = mkfifo(fyle_in, 0644);                  // Make a named pipe.
  if ( err ) {
  #ifdef DEBUG
    perror("BlackBox::doStart: mkfifo");
  #endif
    return FALSE;
  }
  err = mkfifo(fyle_out, 0644);                 // Make a named pipe.
  if ( err ) {
  #ifdef DEBUG
    perror("BlackBox::doStart: mkfifo");
  #endif
    return FALSE;
  }
  
  std::ostrstream system_command;
  system_command << command;
  system_command << " < " << fyle_in;
  system_command << " > " << fyle_out;
  system_command << " &" << ends;
  
  err = system( system_command.str() );
  system_command.freeze(0);                    //@rn Otherwise, memory leak
  if ( err && (errno != ECHILD) ) {
    #ifdef DEBUG
	   std::ostrstream error_msg;
		error_msg << "BlackBox::doStart: error in system call '";
		error_msg << system_command.str() << "', error code:" << err;
		error_msg << " errno:" << errno;
      error(error_msg.str());
		error_msg.freeze(0);
    #endif
    return FALSE;
  }

  //@rn freezes here if system call went bad:
  // Opening a pipe for reading blocks until a process opens the pipe
  // for writing (man 2 open).
  
  streamToCommand = new ofstream(fyle_in);
  streamFromCommand = new ifstream(fyle_out);
  // Changing the order of these causes a deadlock in the system call.
  
//@rn  if ( *streamToCommand && *streamFromCommand ) {
  if ( (streamToCommand != NULL) && (streamFromCommand != NULL) ) {
	 if ( checkGreeting(greeting) ) {
		status = 1;
		return TRUE;
	 }
  } else {
	 warn("trouble opening fstream(s) in BlackBox::doStart");
	 return FALSE;
  }
}


BlackBox::~BlackBox( )
{
  //closeStreams( );

  BlackBox* tmp_ptr = this;
  BlackBox_known_BBs.removeElement( tmp_ptr );
}


void BlackBox::closeStreams( )
{
  int err;

  if ( streamFromCommand != NULL ) {
	 streamFromCommand->close();
	 err = unlink(fyle_out);
	 if ( err ) {
		warn("trouble unlinking named pipe:");
		warn(fyle_out);
	 }	 
	 delete streamFromCommand;
	 streamFromCommand = NULL;
  }

  if ( streamToCommand != NULL ) {
	 streamToCommand->close();
	 err = unlink(fyle_in);
	 if ( err ) {
		warn("trouble unlinking named pipe:");
		warn(fyle_in);
	 }
	 delete streamToCommand;
	 streamToCommand = NULL;
  }
}


Bool BlackBox::checkGreeting(const char* greeting)
{
  if ( greeting != NULL ) {
	 char buf[128];
	 do {
		streamFromCommand->getline(buf, 128);
	 } while ( strlen(buf) == 0 );
	 int len = strlen(greeting);
	 if ( len > 127 ) len = 127;
	 buf[len] = '\0';
	 if ( strcmp(greeting, buf) ) {
		cerr << "expected to see" << endl << greeting << endl
			  << "but saw" << endl << buf << endl;
		return FALSE;
	 }
  }
  return TRUE;
}


Bool BlackBox::stillRunning()
{
  return status;
}


ostream& BlackBox::toCommand( )
{
  if ( streamToCommand == NULL )
	 error("wrote to unititialized BlackBox command ostream");
  return *streamToCommand;
}


istream& BlackBox::fromCommand( )
{
  if ( streamFromCommand == NULL )
	 error("wrote to unititialized BlackBox command istream");
  return *streamFromCommand;
}



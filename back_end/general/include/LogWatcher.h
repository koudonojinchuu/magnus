// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class LogFileWatcher.
//
// Principal Author: Dmitry Pechkin
//
// Status: under trial.
//
// Revision History:
//

#ifndef _LOG_WATCHER_H_
#define _LOG_WATCHER_H_

#include <iostream.h>
#include "Chars.h"
#include "Timer.h"

const int WATCHER_BUF_SIZE = 4096;

class LogFileWatcher
{
public:
  LogFileWatcher( const Chars& fname, int timeInterval );

  ~LogFileWatcher( );


  bool changed();
  // Returns true iff there is a line to get from the file.

  Chars getline();
  // Returns complete line and advances position in the file.

  int getInterval( ) const { return interval; }

  void setInterval( int timeInterval ) 
  { 
    interval = timeInterval;
    timer.reset( interval );
  }

private:

  long fileSize( );

  char peekCh( ) { return file.peek(); }

  char getCh( ) { ++filePos; return file.get(); }

  // Data members.

  fstream file;
  Chars fileName;
  long filePos; 


  Chars line;  // current line.
  char *buffer;
  int  bufpos;
  bool completeLine;

  Timer timer;
  int interval;

private:
  LogFileWatcher( const LogFileWatcher& watch ); // hidden, not to be implemented
  LogFileWatcher& operator=( const LogFileWatcher& watch );

};


#endif

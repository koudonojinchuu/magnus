// Copyright (C) 1997 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of class LogFileWatcher.
//
// Principal Author: Dmitry Pechkin
//
// Status: under trial.
//
// Revision History:
//

#include "LogWatcher.h"

LogFileWatcher::LogFileWatcher( const Chars& fname, int timeInterval )
  : file(fname,ios::in), fileName(fname), filePos(0), 
    buffer( new char[WATCHER_BUF_SIZE] ), bufpos(0), completeLine(false),
    timer(timeInterval), interval(timeInterval)
{
  //  file.seekg( 0, ios::beg );
}

/*
LogFileWatcher::LogFileWatcher( const LogFileWatcher& watch )
  : file(watch.file), fileName(watch.fileName), filePos(watch.filePos),
    buffer( new char[WATCHER_BUF_SIZE] ), bufpos(watch.bufpos), 
    completeLine(watch.completeLine),
    timer( watch.timer ), interval(watch.interval)
{
  memcpy( buffer, watch.buffer, bufpos );
}
*/

/*
LogFileWatcher& LogFileWatcher::operator=( const LogFileWatcher& watch )
{
  fileName = watch.fileName;
  filePos = watch.filePos;
  file.open(fileName, ios::in);

  buffer = new char[WATCHER_BUF_SIZE];
  memcpy( buffer, watch.buffer, watch.bufpos );
  bufpos = watch.bufpos;
  completeLine = watch.completeLine;

  timer = watch.timer;
  interval = watch.interval;  
}
*/

LogFileWatcher::~LogFileWatcher( )
{
  delete buffer;
}

long LogFileWatcher::fileSize( )
{
  file.seekg( 0, ios::end );
  long size = file.tellp();
  file.seekg( filePos, ios::beg );
  return size;
}


bool LogFileWatcher::changed() 
{
  if( timer.expired() && filePos != fileSize() ) {

    bool feof;
    while( !(feof = file.eof()) && peekCh() != '\n' && bufpos < WATCHER_BUF_SIZE-3 ) {
      buffer[ bufpos++ ] = getCh();
      // ++filePos;
    }

    if( !feof ) {
      if( peekCh() == '\n' ) 
	getCh();
      buffer[bufpos++] = '\n';
      buffer[bufpos++] = '\0';
      return (completeLine = true);
    }
    // wait another interval for completed line.
    timer.reset(interval);
  }
  return false;
}

Chars LogFileWatcher::getline() 
{
  Chars result;
  if( completeLine ) {
    result = buffer;
    buffer[0] = 0;
    bufpos = 0;
    completeLine = false;
  }
  return result;
}

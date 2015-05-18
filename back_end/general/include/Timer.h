// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of class Timer
//
// Principal Author: Roger Needham
//
// Status: complete
//
// Revision History:
//
// 07/96 Alexey M. implemeted IPC tools

#ifndef _TIMER_H_
#define _TIMER_H_


#include <sys/time.h>
#include <iostream.h>
#include <IPC.h>
#include "config.h"

//@db system porting

#if defined(BSD)
//@njz
//extern "C" int gettimeofday(void*, void*);
extern "C" int gettimeofday(struct timeval *, struct timezone *);
//
#elif defined(SYSV)
extern "C" int gettimeofday(struct timeval *, void *);
#endif


//---------------------------------------------------------------------------//
//--------------------------------- Timer -----------------------------------//
//---------------------------------------------------------------------------//

// The resolution is 1 milisecond, with maximum interval of 2^31 / 1000 seconds
// (24.9 days).

class Timer
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Timer(int miliseconds) { reset( miliseconds ); }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool expired( ) const {
	 timeval tv;
	 gettimeofday( &tv, 0 );
	 return ( tv.tv_sec > alarmSecs ||
				 ( tv.tv_sec == alarmSecs ) && ( tv.tv_usec >= alarmUSecs )
				 );
  }

  void reset(int miliseconds) {
	 timeval tv;
	 gettimeofday( &tv, 0 );
	 alarmSecs = tv.tv_sec + ( miliseconds / 1000 );
	 alarmUSecs = tv.tv_usec + ( miliseconds % 1000 ) * 1000;
  }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC         :                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const Timer& T )
  {
    ostr < T.alarmSecs;
    ostr < T.alarmUSecs;
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, Timer& T)
  {
    istr > T.alarmSecs;
    istr > T.alarmUSecs;
    return istr;
  }
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  long alarmSecs, alarmUSecs;

};

#endif

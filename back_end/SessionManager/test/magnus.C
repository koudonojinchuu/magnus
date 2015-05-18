// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: The Magnus back end main
//
// Principal Author: Roger Needham
//
// Status: Complete
//
// Revision History:
//

// This is in the test directory because:
//   1) With our existing makefiles, it's easiest.
//   2) We do not have a proper install protocol yet, so it doesn't really
//      matter where it goes.
//   3) So there.

#include "config.h"
#if defined(LINUX)
# include <termios.h>
# include <sys/ioctl.h>
#else
# include <sys/filio.h>
#endif
#include <iostream.h>
#include <errno.h>


//@db system porting
#ifdef BSD
  extern "C" int ioctl(int, int, void*);
#elif defined(SYSV)
  extern "C" int ioctl(int, int, ...);
#endif


#include "MagnusHome.h"
#include "SessionManager.h"

//@db temporary hack to compile class templates
#include "Polynomial.h"
#include "Matrix.h"

int main(int argc, char* argv[])
{
  //@db 2.91 temporary hack to compile class templates 
  Polynomial<Integer> p;
  Monomial<Integer> m;
  //cout < p;

  if ( argc > 1 )
	 MagnusHome::magnus_home = argv[1];
  else
	 cerr << "\nNo root path supplied on the command line; using\n"
			<< MagnusHome::magnus_home << endl << endl << flush;

  if ( argc > 1 ){
	 MagnusTmp::magnus_tmp = argv[2];      
  }
  else
	 cerr << "\nNo tmp directory supplied on the command line; using\n"
			<< MagnusTmp::magnus_tmp << endl << endl << flush;

  int temp = 1;
 
  ioctl(0, FIONBIO, &temp);  // Make stdin non-blocking.

  
  //@db system porting
  
#if defined(SYSV)
  struct sigaction sa;
  sigaction(SIGCHLD, 0, &sa);
  sa.sa_flags |= SA_NOCLDWAIT;
  sigaction(SIGCHLD, &sa, 0);
#endif

#if defined(LINUX)
  signal(SIGCHLD, SIG_IGN);
#endif

  SessionManager::start();

}

// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class ObjectSmith
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#ifndef _OBJECTSMITH_H_
#define _OBJECTSMITH_H_


#include "Menu.h"


//---------------------------------------------------------------------------//
//---------------------------- ObjectSmith ----------------------------------//
//---------------------------------------------------------------------------//


class ObjectSmith : protected Menu
{
public:
  static void outputPackagesMenuDefns(ostream& ostr);
private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  ObjectSmith( );
  // No ctors for this static class; not to be implemented.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // `Public' Members Touched by Friends:                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend class SessionManager;
  friend class Menu;     //@rn Hackery, so it can call registerCallBack.

  static void outputCheckinMenuDefns(ostream& ostr);
  static void outputToolsMenuDefns(ostream& ostr);
  static void outputMakeMenuDefns(ostream& ostr);
  static void outputTestingMenuDefns(ostream& ostr);
  // These methods are implemented in the file "menuDefns.C".
  // Used by class SessionManager.

  static void readMessage(istream& istr);
  // Used by class SessionManager.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Call Back Handlers:                                                 //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static int registerCallBack(Ctor* ctor);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static int theCallBacksLen;
  static Ctor** theCallBacks;
  static Ctor** freeCallBack;
  static const int jumpSize = 64;
};

#endif

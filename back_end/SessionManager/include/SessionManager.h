// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Declaration of class SessionManager
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//


#ifndef _SESSIONMANAGER_H_
#define _SESSIONMANAGER_H_

#include <iostream.h>
#include "ObjectSmith.h"
#include "OutMessages.h"


//---------------------------------------------------------------------------//
//------------------------- SessionManager ----------------------------------//
//---------------------------------------------------------------------------//

class SessionManager
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // No constructors for this static class.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Outgoing Messages:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  static OID getObjectSmithOid( ) { return objectSmithOid; }


private:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Restricted Access:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend int main(int argc, char* argv[]);

  static void start( );
  static void initializeFrontEnd( );
  static bool areMessages( );
  static void readMessages( );
  static void readMessage(istream& istr);
  static void takeControl( );

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // SessionManager dispatches incoming messages.
  // Generic message handling could be implemented with a class
  // MessageHandler, which would provide `mailbox ids' and automatic
  // registration for its derivates in the same way class SMObject
  // provides object ids.
  // But the recipient of a message can now only be a SMObject, or one
  // of the special cases: SessionManager, ObjectSmith.
  // So for now we assign `pseudo' object ids to the latter, and use
  // ordinary object ids for the `mailbox ids'.
  // Class SMObject provides a message reading interface.

  static const int sessionManagerOid = -1;
  static const int objectSmithOid = -2;
  static const int databaseManagerOid = -3;
  static const int randomDefnGeneratorOid = -4;
  // Some messages are handled by SessionManager directly. These are
  // the message tags:

  enum MessageTag { QUIT, DELETE, SET_NAME };

  static const long magicCookie;
  // Will be used for low-rent message authentication.

  static bool quitRequested;
};

#endif

// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of interface class OID
//
// Principal Author: Roger Needham
//
// Status: complete
//
// Revision History:
//

#ifndef _OID_H_
#define _OID_H_


#include <iostream.h>
#include "IPC.h"


//---------------------------------------------------------------------------//
//------------------------------- OID ---------------------------------------//
//---------------------------------------------------------------------------//

class OID
{
public:

  bool operator == ( const OID& oid ) const {
	 return theOID == oid.theOID;
  }

  bool operator != ( const OID& oid ) const {
	 return theOID != oid.theOID;
  }

  inline friend ostream& operator << ( ostream& ostr, const OID& oid ) {
	 return ostr << oid.theOID;
  }

  int hash( ) const { return theOID; }

  int unwrap( ) const { return theOID; }
  // We don't want a conversion, even for friends, because of possible
  // ambiguity. Making this private creates friendship problems for
  // classes in the scope of another class. It seems safe enough to
  // permit public unwrapping, since only friends can rewrap.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const OID& oid )
  {
    ostr < oid.theOID;
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, OID& oid )
  {
    istr > oid.theOID;
    return istr;
  }

private:

  friend class Ctor;
  friend class SessionManager;
  friend class TheObjects;
  friend class SMFPGroup;
  friend class RunPackage; // @am need to find SMObject by its OID, why is it private?
  friend class DatabaseManager;

  OID( int i ) : theOID( i ) { }
  // Used by class Ctor, class SessionManager, class TheObjects.

/*@@db
  OID( );  // Hidden, not to be implemented.
*/

  int theOID;

 
  //@@db temporary declaration.

  
public:

  OID( ) : theOID( 0 ) { }
  
};

#endif

// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of interface class ARCSlotID
//
// Principal Author: Roger Needham
//
// Status: complete
//
// Revision History:
//

#ifndef _ARCSLOTID_H_
#define _ARCSLOTID_H_


#include <iostream>


//---------------------------------------------------------------------------//
//---------------------------- ARCSlotID ------------------------------------//
//---------------------------------------------------------------------------//

class ARCSlotID
{
public:

  bool operator == ( const ARCSlotID& asi ) const {
	 return theARCSlotID == asi.theARCSlotID;
  }

  bool operator != ( const ARCSlotID& asi ) const {
	 return theARCSlotID != asi.theARCSlotID;
  }

  inline friend ostream& operator << ( ostream& ostr, const ARCSlotID& asi ) {
	 return ostr << asi.theARCSlotID;
  }

  int unwrap( ) const { return theARCSlotID; }
  // We don't want a conversion, even for friends, because of possible
  // ambiguity. Making this private creates friendship problems for
  // classes in the scope of another class. It seems safe enough to
  // permit public unwrapping, since only friends can rewrap.

protected:

  friend class ComputationManager;
  friend class Supervisor;
  friend class EnumeratorSupervisor;

  ARCSlotID( int i ) : theARCSlotID( i ) { }
  // Used by classes ComputationManager,  Supervisor.

  ARCSlotID( );  // Hidden, not to be implemented.

  int theARCSlotID;

};


struct ThisARCSlotID : public ARCSlotID {
  ThisARCSlotID( ) : ARCSlotID( 0 ) { }
};
// By convention, ARCSlotID( 0 ) refers to the Supervisor itself.
// See Supervisor.h.

#endif

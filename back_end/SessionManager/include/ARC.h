// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Declaration of interface class ARC
//
// Principal Author: Roger Needham
//
// Status: complete
//
// Revision History:
//

#ifndef _ARC_H_
#define _ARC_H_


#include <iostream.h>


//---------------------------------------------------------------------------//
//------------------------------- ARC ---------------------------------------//
//---------------------------------------------------------------------------//

class ARC
{
public:

  ARC( ) : theARC( 0 ) { }

  operator int ( ) const { return theARC; }

  ARC operator + ( const ARC& arc ) const {
	 return ARC( theARC + arc.theARC );
  }

  ARC operator - ( const ARC& arc ) const {
	 return ARC( theARC - arc.theARC );
  }

  ARC operator += ( const ARC& arc ) {
	 theARC += arc.theARC;
	 return *this;
  }

  ARC operator -= ( const ARC& arc ) {
	 theARC -= arc.theARC;
	 return *this;
  }

  inline friend ostream& operator << ( ostream& ostr, const ARC& arc ) {
	 return ostr << arc.theARC;
  }

protected:

  friend class ComputationManager;
  friend class Supervisor;

  ARC( int i ) : theARC( i ) { }
  // Used by classes ComputationManager, Supervisor.

private:

  int theARC;

};


struct ZeroARC : public ARC { ZeroARC( ) : ARC( 0 ) { } };

struct OneARC : public ARC { OneARC( ) : ARC( 1 ) { } };

#endif

// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the Elt class.
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
//

#include "Elt.h"


Elt commutator( const Elt& e1, const Elt& e2 ) {
  return Elt( e1.look()->commutatorWith( e2.look() ) );
}

Elt  Elt::raiseToPower( Integer power ) const {
    Elt tmp;
    for (int i = 0;i<power;i++)
      tmp = Elt(*tmp.look() * *look());
    return tmp;
}

void Elt::debugPrint( ostream& o ) const {
  
      o << "this:" << this << "->" << "[";
      look()->debugPrint(o);
      o << "]";
}

ostream& operator << ( ostream& ostr, const Elt& e ) {
  e.look()->printOn(ostr);
  return ostr;
}


//------------------- static member initialisers --------------------//

const Type EltRep::theEltType = Type( Type::unique() );
const Type EltIdentityRep::theEltIdentityType = Type( Type::unique() );

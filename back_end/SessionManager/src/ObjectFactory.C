// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class 
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//

#include <iostream.h>
#include "SMFPGroup.h"
#include "ObjectFactory.h"
//#include "FreeGroup.h"

SMObject* SymmetricGroupFactory::factory() const
{

  if( n < 2 ) 
    return 0;

  // We use Moore's presentation (1897) of symmetric group Sn
  // Sn = < a,b | a^n, b^2, (a b)^(n-1), (b A b a)^3, (b A^j b a^j)^2 > 
  // where 
  //       a = (1 2 3 .. n), 
  //       b = (1 2)
  //       j=2,..,n-2.

  FreeGroup F(2);
  Generator a(1);
  Generator b(2);

  Word R1 = F.raiseToPower( Word(a), n ); 
  //  for( int i = 0; i<n; ++i) {    R1 = R1 * a;  }
  Word R2 = b * b;
  Word R3 = F.raiseToPower( a * b, n-1 );
  //  for( int i = 0; i < n-1; ++i ) {    R3 *= a * b;  }
  Word R4 = F.raiseToPower( b * inv(a) * b * a, 3);
  

  SetOf<Word> relators;

  relators |= R1;
  relators |= R2;
  relators |= R3;
  relators |= R4;// * R4 * R4;

  Word A = inv(a);
  for( int j = 2; j < n-1; ++j ) {
    A = A * inv(a);
    Word R_j = b * A * b * A.inverse();
    relators |= R_j * R_j;
  }

  VectorOf<Chars> names(2);
  names[0] = "a";
  names[1] = "b";

  FPGroup G( names, relators );

  //@njz
  //  ostrstream heritage;
  std::ostrstream heritage;
  //
  heritage << "Symmetric group S" << n << ends;
  
  SMObject *smo = new SMFPGroup( G, heritage.str(), SMFPGroup::FP );

  heritage.freeze(0);

  return smo;
}

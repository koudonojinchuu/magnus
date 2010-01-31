// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents:
//
// Principal Author: Stephane Collart
//
// Status:
//
// Revision History:
//

#include "Type.h"
#include "PureRep.h"
#include "PureObject.h"

#include <iostream>


struct AR : PureRep {

	static const Type theAType;

	virtual Type actualType( ) const { return theAType; }

	PureRep* clone( ) { return new AR(*this); }

};

struct A : PureObject {

	A( ) : PureObject( new AR ) { }

	Type actualType( ) { return look()->actualType(); }

	const AR* look( ) const { return (AR*)PureObject::look(); }

};

const Type AR::theAType = Type( Type::unique() );

main ( ) {

A a;

cout << a.actualType();

Type t = Type( Type::notype() );
// @stc Type t( Type::notype() ) makes g++ 2.6.0 think this is a function
// Type ()( Type::notype (*)() ) declaration -- need to check whether
// that interpretation is legitimate.

cout << endl << t;

}

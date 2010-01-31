// Principal Author: Stephane Collart


#include <iostream>

#include "PureRep.h"
#include "PureObject.h"


struct AR : virtual PureRep {
	PureRep* clone( ) { return new AR(*this); }
};

struct A : virtual PureObject {
	A() : PureObject(new AR) { }
};

struct BR : virtual AR {
	PureRep* clone( ) { return new BR(*this); }
};

struct B : virtual A {
	B() : PureObject(new BR) { }
};

main ( ) {

cout << sizeof(A);

cout << endl << sizeof(B);

}

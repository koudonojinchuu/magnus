// Principal Author: Stephane Collart

#include "RefCounter.h"
#include "ObjectOf.h"

#include <iostream.h>

struct R : RefCounter {

R* clone( ) { return new R(*this); }

};

class O : protected ObjectOf<R> {

public:

	O( ) : Rep( new R ) { }
};

main ( ) {

	O x;
	O y = x, z = y;

	cout << endl;
	x.debugPrint(cout);
	cout << endl;
	y.debugPrint(cout);
	cout << endl;
	z.debugPrint(cout);

	z.change();

	cout << endl;
    z.debugPrint(cout);
	cout << endl;
	x.debugPrint(cout);
	cout << endl;
	y.debugPrint(cout);

}

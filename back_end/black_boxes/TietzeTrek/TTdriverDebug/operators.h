#ifndef Frank_operators
#define Frank_operators
#include "Ftypes.h" // a lot of code uses the definitions 
					// in this file

#include "FResources.h"
#include <stdio.h>
#include "FGLvarInterface.h"
class FxWord;




#ifdef Frank_GenuineMac

// In the Genuine Mac version we set up a "fake" iostream capability
// to maintain a high level of compatibility with the SUN version	


#include "CLogPane.h"

class FrankStream{
	public:
	CLogPane *xgetDoc(void);  	// stream to Log window and/or file
};
typedef FrankStream ostream;

#else
#include <iostream.h>
#endif

void operator+(FGLvarInterface *v,const GLref& ref);

void operator-(FGLvarInterface *v,const GLref& ref);
Ptr operator&(FGLvarInterface *v,const GLref& ref);
 

// the following operators stream to both window and file
// in the SUN version.  The Mac version is controlled dynamically
// by the user.

ostream& operator<=(ostream& s,const Str255 str);
ostream& operator<=(ostream& s,const char* str);
ostream& operator<=(ostream& s,const short num);
ostream& operator<=(ostream& s,const int num);
ostream& operator<=(ostream& s,const long num);
ostream& operator<=(ostream &s,const char aChar);
ostream& operator<=(ostream &s,const unsigned char aChar);
ostream& operator<=(ostream &s,const Handle han);
ostream& operator<=(ostream& s,const GLref& ref);
ostream& operator<=(ostream& s,FxWord& x);
#endif


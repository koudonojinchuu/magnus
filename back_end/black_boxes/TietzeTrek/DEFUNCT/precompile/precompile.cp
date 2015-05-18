
// select the desired compiling options by 
// uncommenting the appropriate definitions

// if Frank_GenuineMac is defined, then the Macintosh interface
// will be used.  Otherwise the iostream interface will be used.

#define Frank_GenuineMac

#ifdef Frank_GenuineMac
#include <TCL #includes.cp>
#endif

//#define Frank_MemoryDebugging


// bedrock classes
#include "GLref.h"
#include "FGLink.h"
#include "FGLrecycleStack.h"
#include "FGLstack.h"
#include "GLfunctions.h"

// FBlockManager cannot be precompiled because it needs the
// operator declarations, and these cannot be precompiled because
// they cannot be prepended to the operator definitions file.
// Its a catch 22.  However, we can still include FList.h which
// otherwise would not be dragged in.

#include "FList.h"

// FGLnode classes

#include "FGLnode.h"
#include "FGLwalkNode.h"

// FGLvar classes

#include "FGLvar.h"
#include "FGLvarHigh.h"
#include "FGLvarInitInfo.h"
#include "FGLvarInterface.h"
#include "FGLvarLow.h"
#include "FGLvarTypes.h"


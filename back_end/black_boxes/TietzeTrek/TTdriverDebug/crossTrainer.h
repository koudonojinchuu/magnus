#ifndef Frank_crossTrainer
#define Frank_crossTrainer

#include "Ftypes.h" // a lot of code uses the definitions 
					// in this file

// the file operators.h declares iostream capabilities for both Mac and SUN
#include "operators.h"
#ifdef Frank_GenuineMac
// the genuine mac version defines its own stream cout for output.  cout is
// of class FrankStream, and ostream is typedef'ed to be a FrankStream.  Thus
// ostream can be used generically for all versions and FrankStream is hidden.

extern ostream cout;

#else
// the SUN version uses fout for stream file out.  Use of this stream is 
// hidden by using the <= operators instead of the << operators.  In all 
// versions, <= streams to both file and windown.

#include <fstream.h>
extern ofstream fout;
#endif

extern ostream &memout;
extern ostream &tout;
extern ostream &gout;
extern Boolean xDEBUG;

#ifndef Frank_GenuineMac
extern OSErr memFullErr;
#endif

// The C compiler on the SUN doesn't allow much
// nesting of include files.  To get around this
// limitation, we load in all the primary include 
// files first to prevent nesting from getting very 
// deep at the higher levels.

#ifndef Frank_GenuineMac
#include "FtoolBox.h"
#include "FObject.h"
#include "FCollection.h"
#include "FArray.h"
#include "FCluster.h"
#include "FList.h"
#include "FGLstack.h"
#include "FGLvarLow.h"
#include "FGLvarHigh.h"
#include "FGLvar.h"

#endif
// FBlockManager cannot be precompiled because it needs the
// operator declarations, and these cannot be precompiled because
// they cannot be prepended to the operator definitions file.
// Its a catch 22. 

#include "FBlockManager.h"


// the folowing are precompiled

// bedrock classes
#include "GLref.h"
#include "FGLink.h"
#include "FGLrecycleStack.h"
#include "FGLstack.h"
#include "GLfunctions.h"
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

#endif

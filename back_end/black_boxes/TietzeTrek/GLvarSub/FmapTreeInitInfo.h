/*******************

Written by Frank Rimlinger, May 1993
*******************/

#ifndef Frank_FmapTreeInitInfo
#define Frank_FmapTreeInitInfo


#include "FGLvarInitInfo.h"

class FmapTreeInitInfo: public FGLvarInitInfo{
public:

	FmapTreeInitInfo(long numItems);
	long numItems;
};
#endif



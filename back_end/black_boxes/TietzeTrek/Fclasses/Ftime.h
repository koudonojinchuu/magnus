// stdio seems to have some incompatibilities
// when it comes to time.  This patch is to
// help smooth some ruffled feathers
#ifndef Frank_timePatch
#define Frank_timePatch
#include <time.h>
#ifndef THINK_CPLUS
// on the SUN, clock is a trap that is 
// is defined in stdlib.
#include <stdlib.h>
#define SUN_CLOCKS_PER_SEC 1000000
#define ONE_SECOND SUN_CLOCKS_PER_SEC

//@njz:
//typedef long clock_t;
#ifdef BSD
typedef long unsigned int clock_t;
//#else
//typedef long clock_t;
#endif
//

#else
#define ONE_SECOND CLOCKS_PER_SEC
#endif
#endif

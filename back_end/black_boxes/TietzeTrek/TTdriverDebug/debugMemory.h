#ifndef Frank_debugMemory
#define Frank_debugMemory

#ifdef Frank_MemoryDebugging
#include "operators.h"
ostream& repMemSize(ostream& s,long mem);
void resetMemory(void);
ostream& repMemory(ostream& s);
ostream& repHandleAllocate(ostream& s,Handle handle);
ostream& repHandleDeallocate(ostream& s,Handle handle);
ostream& repSetHandleSize(ostream& s,Handle handle);
ostream& repPtrDeallocate(ostream& s,Ptr ptr);
ostream& repPtrAllocate(ostream& s,Ptr ptr);
#endif

#endif


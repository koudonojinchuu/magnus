/* macros for coset table  */
#ifndef __MISCELLANEOUS__
#define __MISCELLANEOUS__
#define Swap(i,j)       { int swapp; swapp=i;i=j;j=swapp;}
#define Incomplete      i == 0
#define Complete           i
#define Num2Addr(i)       ((((i) - 1) * (NCOL) + 1))
#define Addr2Num(i)       ((((i) - 1) / (NCOL) + 1))
#define Space_overflow    tc_text(13, tc_pvar); \
                           tc_text(21, tc_pvar); \
                           OVERFL = TRUE; \
                           return
#endif



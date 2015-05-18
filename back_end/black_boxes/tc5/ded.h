/* include file for using deduction stack.  */
#include "miscellaneous.h"
/* deduction stack.
* assume that DED_PT is the pointer which points to the  top  of the
 stack.
* And the value at the top of the stack is 'row', next one is 'col'.
the following three macros give the operations with the stack:
*/
#ifndef __DED__
#define __DED__
#define Save_ded(row,col)   if(DED_PT - 2 <= &CT(NEXTDF,1)) {\
				DEADDE++; \
                                  DEDWNG = TRUE; \
                            } else { \
                                    *--DED_PT = col;  \
                                    *--DED_PT = row; \
                            }
#define Get_ded(row,col,res)  row=*DED_PT++; /* it means *(DED_PT++)  */ \
                        col=*DED_PT++; \
                        res=CT(row,col)
#endif

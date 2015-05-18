/* File check_index1.c */
#include "tc.h"
#include "ct.h"

Void
tc_check_index1(tc_pvar)
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
register Int	*ct;
register Int	j;
register Int	i;
register Int	ncol;
	ct = COSET_TABLE;
	ncol = NCOL;
 	for (j = 2; j <= NCOL; j++) { 
                if (CT(1, j)) 
                        continue; 
/* it is possible to have a zero entry in row 1. look down all rows in
* this column looking for a non-zero entry.
*/
                for(i=NCOL+1;i<NEXTDF;i+=NCOL) 
                        if (CT(i, j)) 
                                goto conti;
		return;
conti:		continue;
         }
/* we have a group of INDEX 1, set INDEX1, set all entries in the
* first row to 1 and return.
*/
         INDEX1 = TRUE; 
	 COMPCT = TRUE; 
         NEXTDF = COSET1 + COSET; 
         for (i = 1; i <= NCOL; i++)
                CT(1, i) = 1;
         return;
}


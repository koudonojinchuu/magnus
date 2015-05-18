/* File compact.c */
#include "compact.h"

Void
tc_compact(tc_pvar)
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
register Int    *ct;
register Int    *inv_to_col;
register Int    ncol;
register Int	irow;
register Int 	k;
register Int 	i;
register Int	j;
register Int	upper;
register Int	col;
register Int	hlt;
register Int	fel;
register Int	tab;
register Int	count;
register Int	*pt;
/* subroutine to remove unused coincident cosets from the coset table.
* COMPCT -- logical variable which is .true. whenever the coset table
* has no unused rows in it, otherwise .false.
*/
	{ Init; }
#ifdef __DI
	if(DIAG[3]) {
	char *tt;
	tt = STATUS;
	STATUS = "CM";
	tc_text(11, tc_pvar);
	STATUS = tt;
	}
#endif
/* find the smallest redundant coset irow.  */
	for (irow = COSET1; irow <= upper; irow += COSET)
		if (DeadCoset(irow))
			break;
/* compact Deduction stack first.  */
	if(HaveDed) {
		{ CountOld_New; }
		{ FixDed;  }
	}
/* compact the coset table.  */
	for (i = irow; i <= upper; i += COSET) 
                if (CT(i, 1) < 0) { 
			if (i <= KN_Hlt)
                        	hlt++;
			if (i <= KN_Fel)
				fel++;
			if (i <= TABROW)
				tab++;
                } else {
/* convert row i to row irow.  */
                    for (col = 1; col <= ncol; col++) {
                        j = CT(i, col);
                        if (j) {
                               if (j == i)
                                     j = irow;
                               else {
                                     k = inv_to_col[col];
                                     CT(j, k) = irow;
                               }
                        }
                        CT(irow, col) = j;
                    }
                    irow += ncol;
                }
/* adjust counters.  */
	{ AdjustKnNextdf;  }
#ifdef __DI
	if(DIAG[3]) {
	char *tt;
	tt = STATUS;
	STATUS = "CM";
	tc_text(11, tc_pvar);
	STATUS = tt;
	}
#endif
	return;
}


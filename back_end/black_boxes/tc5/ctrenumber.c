/* File ctrenumber.c  */
#include "ct.h"
#include "tc.h"
#include "miscellaneous.h"

Void
tc_ctrenumber(tc_pvar)
struct  tc_str_vars     *tc_pvar;
{
/* subroutine to change the addresses of cosets back into the number. */
register Int    *p;
Int    *ct;
register Int	*t;

	if (STDCT == TRUE)
		return;
	ct = COSET_TABLE;
	p = &CT(NEXTDF,1);
	for (t = &CT(COSET1,1); t < p; t++)
		*t = (*t) ? Addr2Num(*t) : 0;
	STDCT = TRUE;
	return;
}
tc_unrenumber(tc_pvar)
struct  tc_str_vars     *tc_pvar;
{
register Int    *p;
register Int    *ct;
/* subroutine to change the coset numbers into their addresses.   */

	if (STDCT == FALSE)
		return;
	ct = COSET_TABLE;
        p = &CT(NEXTDF,1);
        for (ct = &CT(COSET1,1); ct < p; ct++)
                *ct = (*ct) ? Num2Addr(*ct) :0;
	STDCT = FALSE;
   	return;
}
/* File End */

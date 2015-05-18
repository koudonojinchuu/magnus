/* File translate.c  */
#include "tc.h"

Void
tc_translate(tc_pvar)
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
Int	*pt;
Int	i;
Int	k;
/* translate the subgroup generators and group defining relators 
*  from strings in terms of generators and inverses to strings 
*  in terms of their associated column numbers in the coset table.
*/
	for (pt = SUBGROUP_GEN; pt < SUBGROUP_GEN + SGEN_LEN; pt++)
		*pt = GEN_TO_COL[*pt];
	for (i = 1; i <= NDREL; i++) {
                k = RELATORS[RELATOR_INDEX[i]];
                if (RELATOR_LENGTH[i] == 2 && RELATOR_EXPONENT[i] == 2 && GEN_TO_COL[k] == GEN_TO_COL[-k])
                        RELATOR_EXPONENT[i] = -2;
        }
	for (pt = RELATORS; pt < RELATORS + 2 * REL_LEN; pt++)
		*pt = GEN_TO_COL[*pt];
	return;
}

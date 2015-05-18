/* File subg_proc.c  */
#include "tc.h"
#include "coset_def.h"

Void
tc_subg_proc(tc_pvar)
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
register Int	subg;
register Int	*beg;
register Int	*end;
register Int	i;
register Int	rel;
/* for cyclic perms  */
Int     base_len, cyclic_perms, cyclic_count;

/* subroutine to process subgroup generators including relators in subgroup;
*/
	for(subg = 1; subg <= NSGPG; subg++) {
		beg = &SUBGROUP_GEN[SUBG_INDEX[subg]];
		end = beg - 1 + SUBG_LENGTH[subg];
		tc_apply(1, beg, end, TRUE, tc_pvar);
		if (OVERFL) {
                    tc_text(13, tc_pvar);
                    tc_text(21, tc_pvar);
                    return;
                }
                if (INDEX1) {
/* there are no more cosets to be searched so the multiplication table is
* complete.
*/
                    tc_text(12, tc_pvar);
                    INDEX = TRUE;
                    return;
                }
	}
	if (RELATORS_IN_SUBGROUP  > 0)
              rel = min0(RELATORS_IN_SUBGROUP , NDREL);
        else if (RELATORS_IN_SUBGROUP == 0)
                rel = NDREL;
        else rel = 0;
/* apply relator_in_subgroup    */
	for (i = 1; i <= rel; i++) {
/* for cyclic perms 
		beg = &RELATORS[RELATOR_INDEX[i]];
*/
           base_len = RELATOR_LENGTH[i] / RELATOR_EXPONENT[i];
           if (PAGE_STRATEGY == 'R')         
            cyclic_count = base_len;
        else         
            cyclic_count = 1;
        for (cyclic_perms = 0; cyclic_perms < cyclic_count; cyclic_perms++) {
            beg = &RELATORS[RELATOR_INDEX[i] + cyclic_perms];
                end = beg + RELATOR_LENGTH[i] - 1;
		tc_apply(1, beg, end, TRUE, tc_pvar);
                if (OVERFL) {
                    tc_text(13, tc_pvar);
                    tc_text(21, tc_pvar);
                    return;
                }
                if (INDEX1) {
/* there are no more cosets to be searched so the multiplication table is
* complete.
*/
                    tc_text(12, tc_pvar);
                    INDEX = TRUE;
                    return;
                }
           }
	}        
	return;
}

Void
tc_newrel_newsg(tc_pvar)
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
register Int subg;
register Int i;
register Int j;
register Int *beg;
register Int *end;
Int	*ct;
Int     base_len, cyclic_perms, cyclic_count;

	ct = COSET_TABLE;
	TABROW = 0;
	OVERFL = FALSE;
	STATUS = "NS";
	INDEX = FALSE;
	INDEX1 = FALSE;
	DEDWNG = FALSE;
	DEADDE = 0;
	HOLES = 0;
	OVERFL = FALSE;
	DED_PT = DED_END;
	if (NEWSG)
	for (subg = NSGPG - NEWSG + 1; subg <= NSGPG;  subg++) {
                beg = &SUBGROUP_GEN[SUBG_INDEX[subg]];
                end = beg - 1 + SUBG_LENGTH[subg];
                tc_apply(1, beg, end, TRUE, tc_pvar);
                if (OVERFL) {
                    tc_text(13, tc_pvar);
                    tc_text(21, tc_pvar);
                    return;
                }
                if (INDEX1) {
/* there are no more cosets to be searched so the multiplication table is
* complete.
*/
                    tc_text(12, tc_pvar);
                    INDEX = TRUE;
                    return;
                }
        }
	STATUS = "NR";
	if (NEWREL)
	for (i = COSET1; i < NEXTDF; i += NCOL) {
		if (CT(i, 1) < 0) 
			continue;
		for(j = NDREL - NEWREL + 1; j <= NDREL; j++) {
/* comment out for cyclic perms  
			beg = &RELATORS[RELATOR_INDEX[j]];
*/
        base_len = RELATOR_LENGTH[j] / RELATOR_EXPONENT[j];
        if (PAGE_STRATEGY == 'R')         
            cyclic_count = base_len;
        else         
            cyclic_count = 1;
        for (cyclic_perms = 0; cyclic_perms < cyclic_count; cyclic_perms++) {
            beg = &RELATORS[RELATOR_INDEX[j] + cyclic_perms];
			end = beg + RELATOR_LENGTH[j] - 1;
			tc_apply(i, beg, end, FALSE, tc_pvar);
                	if (INDEX1) {
/* there are no more cosets to be searched so the multiplication table is
* complete.
*/
                    		tc_text(12, tc_pvar);
                    		INDEX = TRUE;
                    		return;
                	}
			if (CT(i, 1) < 0)
				break;
       	}
            if (CT(i, 1) < 0)
                break;
	}
        }
}

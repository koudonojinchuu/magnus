/* File set_relators.c  */
#include "tcyacc.h"
#include "tc.h"

Void
tc_set_relators(tc_pvar)
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
register Int    j;
register Int    m;
register Int    i;
register Int    length;
register Int    first;
register Int    second;
register Rel_stk_type *ptr;

/* subroutine to read in the group relators and subgroup generators.
* each group generator used must be between 1 to NDGEN.
* make two copies of each relator for convenience in deduction process.
* set relator_index, relator_exponent and relator_length.
*/
	second = 0;
	i = 1;
	for(ptr = Relators; ptr != (Rel_stk_type *) 0; ptr = ptr->next) {
		length = ptr->len;
		RELATOR_LENGTH[i] =  length;
		RELATOR_EXPONENT[i] = ptr->exp;
		first = second;
		second = first + length;
		RELATOR_INDEX[i] = first;
		for(j = 0; j < length; j++) {
			RELATORS[first++] = ptr->rel_n[j];
			RELATORS[second++] = ptr->rel_n[j];
		}
		i++;
	}
/*  read in subgroup generators. */
	i = 1;
	m = 0;
	for(ptr = S_gens; ptr != (Rel_stk_type *) 0; ptr = ptr->next) {
		SUBG_LENGTH[i] = ptr->len;
		SUBG_INDEX[i] = m;
		for(j = 0; j < ptr->len; j++)
			SUBGROUP_GEN[m++] = ptr->rel_n[j];
		i++;
	}
	return;
}

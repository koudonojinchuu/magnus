/* File assemble_edp.c  */
#include "tc.h"

Void
tc_assemble_edp(tc_pvar)
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
register Int    i;
register Int    k;
register Int    l;
register Int    m;
register Int    ii;
register Int    start;
register Int    wm;
register Int    rel;
register Int    gen;

/*
* subroutine to assemble the pointers to essentially different positions.
* in the case of a felsch enumeration, note where each generator occurs
* in a unexpanded group defining relator. these positions correspond to
* each essentially different position for each generator for that group
* defining relator. EDP_BEG(generator) points to a list set up in array
* EDP for the essentially different positions for each
* defining relator for that generator. this list consists of the number
* of positions involved for a relator followed by the position in array
* RELATORS where this generator occurs in that relator for each group
* definitions of arrays:
* RELATOR_INDEX---index of relators. RELATOR_INDEX[i] gives the 
* 	  beginning position of ith relator in array RELATORS.
* RELATOR_LENGTH---length of relators. RELATOR_LENGTH[i] gives length of ith
*         relator (original length).
* EDP_BEG---index of essentially different positions for each generator.
*         EDP_BEG[i] gives beginning positions in array edp for generator i.
*/
	start = 0;
	for (gen = 1; gen <= NDGEN; gen++) { 
		i = GEN_TO_COL[gen];
          	ii = GEN_TO_COL[-gen];
/* set the start position of this gen in the array edp.  */
		EDP_BEG[i] = start;
		for (rel = 1; rel <= NDREL; rel++)  
/* k points to the beginning and l to the end of base of a relator
* (first of the two). look through each relator for occurrences
* of generator gen or its inverse and note this position in edp.
* RELATOR_EXPONENT[rel] < 0 means this relator is an involutory generator 
*  relator.  
*/
		if (RELATOR_EXPONENT[rel] > 0) {
			k = RELATOR_INDEX[rel];
			l = k - 1 + RELATOR_LENGTH[rel] / RELATOR_EXPONENT[rel];
			for (m = k; m <= l; m++) {
				wm = RELATORS[m];
				if (wm == i || wm == ii) {
					EDP[start++] = m;
					EDP[start++] = RELATOR_LENGTH[rel];
				}
			}
		}
/* set the end position of this gen in the array EDP.  */
		EDP_END[i] = start - 2;
	}
	return;
}

/* file coinc.c  */
#ifdef __DI
#include "tcyacc.h"
#endif
#include "coinc.h"

Void
tc_coinc(lows,highs,tc_pvar)
Int	lows;
Int 	highs;
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
register Int 	*ct;
register Int 	low;
register Int 	high;
register Int 	i;
register Int 	j;
register Int 	highi;
register int	lowi;
register Int	*ppt;
register Int	*rpt;
/* subroutine to process the primary coincidence coset lows=coset highs
* and its consequences.
* CTAIL -- the tail of the coincidence queue.
* CHEAD -- the head of the coincidence queue.
* within the coincidence processing CT(high,2) is used to link the
* coincidence queue together, CT(high,1) contains minus the value of
* the equivalent (lower numbered) coset (the minus sign acting as a
* redundancy flag).
* this routine utilizes a principle suggested by beetham (space saving
* in coset enumeration, unpublished) but not the data structure. at
* the suggestion of c c simms it uses the data structure used in
* cannon, dimino, havas, watson with some modifications.
*/
	Init;
/* start of coinc processing report  */
#ifdef __DI
	if(DIAG[4]) {
	char *tt;
	tt = STATUS;
	STATUS = "CS";
	tc_text(11,tc_pvar);
	fprintf(fout,"******* HIGH= %d    LOW= %d *******\n",Addr2Num(high),Addr2Num(low));
	STATUS = tt;
	}
#endif
/* process columns 1 and 2 of the primary coincidence. */
	tc_coinc_cols12(low,high,tc_pvar);
	if (INDEX1)
                return;
	for(;;) {
		if(NCOL > 2)
/* process columns 3 to NCOL of the coincidence high=low. */
		for(i = 3; i <= NCOL; i++) {
/* highi -- column i entry of coset high. */
			highi = CT(high, i);
			if (highi == 0)
				continue;
			j = INV_TO_COL[i];
/* delete CT(highi,j) at this stage rather than replace by low to
* avoid having two occurrences of low in the one column. 
*/
			CheckHighi;
/* lowi -- column i entry for coset low. */
			lowi = CT(low, i);
			if (lowi) {
				if(lowi == high)
					lowi = low;
/* we have found a possibly new coincidence highi=lowi. */
				tc_coinc_cols12(lowi, highi, tc_pvar);
				if (INDEX1)
					return;
                  	} else {
/* mark a new deduction to be processed later. */
				CT(low, i) = highi;
#ifdef __DI
        if (DIAG[7]) {
        int n,*pt_d;
        char ch;
        n = COL_TO_GEN[i];
        fprintf(fout,"**** C %d*",Addr2Num(low));
        if(n < 0) {
                ch = Gen_st[-n].gen;
                fprintf(fout,"%c",toupper(ch));
        } else {
                ch = Gen_st[n].gen;
                fprintf(fout,"%c",ch);
        }
        fprintf(fout,"=%d\n",Addr2Num(highi));
	}
#endif
				if(!VeryFirst)
					Save_ded(low,i);
			}
			CheckSet(low,i);
		}
/* get the next coincidence from the queue for processing. */
		Pull(high,low);
	} /* end of 'for(;;)'  */
/* see if row KN becomes redundant.  */
	CheckIntnal;
	CheckKN;
/* delete all entries referencing dead cosets from the deductions stack.
*/
	CheckDed;
/* end of coinc processing report */
#ifdef __DI
	if(DIAG[3]) tc_text(11,tc_pvar);
#endif
}
		

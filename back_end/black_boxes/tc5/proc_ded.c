/* file of proc_ded.c  */
#ifdef __DI
#include "tcyacc.h"
#endif
#include "proc_ded.h"

Void
tc_proc_ded(tc_pvar)
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
register Int    *beg;
register Int    *f_end;
register Int    *b_end;
register Int    *ct;
register Int    *inv_to_col;
register Int    iback;
register Int    ifront;
register Int    *forscan;
register Int    *backscan;
register Int    *edpPt;
register Int    *end;
Int    i;
register Int    j;
register Int    ncol;
Int     icol;
Int     col;
Int     irow;
Int     ires;
Int     curtim;
Int	row;
Int	ji;
float	utl_second();

/* subroutine to do deduction processing. treate the coset table
*  as a deduction queue. when new deductoins arise, save them in
*  the deduction stack.
*/
	{ Init;  }
	for(row = COSET1; row < NEXTDF; row += COSET) {
		if(CT(row, 1) < 0)
			continue;
		for(col = 1; col <= ncol; col++) {
/* test if it could be looked as a right deduction, which means
*  that 
*/
			if(CT(row, col) == 0 || row > CT(row, col))
				continue;
			else {	
				icol = col;
				irow = row;
				ires = CT(row, col);
				NonInverse(irow,icol,ires);
ProcRes:Get_edp(edpPt,end,icol);
/* go through edps which relevant for icol.  */
        for(; edpPt<=end; edpPt+=2) {
/* scan forward through relator (stored in relators[]) as long as table
* entries are present, starting at the new deduction.
* computing beginning position beg and forward scan end position f_end.
*/
                beg = &RELATORS[*edpPt];
                Set_ifront_iback;
                f_end = beg + *(edpPt + 1) - 1;
                for(forscan=beg+1; forscan<=f_end; forscan++) {
                        i = CT(ifront,*forscan);
                        if (i != 0)
                                ifront = i;
                        else
/* the forward scan is incomplete, goto backward scan. */
                                break;
                }
/* when i != 0 the forward scan is complete,so note the coincidence
*  if non-trivial; otherwise do backward scan.
*/
                if(Incomplete)
/* computing backward scan end position b_end and starting backward
*  scan.
*/
	for(backscan=f_end,b_end=forscan; backscan>=b_end; backscan--) {
                        ji = inv_to_col[*backscan];
                        i = CT(iback,ji);
                        if (i)
                                iback = i;
                        else if (backscan == b_end) {
/* the backward scan is incomplete. new deduction found.
* fill in the inverse of the deduction and note the new deduction.
*/
                                CT(ifront,*forscan) = iback;
                                CT(iback,ji) = ifront;
#ifdef __DI
        if (DIAG[7]) {
        int n,*pt_d;
        char ch;
        n = COL_TO_GEN[ji];
        fprintf(fout,"**** TD %d*",Addr2Num(iback));
        if(n < 0) {
                ch = Gen_st[-n].gen;
                fprintf(fout,"%c",toupper(ch));
        } else {
                ch = Gen_st[n].gen;
                fprintf(fout,"%c",ch);
        }
        fprintf(fout,"=%d",Addr2Num(ifront));
        fprintf(fout," R [");
        for(pt_d = beg; pt_d <= f_end; pt_d++) {
                n = COL_TO_GEN[*pt_d];
                if (pt_d == backscan)
                        fprintf(fout," ");
                if(n < 0) {
                        ch = Gen_st[-n].gen;
                        fprintf(fout,"%c",toupper(ch));
                } else {
                        ch = Gen_st[n].gen;
                        fprintf(fout,"%c",ch);
                }
        }
        fprintf(fout,"]\n");
        }
#endif
/* test if we run out of space?  if so, do compaction and reprocess
*  coset row.
*/ 
				if (iback<irow || iback==irow && ji<icol) 
                                	Save_ded(iback,ji);
			} else 
				break;
                } /* end of 'for(backscan......'  */
                if(Complete && ifront != iback) {
/* call coincidence process subroutine, and reinitiallize preferred
* definition list,and associate counters.
*/
			STATUS  = "TC";
                        tc_coinc(iback, ifront, tc_pvar);
			STATUS  = "TL";
                        if (INDEX1)
                                return;
/* test if the current deduction being processed has become redundant.
*/
                        if (Ded_Redundant)
                                goto ded;
                } /* end of 'if(Complete.....' */
        } /* end of for(; edpPt<=end;........*/
/* test if icol is involutory. */
        if (Icol_Is_Involutory)  {
/* do we have to do it here? YES! 12/9/91  */
                Swap(irow,ires);
                goto ProcRes;
        }
/* check time limit. if it was exeeded, report and return; */
        { Test_time_limit; }
	}
      } /* end of 'for(col = 1; col <= ncol;...'  */
ded:    continue;
      } /* end of 'for(row = COSET1; ...'  */
	return;
}

		

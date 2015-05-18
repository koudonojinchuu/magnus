/* File apply.c */
#ifdef __DI
#include "tcyacc.h"
#endif
#include "apply.h"
Void
tc_apply(hcoset, beg, end, defflg, tc_pvar)
Int     hcoset;
Int     *beg;
Int     *end;
Logical	defflg;
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
register Int	*forscan;
register Int	*backscan;
register Int	col;
register Int	i;
register Int	j;
register Int	ifront;
register Int	iback;
register Int 	ji;
register Int	k;
register Int	*ct;
register Int 	ncol;
/*
* subroutine to apply haselgrove-leech-trotter's method to a word
* starting at beg to end , applying coset
* hcoset at the beginning and end of the word and if defflg is .true.
* defining cosets to complete the tracing out of the word if needed.
*/

	{ Init; }
	ifront = iback = hcoset;
/* forward scan */
	for(forscan = beg; forscan <= end; forscan++) {
                i = CT(ifront, *forscan);

                if (i)
                	ifront = i;
                else
                	break;
        }
        if (Incomplete)
/* backward scan */
        for(backscan = end; backscan >= forscan; backscan--) {
                j = *backscan;
                ji = INV_TO_COL[j];
                i = CT(iback, ji);
                if (i)
                        iback = i;
                else if(backscan == forscan) {
/* the backward scan has only one gap so note the deduction to complete
* the cycle, and check for a new coincidence.
*/
                        CT(iback, ji) = ifront;
#ifdef __DI
        if (DIAG[7]) {
        int n,*pt_d;
        char ch;
        n = COL_TO_GEN[ji];
        fprintf(fout,"**** SD %d*",Addr2Num(iback));
        if(n < 0) {
                ch = Gen_st[-n].gen;
                fprintf(fout,"%c",toupper(ch));
        } else {
                ch = Gen_st[n].gen;
                fprintf(fout,"%c",ch);
        }
        fprintf(fout,"=%d",Addr2Num(ifront));
        fprintf(fout," S [");
        for(pt_d = beg; pt_d <= end; pt_d++) {
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
                        if(!VeryFirst) {
                                Save_ded(iback, ji);
			}
			if (CT(ifront, j)) {
                                ifront = CT(ifront, j);
                        } else {
                                CT(ifront, j) = iback;
                                ifront = iback;
                        }
                 } else if (defflg) {
/* obtain the next coset available sequentially. */
			if (&CT(NEXTDF,NCOL) >= DED_PT || NALIVE >= MAXROW) {
                                        Space_overflow;
			}
                        {       Get_cos(k);  }
                        { Fill_vac_in_rel(iback, ji, k,j); }
#ifdef __DI
        if (DIAG[7]) {
        int n,*pt_d;
        char ch;
        n = COL_TO_GEN[ji];
        fprintf(fout,"**** S  %d*",Addr2Num(iback));
        if(n < 0) {
                ch = Gen_st[-n].gen;
                fprintf(fout,"%c",toupper(ch));
        } else {
                ch = Gen_st[n].gen;
                fprintf(fout,"%c",ch);
        }
        fprintf(fout,"=%d",Addr2Num(k));
        fprintf(fout," S [");
        for(pt_d = beg; pt_d <= end; pt_d++) {
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
                        iback = k;
                } else 
/* in the case of defflg == FALSE do not define new cosets. */
			return; 
	} /* end of 'for(backscan.....)' */
/* -------------------------------------------------------------------
 * end of the relator scan, so note the equivalence of ifront and iback.  
*/
        if (ifront != iback) 
                tc_coinc(ifront, iback, tc_pvar);
	return;
}

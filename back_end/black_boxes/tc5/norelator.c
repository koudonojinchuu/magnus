/* File norelator.c  */
#ifdef __DI
#include "tcyacc.h"
#endif
#include "norelator.h"

Void
tc_norelator(tc_pvar)
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
register Int	*ct;
register Int    ncol;
register Int    col;
register Int    icol;
register Int    k;
Int		*inv_to_col;
/* subroutine to fill coset table when there are no group relators
*  and coset table is incomplete after completing subgroup generators
*  tables. this subroutine stops at space overflow and gives a
*  incomplete coset table.
*/        
	ct = COSET_TABLE;
	inv_to_col = INV_TO_COL;
        ncol = NCOL;
   do {
        for (icol = 1; icol <= ncol; icol++) 
                if (CT(KN_Hlt, icol) != 0)
                        continue;
                else if (NALIVE >= MAXROW || &CT(NEXTDF,NCOL) >= DED_PT) {
			Space_overflow;
		} else {
			Get_cos(k);
			Fill_KN_Hlt(KN_Hlt, icol, k);
#ifdef __DI
        if (DIAG[7]) {
        int n;
        char ch;
        n = COL_TO_GEN[icol];
        fprintf(fout,"**** KH %d*",Addr2Num(KN_Hlt));
        if(n < 0) {
                ch = Gen_st[-n].gen;
                fprintf(fout,"%c",toupper(ch));
        } else {
                ch = Gen_st[n].gen;
                fprintf(fout,"%c",ch);
        }
        fprintf(fout,"=%d\n",Addr2Num(k));
        }
#endif
		}
	Next_KN_Hlt;
   } while (CT(KN_Hlt, 1) >= 0);
return;
}

/* file coinc_cols12.c  */
#ifdef __DI
#include "tcyacc.h"
#endif
#include "coinc_cols12.h"

Void
tc_coinc_cols12(lows, highs,tc_pvar)
Int	lows;
Int	highs;
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
register Int	low;
register Int 	high;
register Int	*ct;
register Int 	j;
register Int	high1;
register Int	low1;
register Int	high2;
register Int	low2;
register Int 	k;
register Int	i;
register Int 	l;
register Int    low1s,low2s,high1s,high2s;
/*
* subroutine to process columns 1 and 2 of coincident cosets
* coset lows = coset highs and its consequences.
* in practice, during the processing of the possible coincidences
* arising from the processing of the first 2 columns and the possible
* subsequent coincidences arising from them, we discover at the most
* only 2 more unprocessed coincidences which we need to queue up
* somewhere to have their columns 1 and 2 subsequently processed. thus
* we need to set aside 4 locations, low1s, high1s , low2s,high2s, to
* save information to tell us where possibly more coincident cosets
* come from.
*/

	{ Init; }
	for(;;) {
	{ CompressPath(low); }
	{ CompressPath(high); }
/* start to process cols 1&2 of low and high */
        if (low != high) { 
		{ AdjustLowHigh; }
/* high1 -- contents of column 1 of coset high.
* high2 -- contents of column 2 of coset high.
*/

            	high1 = CT(high, 1);
            	high2 = CT(high, 2);
/* mark high coincident with low and stack the coincidence.
*/
		{ Push(high,low); }	
/* look at the consequences of column 1 of rows low and high.
*/        	if (high1) { 
			j = INV_TO_COL[1];
/* delete ct(high1, j) at this stage rather than replace by low to
* avoid having two occurrences of low in the one column.
*/
			{ Check(high1); }
/* low1 -- contents of column 1 of coset low. */
           		low1 = CT(low, 1);
           		if (low1) {
/* stack the coincidence low1=high1. */
				Save_cols12(low1, high1);
			} else {
/* note the deduction. */
             			CT(low, 1) = high1;
#ifdef __DI
        if (DIAG[7]) {
        int n,*pt_d;
        char ch;
        n = COL_TO_GEN[1];
        fprintf(fout,"**** C %d*",Addr2Num(low));
        if(n < 0) {
                ch = Gen_st[-n].gen;
                fprintf(fout,"%c",toupper(ch));
        } else {
                ch = Gen_st[n].gen;
                fprintf(fout,"%c",ch);
        }
        fprintf(fout,"=%d\n",Addr2Num(high1));
        }
#endif
              			if(!VeryFirst) 
					Save_ded(low,1);
			}	
/* alter CT(CT(low,1), ) to low if 0. */
			CheckSet(low,1);
		} /* end of 'if(high1)' */
/* look at the consequences of column 2 of rows low and high. */
        	if (high2) { 
			j = INV_TO_COL[2];
/* delete ct(high2, j) at this stage rather than replace by low to
* avoid having two occurrences of low in the one column.
*/
			Check(high2);
/* low2 -- contents of column 2 of coset low. */
          		low2 = CT(low, 2);
          		if (low2) {
/* queue the coincidence low2=high2. */
				Save_cols12(low2,high2);
			} else {
/* note the deduction.  */
				CT(low, 2) = high2;
#ifdef __DI
        if (DIAG[7]) {
        int n,*pt_d;
        char ch;
        n = COL_TO_GEN[2];
        fprintf(fout,"**** C %d*",Addr2Num(low));
        if(n < 0) {
                ch = Gen_st[-n].gen;
                fprintf(fout,"%c",toupper(ch));
        } else {
                ch = Gen_st[n].gen;
                fprintf(fout,"%c",ch);
        }
        fprintf(fout,"=%d\n",Addr2Num(high2));
        }
#endif
				if(!VeryFirst)
					Save_ded(low,2);
			}
/* alter CT(CT(low,2),j) to low if 0. */
			CheckSet(low,2);
		} /* end of 'if(high2) ' */
		NALIVE--;
/* check for the INDEX 1 situation here and thus avoid further processing
* if the INDEX 1 situation is detected.
* check that all the entries in coset one are defined (they don't all
* necessarily have to all point to 1 because some coincidences may
* still be queued up).
*/
		CheckIndex1;	
		MessageOutput;
        } /* end of 'if(low != high)'  */
/*-------------------------------------------------------------*/
/* remove coset high from the stored up coincidences to be processed.
*/
	{ RemoveHigh; }
/* find the next coincident cosets to process. */
	{ Get_cols12(low,high); }
	} /* end of 'for(;;)'  */
	return;
}

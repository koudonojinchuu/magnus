/* File text.c */
#include "defs.h"
#include "coset_def.h"
#include "tc.h"
#include "miscellaneous.h"
/*#include "tctime.h"*/


Void
tc_text(arg, tc_pvar)
Int	arg;
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
register Int	i;
/*
* Subroutine to write out various messages.
* Message numbers less than twenty are used for informative
* messages controlled by the global print flag.
* The remainder are error messages.
*/
	i = arg - 10;
	switch (i)
	{
	case 1:
		fprintf(fout,"%s ALIVE=%7.d MAX=%7.d TOTAL=%7.d KNR=%7.d KNC=%7.d ", STATUS, NALIVE, MAXCOS, TOTCOS,Addr2Num(KN_Hlt),Addr2Num(KN_Fel));
#ifdef __DI
	if(DIAG[8]) {
		int i, j, *ct;
		ct = COSET_TABLE;
		HOLES = 0;
		for(i = 1; i < NEXTDF; i += NCOL)
			if (CT(i, 1) < 0)
				continue;
			else
				for(j = 1; j <= NCOL; j++) 
					if (CT(i, j) == 0)
						HOLES++;
		fprintf(fout,"HOLES=%d%%\n", (HOLES * 100 / (NALIVE * NCOL)));
	} else 
		fprintf(fout,"\n");
		if(MSGLVE < 0) tc_time(0); 
#else
	fprintf(fout,"\n");
		if(MSGLVE < 0) tc_time(0); 
#endif
		return;
	case 2:
		fprintf(fout," INDEX = %d  MAX = %d   TOTAL = %d    KNR=%d, KNC=%d\n", NALIVE, MAXCOS, TOTCOS,Addr2Num(KN_Hlt),Addr2Num(KN_Fel)); 
		return;
	case 3:
		fprintf(fout," OVERFLOW:  MAX = %d   TOTAL = %d\n", MAXCOS, TOTCOS);
		return;
	case 4:
		fprintf(fout," OVERFLOW AT APPLY COSET PHASE\n");
		return;
	case 5:
		fprintf(fout," DEDUCTION STACK DISCARDED (%d DEDS)\n",(DED_END - DED_PT + 1)/8);
		return;
	case 6:
		fprintf(fout," MAXIMUM TABLE SIZE = %d\n",MAXROW);
		return;
	case 7:
		fprintf(fout," EXECUTION TIME: %.3f SECONDS\n", FNTIME);
		return;
	case 8: fprintf(fout," %d DEDUCTIONS ARE DISCARDED \n",DEADDE);
                return;
	case 9: (void)fprintf(fout," THE COSET TABLE IS ALREADY COMPLETED\n");
                return;
	case 10:
		fprintf(fout," TIME LIMIT EXCEEDED\n");
		return;
	case 11:
		fprintf(fout," INSUFFICIENT SPACE\n");
		return;
/*
	case 12:
		fprintf(fout," LOOKAHEAD LIMIT EXCEEDED\n");
		return;
*/
	case 13:
		fprintf(fout," INSUFFICIENT SPACE RECOVERED BY LOOKAHEAD\n");
	case 14:
		fprintf(fout," COSET TABLE COMPACTED\n");
		return;
        case 15: 
                fprintf(fout," MAXIMUM TABLE SIZE = %d   PAGE STRATEGY = %c\n",MAXROW,PAGE_STRATEGY);
		return;
	}
}
/* End File */

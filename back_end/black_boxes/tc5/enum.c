/* file enum.c */
#include "enum.h"
#ifdef __DI
#include "tcyacc.h"
#endif

Void
tc_enum(restart, tc_pvar)
Int	restart;
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
register Int 	*ct;
Int  	*inv_to_col;
register Int	iback;
register Int	ifront;
register Int	k;
register Int	*forscan;
register Int 	*backscan;
register Int	*edpPt;
register Int	*edp_end;
register Int	*f_end;
register Int	*beg;
register Int	*b_end;
Int	i;
register Int	ji;
register Int	rel;
register Int	j;
register Int 	ncol;
Int 	row;
Int 	Felsch_phase;
Int	ded_fill_pd;
Int	fill_kn_nopd;
Int	fill_kn_havepd;
Int	fill_kn_deadpdl;
Int	fill_pd;
Int	gap;
Int	maxgap;
Int	icol;
Int	col;
Int	FELLMT;
Int	HLTLMT;
Int	defhlt;
Int	deffel;
Int	be;
Int	ed;
Int	irow;
Int	ires;
/* for cyclic perms */
Int 	base_len, cyclic_perms, cyclic_count;

register Float	curtim;
Int	compact;
Float	utl_second();
#ifdef __DI
	Int	diag_pd;
	diag_pd = 0;
#endif

/*
* subroutine to set up a partial or complete coset table using a
* modified felsch and HLT method. the felsch algorithm has been modified to the
* following extent: if FILL_FACTOR is positive, if during the relator scan a
* gap of length 1 occurs then this position is put into the Preferred 
* Definition List(PDL, a overwriting ring). provided no
* deductions fill this gap in or a coincidence occurs (when the noting 
* of gaps of length 1 starts all over again) the next coset will be 
* defined to fill the gap of length 1 which is in the front of the PDL. 
* on certain perverse examples, e.g. f(2,7),
* this can cause infinite looping which is stopped by also insisting
* that before defining a coset to fill a gap of length 1 that
* FILL_FACTOR*KN be at least NALIVE to force the KN scan to fill up rows.
* (this condition has been arrived at empirically and the default value
* of 5 for FILL_FACTOR has given the best results on various test examples
* but it may be altered in option). 
* when FILL_FACTOR=1, the original felsch algorithm is used.
* be warned that putting FILL_FACTOR to large absolute values (via option)
* can cause infinite looping on some perverse examples but in general
* a large positive value for FILL_FACTOR gives good results. 
* haselgrove-leech-trotter method has been modified to the extent:
* lookahead is done by felsch type scan word. 
* note that when MAXROW is near MAXCOS because the infinite looping test
* involves KN the statistics for MAXCOS and TOTCOS may vary and in some
* cases vary significantly.
* Space strategy: 
*        in two case we get to space overflow, they are:
* 1. we can not define new cosets as there are no more cosets available in
*    logical (we reach MAXROW);
* or 2. there are no more cosets available in physical(reach the top of 
*       deduction stack) and reusable space is smaller than the compaction %.
* CT(i,j) with j=1,...,ncol indicates the action of the associated
* 	generator or inverse of column j on coset i. it contains the coset
* 	number if known, otherwise 0. coset 1 is the subgroup.
* KN_Hlt -- (for HLT type) coset in the coset table at which the 
* 	search for an undefined coset table entry is to begin.
* KN_Fel -- (for felsch type) coset in the coset table at which the
* 	search for an undefined coset table entry is to begin.
* MAXCOS -- maximum number of cosets active at any time.
* TOTCOS -- total number of cosets defined.
* NALIVE -- number of active cosets.
* INTNAL -- every LVE(or MSGLVE) cosets defined or processed by 
*	the coincidence	routine a message is printed. INTNAL 
*	is the next NALIVE at which this message is to be printed.
* OLDNAL -- the printing of NALIVE every LVE(or MSGLVE) cosets 
* 	can produce far too much output if NALIVE oscillates 
*	around a multiple of LVE(or MSGLVE). OLDNAL is the last 
*	value of NALIVE when this message was printed and the message 
*	only gets printed when NALIVE is different from OLDNAL.
* DED_PT -- unworked deductions are placed at the back of y. 
* DED_PT is the pointer of the deductions stack.*DED_PT is the row,
* 	*(DED_PT+1) is the colunm, CT(row,colunm) is the position in CT
* 	at which the next deduction can be filled in. 
* 	when no further space is available for the deductions stack 
* 	subsequent deductions are discarded and a message is printed at the 
* 	end of the enumeration to tell how many deductions are discarded.
* NDREL -- number of group defining relators.
* CT(NEXTDF,1) -- last position avaliable for the deductions stack. 
* NEXTDF -- next sequentially available coset while NEXTDF is not
* 	greater than DED_PT, otherwise there is no more cosets sequentially.
* 	initially as new cosets are required they are defined sequentially
* 	until ct is exhausted, then compaction are re-used.
* INDEX1 -- logical variable set to .false. initially. if in 
*	tc_coinc_cols12() it is detected that NALIVE has fallen 
*	to 1 and all entries in coset 1 are defined, then INDEX1 
*	is set to .true. and we know that the subgroup has INDEX 1.
* mingpl -- logical variable set to .true. to use the mimimum gap or to
* 	.false. to use the gap of length 1.
* minlen -- logical variable to indicate whether a gap of length 1 or
* 	the minimum gap has been found in the relator scan.
* 	initiallize coset 1, counters and pointers.
* RELATORS --group defining relators in full length and every 
*	relator is three times for convenience of scan word.
* RELATOR_LENGTH---length of relators. RELATOR_LENGTH[i] gives length of i
*         relator (original one , not three times).
* RELATOR_INDEX---index of relators. RELATOR_INDEX[i] gives the
*         beginning position of ith relator in array RELATORS.
* EDP_BEG---index of essentially different positions for each generator.
*         EDP_BEG[i] gives beginning positions in array edp for generator i.
* EDP---essentially different positions for each relator for generators.
* Felsch_phase -- TRUE, when in felsch defining phase, otherwise FALSE.
*/

  { Init; }
  { Init_fel_counters; }
  { Init_LMT; } 
  { Output_fillfactor_etc; }
  if (NotRestart) { 
  	Init_enum;
	if (HaveSubgGen) {
/* subg generator phase */
		tc_subg_proc(tc_pvar);
#ifdef __DI
if(DIAG[9]) {
tc_text(11,tc_pvar);
}
#endif
		if(INDEX || OVERFL)
			return; 
	} else if (!VeryFirst) {
		Get_cos(k);
		Fill_kn(KN_Fel,1,k);
#ifdef __DI
	if (DIAG[7]) {
 	int n; 
	char ch;
	n = COL_TO_GEN[1];
	fprintf(fout,"**** KF %d*",Addr2Num(KN_Fel));
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
  }
  { Init_status; }
  if (NoRelator) {
/* there's no group defining relators.  */
	tc_norelator(tc_pvar);
	return;
  }
  for(;;) {
  for(;;) /* felsch phase */	{
     while (HaveDed) { /* process deduction */
	{ Get_ded(irow,icol,ires); }
	{ NonInverse(irow,icol,ires); }
ProcRes:Get_edp(edpPt,edp_end,icol);
/* go through edps which relevant for icol.  */
        for(; edpPt<=edp_end; edpPt+=2) {
/* scan forward through relator (stored in RELATORS[]) as long as table
* entries are present, starting at the new deduction.
* computing beginning position beg and forward scan end position f_end.
*/
		beg = &RELATORS[*edpPt];
		Set_ifront_iback;
		f_end = beg + *(edpPt + 1) -  1;
		for(forscan=beg+1; forscan<=f_end; forscan++) {
		 	i = CT(ifront,*forscan);
			if (i) 
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
				Save_ded(iback,ji);
#ifdef __DI
        if (DIAG[7]) {
        int n,*pt_d;
        char ch;
        n = COL_TO_GEN[ji];
        fprintf(fout,"**** DD %d*",Addr2Num(iback));
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
			} else if(backscan - 1 > b_end)
/* no gap of length 1 occurred, go to process next edp */
				break;
			else {
/* a gap of length 1 occurred, note it(put it into preferred 
*  definition list(an overwriting ring queue). 
*/
				Save_pd(iback,ji);
  				Fix_pd_counters;
				break;
			}
		} /* end of 'for(backscan......'  */
		if(Complete && ifront != iback) { 
/* call coincidence process subroutine, and reinitiallize preferred
* definition list,and associate counters.
*/
			TcCoinc(iback, ifront, tc_pvar);
			if (INDEX1)  
				{ Index_return;}	
/* test if the current deduction being processed has become redundant.
*/
			if(COMPCT) {
				STATUS = "RD"; 
				FELLMT = 0;
				HLTLMT = 0;
				goto Hlt;
			} else if (Ded_Redundant)
				goto ded;
		} /* end of 'if(Complete.....' */
	} /* end of for(; edpPt<=end;........*/
/* check time limit. if it was exeeded, report and return; */
/* test if icol is involutory. */
	if (Icol_Is_Involutory)  {
		Swap(irow,ires);
		goto ProcRes;
	}
	{ Test_time_limit; }  
ded:    continue;
	} /* end of 'while(HaveDed)   */
/* there are no more new deductions to be examined. search the coset
* table for a vacancy and define the next coset to fill in this 
* vacancy or where a gap of length 1.
*/
#ifdef __DI
if(DIAG[6] && *STATUS == 'A' && HLTLMT != 0)  
	 tc_text(11, tc_pvar);
#endif
    if(*STATUS == 'A' && deffel < FELLMT && Felsch_phase) {
STATUS = "CD";
} 
    if(deffel < FELLMT && Felsch_phase) {
    for(; KN_Fel < NEXTDF; KN_Fel += ncol) {
	if (DeadKN_Fel)
		continue;
	for (icol = 1; icol <= ncol; icol++)
		if (CT(KN_Fel, icol) == 0)
			goto DefCos;
    }
/* coset table is completed */
    { Index_return;}
/* before define a new coset check if coset number exceeds limit. */
DefCos: if(NALIVE >= MAXROW || &CT(NEXTDF,NCOL) >= DED_PT) {
/* in two case we get to space overflow, they are:
* 1. we can not define new cosets as there are no more cosets available in
*    logical (we reach MAXROW);
* or 2. there are no more cosets available in physical and reusable space
*       is smaller than the compaction %.
*/
		if (NALIVE >= MAXROW || ((NEXTDF-NCOL-Num2Addr(NALIVE)) * 100.0 / (NEXTDF-NCOL) < COMP_PCT)) {
			Space_overflow;
		} else {
			TcCompact(tc_pvar);
		}
	}
	{	Get_cos(k);  }
	deffel++;
  	if(Nopd) {
		Fill_kn_nopd(KN_Fel,icol,k);
	} else if (HaveToFillKn) {
		Fill_kn_havepd(KN_Fel,icol,k);
	} else {
		{ CheckPdl; }
		if(Havepd) {
   			Get_pd(row,col);
			Fill_pd(row,col,k);
#ifdef __DI
diag_pd=1;
#endif
		} else {
			Fill_kn_deadpdl(KN_Fel,icol,k);
		}
	}
#ifdef __DI
        if (DIAG[7]) {
        int n,rpd;
        char ch;
	if (diag_pd) {
		diag_pd = 0;
		n = col;
		rpd = row;
		fprintf(fout,"**** PD");
	} else {
		rpd = KN_Fel;
		n = icol;
		fprintf(fout,"**** KF");
	}
        n = COL_TO_GEN[n];
        fprintf(fout," %d*",Addr2Num(rpd));
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
/* we normally continue to process in Felsch style until FELLMT is exceeded.
*/
      } else /* end of if(deffel < FELLMT)...*/
		break;
   } /* end of for(;;) for FELSCH phase   */
#ifdef __DI
if(DIAG[1] && *STATUS == 'C') tc_text(11,tc_pvar);
#endif
   ResetFelsch;
Hlt: if(DeadKN_Hlt) {
                {  Next_KN_Hlt; }
}
/* switch to HLT */
   while(defhlt < HLTLMT || VeryFirst) {
/* scan forward through relator (stored in RELATORS[]) as long as table
* entries are present.
* computing begining position beg and forward scan end position f_end.
*/
    for(rel = 1; rel <= NDREL; rel++) {
	if (RELATOR_EXPONENT[rel] < 0)
		continue;
/* for cyclic perms 
		beg = &RELATORS[RELATOR_INDEX[rel]];	
*/
        base_len = RELATOR_LENGTH[rel] / RELATOR_EXPONENT[rel];
        if (PAGE_STRATEGY == 'R') 
            cyclic_count = base_len;
        else 
            cyclic_count = 1;
        for (cyclic_perms = 0; cyclic_perms < cyclic_count; cyclic_perms++) {
            beg = &RELATORS[RELATOR_INDEX[rel] + cyclic_perms];
 	    f_end = beg + RELATOR_LENGTH[rel];
		ifront = iback = KN_Hlt;
		for(forscan = beg; forscan < f_end; forscan++) {
			i = CT(ifront, *forscan);
			if (i) 
				ifront = i;
			else 
				break;
		}
		if (Incomplete) 
		for(backscan = f_end - 1; backscan >= forscan; backscan--) {
			j = *backscan;
			ji = inv_to_col[j];
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
        fprintf(fout,"**** DD %d*",Addr2Num(iback));
        if(n < 0) {
                ch = Gen_st[-n].gen;
                fprintf(fout,"%c",toupper(ch));
        } else {
                ch = Gen_st[n].gen;
                fprintf(fout,"%c",ch);
        }
        fprintf(fout,"=%d",Addr2Num(ifront));
	fprintf(fout," R [");
	for(pt_d = beg; pt_d < f_end; pt_d++) {
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
				if(!VeryFirst) 
					Save_ded(iback, ji);
				if (CT(ifront, j)) {
					ifront = CT(ifront, j);
				} else {
					CT(ifront, j) = iback;
					ifront = iback;
				}
			} else { 
				if (NALIVE >= MAXROW || &CT(NEXTDF,NCOL) >= DED_PT) {
/* if it is pure hlt we do table lookahead. */
					if(VeryFirst) {
						{ ProcVeryfirst; }
#ifdef __DI
if(DIAG[5]) tc_text(11,tc_pvar);
#endif
						STATUS = "AL";
						goto Felsch;
					} else if (HaveDed) {
/* go to felsch phase to process ded to see if we can complete
*  the table!
*/
                                                        STATUS = "AL";
                                                        goto Felsch;
   } else if (NALIVE >= MAXROW ||((NEXTDF-NCOL-Num2Addr(NALIVE)) * 100.0 / (NEXTDF-NCOL) < COMP_PCT)) {
/* in two case we get to space overflow, they are:
* 1. we can not define new cosets as there are no more cosets available in
*    logical (we reach MAXROW);
* or 2. there are no more cosets available in physical and reusable space
*       is smaller than the compaction %.
*/
                                               Space_overflow;
                                        } else {
						tc_compact(tc_pvar);
						goto redo;
					}
        			}
				{       Get_cos(k);  }
				{ Fill_vac_in_rel(iback, ji, k,j); }
#ifdef __DI
        if (DIAG[7]) {
        int n,*pt_d;
        char ch;
        n = COL_TO_GEN[ji];
        fprintf(fout,"**** RD %d*",Addr2Num(iback));
        if(n < 0) {
                ch = Gen_st[-n].gen;
                fprintf(fout,"%c",toupper(ch));
        } else {
                ch = Gen_st[n].gen;
                fprintf(fout,"%c",ch);
        }
        fprintf(fout,"=%d",Addr2Num(k));
        fprintf(fout," R [");
        for(pt_d = beg; pt_d < f_end; pt_d++) {
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
			}
		} /* end of 'for(backscan.....)' */
/* -------------------------------------------------------------------
 * end of the relator scan, so note the equivalence of ifront and iback.
 */
		if (ifront != iback) {
			Tccoinc;
			if (INDEX1) 
                             {   Index_return; }
			else if(DeadKN_Hlt)
				break;
		}
		continue;
/* for cyclic perms, add 3 lines  */
           } /* end of ' for (cyclic_perms ...  */
             if (DeadKN_Hlt)
                break;
		} /* end of 'for(rel......'  */	 
	if(DeadKN_Hlt) {
		{  Next_KN_Hlt; }
		continue;
	}
/* scan across row defhlt to see that all the entries are defined.  */
	for(icol = 1; icol <= ncol; icol++)
		if(CT(KN_Hlt,icol) == 0){
/* we have a vacancy in row defhlt which needs to be filled in.
*  first check if there are any new cosets available.
*/
			if (NALIVE >= MAXROW || &CT(NEXTDF,NCOL) >= DED_PT) {
/* if it is pure hlt we do table lookahead. */
				if(VeryFirst) {
						{ ProcVeryfirst; }
#ifdef __DI
if(DIAG[5]) tc_text(11,tc_pvar);
#endif
                                                STATUS = "AL";
                                                goto Felsch;
				} else if (HaveDed) {
/* go to felsch phase to process ded to see if we can complete
*  the table!
*/
                                        STATUS = "AL";
                                        goto Felsch;
 } else if ( NALIVE >= MAXROW || ((NEXTDF-NCOL-Num2Addr(NALIVE)) * 100.0 / (NEXTDF-NCOL) < COMP_PCT)) {
/* in two case we get to space overflow, they are:
* 1. we can not define new cosets as there are no more cosets available in
*    logical (we reach MAXROW);
* or 2. there are no more cosets available in physical and reusable space
*       is smaller than the compaction %.
*/ 
                                        Space_overflow;
                                } else {
                                        tc_compact(tc_pvar);
                                }
                        }
                        {       Get_cos(k);  }
                        { Fill_KN_Hlt(KN_Hlt, icol, k); }
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
	{  Next_KN_Hlt; }
	defhlt++;
redo:	continue;
	} /* end of HLT Defining phase  */
#ifdef __DI
if(DIAG[2] && *STATUS == 'R' && HLTLMT != 0) tc_text(11,tc_pvar);
#endif
    	ResetHLT;	
Felsch: Init_pdl;
    }
}

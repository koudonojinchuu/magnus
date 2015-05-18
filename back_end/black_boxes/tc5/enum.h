/* Include file for enum.c */
/* delete the following in Init: 
*                             if (COMP_PCT == 0) \
                                COMP_PCT = 20; \
* 24/11/1993
*/
#include "ded.h"
#include "coset_def.h"
#include "miscellaneous.h"
#include "tc.h"
#include "ct.h"

#ifndef __ENUM__
#define __ENUM__
#define Adjust_HLTLMT	HLTLMT += Hltlmt * ncol; \
			if(HLTLMT >= NEXTDF) {\
				HLTLMT = Hltlmt * ncol; \
				DEFHLT = COSET1; \
			}
#define CheckPdl      while (CT(PDL_ROW[be],PDL_COL[be]) != 0) { \
                                gap--; \
                                ded_fill_pd++; \
                                be = Step(be); \
                                if (Nopd) \
                                        break; \
                        }

#define DeadKN_Fel             CT(KN_Fel, 1) < 0
#define DeadKN_Hlt	CT(KN_Hlt,1) < 0
#define Ded_Redundant	CT(irow, 1) < 0 || CT(ires, 1) < 0

#define Fill_KN_Hlt(row,col,k)	CT(k,inv_to_col[col]) = row; \
				CT(row,col) = k; \
				if(!VeryFirst) \
					{ Save_ded(row,col); }
#define Fill_kn(kn,col,k)  CT(k,inv_to_col[col]) = kn; \
                            CT(kn,col) = k; \
                            { Save_ded(kn,col); } 
#define Fill_kn_nopd(kn,col,k)   { Fill_kn(kn,col,k); } \
				   fill_kn_nopd++
#define Fill_kn_havepd(kn,col,k)  { Fill_kn(kn,col,k); } \
                                  fill_kn_havepd++

#define Fill_kn_deadpdl(kn,col,k)  { Fill_kn(kn,col,k); } \
                            fill_kn_deadpdl++

#define Fill_pd(kn,col,k)   { Fill_kn(kn,col,k); } \
                             fill_pd++
#define Fix_pd_counters		gap++; \
                                maxgap++

#define Get_edp(beg,end,gen)    beg=&EDP[EDP_BEG[gen]]; \
                                end=&EDP[EDP_END[gen]]

#define Get_pd(row,col)    row = PDL_ROW[be]; \
                            col = PDL_COL[be]; \
                            be = Step(be)

#define HaveDed                  DED_PT<DED_END
#define Havepd              be != ed
#define HaveSubgGen 	RELATORS_IN_SUBGROUP >= 0 || NSGPG		
/* test if fill factor is APPROXIMATELY violated */
/*
#define HaveToFillKn 	Addr2Num(KN_Fel) * iabs(FILL_FACTOR) - NALIVE  < 0
*/
#define HaveToFillKn   KN_Fel * iabs(FILL_FACTOR) - NEXTDF  < 0
#define Icol_Is_Involutory	icol == inv_to_col[icol] && irow < ires
#define Index_return      tc_text(12, tc_pvar); \
if(DEADDE) \
printf("%d DEDUCTIONS ARE DISCARDED\n",DEADDE); \
                            INDEX = TRUE; \
                            if (MSGLVE) { \
printf("max PDL=%d  filled (PDL=%d kn=%d ff=%d PDL deductions=%d dead PDL=%d)\n",maxgap,fill_pd,fill_kn_nopd,fill_kn_havepd,ded_fill_pd,fill_kn_deadpdl); \
                          } \
                          return

#define Init                ct = COSET_TABLE;  \
                            inv_to_col = INV_TO_COL;  \
                            ncol = NCOL; \
			    INDEX = FALSE; \
                            INDEX1 = FALSE; \
                            DEDWNG = FALSE; \
			    DEADDE = 0; \
			    HOLES = 0; \
			    STDCT = FALSE; \
			    Felsch_phase = TRUE; \
			    KN_Hlt = KN_Fel = COSET1; \
			    TABROW = 0; \
                            OVERFL = FALSE


#define Init_fel_counters  ded_fill_pd = 0; \
                            fill_kn_nopd = 0; \
                            fill_kn_havepd = 0; \
                            fill_kn_deadpdl = 0; \
                            fill_pd = 0; \
                            gap = 0; \
                            maxgap = 0; \
			    DED_PT = DED_END; \
                            Init_pdl  

#define Init_enum          Init_coset1; \
                            MAXCOS = 1; \
                            TOTCOS = 1; \
			    NALIVE = 1; \
			   STATUS = "SG"; \
			if (MSGLVE) { \
				LVE = (MSGLVE>0)? MSGLVE : -MSGLVE; \
				INTNAL = LVE; \
			} else { \
				LVE = 0; \
				INTNAL = 0; \
			}\
			OLDNAL = 1; COMPCT = TRUE; \
			HOLES = 0; \
			NEXTDF = COSET1 + COSET
#define Init_LMT	FELLMT = F_TYPE_DEF; \
			if(A_FACTOR < 0) \
				HLTLMT = -A_FACTOR; \
			else \
				HLTLMT = A_FACTOR; \
			if (FELLMT == 0) { \
				VeryFirst = TRUE; \
			} else { \
				VeryFirst = FALSE; \
			} \
			defhlt = 0; \
			deffel = 0
#define Init_status	if (FELLMT == 0) { \
                                STATUS = "RD"; \
                        } else { \
                                STATUS = "CD"; \
                        } 

#define Init_pdl           be = ed = gap = 0 
/* macros for perfered definition list:  */
#define List_length	255
#define Next_KN_Hlt	do KN_Hlt += COSET; \
			while((KN_Hlt < NEXTDF) && CT(KN_Hlt,1) < 0 );\
			if(KN_Hlt == NEXTDF) \
				 { Index_return; }			
#define NonInverse(irow,icol,ires)	j = inv_to_col[icol]; \
					if(j < icol) { \
						icol = j; \
						Swap(irow,ires); \
					} else if (icol == j && irow > ires) {\
						Swap(irow,ires); \
					} 
#define Nopd                be == ed
#define NoRelator	NDREL == 0
#define NotRestart	!restart
#define Output_fillfactor_etc	if(MSGLVE) \
					printf("Fill Factor=%d, CT Factor=%d,  RT Factor=%d\n",FILL_FACTOR,F_TYPE_DEF,A_FACTOR)
#define ProcVeryfirst	defhlt = 0; \
			STATUS = "TL"; \
                        tc_proc_ded(tc_pvar); \
			if (INDEX1) \
				{ Index_return; } \
			if (OVERFL) \
				return; \
			VeryFirst = FALSE; \
			if (HLTLMT == 0) { \
				FELLMT = 1000; \
				HLTLMT = FELLMT / (2 * REL_LEN); \
			}
#define ResetFelsch	deffel = 0; \
			STATUS  = "RD";  \
			Felsch_phase = FALSE
#define ResetHLT	defhlt = 0; \
			STATUS = "AL"; \
			if (A_FACTOR < 0) defhlt = HLTLMT; \
			if (KN_Fel < KN_Hlt) KN_Fel = KN_Hlt; \
			if (FELLMT) Felsch_phase = TRUE; \
			else \
				Felsch_phase = FALSE
			
#define Save_pd(row,col)   PDL_ROW[ed] = row; \
                            PDL_COL[ed] = col; \
                            ed = Step(ed); \
                            if (ed == be) \
                                  be = Step(be)


#define Set_ifront_iback_by_DEFHLT		ifront = iback = DEFHLT 
#define Set_ifront_iback        if(icol == *beg) { \
                                        iback = irow; ifront = ires; \
                                } else { \
                                        iback = ires; ifront = irow; \
                                }

#define Step(i)             (++i) & List_length

#define Test_time_limit;    if (LTIME == 0.000000) \
                                  continue; \
                            curtim = utl_second(); \
                            if (curtim > LTIME) { \
                	    	tc_text(20, tc_pvar); OVERFL = TRUE; \
                	    	return; \
        	    	    }        

#define TcCoinc(iback, ifront, tc_pvar)  if( *STATUS == 'C') \
						STATUS = "CC"; \
					 else \
						STATUS = "LC"; \
					 tc_coinc(iback, ifront, tc_pvar); \
					if( *STATUS == 'L') \
						STATUS = "LA"; \
					else \
						STATUS = "CD"; \
					Init_pdl
#define Tccoinc		STATUS = "RC"; \
			tc_coinc(iback, ifront, tc_pvar); \
                        STATUS = "RD"
#define	TcCompact(tc_pvar)	tc_compact(tc_pvar); \
				Init_pdl
#endif

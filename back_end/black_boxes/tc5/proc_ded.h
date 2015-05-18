/* include file of  proc_ded.c */
#include "tc.h"
#include "ct.h"
#include "ded.h"
#include "miscellaneous.h"
#ifndef __PROC_DED__
#define __PROC_DED__
#define Ded_Redundant   CT(irow, 1) < 0 || CT(ires, 1) < 0
#define Get_edp(beg,end,gen)    beg=&EDP[EDP_BEG[gen]]; \
                                end=&EDP[EDP_END[gen]]

#define Test_time_limit;    if (LTIME == 0.000000) \
                                  continue; \
                            curtim = utl_second(); \
                            if (curtim > LTIME) { \
                		tc_text(20, tc_pvar); OVERFL = TRUE; \
                		return; \
        		    }
#define Icol_Is_Involutory      icol == inv_to_col[icol] && irow < ires

#define Set_ifront_iback        if(icol == *beg) { \
                                        iback = irow; ifront = ires; \
                                } else { \
                                        iback = ires; ifront = irow; \
                                }
#define NonInverse(irow,icol,ires)      j = inv_to_col[icol]; \
                                        if(j < icol) { \
                                                icol = j; \
                                                Swap(irow,ires); \
                                        } else if (icol == j && irow > ires) {\
                                                Swap(irow,ires); \
                                        }
#define Init		ct = COSET_TABLE;  \
                            inv_to_col = INV_TO_COL;  \
                            ncol = NCOL	
#endif

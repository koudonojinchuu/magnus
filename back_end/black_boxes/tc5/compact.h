/* include file for compact.c  */
#include "ct.h"
#include "tc.h"
#include "miscellaneous.h"
#ifndef __COMPACT__
#define __COMPACT__
#define DeadCoset(row)		CT(row,1) < 0
#define Init		ct = COSET_TABLE; \
			 inv_to_col = INV_TO_COL; \
        		ncol = NCOL; \
        		if (COMPCT) \
                		return; \
        		upper = NEXTDF - ncol; \
			fel = 0; \
			hlt = 0; \
			tab = 0
#define FixDed		for(pt = DED_END - 2; pt >= DED_PT; pt -= 2) \
				for(i = *pt; i >= COSET1; i -= COSET) \
                                	if(DeadCoset(i)) { \
						*pt -= CT(i, ncol); \
						break; \
					}
#define CountOld_New	count = COSET; \
			for (i = irow; i <= upper; i += COSET) \
				if (DeadCoset(i)) { \
					CT(i, ncol) = count; \
					count += COSET; \
				}
#define AdjustKnNextdf		KN_Hlt -= hlt * ncol; \
				KN_Fel -= fel * ncol; \
				TABROW -= tab * ncol; \
				NEXTDF = irow; \
				COMPCT = TRUE; \
				if (MSGLVE) { \
                			tc_text(24,tc_pvar); \
					if (MSGLVE < 0) \
						tc_time(0); \
        			} 
#define HaveDed                  DED_PT<DED_END
#endif

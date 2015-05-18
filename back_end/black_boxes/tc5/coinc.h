/* file coinc.h */
#include "tc.h"
#include "ct.h"
#include "ded.h"
#include "miscellaneous.h"
#ifndef __COINC__
#define __COINC__
#define CheckHighi        if(highi != high) \
                                CT(highi,j) = 0;  \
                        else \
                                highi = low
#define	CheckSet(low,i)	lowi = CT(low, i); \
			if (lowi && CT(lowi, j) == 0) \
				CT(lowi, j) = low	
/* macros for push and pull coincs from coinc_stack:
*  high_low() is a link to link high to equivalent low.
*  coinc_stk() is a stack to save unprocessed coincident coset high.
*/
#define high_low(i)             CT((i),1)
#define Init		ct = COSET_TABLE; \
			low = lows; \
        		high = highs; \
        		COMPCT = FALSE; \
			if (low > high) { \
				low = highs; \
				high = lows; \
			} \
			if(MSGLVE) \
        		do \
                		INTNAL -= LVE; \
        		while (NALIVE <= INTNAL);  \
			CTAIL = 0; \
        		CHEAD = 0
#define coinc_stk(i)          CT((i),2)
#define Pull(high,low)		CHEAD = CT(high,2); \
				if(CHEAD) {\
				high = CHEAD; \
                                     low = -CT(high,1); \
		                   } else \
                                        break

#define	CheckKN		while (CT(KN_Fel, 1) < 0) \
				KN_Fel -= NCOL
#define	CheckDed	if (DED_PT<DED_END) { \
				rpt = DED_END; \
				for(ppt = DED_END - 2; ppt >= DED_PT; ppt -= 2) \
					if(CT(*ppt,1) < 0) \
						continue; \
					else { \
						*--rpt = *(ppt + 1); \
						*--rpt = *ppt; \
					} \
				DED_PT = rpt; \
			}
#define CheckIntnal	if(MSGLVE) \
			do \
				INTNAL += LVE; \
                        while (NALIVE >= INTNAL)	
#endif			

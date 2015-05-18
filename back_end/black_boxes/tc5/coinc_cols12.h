/* file coinc_cols12.h */
#include "tc.h"
#include "ct.h"
#include "ded.h"
#include "miscellaneous.h"
#ifndef __COINC__12__
#define __COINC__12__
#define AdjustLowHigh	if (low > high) \
				{ Swap(low,high); }
#define CheckIndex1 	if (NALIVE == 1 && CT(1, 1) != 0) { \
				tc_check_index1(tc_pvar); \
				if (INDEX1) \
					return; \
			}
#define Check(i)	if(i != high) \
				CT(i,j) = 0;  \
			else \
				i = low
#define CheckSet(low,i)	k = CT(low, i); \
                        if (k && CT(k, j) == 0 && k != high) \
                		CT(k, j) = low
#define Init	ct = COSET_TABLE; \
		low1s = 0; \
		low2s = 0; \
		high1s = 0; \
		high2s = 0; \
        	low = lows; \
        	high = highs
#define MessageOutput	if (MSGLVE && NALIVE == INTNAL) { \
				INTNAL -= LVE; \
 				if (NALIVE != OLDNAL)  {  \
					OLDNAL = NALIVE; \
					tc_text(11, tc_pvar); \
                		} \
          		}
#define	Save_cols12(low,high)	if (low != high) { \
                                        if (low > high) \
                                                { Swap(low,high); } \
				if(low1s!=0&&low1s==low&&high1s==high) { ; \
			      }else if(low2s!=0&&low2s==low&&high2s==high){ ;\
				} else { \
        				if (low1s == 0) { \
                				low1s = low; \
                				high1s = high; \
        				} else { \
                				low2s = low; \
                				high2s = high; \
        				} \
				} \
				}	

#define Get_cols12(low,high)     if (low1s != 0) { \
                                        low = low1s; low1s = 0; \
                                        high = high1s; \
                                 } else if (low2s != 0) {\
                                        low = low2s; low2s = 0; \
                                        high = high2s; \
                                } else \
                                        break
#define high_low(i)             CT((i),1) 
/* Break down the coinc chain respect with low: 
* compute the ultimate irredundant value of low.  */
#define CompressPath(low)   if((i = CT(low,1)) < 0) { \
      					i = -i; \
      					l = low; \
      					while ((j = high_low(i)) < 0) \
            					i = -j; \
      					j = CT(low,1);  \
      				while ((j = high_low(-j)) < 0) { \
						 high_low(l) = -i;  \
          					l = -j; \
      					} \
      					low = i;   \
   				}
/* macros for push and pull coincs from coinc_stack:  
*  high_low() is a link to link high to equivalent low.
*  coinc_stk() is a stack to save unprocessed coincident coset high.
*/
#define Coinc_stk(i)          CT((i),2)
#define Push(high,low)		CT(high,1)= -low; \
				if(CHEAD == 0) \
					CHEAD = high; \
				else \
					CT(CTAIL,2) = high; \
				CTAIL = high; \
				CT(high,2)=0
#define	RemoveHigh	if (low1s) { \
				if (low1s == high) \
                			low1s = low; \
            			if (high1s  == high) \
                			high1s  = low; \
            			if (low1s >= high1s ) \
                			if (low1s == high1s ) \
                        			low1s = 0; \
                			else { \
						Swap(low1s, high1s); \
					} \
          		} \
        		if (low2s) { \
				if (low2s == high) \
                			low2s = low; \
            			if (high2s == high) \
                			high2s = low; \
            			if (low2s >= high2s) \
                			if (low2s == high2s) \
                        			low2s = 0; \
                			else { \
						Swap(low2s, high2s); \
					} \
          		}
#endif

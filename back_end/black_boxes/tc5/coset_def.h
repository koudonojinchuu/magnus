/* include file for define a coset. */
#include "ct.h"
#include "ded.h"
#include "miscellaneous.h"
#ifndef __COSET__DEF__
#define __COSET__DEF__
#define Get_cos(k)           k = NEXTDF; \
                           Init_coset(k); \
                           NEXTDF += ncol; \
                           TOTCOS++; \
                           NALIVE++; \
                           MAXCOS = max0(MAXCOS, NALIVE); \
                           if (MSGLVE && NALIVE == INTNAL) { \
                                   INTNAL += LVE; \
                                   if (NALIVE != OLDNAL) { \
                                           OLDNAL = NALIVE; \
					tc_text(11, tc_pvar); \
                                   } \
                           }
#define Init_coset1        for (j = 1; j <= ncol; j++)  \
                                 CT(COSET1, j) = 0
#define Init_coset(k)   for (col = 1; col <= ncol; col++) \
                                CT(k, col) = 0
#define Fill_vac_in_rel(row,col,k,j)    CT(row,col) = k; \
                                        CT(k,j) = row; \
                                        if(!VeryFirst) \
                                                { Save_ded(row,col); }
#endif

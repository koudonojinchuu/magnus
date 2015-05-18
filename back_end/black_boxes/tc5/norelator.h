/* File norelator.h */
#include "ct.h"
#include "tc.h"
#include "miscellaneous.h"
#include "coset_def.h"
#define Fill_KN_Hlt(row,col,k)  CT(k,inv_to_col[col]) = row; \
                                CT(row,col) = k
#define Next_KN_Hlt     do KN_Hlt += COSET; \
                        while((KN_Hlt < NEXTDF) && CT(KN_Hlt,1) < 0 ); \
                        if(KN_Hlt == NEXTDF) \
                                 { Index_return; }
#define Index_return      tc_text(12, tc_pvar); \
                            INDEX = TRUE; \
                          return


/* File print_ct.h  */
#include "tc.h"
#include "tcyacc.h"
#include "ct.h"
#include "ctype.h"
#include "miscellaneous.h"
#define PrintCosetRep	fprintf(fout,"coset rep "); \
			if (Gen_st[1].gen == '0') {  \
                                fprintf(fout,"%d",COSREP[1]); \
                                for (j = 2; j <= cosrepl; j++) \
                                        fprintf(fout,"*%d",COSREP[j]); \
                        } else \
                                for (j = 1; j <= cosrepl; j++) { \
                                        fprintf(fout,"%c",COSREP_A[j]); \
                                } \
                        printf("\n")


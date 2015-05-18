/* include file todd_coxeter.h  */
#include "ctype.h"
#include "tc.h"
#include "tcyacc.h"
#include "miscellaneous.h"
#ifndef __TODD__
#define __TODD__
#define Pdl_Space	512
#define Edp_Space	2 * Rel_len + 4 * NDGEN
#define Other_Space	6 * NDGEN + 30
#define Trans_Parameter  RELATORS_IN_SUBGROUP = Rel_in_sgp; \
        		FILL_FACTOR = Fill_factor; \
			COMP_PCT = Compact; \
			F_TYPE_DEF = F_type; \
			A_FACTOR = A_factor; \
			NDGEN = Gen_no; \
			NDREL = Rel_no; \
			NSGPG = Sgen_no; \
			SGEN_LEN = Sgn_len; \
			REL_LEN = Rel_len; \
        		MSGLVE = Message_interval; \
			PAGE_STRATEGY = toupper(Page_strategy); \
        		MAXTAB = Max_coset; \
			NEWREL = Newrel; Newrel = 0; \
			NEWSG = Newsg; Newsg = 0; \
			for (i=0; i<=9; i++) \
				DIAG[i] = Diag[i]; \
			relator_space = 2 * Rel_len + 3 * NDREL; \
			subg_gen_space = Sgn_len + 2 * NSGPG; \
			Y = Space; \
        		if (Time != 0.000000)  { \
				LTIME = utl_second(); \
            			LTIME = LTIME + Time; \
          		} else \
            			LTIME = 0.000000;
#define Assign_space	PDL_COL = &y[BACKY - 256]; \
			PDL_ROW = PDL_COL - 256; \
			GEN_TO_COL = PDL_ROW - 1 - NDGEN; \
			INV_TO_COL = GEN_TO_COL - 3 * NDGEN - 1; \
			COL_TO_GEN = INV_TO_COL - 2 * NDGEN - 1; \
			SUBG_LENGTH = COL_TO_GEN - NSGPG - 3; \
			SUBG_INDEX = SUBG_LENGTH - NSGPG - 1; \
			SUBGROUP_GEN = SUBG_INDEX - Sgn_len - 1; \
			RELATOR_EXPONENT = SUBGROUP_GEN - NDREL - 1; \
			RELATOR_LENGTH = RELATOR_EXPONENT - NDREL - 1; \
			RELATOR_INDEX = RELATOR_LENGTH - NDREL - 1; \
			RELATORS = RELATOR_INDEX - 2 * Rel_len - 1; \
			EDP = RELATORS - 2 * Rel_len - 2; \
			EDP_END = EDP - 2 * NDGEN - 2; \
			EDP_BEG = EDP_END - 2 * NDGEN - 1; \
			DED_END = EDP_BEG - 2; \
			COSET_TABLE = &y[FRONTY]
#define Set_maxrow	nmax = relator_space + subg_gen_space; \
			nmax += Pdl_Space + Edp_Space + Other_Space - 1; \
			nmax = BACKY - nmax; \
			nmax = nmax/NCOL; \
			if (MAXTAB > 0) { \
				nmax = min0(nmax, MAXTAB); \
				MAXROW = nmax; \
			} else if (MAXTAB < 0) \
				MAXROW = nmax + MAXTAB; \
			else \
				MAXROW = nmax; \
			if (MSGLVE && PAGE_STRATEGY) \
				tc_text(25,tc_pvar); \
			else if (MSGLVE) \
				tc_text(16, tc_pvar)
#define Execution_time	FNTIME = utl_second(); \
                        FNTIME = FNTIME - sttime; \
                        tc_text(17, tc_pvar)
#define Check_overflow	if (OVERFL) { \
				Execution_time; \
				return; \
			}
#endif
			

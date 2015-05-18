/* File assemble_gens.c  */
#include "tc.h"
#include "tcyacc.h"

Void
tc_assemble_gens(tc_pvar)
struct  tc_str_vars     *tc_pvar;
{
#include "tc_void.h"
register Int 	k;
register Int	ncol;
register Int	i;
register Int	gens;
register Int	j;
/* note which group generators are involutory.
* set GEN_TO_COL(generator) =0 if non-involutory, =-1 if involutory.
*/
	for (k = 1; k <= NDGEN; k++)
            if (Gen_st[k].gen_inv == k)
                GEN_TO_COL[k] = -1;
	    else 
		GEN_TO_COL[k] = 0;
/* allocate columns to the generators and inverses and note ncol.
* also set up the tables COL_TO_GEN and INV_TO_COL.
*/
        ncol = 0;
        if (NDGEN == 1)
        { ncol = 2;
            INV_TO_COL[1] = 2; COL_TO_GEN[1] = 1;
            COL_TO_GEN[2] = -1; GEN_TO_COL[1] = 1;
            INV_TO_COL[2] = 1; GEN_TO_COL[-1] = 2;
        } else
          { for (i = 1; i <= NDGEN; i++)
            { gens = GEN_TO_COL[i];
              if (gens > 0)
                continue;
              else if (gens == 0)
/* non-involutary generator, allocate 2 columns in the coset table.
*/
              { ncol++;
                INV_TO_COL[ncol] = ncol + 1;
                COL_TO_GEN[ncol] = i; GEN_TO_COL[i] = ncol;
                ncol++;
                INV_TO_COL[ncol] = ncol - 1;
                COL_TO_GEN[ncol] = -i; GEN_TO_COL[-i] = ncol;
              }
/* involutary generator, allocate only 1 column in the coset table.
*/
              else if (i != 1)
              { ncol++;
                INV_TO_COL[ncol] = ncol; COL_TO_GEN[ncol] = i;
                GEN_TO_COL[i] = ncol; GEN_TO_COL[-i] = ncol;
              }
/* for coincidence processing we must have the first 2 columns of the
* coset table assigned to a non-involutory generator and its inverse or
* 2 involutory generators. find a non-involutory generator (if it
* exists).
*/
              else
              { for (j = 1; j <= NDGEN; j++)
                 if (GEN_TO_COL[j] == 0)
/* swap the columns for involutory generator 1 and non-involutory
* generator j.
*/
                 { ncol = 3;
                   INV_TO_COL[1] = 2; COL_TO_GEN[1] = j;
                   GEN_TO_COL[j] = 1; INV_TO_COL[2] = 1;
                   COL_TO_GEN[2] = -j; GEN_TO_COL[-j] = 2;
/* set up the information for involutory generator 1.  */
                   INV_TO_COL[3] = 3; COL_TO_GEN[3] = 1;
                   GEN_TO_COL[1] = 3; GEN_TO_COL[-1] = 3;
                   break;
                 }
               if (j > NDGEN)
/* all generators are involutary, allocate only 1 column in the coset table.
*/
               { ncol++;
                INV_TO_COL[1] = 1; COL_TO_GEN[1] = 1;
                GEN_TO_COL[1] = 1; GEN_TO_COL[-1] = 1;
               }
           }
        } /* end of 'for (i = ...)'  */
        }
	NCOL = ncol;
 	return;
}

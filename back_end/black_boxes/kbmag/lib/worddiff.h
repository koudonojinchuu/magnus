#ifndef BLACKBOX_WORDDIFF_H
#define BLACKBOX_WORDDIFF_H

#include "rws.h"

void initialise_wd_fsa(fsa *wd_fsaptr, srec *alphptr, int maxwdiffs);
void build_wd_fsa(fsa *wd_fsaptr, rewriting_system rws, boolean *new_wd);
boolean add_wd_fsa(fsa *wd_fsaptr, reduction_equation *eqn, int *inv, boolean reverse);
void make_full_wd_fsa(fsa *wd_fsaptr, int *inv, int start_no);
void clear_wd_fsa(fsa *wd_fsaptr);
int diff_no(fsa *wd_fsaptr, char *w);
void calculate_inverses(int **invptr, int ngens);

#endif
